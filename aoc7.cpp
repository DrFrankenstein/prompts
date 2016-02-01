#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <utility>
#include <exception>
#include <iostream>
#include <iterator>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_uint.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_eol.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_match.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/operator.hpp>

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace p = boost::phoenix;

using std::uint16_t;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::make_pair;
using std::map;
using std::exception;
using std::ostream;
using std::cout;
using std::cin;
using std::cerr;
using std::noskipws;
using std::endl;
using std::istreambuf_iterator;
using std::getline;

struct Expression
{   // abstract class for any expression that can be evaluated (value, operation or a wire itself)
    // operator () evaluates and returns the resulting value
    virtual uint16_t operator ()() = 0;
};

class Wire : public Expression
{   // an expression that matches a wire to a name
    shared_ptr<Expression> expr;
    string name;

    static map<string, shared_ptr<Wire> > wires;  // all the wires by name
    static map<string, uint16_t> boundValues;     // cache of the values output by wires after evaulation

public:
    Wire() {}
    template <class TExpr>
    Wire(const TExpr& expr, const string& name)
        : expr(make_shared<TExpr>(expr)), name(name)
    {
        wires.erase(this->name);            
        wires.insert(make_pair(this->name, make_shared<Wire>(*this)));
        boundValues.clear();
    }

    virtual uint16_t operator ()() 
    {   // try looking up the result in the cache...
        map<string, uint16_t>::iterator it;
        if ((it = boundValues.find(name)) != boundValues.end())
            return it->second;

        // ...not found, actually evaluate it...
        uint16_t value = (*expr)();
        // and add it to the cache
        boundValues.insert(make_pair(name, value));

        return value;
    }

    static shared_ptr<Wire> get(const string& name)
    {
        auto it = wires.find(name);
        if (it == wires.end())
            return shared_ptr<Wire>();
        else
            return it->second;
    }

    void dump(ostream& os)
    {   // output a wire name and its value
        os << name << ": " << (*this)() << endl;
    }

    static void dumpAll(ostream& os)
    {
        for (auto wire : wires)
            wire.second->dump(os);
    }
};

map<string, shared_ptr<Wire> > Wire::wires;
map<string, uint16_t> Wire::boundValues;

struct ExpressionWrapper : Expression
{   // wraps an expression into a pointer so that spirit.qi's alternative 
    // parser has a common type to work with without downcasting by value and
    // losing info
    shared_ptr<Expression> expr;

    ExpressionWrapper() {}
    template <class Expr>
    ExpressionWrapper(Expr expr) : expr(make_shared<Expr>(expr)) {}
    ExpressionWrapper(const ExpressionWrapper& ew) : expr(ew.expr) {}

    virtual uint16_t operator ()() { return (*expr)(); }
};

struct PrimaryExpression : Expression
{   // any value (number or wire name) in an expression
};

struct WireRefExpression : PrimaryExpression
{   // a wire name in an expression
    string name;

    WireRefExpression() {}
    WireRefExpression(string name) : name(name) {}

    virtual uint16_t operator ()()
    {
        auto expr = Wire::get(name);
        if (!expr)
            throw exception(("Reference to undefined wire '" + name + "'").c_str());
        else
            return (*expr)();
    }
};

struct LiteralExpression : PrimaryExpression
{   // a literal value in an expression
    uint16_t value;

    LiteralExpression() {}
    LiteralExpression(uint16_t value) : value(value) {}

    virtual uint16_t operator ()() 
    {
        return value;
    }
};

enum Operator { OP_NOT, OP_AND, OP_OR, OP_LSHIFT, OP_RSHIFT };

struct UnaryExpression : Expression
{   // an expression with a unary operator (e.g. 'NOT xy')
    Operator op;
    shared_ptr<Expression> arg;

    UnaryExpression() {}
    template <class TExpr>
    UnaryExpression(Operator op, const TExpr& arg)
        : op(op), arg(make_shared<TExpr>(arg))
    {}

    virtual uint16_t operator ()() 
    {
        if (op != OP_NOT)
            throw exception("Invalid operator in unaryExpression");

        return ~(*arg)();
    }
};

struct BinaryExpression : Expression
{   // an expression consisting of two values and an operator
    Operator op;
    shared_ptr<Expression> left, right;

    BinaryExpression() {}
    template <class LExpr, class RExpr>
    BinaryExpression(const LExpr left, Operator op, const RExpr right)
        : op(op), left(make_shared<LExpr>(left)), right(make_shared<RExpr>(right))
    {}

    virtual uint16_t operator ()() 
    {
        uint16_t leftValue = (*left)(), rightValue = (*right)();

        switch (op)
        {
        case OP_AND:    return leftValue & rightValue;
        case OP_OR:     return leftValue | rightValue;
        case OP_LSHIFT: return leftValue << rightValue;
        case OP_RSHIFT: return leftValue >> rightValue;
        default: throw exception("Invalid operator in binaryExpression");
        }
    }
};

/************
PARSING: We use Spirit.Qi, a library that lets you create parsers declaratively
using operator overloading. We just define our operators and the grammar, and
it creates our syntax tree from that definition and the input file in the call
to phrase_match.
*/

struct UnaryOperators : qi::symbols<char, Operator>
{
    UnaryOperators()
    {
        add("NOT", OP_NOT);
    }
} unaryOperator;

struct BinaryOperators : qi::symbols<char, Operator>
{
    BinaryOperators()
    {
        add ("AND", OP_AND)
            ("OR",  OP_OR)
            ("LSHIFT", OP_LSHIFT)
            ("RSHIFT", OP_RSHIFT)
        ;
    }
} binaryOperator;

template <class Iterator>
struct BitwiseGrammar : qi::grammar<Iterator, vector<Wire>(), ascii::blank_type>
{
    BitwiseGrammar() 
        : BitwiseGrammar::base_type(input)
    {
        input %= *(statement >> qi::eol) >> statement;

        statement = (
               (unaryExpression | binaryExpression | primaryExpression) 
            >> "->" 
            >> name
        ) [qi::_val = p::construct<Wire>(qi::_1, qi::_2)];

        unaryExpression = (
               unaryOperator
            >> primaryExpression
        ) [qi::_val = p::construct<UnaryExpression>(qi::_1, qi::_2)];

        binaryExpression = (
               primaryExpression
            >> binaryOperator
            >> primaryExpression
        ) [qi::_val = p::construct<BinaryExpression>(qi::_1, qi::_2, qi::_3)];

        primaryExpression %= symbol | literal;

        symbol %= name;

        name %= qi::lexeme[+qi::char_('a','z')];

        literal %= qi::ushort_;
    }

    qi::rule<Iterator, vector<Wire>(), ascii::blank_type> input;
    qi::rule<Iterator, Wire(), ascii::blank_type> statement;
    qi::rule<Iterator, ExpressionWrapper(), ascii::blank_type> unaryExpression;
    qi::rule<Iterator, ExpressionWrapper(), ascii::blank_type> binaryExpression;
    qi::rule<Iterator, ExpressionWrapper(), ascii::blank_type> primaryExpression;
    qi::rule<Iterator, WireRefExpression(), ascii::blank_type> symbol;
    qi::rule<Iterator, string(), ascii::blank_type> name;
    qi::rule<Iterator, LiteralExpression(), ascii::blank_type> literal;
};

void exec()
{
    //string foo ("123 -> x\n"
    //            "456 -> y\n"
    //            "x AND y -> d\n"
    //            "x OR y -> e\n"
    //            "x LSHIFT 2 -> f\n"
    //            "y RSHIFT 2 -> g\n"
    //            "NOT x -> h\n"
    //            "NOT y -> i");

    // parse and compile
    vector<Wire> ast;
    BitwiseGrammar<spirit::istream_iterator> bitwise;
    cin >> noskipws >> qi::phrase_match(bitwise, ascii::blank, ast);

    if (!cin.eof() && cin.good())
    {
        string line;
        getline(cin, line);
        throw exception(("Failed to parse near '" + line + "'").c_str());
    }

    // evaluate
    Wire::get("a")->dump(cout);

    // override 'b' with the value of 'a' and recompute
    uint16_t a = (*Wire::get("a"))();
    Wire b (LiteralExpression(a), "b");

    Wire::get("a")->dump(cout);

    //auto begin = foo.cbegin(), end = foo.cend();
    //qi::phrase_parse(begin, end, bitwise, ascii::blank, ast);

    //if (begin != end)
    //    throw exception(("Failed to parse near '" + string(begin, begin + 50) + "'").c_str());

    //Wire::dumpAll(cout);
}

int main()
{
    try
    {
        exec();
    }
    catch (exception& ex)
    {
        cerr << ex.what() << endl;
    }
}
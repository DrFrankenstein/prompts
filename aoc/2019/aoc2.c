#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#define DISASM 0      // if set to 1, turns on disassembly output

typedef int IntCode;  // an IntCode computer word
#define IC "d"        // the printf/scanf specifier that goes with IntCode

enum Opcode
{	// names for the IntCode opcodes
	ADD = 1,
	MUL = 2,
	IN = 3,
	OUT = 4,
	HALT = 99
};

enum ParamMode
{	// names for the instruction parameter modes
	MEMORY = 0,
	IMMEDIATE = 1
};

struct Computer
{
	size_t ip;       // instruction pointer
	size_t memsize;  // size of the memory
	IntCode* mem;    // memory
	IntCode opcode;  // opcode register
	bool running;    // running/idle flag
};

#pragma region Image file parser //////////////////////////////////////////////////////////////////
// reads one integer from 'image', followed by a comma or EOF.
static IntCode read_integer(FILE* image)
{
	IntCode integer;

	// read a number, possibly surrounded by whitespace.
	// there _is_ whitespace in one case in the input file: the final line return
	int status = fscanf(image, " %"IC" ", &integer);

	if (status == EOF)
	{	// failed to read anything from the file
		perror("unexpected end-of-file or stream error");
		exit(1);
	}

	if (status < 1)
	{	// it wasn't a number
		fprintf(stderr, "unexpected character '%c' at position %ld\n", getc(image), ftell(image));
		exit(1);
	}

	// number should be followed by a comma or EOF
	int comma = getc(image);

	if (comma != ',' && comma != EOF)
	{
		fprintf(stderr, "expected ',' but got '%c' at position %ld\n", comma, ftell(image));
		exit(1);
	}

	return integer;
}

// appends an integer to the memory, resizing the array to accomodate if needed
static IntCode* append_integer(IntCode* mem, IntCode integer, size_t size, size_t* capacity)
{
	if (size == *capacity)
	{	// memory is too small, buy more!
		// we increment by 4 so that we don't have to do it every time.
		*capacity += 4;
		IntCode* newmem = realloc(mem, *capacity * sizeof * mem);
		if (!newmem)
		{
			perror("can't allocate memory for image");
			exit(1);
		}

		mem = newmem;
	}

	mem[size] = integer;

	return mem;
}

// reads a memory image file (the input file) into the memory
static IntCode* read_image(FILE* image, size_t* sizeref)
{
	size_t size = 0, capacity = 0;
	IntCode* mem = NULL;

	while (!feof(image))
	{
		IntCode integer = read_integer(image);
		mem = append_integer(mem, integer, size++, &capacity);
	}

	*sizeref = size;
	return mem;
}
#pragma endregion

#pragma region Disassembler ///////////////////////////////////////////////////////////////////////
static void disasm_vout(const char* format, va_list args)
{
#if DISASM
		vprintf(format, args);
#endif
}

static void disasm_out(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	disasm_vout(format, args);

	va_end(args);
}

static bool disasm_first_param = false;
static void disasm_ip(struct Computer* computer)
{
	disasm_out("%08zu ", computer->ip);
}

static void disasm_op(const char* mnemonic)
{
	disasm_out("%-4s", mnemonic);
	disasm_first_param = true;
}

static void disasm_param(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	disasm_out("%c", disasm_first_param ? ' ' : ',');
	disasm_vout(format, args);

	va_end(args);
	disasm_first_param = false;
}
#pragma endregion

#pragma region Param decoder + memory load/store //////////////////////////////////////////////////
static IntCode consume(struct Computer* computer)
{
	if (computer->ip >= computer->memsize)
	{
		fprintf(stderr, "%08zu !!! ip out of bounds\n", computer->ip);
		exit(1);
	}

	return computer->mem[computer->ip++];
}

static IntCode load(struct Computer* computer)
{
	IntCode ptr = consume(computer);
	disasm_param("(%"IC")", ptr);
	return computer->mem[ptr];
}

static void store(struct Computer* computer, const IntCode rvalue)
{
	IntCode ptr = consume(computer);
	disasm_param("(%"IC") = %"IC, ptr, rvalue);
	computer->mem[ptr] = rvalue;
}

static IntCode immediate(struct Computer* computer)
{
	IntCode val = consume(computer);
	disasm_param("%"IC, val);
	return val;
}

static IntCode decode_param(struct Computer* computer, unsigned idx)
{
	IntCode mode = computer->opcode / 100;
	while (idx--) mode /= 10;
	mode %= 10;

	switch (mode)
	{
	case IMMEDIATE: return immediate(computer);
	case MEMORY: return load(computer);

	default:
		disasm_param("%s", "???");
		printf("illegal parameter mode %"IC"\n", mode);
		exit(1);
	}
}
#pragma endregion

#pragma region Arithmetic and logic unit //////////////////////////////////////////////////////////
// the 'add' opcode
static void add(struct Computer* computer)
{
	disasm_op("ADD");
	IntCode lhs = decode_param(computer, 0);
	IntCode rhs = decode_param(computer, 1);
	store(computer, lhs + rhs);
}

// the 'multiply' opcode
static void mul(struct Computer* computer)
{
	disasm_op("MUL");
	IntCode lhs = decode_param(computer, 0);
	IntCode rhs = decode_param(computer, 1);
	store(computer, lhs * rhs);
}

// the 'input' opcode
static void in(struct Computer* computer)
{
	disasm_op("IN");
	disasm_out("%s", " ...\n");

	printf("%s", "? ");
	IntCode input;
	int status;
	do
	{
		status = scanf(" %"IC, &input);

		if (status == EOF)
		{
			perror("error reading input");
			exit(1);
		}
	} while (status < 1);

	disasm_out("%s", "  ... ");

	store(computer, input);
}

static void out(struct Computer* computer)
{
	disasm_op("OUT");
	printf("= %"IC"\n", load(computer));
}

static void halt(struct Computer* computer)
{
	disasm_op("HALT");
	computer->running = false;
}
#pragma endregion

#pragma region Main instruction decoder ///////////////////////////////////////////////////////////
// runs one instruction; returns false on halt
static void step(struct Computer* computer)
{
	disasm_ip(computer);

	computer->opcode = consume(computer);
	switch (computer->opcode % 100)
	{
	case ADD:
		add(computer);
		break;

	case MUL:
		mul(computer);
		break;

	case IN:
		in(computer);
		break;

	case OUT:
		out(computer);
		break;

	case HALT:
		halt(computer);
		break;

	default:
		disasm_op("???");
		printf("illegal instruction %"IC"\n", computer->opcode);
		exit(1);
	}

	disasm_out("%c", '\n');
}
#pragma endregion

#pragma region Clock generator ////////////////////////////////////////////////////////////////////
// runs the IntCode machine until halt or memory overrun
static void run(struct Computer* computer)
{
	computer->ip = 0;
	computer->running = true;
	while (computer->running)
	{
		step(computer);
	}
}
#pragma endregion

// creates and runs an IntCode machine with 'noun' and 'verb' params set
static IntCode run_with_params(IntCode* initmem, size_t size, IntCode noun, IntCode verb)
{	// we create a copy of the memory so that we don't clobber anything between reruns
	IntCode* mem = calloc(size, sizeof * initmem);
	if (!mem)
	{
		perror("cannot create an IntCode computer");
		exit(1);
	}

	memcpy(mem, initmem, size * sizeof * mem);
	mem[1] = noun;
	mem[2] = verb;

	struct Computer computer;
	computer.mem = mem;
	computer.memsize = size;

	run(&computer);

	IntCode result = mem[0];
	free(mem);
	return result;
}

// finds the params that result in 'target' (Day 2 part 2)
int find_params(IntCode* initmem, size_t size, IntCode target)
{
	for (IntCode noun = 0; noun < 100; ++noun)
		for (IntCode verb = 0; verb < 100; ++verb)
		{
			IntCode result = run_with_params(initmem, size, noun, verb);
			if (result == target)
			{
				printf("noun = %"IC", verb = %"IC", result = %"IC"\n", noun, verb, 100 * noun + verb);
				return 0;
			}
		}

	puts("not found");
	return 1;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "usage: %s <image-file>\n", argv[0]);
		return 1;
	}

	FILE* image = fopen(argv[1], "r");

	size_t size;
	IntCode* initmem = read_image(image, &size);

	//return find_params(initmem, size, 19690720);

	struct Computer computer;
	computer.mem = initmem;
	computer.memsize = size;
	run(&computer);

	return 0;
}

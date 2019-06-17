#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

// <http://www.rosettacode.org/wiki/Abbreviations,_easy>
// ...with a twist: we automatically find valid abbreviations instead of using the capitals from the file

/* ENGINEERING NOTES
 * There are several ways to store the list of commands:
 *
 * - An array, which we then walk in search of a matching command.
 *   Optionally, the array can be sorted to moderately speed up searches.
 *
 * - A linked list, which we also talk through.
 *   It can also be kept sorted. In that case, insertions are actually faster, since we can just
 *   splice the command in.
 *   (this is how the first iteration of this was done.)
 *
 * - A search tree, for fast O(log n) searches *and* insertions. It is naturally sorted.
 *   (this is what we're using now.)
 *
 * We cannot use a hash table, as it can only find exact matches. We're case insensitive,
 * and we support autocompleting, so that's a no-go.
 *
 * The type of search tree we use is a 'trie', where each character is its own node, with the possible
 * next characters as its children. A valid command string is represented by a unique path down the trie.
 * A abbreviation is valid if there is only one path down from what was input.
 *
 * The trie is implemented as a doubly-chained (or left-child/right-sibling) tree. Every character node
 * is part of a linked list of its siblings, and points to the start of its childrens' linked list.
 */

typedef char Command[16];

struct CommandNode
{
    char character;
    struct CommandNode* next;
    struct CommandNode* children;
    bool leaf;
};

struct CommandNode* commands = NULL;

struct CommandNode* create_node(char c)
{
    struct CommandNode* newnode = calloc(1, sizeof *newnode);

    if (!newnode)
        abort();

    newnode->character = c;
    return newnode;
}

struct CommandNode* add_command(struct CommandNode* list, Command command);

void add_next_character(struct CommandNode* node, Command command)
{
    if (*command)
        node->children = add_command(node->children, command);
    else
        node->leaf = true;
}

struct CommandNode* add_command(struct CommandNode* list, Command command)
{
    if (!list)
    {   // FIRST! this level of the trie doesn't even exist... yet
        list = create_node(*command);
        add_next_character(list, ++command);
        return list;
    }

    // find the current character at this level of the trie
    struct CommandNode* node = list;
    struct CommandNode* prev = NULL;
    while (node)
    {
        int diff = tolower(node->character) - tolower(*command);

        if (diff >= 0)
        {   
            if (diff > 0)
            {   // we're past where current char should be, need to add ours...
                struct CommandNode* newnode = create_node(*command);
                newnode->next = node;       // ...before current node

                if (prev)
                    prev->next = newnode;   // splice after prev
                else
                    list = newnode;         // prepend to the entire list

                node = newnode;
            }

            add_next_character(node, ++command);
            return list;
        }

        prev = node;
        node = node->next;
    }
    
    // made it to the end of this level without finding current character
    // (or anything past it), so append it to the list
    struct CommandNode* newnode = create_node(*command);
    prev->next = newnode;

    add_next_character(newnode, ++command);
    return list;
}

void load_commands(void)
{
    FILE* file = fopen("input.txt", "r");
    Command command;
    while (fscanf(file, " %15s", command) == 1)
    {
        commands = add_command(commands, command);
    }
}

char* create_string(size_t length)
{
    char* string = malloc(length + 1);

    if (!string)
        abort();

    string[length] = '\0';
    return string;
}

char* try_insert_at(char* string, size_t idx, char c)
{
    if (!string)
        return NULL;

    string[idx] = c;
    return string;
}

char* autocomplete(struct CommandNode* node, size_t idx)
{
    if (node->next)
        // ambiguous because there's more than one possible next character
        return NULL;

    if (node->children)
    {
        if (node->leaf)
            // ambiguous because there's more than one command along this branch (e.g. 'find', 'findNext')
            return NULL;

        return try_insert_at(autocomplete(node->children, idx + 1), idx, node->character);
    }

    assert(node->leaf);

    return try_insert_at(create_string(idx + 1), idx, node->character);
}

char* do_find_command(struct CommandNode* list, Command command, size_t idx)
{
    while (list)
    {
        int diff = tolower(list->character) - tolower(*command);

        if (diff == 0)
        {   // this node matches...
            if (!command[1])
            {   // we're at the end of 'command'
                if (list->leaf)
                    // and it's a perfect match!
                    return try_insert_at(create_string(idx + 1), idx, list->character);

                // 'command' is a prefix
                return try_insert_at(autocomplete(list->children, idx + 1), idx, list->character);
            }

            if (!list->children)
                // input is longer than this branch (can't go further down)
                return NULL;

            // continue with the next character
            return try_insert_at(do_find_command(list->children, ++command, idx + 1), idx, list->character);
        }

        if (diff > 0)
            // we're past where our character should be
            return NULL;

        // no match, try the next branch
        list = list->next;
    }

    // ran out of branches without finding our character
    return NULL;
}

char* find_command(struct CommandNode* list, Command command)
{
    return do_find_command(list, command, 0);
}

int main(void)
{
    load_commands();

    while (!feof(stdin))
    {
        Command command;
        (void) scanf(" %15s", command);

        char* actual = find_command(commands, command);
        if (actual)
            printf("Matching command: %s\n", actual);
        else
            printf("No matching command\n");

        free(actual);
    }
}

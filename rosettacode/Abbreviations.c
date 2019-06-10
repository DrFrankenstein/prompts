#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef char Command[16];

struct CommandNode
{
    Command command;
    struct CommandNode* next;
};

struct CommandNode* commands = NULL;

void add_command(Command command)
{
    struct CommandNode* newnode = calloc(1, sizeof *newnode);

    if (!newnode)
        abort();

    strcpy(newnode->command, command);

    if (!commands)
    {
        commands = newnode;
        return;
    }

    struct CommandNode* node = commands,
        * last = NULL;

    while (node)
    {
        if (strcmpi(command, node->command) < 0)
        {   // splice before 'node'
            newnode->next = node;

            if (!last)  // we're at the start of the list
                commands = newnode;
            else  // we're splicing after 'last'
                last->next = newnode;

            return;
        }

        last = node;
        node = node->next;
    }

    // we're at the end
    last->next = newnode;
}

void load_commands()
{
    FILE* file = fopen("input.txt", "r");
    Command command;
    while (fscanf(file, " %15s", command) == 1)
    {
        add_command(command);
    }
}

size_t prefix_length(Command candidate, Command command)
{
    size_t idx = 0;
    while (command[idx])
    {
        char left = candidate[idx], right = command[idx];

        if (!left || tolower(left) != tolower(right))
            return 0;

        ++idx;
    }

    return idx;
}

struct CommandNode* find_command(Command command)
{
    struct CommandNode* node = commands;
    struct CommandNode* best_match = NULL;
    size_t best_match_len = 0;

    while (node)
    {
        size_t match_len = prefix_length(node->command, command);

        if (match_len)
        {
            if (match_len == best_match_len)
                return NULL;    // ambiguous

            if (match_len > best_match_len)
            {
                best_match = node;
                best_match_len = match_len;
            }
        }
        else
        {
            if (strcmpi(command, node->command) < 0)
                // we've gone past where it would be
                break;
        }

        node = node->next;
    }

    return best_match;
}

int main(void)
{
    load_commands();

    while (!feof(stdin))
    {
        Command command;
        scanf(" %15s", command);

        struct CommandNode* node = find_command(command);
        if (node)
            printf("Matching command: %s\n", node->command);
        else
            printf("No matching command\n");
    }
}

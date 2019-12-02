#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef unsigned int IntCode;
#define IC "u"
enum Opcode { ADD = 1, MUL = 2, HALT = 99 };

static IntCode read_integer(FILE* image)
{
	int integer;
	int status = fscanf(image, " %d ", &integer);

	if (status == EOF)
	{
		perror("unexpected end-of-file or stream error");
		exit(1);
	}

	if (status < 1)
	{
		fprintf(stderr, "unexpected character '%c' at position %ld\n", (char)getc(image), ftell(image));
		exit(1);
	}

	int comma = getc(image);

	if (comma != ',' && comma != EOF)
	{
		fprintf(stderr, "expected ',' but got '%c' at position %ld\n", (char)comma, ftell(image));
		exit(1);
	}

	return integer;
}

static IntCode* append_integer(IntCode* mem, IntCode integer, size_t size, size_t* capacity)
{
	if (size == *capacity)
	{
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

static size_t read_image(FILE* image, IntCode** data)
{
	size_t size = 0, capacity = 0;
	IntCode* mem = NULL;

	while (!feof(image))
	{
		IntCode integer = read_integer(image);
		mem = append_integer(mem, integer, size++, &capacity);
	}

	*data = mem;
	return size;
}

static void add(IntCode* mem, size_t pc)
{
	IntCode arg1 = mem[pc + 1], 
		arg2 = mem[pc + 2], 
		to = mem[pc + 3];
	printf("%08zu ADD %"IC"(%"IC"),%"IC"(%"IC"),%"IC"\n",
		pc, arg1, mem[arg1], arg2, mem[arg2], to);

	mem[to] = mem[arg1] + mem[arg2];
}

static void mul(IntCode* mem, size_t pc)
{
	IntCode arg1 = mem[pc + 1], 
		arg2 = mem[pc + 2], 
		to = mem[pc + 3];
	printf("%08zu MUL %"IC"(%"IC"),%"IC"(%"IC"),%"IC"\n",
		pc, arg1, mem[arg1], arg2, mem[arg2], to);

	mem[to] = mem[arg1] * mem[arg2];
}

static bool step(IntCode* mem, size_t pc)
{
	IntCode code = mem[pc];
	switch (code)
	{
	case ADD:
		add(mem, pc);
		return true;

	case MUL:
		mul(mem, pc);
		return true;

	case HALT:
		printf("%08zu HALT\n", pc);
		return false;

	default:
		printf("%08zu ??? (%"IC")\n", pc, code);
		exit(1);
	}
}

static void run(IntCode* mem, size_t size)
{
	size_t pc = 0;
	while (step(mem, pc))
	{
		pc += 4;
		if (pc >= size)
		{
			fprintf(stderr, "%08zu !!! out of bounds\n", pc);
			exit(1);
		}
	}
}

int main(void)
{
	IntCode* mem;
	size_t size = read_image(stdin, &mem);

	mem[1] = 12;
	mem[2] = 2;

	run(mem, size);

	printf("00000000 = %"IC"\n", mem[0]);
}

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef int IntCode;  // the type of the elements in the machine's memory; in other words, an IntCode "byte"
#define IC "d"        // the printf/scanf specifier that goes with IntCode

enum Opcode
{	// names for the IntCode opcodes
	ADD = 1,
	MUL = 2,
	IN = 3,
	OUT = 4,
	HALT = 99
};

struct Computer
{
	size_t ip;
	size_t memsize;
	IntCode* mem;
	bool running;
};

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

// the 'add' opcode
static void add(struct Computer* computer)
{
	IntCode p1 = computer->mem[computer->ip + 1],
		p2 = computer->mem[computer->ip + 2],
		to = computer->mem[computer->ip + 3];
	//printf("%08zu ADD %"IC"(%"IC"),%"IC"(%"IC"),%"IC"\n",
	//	ip, p1, mem[p1], p2, mem[p2], to);

	computer->mem[to] = computer->mem[p1] + computer->mem[p2];
	computer->ip += 4;
}

// the 'multiply' opcode
static void mul(struct Computer* computer)
{
	IntCode p1 = computer->mem[computer->ip + 1],
		p2 = computer->mem[computer->ip + 2],
		to = computer->mem[computer->ip + 3];
	//printf("%08zu MUL %"IC"(%"IC"),%"IC"(%"IC"),%"IC"\n",
	//	ip, p1, mem[p1], p2, mem[p2], to);

	computer->mem[to] = computer->mem[p1] * computer->mem[p2];
	computer->ip += 4;
}

// the 'input' opcode
static void in(struct Computer* computer)
{
	printf("Input requested at %zu: ", computer->ip);
	IntCode input;
	int status;
	do
	{
		status = scanf(" %"IC, &input);
	} while (status < 1);

	IntCode p1 = computer->mem[computer->ip + 1];
	computer->mem[p1] = input;

	computer->ip += 2;
}

static void out(struct Computer* computer)
{
	IntCode p1 = computer->mem[computer->ip + 1];
	printf("(%08zu) %"IC"\n", computer->ip, computer->mem[p1]);

	computer->ip += 2;
}

static void halt(struct Computer* computer)
{
	computer->running = false;
}

// runs one instruction; returns false on halt
static void step(struct Computer* computer)
{
	IntCode code = computer->mem[computer->ip];
	switch (code)
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
		//	printf("%08zu HALT\n", ip);
		break;

	default:
		printf("%08zu ??? (%"IC")\n", computer->ip, code);	// invalid instruction
		exit(1);
	}
}

// runs the IntCode machine until halt or memory overrun
static void run(struct Computer* computer)
{
	computer->ip = 0;
	computer->running = true;
	while (computer->running)
	{
		step(computer);
		if (computer->ip >= computer->memsize)
		{
			fprintf(stderr, "%08zu !!! out of bounds\n", computer->ip);
			exit(1);
		}
	}
}

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

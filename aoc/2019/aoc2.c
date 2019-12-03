#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned int IntCode;                 // the type of the elements in the machine's memory; in other words, an IntCode "byte"
#define IC "u"                                // the printf/scanf specifier that goes with IntCode
enum Opcode { ADD = 1, MUL = 2, HALT = 99 };  // names for the IntCode opcodes

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
static void add(IntCode* mem, size_t ip)
{
	IntCode p1 = mem[ip + 1], 
		p2 = mem[ip + 2], 
		to = mem[ip + 3];
	//printf("%08zu ADD %"IC"(%"IC"),%"IC"(%"IC"),%"IC"\n",
	//	ip, p1, mem[p1], p2, mem[p2], to);

	mem[to] = mem[p1] + mem[p2];
}

// the 'multiply' opcode
static void mul(IntCode* mem, size_t ip)
{
	IntCode p1 = mem[ip + 1], 
		p2 = mem[ip + 2], 
		to = mem[ip + 3];
	//printf("%08zu MUL %"IC"(%"IC"),%"IC"(%"IC"),%"IC"\n",
	//	ip, p1, mem[p1], p2, mem[p2], to);

	mem[to] = mem[p1] * mem[p2];
}

// runs one instruction; returns false on halt
static bool step(IntCode* mem, size_t ip)
{
	IntCode code = mem[ip];
	switch (code)
	{
	case ADD:
		add(mem, ip);
		return true;

	case MUL:
		mul(mem, ip);
		return true;

	case HALT:
	//	printf("%08zu HALT\n", ip);
		return false;

	default:
		printf("%08zu ??? (%"IC")\n", ip, code);	// invalid instruction
		exit(1);
	}
}

// runs the IntCode machine until halt or memory overrun
static void run(IntCode* mem, size_t size)
{
	size_t ip = 0;
	while (step(mem, ip))
	{
		ip += 4;
		if (ip >= size)
		{
			fprintf(stderr, "%08zu !!! out of bounds\n", ip);
			exit(1);
		}
	}
}

// creates and runs an IntCode machine with 'noun' and 'verb' params set
static IntCode run_with_params(IntCode* initmem, size_t size, IntCode noun, IntCode verb)
{	// we create a copy of the memory so that we don't clobber anything between reruns
	IntCode* mem = calloc(size, sizeof *initmem);
	if (!mem)
	{
		perror("cannot create an IntCode computer");
		exit(1);
	}

	memcpy(mem, initmem, size * sizeof *mem);
	mem[1] = noun;
	mem[2] = verb;

	run(mem, size);

	IntCode result = mem[0];
	free(mem);
	return result;
}

int main(void)
{
	size_t size;
	IntCode* initmem = read_image(stdin, &size);

	for (IntCode noun = 0; noun < 100; ++noun)
		for (IntCode verb = 0; verb < 100; ++verb)
		{
			IntCode result = run_with_params(initmem, size, noun, verb);
			if (result == 19690720)
			{
				printf("noun = %"IC", verb = %"IC", result = %"IC"\n", noun, verb, 100 * noun + verb);
				return 0;
			}
		}

	puts("not found");
	return 1;
}

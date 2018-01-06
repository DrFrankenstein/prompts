#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int dist_to_port(int n)
{
	if (n == 1)
		return 0;

	int r = ((int) sqrt(n - 1) + 1) / 2, // straight-line component / index of the ring (from 0)
		s = 2 * r,                         // max distance from 1 in ring (at corner)
		p = (n + r - 1) % s,               // position of the number in its edge of the ring (0 at corner)
		c = abs(r - p),                    // distance from nearest corner of the ring (negation of 2nd distance component)
		d = s - c;                         // Manhattan distance taken by starting from the corner of that ring and subtracting how far 'n' is from it

	return d;
}

int main(void)
{
	int n;	// requested memory address
	int read = scanf("%d", &n);
	
	if (read != 1)
	{
		perror("could not read input");
		return EXIT_FAILURE;
	}

	printf("Distance is: %d\n", dist_to_port(n));
}
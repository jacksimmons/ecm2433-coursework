#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "riffle.h"

int main()
{
	// Set the seed based on time
	srand(time(NULL));
	
	// 20 integer array
	int ints[] = {1,2,3,4,5,6,7,8,9,10,
	11,12,13,14,15,16,17,18,19,20};
	
	// Get pointer to start of `ints`
	int *ints_ptr = &ints[0];
	int ints_len = 20;

	// Perform riffle on `ints`
	riffle(ints_ptr, ints_len, sizeof(int), 20);

	// Output the result of the riffle
	printf("Integer array:\n");
	for (int i = 0; i < ints_len; i++)
	{
		printf("%i\n", ((int*)ints_ptr)[i]);
	}

	// Output the shuffle check of the riffle
	printf("Shuffle check: %i\n", check_shuffle(ints_ptr, ints_len, sizeof(int), &cmp_int));

	// Greek array
	char *greek[] = {"alpha", "beta", "gamma", "delta", "epsilon", "zeta",
	"eta", "theta", "iota", "kappa", "lambda", "mu"};
	
	// Get pointer to start of `greek`
	char **greek_ptr = &greek[0];
	int greek_len = 12;

	// Perform riffle on `greek`
	riffle(greek, greek_len, sizeof(char) * 8, 20);

	// Output the result of the riffle
	printf("Greek array:\n");
	for (int i = 0; i < greek_len; i++)
	{
		printf("%s\n", ((char**)greek)[i]);
	}
	
	// Output the shuffle check of the riffle
	printf("Shuffle check: %i\n", check_shuffle(greek_ptr, greek_len, sizeof(char) * 8, &cmp_str));
}
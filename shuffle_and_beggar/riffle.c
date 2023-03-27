#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <string.h>
#include "riffle.h"

// Performs one iteration of the riffle shuffle
// *work requires as much space as *L
// *work starts as a copy of L.
// *L - The array to riffle shuffle
// len - The exact length of L
// size - The exact size of any element in L
// *work - Workspace array, stores the shuffled version of L
void riffle_once(void *L, int len, int size, void *work)
{
	// Begin by populating `work` with `L`
	memcpy(work, L, len * size);
	
	// Left array is `split_index` and before; Right array is the rest
	int split_index;
	int left_split_len;
	int right_split_len;
	if (len > 0)
	{
		// Even length (e.g. len 4 -> split_index = 1 ->
		// listL = [0,1], listR = [2,3]
		if (len % 2 == 0)
		{
			split_index = (len / 2) - 1;
		}
		// Odd length (e.g. len 5 -> split_index = 2 ->
		// listL = [0,1,2], listR = [3,4]
		// -> listL splits into listLL [0,1] listLR [2]
		// -> listLL splits into [0] [1] as expected
		else
		{
			split_index = (len - 1) / 2;
		}

		left_split_len = split_index + 1;
		right_split_len = len - left_split_len;

		void *work_ptr = work;
		int left_count = 0;
		int right_count = 0;
		
		// Iterate over every element in L, splitting it randomly into
		// one of two lists (left and right).
		for (int i = 0; i < len; i++)
		{
			// -1 = left, 0 = undefined, 1 = right
			int split_to_take_from = 0;

			// Override the random selection if one split is completed.
			// Now only the other split needs to be completed. Note: it
			// is impossible for both counts to be complete at the same
			// time when i < len.
			if (left_count >= left_split_len)
			{
				split_to_take_from = 1;
			}
			if (right_count >= right_split_len)
			{
				split_to_take_from = -1;
			}

			if (split_to_take_from == 0)
			{
				// Simple 50/50 random split
				// Take from left split (if even)
				if (rand() % 2 == 0)
				{
					split_to_take_from = -1;
				}
				// Take from right split (if odd)
				else
				{
					split_to_take_from = 1;
				}
			}
			
			// Pointer arithmetic, adding {direction}_count * size bytes to
			// L_ptr. L must be cast to (char *) so that we can perform pointer
			// arithmetic in terms of 1 byte chunks.
			// Removing the cast of L to (char *) means this program will ONLY
			// necessarily compile with gcc, as gcc allows void ptr arithmetic.

			// Take from left split
			if (split_to_take_from == -1)
			{
				// Point to L from left_count
				void *L_ptr = (char *)L + left_count * size;
				// Copy `L_ptr` into `work_ptr`
				memcpy(work_ptr, L_ptr, size);
				// Increment the index we are looking at for the next left
				// element.
				left_count++;
			}
			// Take from right split
			else if (split_to_take_from == 1)
			{
				// Point to L from (split_index + 1) + right_count. `split_index`
				// + 1 ensures the right list is only copied from the first index
				// after the final index in the left list (`split_index`).
				void *L_ptr = (char *)L + (split_index + 1 + right_count) * size;
				// Copy `L_ptr` into `work_ptr`
				memcpy(work_ptr, L_ptr, size);
				// Increment the index we are looking at for the next right
				// element.
				right_count++;
			}

			// Set pointer to the next element
			work_ptr += size;
		}
	}
	else
	{
		printf("A list of length 0 cannot be shuffled!\n");
		exit(1);
	}
}

// Performs N iterations of the riffle shuffle
// *L - The starting array, also the fully riffled array after completion
// len - The length of L, which must be exact
// size - The size of each element in L, which must be exact
// N - The number of iterations to perform
void riffle(void *L, int len, int size, int N)
{
	// Allocate memory for work, the workspace array used in riffle_once
	// #work
	void *work;
    if (!(work = malloc(len * size)))
    {
        printf("Out of memory");
        exit(1);
    }

	// Riffle until N riffles have been completed.
	for (int i = 0; i < N; i++)
	{
		riffle_once(L, len, size, work);

		// `work` is the output of the riffle, so point `L` to `work`.
		// This means the input for the next pass is the output for the last.
		// And the final result is pointed to by `L` after the Nth pass.
		memcpy(L, work, len * size);
	}

	// ~work
	free(work);
}

// Checks the outcome of 10 successive riffles is as expected.
// *L - The array to shuffle
// len - The exact length of L
// size - The exact size of any element in L
// *cmp(a, b) - The comparison function, which must return the following:
// a > b => -1
// a == b => 0
// a < b => 1
int check_shuffle(void *L, int len, int size, int (*cmp)(void *, void *))
{
	// `unriffled` is a pointer to `L` which moves around `L` to allow
	// iteration over its elements.
	void *unriffled = L;
	
	// Allocate memory for `riffled`, which will store the riffled version
	// of L (unriffled).
	//#riffled
	void *riffled;
	if (!(riffled = malloc(len * size)))
	{
		printf("Out of memory.");
		exit(1);
	}
	// For garbage collection, since the `riffled` pointer moves around,
	// we need to store the original location of the `riffled` pointer
	// to ensure all of it gets freed.
	void *riffled_start = riffled;
	
	// Copy `L` into `riffled` then riffle `riffled`.
	// `riffled` becomes a pointer to shuffled `L` which moves around
	// shuffled `L` to allow iteration over its elements.
	memcpy(riffled, L, len * size);
	riffle(riffled, len, size, 10);

	// Iterate through every element in `riffled`
	for (int i = 0; i < len; i++)
	{
		// Iterate through every element in `unriffled`
		for (int j = 0; j < len; j++)
		{
			// If one of the values in `unriffled` == the value in `riffled`
			// Then the riffle has been successful thus far.
			if (cmp(unriffled, riffled) == 0)
				break;
			
			// Final iteration passed with no corresponding value in the
			// `riffled` array to the current one in the `unriffled` array.
			// Therefore we can say the shuffle has failed and return 0.
			if (j == len - 1)
			{
				// Need to perform garbage collection before returning.
				//~riffled
				free(riffled_start);
				return 0;
			}
			
			// Increment `unriffled` pointer
			unriffled += size;
		}

		// Increment `riffled` pointer and reset `unriffled` pointer for
		// next iteration.
		unriffled = L;
		riffled += size;
	}
	
	// The shuffle was successful. Perform garbage collection and return 1.
	//~riffled
	free(riffled_start);
	return 1;
}

// The comparison function for integers.
int cmp_int(void *int_a, void *int_b)
{
	int a;
	int b;

	// As int_a and int_b are still void pointers, it is necessary to
	// copy their memory into a and b.
	memcpy(&a, int_a, sizeof(int));
	memcpy(&b, int_b, sizeof(int));

	if (a > b)
		return -1;
	else if (a == b)
		return 0;
	else if (a < b)
		return 1;
}

// The comparison function for null-terminated strings
int cmp_str(void *str_a, void *str_b)
{
	// strcmp returns the opposite of what we want
	// (a > b => 1, a == b => 0, a < b => -1)
	// So flip the arguments to get:
	// (b > a => 1 [a < b => 1], b == a => 0 [a == b => 0], b < a => -1
	// [a > b => -1])
	
	// If a > b, return -1
	// If a == b, return 0
	// If a < b, return 1
	return strcmp((char *)str_b, (char *)str_a);
}

// Function which calculates, on an array {a, b, c, d, e}:
// Number of occurrences of a < b, b < c, etc. divided by the total number
// of elements.
float quality(int *numbers, int len)
{
	// The previous number in `numbers`
	int number_prev;
	// The current number in `numbers`
	int number;
	
	// Number of less than - greater than adjacencies
	int num_lt_gt_adjacencies = 0;
	
	// Find number of lt_gt adjacencies
	for (int i = 1; i < len; i++)
	{
		number_prev = numbers[i-1];
		number = numbers[i];
		
		if (number > number_prev)
			num_lt_gt_adjacencies++;
	}
	
	// Return the float division calculating quality
	return (float)num_lt_gt_adjacencies / (float)len;
}

// Evaluates the average quality of a shuffle of the integers 0 - (`N` - 1),
// shuffled `shuffles` times, over `trials` trials.
float average_quality(int N, int shuffles, int trials)
{
	// Stores the elements 0 - (`N` - 1), and doesn't change.
	int *array;
	// Gets copied into by `array`, and shuffling is performed on this.
	int *array_copy;

	// Allocate memory for the array and array_copy
	//#array
	//#array_copy
	if (!(array = malloc(N * sizeof(int))))
	{
		printf("Out of memory!");
		exit(1);
	}
	if (!(array_copy = malloc(N * sizeof(int))))
	{
		printf("Out of memory!");
		exit(1);
	}
	
	// Populate `array`
	for (int i = 0; i < N; i++)
	{
		array[i] = i;
	}
	
	float quality_sum = 0;
	// Calculate the average quality of `trials` trials
	for (int i = 0; i < trials; i++)
	{
		// As riffle edits the array, need to copy back into
		// `array_copy` every iteration.
		memcpy(array_copy, array, N * sizeof(int));
		// Perform the riffle `shuffles` times
		riffle(array_copy, N, sizeof(int), shuffles);
		// Calculate quality of the output
		quality_sum += quality((int *)array_copy, N);
	}
	
	// Calculate average of the qualities
	float average_quality = quality_sum / trials;
	
	//~array
	//~array_copy
	free(array);
	free(array_copy);
	
	return average_quality;
}
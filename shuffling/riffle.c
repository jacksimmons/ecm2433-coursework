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
void riffle_once(void *L, int len, int size, void *work)
{
	// Begin by populating work with L
	memcpy(work, L, len * size);

	// Set the seed based on time
	srand(time(NULL));
	
	// First array is split_index and before
	int split_index;
	int left_split_len;
	int right_split_len;
	// Second array is after
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
		for (int i = 0; i < len; i++)
		{
			// -1 = left, 0 = undefined, 1 = right
			int split_to_take_from = 0;

			// Override the random selection if one split is completed.
			// Not possible for both to occur, as left_count + right_count > len
			// And only one can be incremented every loop pass.
			if (left_count >= left_split_len)
			{
				split_to_take_from = 1;
			}
			if (right_count >= right_split_len)
			{
				split_to_take_from = -1;
			}

			// if (i < left_split_len && i >= right_split_len)
			// {
			// 	void *L_ptr = L + i * size;
			// 	memcpy(work_ptr, L_ptr, size);
			// 	break;
			// }

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

			if (split_to_take_from == -1)
			{
				void *L_ptr = L + left_count * size;
				memcpy(work_ptr, L_ptr, size);
				left_count++;
			}
			else if (split_to_take_from == 1)
			{
				void *L_ptr = L + (split_index + 1 + right_count) * size;
				memcpy(work_ptr, L_ptr, size);
				right_count++;
			}

			// Set pointer to the next element
			work_ptr += size;
		}
	}
	else
		exit(1);
}

void riffle(void *L, int len, int size, int N)
{
	//*work
	void *work;
    if (!(work = malloc(len * size)))
    {
        printf("Out of memory");
        exit(1);
    }

	for (int i = 0; i < N; i++)
	{
		riffle_once(L, len, size, work);

		// work is the output of the riffle, so point L to work.
		// This means the input for the next pass is the output for the last.
		// And the final result is pointed to by L after the Nth pass.
		memcpy(L, work, len * size);
	}

	//~work
	free(work);
}

// Checks the outcome of 10 successive riffles is as expected.
int check_shuffle(void *L, int len, int size, int (*cmp)(void *, void *))
{
	void *unriffled = L;
	
	//*riffled
	void *riffled;
	if (!(riffled = malloc(len * size)))
	{
		printf("Out of memory.");
		exit(1);
	}
	// For garbage collection, as the riffled pointer moves, we need to store
	// the location of the riffled pointer.
	void *riffled_start = riffled;
	
	memcpy(riffled, L, len * size);
	riffle(riffled, len, size, 10);

	// Iterate through every element in riffled
	for (int i = 0; i < len; i++)
	{
		// Iterate through every element in unriffled
		for (int j = 0; j < len; j++)
		{
			// If one of the values in unriffled == the value in riffled
			// Then the riffle has been successful thus far.
			if (cmp(unriffled, riffled) == 0)
				break;
			
			// Final iteration passed with no corresponding value in the
			// riffled array to the current one in the unriffled array.
			if (j == len - 1)
			{
				//~riffled
				free(riffled_start);
				return 0;
			}
			
			// Increment unriffled pointer
			unriffled += size;
		}

		// Increment riffled pointer and reset unriffled pointer for next iteration
		unriffled = L;
		riffled += size;
	}
	
	//~riffled
	free(riffled_start);
	return 1;
}

int cmp_int(void *int_a, void *int_b)
{
	int a;
	int b;

	memcpy(&a, int_a, sizeof(int));
	memcpy(&b, int_b, sizeof(int));

	if (a > b)
		return -1;
	else if (a == b)
		return 0;
	else if (a < b)
		return 1;
}

int cmp_str(void *str_a, void *str_b)
{
	// Matches cmp_int
	// If a > b, return -1
	// If a == b, return 0
	// If a < b, return 1
	return strcmp((char *)str_b, (char *)str_a);
}

float quality(int *numbers, int len)
{
	int number_prev;
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

float average_quality(int N, int shuffles, int trials)
{
	int *array;
	int *array_copy;
	//*array
	//*array_copy
	// Attempt to malloc the array and work
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
	
	// Populate the array
	for (int i = 0; i < N; i++)
	{
		array[i] = i;
	}
	
	float quality_sum = 0;
	// Calculate the average `trials` times
	for (int i = 0; i < trials; i++)
	{
		// As riffle edits the array, need to copy back into
		// array_copy every iteration.
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
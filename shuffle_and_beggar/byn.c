#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "beggar.h"
#include "byn.h"
#include "single.h"

int main()
{
	// Set the seed based on time
	srand(time(NULL));
	
	Lengths *all_stats[9];
	for (int i = 2; i <= 10; i++)
		all_stats[i-2] = statistics(i, 100);
	
	// File access block
	FILE *f_ptr;
	f_ptr = fopen("statistics.txt", "w");
	
	if (f_ptr == NULL)
	{
		printf("Error opening file.");
		exit(1);
	}
		
	// Output the stats to file
	for (int i = 0; i < 9; i++)
	{
		Lengths *stats = all_stats[i];
		fprintf(f_ptr, "For %i players:\nShortest: %i\nAverage: %f\nLongest: %i\n\n",
		i+2, stats->shortest, stats->average, stats->longest);
		free(stats);
	}
	
	fclose(f_ptr);
}

Lengths *statistics(int Nplayers, int games)
{	
	int sum_turns = 0;
	int shortest = INT_MAX;
	int longest = 0;
	
	Lengths *lengths;
	if (!(lengths = malloc(sizeof(Lengths))))
	{
		printf("Out of memory!");
		exit(1);
	}
	
	for (int i = 0; i < games; i++)
	{
		// We want talkative off to reduce runtime
		int turns = single(Nplayers, 0);
		if (turns < shortest)
			shortest = turns;
		if (turns > longest)
			longest = turns;
		sum_turns += turns;
	}
		 	
	lengths->average = (float)sum_turns / (float)games;
	lengths->shortest = shortest;
	lengths->longest = longest;
	return lengths;
}
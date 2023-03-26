#include <stdlib.h>
#include <stdio.h>
#include "beggar.h"

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		int Nplayers = atoi(argv[1]);
		if (Nplayers > 0 && Nplayers <= 52)
		{					
			int *deck;

			//*deck
			if (!(deck = malloc(sizeof(int) * 52)))
			{
				printf("Out of memory!");
				exit(1);
			}

			for (int i = 0; i < 52; i++)
				deck[i] = i;

			beggar(atoi(argv[1]), deck, 1);
		}
		else
		{
			printf("Please enter an integer number of players between 1 and 52.\n");
			exit(1);
		}
	}
}
#include <stdlib.h>
#include <stdio.h>
#include "riffle.h"
#include "beggar.h"
#include "single.h"

int main(int argc, char *argv[])
{
	// Set the seed based on time
	srand(time(NULL));
	
	if (argc > 1)
	{
		int Nplayers = atoi(argv[1]);
		single(Nplayers, 1);
	}
}
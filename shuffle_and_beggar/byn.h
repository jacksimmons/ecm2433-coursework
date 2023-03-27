typedef struct GAME_LENGTHS
{
	int shortest;
	int longest;
	float average;
} Lengths;

Lengths *statistics(int Nplayers, int games);
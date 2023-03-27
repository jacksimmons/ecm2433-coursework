** Notes **
I have included comments in some places with the format
//#{variable_name} - allocation
//~{variable_name} - deallocation/free
These are to help show where memory is allocated and deallocated,
were mostly used to help during development.

** Navigate to pig_latin/ for the following: **
To run pig latin (piglatin.c), run the commands:
gcc piglatin.c pig.c -o piglatin
./piglatin

To run pig latin test (test_pig.c), run the commands:
gcc test_pig.c pig.c -o test_pig
./test_pig

** Navigate to shuffle_and_beggar/ for the following: **
To run demo_riffle (demo_shuffle.c), run the commands:
gcc demo_shuffle.c riffle.c -o demo_shuffle
./demo_shuffle

To run riffle quality check (quality.c), run the commands:
gcc quality.c riffle.c -o quality
./quality
(Then check shuffle.txt)

To run a single beggar game (single.c), run the commands:
make single
./single

To run the stats of the beggar game (byn.c), run the commands:
make byn
./byn
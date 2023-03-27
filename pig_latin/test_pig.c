#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pig.h"

int main()
{
    // Note how the length of each array must be the length of the string
    // + 3, to accommodate for the extra chars that might be added in pig
    char words[7][11] = {"happy", "duck", "glove", "evil", "eight", "yowler", "crystal"};

    // Iterate over all the words, and output the pig latin form for each
    for (int i = 0; i < 7; i++)
    {
        char word[11];
        strcpy(word, words[i]);
        
        //#pig_word - malloc'd inside pig
        char* pig_word = pig(word);
        
        printf("%s => %s\n", words[i], pig_word);
        
        //~pig_word
        free(pig_word);
    }
}
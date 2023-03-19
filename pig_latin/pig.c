#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "pig.h"

char *pig(char *word)
{
    char *new_word;
    
    //*word (as word = new_word later)
    if (!(new_word = malloc(strlen(word) + 4)))
    {
        printf("Out of memory");
        exit(1);
    }

    char vowels_start[] = {'a', 'e', 'i', 'o', 'u'};
    char vowels_end[] = {'a', 'e', 'i', 'o', 'u', 'y'};

    bool first_is_vowel = false;
    for (int i = 0; i < sizeof(vowels_start); i++)
    {
        if (word[0] == vowels_start[i])
        {
            first_is_vowel = true;
        }
    }

    // Options 1-3 are handled below
    if (first_is_vowel)
    {
        // Option 2.
        strcpy(new_word, word);
        strcat(new_word, "way");
    }
    else
    {
        // Option 1.

        // This will be defined, as an entire vowel-based word
        // must begin with a vowel, hence this block won't enter
        int final_consonant_index;
        bool final_consonant_index_found = false;

        // Test with y acting as a vowel
        for (int i = 0; i < strlen(word); i++)
        {
            if (!final_consonant_index_found)
            {
                for (int j = 0; j < sizeof(vowels_end); j++)
                {
                    if (word[i] == vowels_end[j])
                    {
                        // The index just explored was a vowel;
                        // The last consonant was the i-1th element.
                        final_consonant_index = i - 1;
                        final_consonant_index_found = true;
                        break;
                    }
                }
            }
            else
                break;
        }

        // The only situation in which this could be true is if all the characters
        // are consonants.
        if (!final_consonant_index_found)
            final_consonant_index = strlen(word) - 1;

        // *1
        char *consonants;
        // Need to malloc (final_consonant_index + 1) for the consonants,
        // and then another byte for the null terminator.
        if (!(consonants = malloc(final_consonant_index + 2)))
        {
            printf("Out of memory.\n");
            exit(1);
        }

        // Copy the consonants into the consonants string
        for (int i = 0; i < final_consonant_index + 1; i++)
        {
            consonants[i] = word[i];
        }
        // Terminate the string
        consonants[final_consonant_index + 1] = '\0';

        // Move the pointer along to the final consonant index
        word += final_consonant_index + 1;

        // Copy the chars after the final consonant into new_word
        // new_word now starts with the part after the final consonant
        strcpy(new_word, word);

        // Then concatenate the consonants onto new_word
        strcat(new_word, consonants);
        // *1
        free(consonants);

        // Then add "ay"
        strcat(&new_word[0], "ay");
    }

    // Terminate new_word
    new_word[strlen(new_word)] = '\0';
    word = new_word;
    return word;
}
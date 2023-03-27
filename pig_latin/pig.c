#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "pig.h"

char *pig(char *word)
{
    char *new_word;
    
    // Allocate sufficient memory for `new_word` (#new_word)
    // strlen(word) + 1 gives the full length of the string word (incl. \0)
    // + 3 gives space for the maximum number of additional characters (+way)
    if (!(new_word = malloc(strlen(word) + 4)))
    {
        printf("[new_word:malloc] Out of memory");
        exit(1);
    }

    // Characters which act as vowels when they are the first character
    char vowels_head[] = {'a', 'e', 'i', 'o', 'u'};
    
    // Characters which act as vowels when they are not the first character
    char vowels_tail[] = {'a', 'e', 'i', 'o', 'u', 'y'};

    // Test for (2.); is the first character a vowel?
    bool first_is_vowel = false;
    for (int i = 0; i < sizeof(vowels_head); i++)
    {
        if (word[0] == vowels_head[i])
        {
            first_is_vowel = true;
        }
    }

    if (first_is_vowel)
    {
        // Option 2. `new_word` = `word` + "way"
        strcpy(new_word, word);
        strcat(new_word, "way");
    }
    else
    {
        // Option 1.

        // Note: There must be a consonant, as `first_is_vowel` is false.
        
        // The index of the final consonant in the word
        // i.e. the index directly before the first vowel,
        // or length - 1.
        int final_consonant_index;
        bool final_consonant_index_found = false;
        
        // Iterate through every character in the word, until a vowel is
        // found, or the word ends. (Excluding the first character, as
        // we have already determined this is a consonant)
        for (int i = 1; i < strlen(word); i++)
        {
            if (!final_consonant_index_found)
            {
                // Iterate through every vowel to see if one matches the
                // character in word.
                for (int j = 0; j < sizeof(vowels_tail); j++)
                {
                    if (word[i] == vowels_tail[j])
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

        // Allocate sufficient memory for `consonants` (#consonants)
        char *consonants;
        // Need to malloc (`final_consonant_index` + 1) for the consonants up to and
        // including `final_consonant_index`, + 1 byte for the null terminator.
        if (!(consonants = malloc(final_consonant_index + 2)))
        {
            printf("[consonants:malloc] Out of memory!\n");
            exit(1);
        }

        // Copy the consonants into `consonants`
        for (int i = 0; i < final_consonant_index + 1; i++)
        {
            consonants[i] = word[i];
        }
        // Add null terminator to `consonants`
        consonants[final_consonant_index + 1] = '\0';

        // Move the word pointer along to `final_consonant_index`
        word += final_consonant_index + 1;

        // Copy the chars after `final_consonant_index` into `new_word`
        // `new_word` now starts with the part after the final consonant
        strcpy(new_word, word);

        // Now concatenate `consonants` after `new_word`
        strcat(new_word, consonants);
        
        // Deallocate memory for `consonants` (~consonants)
        free(consonants);

        // Finally, concatenate "ay" after `new_word`
        strcat(new_word, "ay");
    }

    // Add null terminator to `new_word`
    new_word[strlen(new_word)] = '\0';
    
    // Point `word` to the same address as `new_word`
    word = new_word;
    
    return word;
}
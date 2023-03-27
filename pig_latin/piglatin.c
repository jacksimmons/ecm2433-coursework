#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "piglatin.h"
#include "pig.h"

int main()
{
    // Output for the first input
    printf("Enter a line of English:\n");

    // The program loops, asking for inputs until it exits.
    while (1)
    {
        // A string storing all of the user's characters, with no limit on the input
        // size. (#total_input)
        char *total_input = scand();

        if (strlen(total_input) > 0)
        {
            // Add a null terminator to `total_input`
            total_input[strlen(total_input)] = '\0';

            // Copy `total_input` into `copied_input`, for use in the print at the end.
            // This is done as strtok manipulates the `total_input` pointer. (#copied_input)
            char *copied_input;
            if (!(copied_input = malloc(strlen(total_input) + 1)))
            {
                printf("[copied_input:malloc] Out of memory.\n");
                exit(1);
            }
            strcpy(copied_input, total_input);
            // Terminate `copied_input` to make it a valid string for str operations
            copied_input[strlen(total_input)] = '\0';

            // The output of the pig function applied to every word in turn, delimited by " ".
            // #final_output
            char *final_output;
            if (!(final_output = malloc(1)))
            {
                printf("[final_output:malloc] Out of memory.\n");
                exit(1);
            }
            // Add a null terminator
            final_output[0] = '\0';

            // Get the first token with " " delimiter
            char *token;
            token = strtok(total_input, " ");

            // The current length of the output, used for realloc of `final_output`
            int output_length = 0;
            // Continue this loop until no more tokens can be generated
            while (token != NULL)
            {
                // Copy the token into `copied_token` for use in the pig function (#copied_token)
                char *copied_token;
                if (!(copied_token = malloc(strlen(token) + 1)))
                {
                    printf("[copied_token:malloc] Out of memory.\n");
                    exit(1);
                }
                // Copy `token` into `copied_token`
                strcpy(copied_token, token);
                // Terminate `copied_token` to make it a valid string for str operations
                copied_token[strlen(token)] = '\0';

                // `pig_token` - `copied_token` is converted into pig latin. (#pig_token)
                char *pig_token = pig(copied_token);

                // A copy of `final_output` to be copied back into `final_output` after 
                // `final_output` is resized to fit the delimited token as well (#copied_output)
                char *copied_output;
                if (!(copied_output = malloc(strlen(final_output) + 1)))
                {
                    printf("[copied_output:malloc] Out of memory.\n");
                    exit(1);
                }
                // Copy `final_output` into `copied_output`
                strcpy(copied_output, final_output);
                // Terminate copied_output to make it a valid string for str operations
                copied_output[strlen(final_output)] = '\0';

                // Update size (accounting for delimiter " ") to include the next token
                output_length += strlen(copied_output) + 1 + strlen(pig_token);

                // Reallocate to output_length + 1 (+ 1 for null terminator)
                if (!(final_output = realloc(final_output, output_length + 1)))
                {
                    printf("[final_output:realloc] Out of memory.\n");
                    exit(1);
                }
                
                // Copy `copied_output` back into `final_output`, then concatenate delimiter,
                // then concatenate `pig_token`
                strcpy(final_output, copied_output);
                strcat(final_output, " ");
                strcat(final_output, pig_token);

                // Add null terminator to `final_output`
                final_output[output_length] = '\0';
                
                //~copied_token
                //~copied_output
                //~pig_token
                free(copied_token);
                free(copied_output);
                free(pig_token);

                // Continue tokenising the string previously provided (`total_input`)
                token = strtok(NULL, " ");
            }
            
            // Add a null terminator to `final_output`
            final_output[output_length] = '\0';
            
            // Print total translation
            printf("%s =>%s\n", copied_input, final_output);
            
			//~copied_input
            free(copied_input);
            //~final_output
            free(final_output);

            // Prompt for the next input
            printf("Enter a line of English:\n");
        }

        //~total_input
        free(total_input);
    }
}

// A function which performs scanf dynamically - this means there is no limit
// on the size of input provided.
char *scand()
{
    // Allocate memory for the provided input
    char *input;
    if (!(input = malloc(1)))
    {
        printf("[scand:malloc] Out of memory.\n");
        exit(1);
    }
    input[0] = '\0';

    // Read the next character
    char next_ch;
    while (scanf("%c", &next_ch))
    {
        // If the program is exited by a newline and no other input
        if (next_ch == '\n' && strlen(input) == 0)
        {
            // Usually deallocate outside of this function, but not this time
            // since the program is exiting here.
            free(input);
            exit(0);
        }
        
        // If the input is completed by a newline and isn't empty
        else if (next_ch == '\n')
            break;
        
        // If there are more characters to be read
        else
        {
            // Reallocated size is strlen(input), plus a byte (for terminator),
            // plus another byte (for new char).
            int length = strlen(input) + 2;
            if (!(input = realloc(input, length)))
            {
                printf("[scand:realloc] Out of memory.\n");
                exit(1);
            }
            input[length - 2] = next_ch;
            input[length - 1] = '\0';
        }
    }

    return input;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pig.h"
#include "test_pig.h"

int main()
{
    test_pig();

    // Output for the first input
    printf("Enter a line of English:\n");

    while (1)
    {
        // // Temporary input buffer, emptied every 99 characters
        // char input[100];

        // A string storing all of the user's characters, with no limit on the input
        // size.
        //*total_input
        char *total_input = scand();

        // Allows progression of the while loop when input is provided.
        bool input_provided = (strlen(total_input) > 0);

        // Clears the buffer, allowing repeat inputs
        //fseek(stdin,  0, SEEK_END);

        if (strlen(total_input) > 0)
        {
            // Terminate total_input for the last time
            total_input[strlen(total_input)] = '\0';

            // Copy total_input into copied_input, for use in the print at the end
            // This is done since in case strtok manipulates the total_input pointer.
            // *copied_input
            char *copied_input;
            if (!(copied_input = malloc(strlen(total_input) + 1)))
            {
                printf("[malloc] Out of memory.\n");
                exit(1);
            }
            strcpy(copied_input, total_input);
            // Terminate copied_input to make it a valid string for str operations
            copied_input[strlen(total_input)] = '\0';

            // Output pointers
            // final_output - the output of the pig function applied to every word
            // in turn, delimited by " "
            // copied_output - a copy of final_output to be copied back into final_output
            // after final_output is resized to fit the delimited token as well
            
            // Need to malloc final_output, as an assigned string is required for
            // copied_output, but a literal assignment results in invalid realloc()ing.
            //*final_output
            char *final_output;
            if (!(final_output = malloc(1)))
            {
                printf("[malloc] Out of memory.\n");
                exit(1);
            }
            strcpy(final_output, "");

            char *copied_output;

            // Get the first token with " " delimiter
            char *token;
            token = strtok(total_input, " ");

            // The current length of the output, used for realloc of final_output
            int output_length = 0;
            while (token != NULL)
            {
                // Copy the token into copied_token for use in the pig function
                char *copied_token;
                //*copied_token
                if (!(copied_token = malloc(strlen(token) + 1)))
                {
                    printf("[malloc] Out of memory.\n");
                    exit(1);
                }
                strcpy(copied_token, token);
                // Terminate copied_token to make it a valid string for str operations
                copied_token[strlen(token)] = '\0';

                // pig_token - copied_token is converted into pig latin.
                //*pig_token = *word
                char *pig_token = pig(copied_token);

                // Copy final_output
                //*copied_output
                char *copied_output;
                if (!(copied_output = malloc(strlen(final_output) + 1)))
                {
                    printf("[malloc] Out of memory.\n");
                    exit(1);
                }
                strcpy(copied_output, final_output);
                // Terminate copied_output to make it a valid string for str operations
                copied_output[strlen(final_output)] = '\0';

                // Update size (accounting for delimiter " ")
                output_length += strlen(copied_output) + 1 + strlen(pig_token);

                // Concatenate delimiter, then pig_token
                if (!(final_output = realloc(final_output, output_length + 1)))
                {
                    printf("[realloc] Out of memory.\n");
                    exit(1);
                }
                strcpy(final_output, copied_output);
                strcat(final_output, " ");
                strcat(final_output, pig_token);

                // Terminate final_output
                final_output[output_length] = '\0';
                
                //~copied_token
                //~copied_output
                //~pig_token
                free(copied_token);
                free(copied_output);
                free(pig_token);

                // Separate input by " " delimiter
                token = strtok(NULL, " ");
            }

            // Print translation
            final_output[output_length] = '\0';
            printf("%s =>%s\n", copied_input, final_output);
            
			//~copied_input
            free(copied_input);
            //~final_output
            free(final_output);

            // Output for the next input
            printf("Enter a line of English:\n");
        }

        //~total_input
        free(total_input);
    }
}

// A function to use scanf dynamically - this means there is no limit on
// the input size.
char *scand()
{
    char *input;
    if (!(input = malloc(1)))
    {
        printf("[malloc] Out of memory.\n");
        exit(1);
    }
    input[0] = '\0';

    char next_ch;
    while (scanf("%c", &next_ch))
    {
        if (next_ch == '\n' && strlen(input) == 0)
        {
            free(input);
            exit(1);
        }
        else if (next_ch == '\n')
            break;
        else
        {
            // Realloc to strlen + 1 (for terminator) + 1 (for new char)
            // char *copied_input;
            // copied_input = safe_malloc(copied_input, strlen(input) + 1);
            // strcpy(copied_input, input);
            // copied_input[strlen(input)] = '\0';

            int length = strlen(input) + 2;
            if (!(input = realloc(input, length)))
            {
                printf("[realloc] Out of memory.\n");
                exit(1);
            }
            input[length - 2] = next_ch;
            input[length - 1] = '\0';
        }
    }

    return input;
}
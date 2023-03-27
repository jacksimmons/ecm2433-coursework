#include <stdio.h>
#include <stdlib.h>
#include "beggar.h"
#include "riffle.h"

int finished(Hand **players, int Nplayers)
{
    int num_nulls = 0;
    for (int i = 0; i < Nplayers; i++)
    {
        if (players[i] == NULL)
            num_nulls++;
    }
    
    if (num_nulls == Nplayers - 1)
        return 1;
    return 0;
}

int single(int Nplayers, int talkative)
{
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

		// Make the deck from 1-13, not 0-51
		int *deck_normalised = get_cards(deck);
		//~deck
		free(deck);

		riffle(deck_normalised, 52, sizeof(int), 8);

		return beggar(Nplayers, deck_normalised, talkative);
	}
	else
	{
		printf("Please enter an integer number of players between 1 and 52.\n");
		exit(1);
	}
}


int *get_cards(int *deck)
{
    int *cards;
    //*cards
    if (!(cards = malloc(sizeof(int) * 52)))
    {
        printf("Out of memory!");
        exit(1);
    }

    for (int i = 0; i < 52; i++)
    {
        cards[i] = get_card(deck[i]);
    }
    
    return cards;
}

// 0-12: Clubs
// 13-25: Diamonds
// 26-38: Hearts
// 39-51: Spades
int get_card(int number)
{ 
    if (number <= 12)
    {
        return number + 1;
    }
    else if (number <= 25)
    {
        return number - 12;
    }
    else if (number <= 38)
    {
        return number - 25;
    }
    else
    {
        return number - 38;
    }
}

int beggar(int Nplayers, int *deck, int talkative)
{
    // Get the remainder and quotient of the division 52 / Nplayers
    int hand_size_remainder = 52 % Nplayers;
    int hand_size_quotient = 52 / Nplayers;

    // Create the array of pointers to the player hands
    //*players
    Hand **players;
    if (!(players = malloc(sizeof(Hand *) * Nplayers)))
    {
        printf("Out of memory!");
        exit(1);
    }

    // Create the pile which players will put their cards onto
    //*pile
    Hand *pile;
    if (!(pile = malloc(sizeof(Hand))))
    {
        printf("Out of memory!");
        exit(1);
    }

    // Give the players their hands
    for (int i = 0; i < Nplayers; i++)
    {
        // Make the player
        Hand *player;
        if (!(player = malloc(sizeof(Hand))))
        {
            printf("Out of memory!");
            exit(1);
        }
        
        // Make new player->hand
        int *player_hand;
        if (!(player_hand = malloc(sizeof(int) * hand_size_quotient)))
        {
            printf("Out of memory!");
            exit(1);
        }

        // Fill the player hand
        for (int j = 0; j < hand_size_quotient; j++)
        {
            player_hand[j] = deck[(i * hand_size_quotient + j)];
        }

        // Set the struct values
        player->hand = player_hand;
        player->hand_size = hand_size_quotient;
        
        // Add the player to the players array
        players[i] = player;
    }

    // Initialise the pile, if there are remaining cards
    int pile_size = hand_size_remainder;
    if (!(pile->hand = malloc(sizeof(int) * pile_size)))
    {
        printf("Out of memory!");
        exit(1);
    }
    
    // Put the rest of the cards on the starting pile
    for (int i = 0; i < pile_size; i++)
    {
        pile->hand[i] = deck[51 - i];
    }
    pile->hand_size = pile_size;
    
    //~deck
    free(deck);
    
    // Turn counter
    int turn_counter = 1;
    
    // Turn 0 (print out the starting hands and pile)
    if (talkative)
    {
        printf("Turn 0\n");
        print_hand("Pile", pile);
        char name[6];
        for (unsigned short i = 0; i < Nplayers; i++)
        {
            sprintf(name, "%i", i);
            print_hand(name, players[i]);
        }
        printf("\n");
    }
    
    while (1)
    {
        for (int i = 0; i < Nplayers; i++)
        {                                    
            if (players[i] != NULL)
            {
                // Output turn number
                if (talkative)
                    printf("Turn %i [Player %i]\n", turn_counter, i);
                turn_counter++;
                
                if (talkative)
                {
                    if (pile->hand_size == 0)
                    {
                        printf("\tPile is empty, so Player %i to put one card on the pile.\n", i);
                    
                        int insufficient_cards = 0;
                        if (players[i]->hand_size == 0)
                            insufficient_cards = 1;
                            
                        if (insufficient_cards == 0)
                            printf("\tPlayer %i puts a %i on the pile.\n", i, players[i]->hand[0]);
                        else
                            printf("\tPlayer %i attempts to put another card on the pile, but has no more.\n", i);
                    }
                    else
                    {
                        int penalty = get_penalty(pile->hand[pile->hand_size-1]);
                        if (penalty == 0)
                        {
                            printf("\tTop card in pile is %i, so Player %i to put one card on the pile.\n",
                            pile->hand[0], i);
                            
                            int insufficient_cards = 0;
                            if (players[i]->hand_size == 0)
                                insufficient_cards = 1;
                            
                            if (insufficient_cards == 0)
                                printf("\tPlayer %i puts a %i on the pile.\n", i, players[i]->hand[0]);
                            else
                                printf("\tPlayer %i attempts to put another card on the pile, but has no more.\n", i);
                        }
                        else
                        {
                            printf("\tTop card in pile is %i [PENALTY], so Player %i to put %i card(s) on the pile.\n",
                            pile->hand[pile->hand_size-1], i, penalty);
                            
                            int iteration_depth = penalty;
                            int insufficient_cards = 0;
                            if (penalty > players[i]->hand_size)
                            {
                                iteration_depth = players[i]->hand_size;
                                insufficient_cards = 1;
                            }
                            for (int j = 0; j < iteration_depth; j++)
                            {
                                if (get_penalty(players[i]->hand[j]) == 0)
                                    printf("\tPlayer %i puts a %i on the pile.\n", i, players[i]->hand[j]);
                                else
                                {
                                    printf("\tPlayer %i puts a %i [PENALTY] on the pile, passing the penalty on to Player %i.\n",
                                    i, players[i]->hand[j], get_next_player(players, i, Nplayers));
                                    break;
                                }  
                            }
                            
                            if (insufficient_cards == 1)
                                printf("\tPlayer %i has no more cards to pay the rest of their penalty.\n", i);
                        }
                    }
                }
                
                Hand *reward = take_turn(players[i], pile);
                
                // If there is a reward to be claimed
                if (reward != NULL)
                {                    
                    // Let the penaliser pick up the whole pile.
                    // Get the 2nd previous player by using get_previous_player
                    // as an argument to get_previous_player
                    int penaliser_index = 
                        get_previous_player(players, i, Nplayers);

                    // Need to store pile->hand_size elsewhere as we can't iterate
                    // over a changing value.
                    int pile_size = pile->hand_size;
                    for (int i = 0; i < pile_size; i++)
                    {
                        int taken = take_card_from_hand(pile);
                        put_card_on_hand(players[penaliser_index], taken);
                    }
                    
                    if (talkative)
                        printf("\tPlayer %i picks up the whole pile!\n", penaliser_index);
                }
                
                if (players[i]->hand_size == 0)
                {
                    if (talkative)
                        printf("\tPlayer %i was eliminated!\n", i);
                    free(players[i]->hand);
                    free(players[i]);
                    players[i] = NULL;
                }
            }
            else
            {
                // Don't count the turn of an eliminated player
                continue;
            }
            
            // Output the hands after every turn
            if (talkative)
            {            
                print_hand("Pile", pile);
                char name[6];
                for (unsigned short i = 0; i < Nplayers; i++)
                {
                    sprintf(name, "%i", i);
                    print_hand(name, players[i]);
                }
                printf("\n");
            }
                                
            if (finished(players, Nplayers) == 1)
            {
                int winner = -1;
                for (int i = 0; i < Nplayers; i++)
                {
                    if (players[i] != NULL)
                        winner = i;
                }
                if (talkative)
                    printf("The game is over; Player %i wins!\n", winner);
                
                //~pile
                free(pile->hand);
                free(pile);
                //~players
                free(players[winner]->hand);
                free(players[winner]);
                free(players);

                return turn_counter;
            }
        }
    }
}

int get_penalty(int card)
{
    int additional_cards = 0;

    switch (card)
    {
        case JACK:
            additional_cards = 1;
            break;
        case QUEEN:
            additional_cards = 2;
            break;
        case KING:
            additional_cards = 3;
            break;
        case ACE:
            additional_cards = 4;
            break;
        default:
            break;
    }

    return additional_cards;
}

int get_previous_player(Hand **players, int current_player, int Nplayers)
{
    if (current_player == -1)
        return -1;
    
    int prev_player = -1;
    int explored_player = -1;
    if (current_player == 0)
        explored_player = Nplayers - 1;
    else
        explored_player = current_player - 1;
    do
    {
        if (players[explored_player] == NULL)
        {
            if (explored_player == 0)
                explored_player = Nplayers - 1;
            else
                explored_player--;
        }
        else
        {
            prev_player = explored_player;
            break;
        }
    } while (explored_player != current_player);
    return prev_player;
}

int get_next_player(Hand **players, int current_player, int Nplayers)
{
    if (current_player == -1)
        return -1;
    
    int next_player = -1;
    int explored_player = -1;
    if (current_player == Nplayers - 1)
        explored_player = 0;
    else
        explored_player = current_player + 1;
    do
    {
        if (players[explored_player] == NULL)
        {
            if (explored_player == Nplayers - 1)
                explored_player = 0;
            else
                explored_player++;
        }
        else
        {
            next_player = explored_player;
            break;
        }
    } while (explored_player != current_player);
    return next_player;
}

Hand *take_turn(Hand *player, Hand *pile)
{
    int additional_cards = 0;
    
    // If the pile isn't empty, check the last card for a penalty
    if (pile->hand_size > 0)
        additional_cards = get_penalty(pile->hand[(pile->hand_size)-1]);
    
    // Non-penalised move
    if (additional_cards == 0)
    {        
        int taken = take_card_from_hand(player);
        if (taken != -1)
        {
            put_card_on_hand(pile, taken);
            //if (talkative)
            //    printf("\tPlayer %i puts a %i on the pile.\n", current_player_index, taken);
        }
        else
        {
            //if (talkative)
            //{
            //    printf("\tPlayer %i attempts to put a card on the pile, but has none left!\n",
            //           current_player_index);
            //}
        }
        
        return NULL;
    }
    return take_penalty(player, pile, additional_cards);
}

Hand *take_penalty(Hand *player, Hand *pile, int additional_cards)
{
    int talkative = 0;
    // additional_cards != 0 also means pile size > 0.
    // No need to check if pile->hand_size > 0 here.
    //if (talkative)
    //{
    //    printf("\tTop card in pile is %i [PENALTY], so Player %i to put %i card(s) on the pile.\n",
    //           pile->hand[pile->hand_size-1], current_player_index, additional_cards);
    //}

    int penalty_passed_on = 0;
    for (int i = 0; i < additional_cards; i++)
    {
        if (player->hand_size > 0)
        {
            int taken = take_card_from_hand(player);
            if (taken != -1)
            {
                put_card_on_hand(pile, taken);
                if (get_penalty(taken) != 0)
                {
                    // The index of the player who the penalty was passed on to
                    penalty_passed_on = 1;
                    //if (talkative)
                    //    printf("\tPlayer %i puts a %i [PENALTY] on the pile, therefore passing the penalty to Player %i.\n",
                    //           current_player_index, taken, passed_on_to);
                    break;
                }
                else
                {
                    //if (talkative)
                    //    printf("\tPlayer %i puts a %i on the pile.\n", current_player_index, taken);
                }
            }
            else
            {
                //printf("\tPlayer %i has no more cards, so cannot pay the penalty.\n", current_player_index);
            }
        }
        else
        {
            //printf("\tPlayer %i has no cards, so cannot pay the penalty.\n", current_player_index);
        }
    }

    if (penalty_passed_on == 0)
    {
        //int penaliser_index = get_previous_player(players, current_player_index, Nplayers);
        //printf("\tPlayer %i didn't play a penalty card during their penalty, so the whole pile goes to Player %i.\n",
        //       current_player_index, penaliser_index); 
        return pile;
    }
    return NULL;
}

// Takes the card from the front of the player
int take_card_from_hand(Hand *hand)
{
    if (hand->hand_size > 0)
    {
        int taken = hand->hand[0];

        // Move every element back one, removing the first
        for (int i = 1; i < hand->hand_size; i++)
        {
            hand->hand[i-1] = hand->hand[i];
        }

        hand->hand_size--;

        // Ensure >0 bytes are allocated
        int mem_size = hand->hand_size * sizeof(int);
        if (mem_size == 0)
            mem_size = 1;
        
        // Reallocate, taking one off the hand
        if (!(hand->hand = realloc(hand->hand, mem_size)))
        {
            printf("Out of memory!");
            exit(1);
        }

        return taken; 
    }

    // Return -1 for if the hand is empty
    return -1;
}

void put_card_on_hand(Hand *hand, int card)
{
    hand->hand_size++;

    // Reallocate, adding one extra int to the hand
    if (!(hand->hand = realloc(hand->hand, sizeof(int) * hand->hand_size)))
    {
        printf("Out of memory!");
        exit(1);
    }
    hand->hand[(hand->hand_size) - 1] = card;
}

void print_hand(char *name, Hand *hand)
{
    if (hand == NULL)
        printf("%s: [ELIMINATED]\n", name);
    else
    {
        if (hand->hand_size > 0)
        {
            printf("%s: ", name);
            for (int i = 0; i < hand->hand_size - 1; i++)
                printf("%i, ", hand->hand[i]);
            printf("%i\n", hand->hand[hand->hand_size - 1]);
        }
        else
            printf("%s: [EMPTY]\n", name);
    }
}

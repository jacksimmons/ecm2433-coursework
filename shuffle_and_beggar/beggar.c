#include <stdio.h>
#include <stdlib.h>
#include "beggar.h"
#include "riffle.h"

// Outputs whether the game is finished (1) or not (0)
int finished(Hand **players, int Nplayers)
{
    // Iterate over every player, checking if they are NULL
    // If every player bar one is NULL, the game is over.
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

// Handles a single game of beggar your neighbour.
int single(int Nplayers, int talkative)
{
    // Ensure Nplayers is valid
	if (Nplayers > 0 && Nplayers <= 52)
	{	
		int *deck;

        // Allocate memory for the deck of cards
		//#deck
		if (!(deck = malloc(sizeof(int) * 52)))
		{
			printf("Out of memory!");
			exit(1);
		}

        // Populate the deck
		for (int i = 0; i < 52; i++)
			deck[i] = i;

		// Make the deck from 1-13, not 0-51
		int *deck_normalised = get_cards(deck);
		//~deck
		free(deck);

        // Perform a riffle on the normalised deck
		riffle(deck_normalised, 52, sizeof(int), 8);

        // Handle beggar your neighbour on this deck
		return beggar(Nplayers, deck_normalised, talkative);
	}
	else
	{
		printf("Please enter an integer number of players between 1 and 52.\n");
		exit(1);
	}
}

// Normalises every card in the deck.
int *get_cards(int *deck)
{
    int *cards;
    
    // Allocate memory for the normalised deck
    //#cards
    if (!(cards = malloc(sizeof(int) * 52)))
    {
        printf("Out of memory!");
        exit(1);
    }

    // Normalise each card
    for (int i = 0; i < 52; i++)
    {
        cards[i] = get_card(deck[i]);
    }
    
    return cards;
}

// Normalises a single card.
// This means 0-12 => 1-13
// 13-25 => 1-13
// 26-38 => 1-13
// 39-51 => 1-13
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

// Carries out the beggar your neighbour game.
int beggar(int Nplayers, int *deck, int talkative)
{
    // Get the remainder and quotient of the division 52 / Nplayers
    int hand_size_remainder = 52 % Nplayers;
    int hand_size_quotient = 52 / Nplayers;

    // Create the array of pointers to the player hands
    //#players
    Hand **players;
    if (!(players = malloc(sizeof(Hand *) * Nplayers)))
    {
        printf("[players:malloc] Out of memory!");
        exit(1);
    }

    // Create the pile which players will put their cards onto
    //#pile
    Hand *pile;
    if (!(pile = malloc(sizeof(Hand))))
    {
        printf("[pile:malloc] Out of memory!");
        exit(1);
    }

    // Give the players their hands
    for (int i = 0; i < Nplayers; i++)
    {
        // Make the player
        Hand *player;
        if (!(player = malloc(sizeof(Hand))))
        {
            printf("[player:malloc] Out of memory!");
            exit(1);
        }
        
        // Make new player->hand
        int *player_hand;
        if (!(player_hand = malloc(sizeof(int) * hand_size_quotient)))
        {
            printf("[player_hand:malloc] Out of memory!");
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
        printf("[pile->hand:malloc] Out of memory!");
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
    
    int game_over = 0;
    while (!game_over)
    {
        // Perform the turns of every player in order
        for (int i = 0; i < Nplayers; i++)
        {          
            // Only perform turn if player is in the game
            if (players[i] != NULL)
            {
                // Output turn number
                if (talkative)
                    printf("Turn %i [Player %i]\n", turn_counter, i);
                turn_counter++;
                
                // Predictive output for the next turn
                if (talkative)
                {
                    if (pile->hand_size == 0)
                    {
                        printf("\tPile is empty, so Player %i to put one card on the pile.\n", i);
                    
                        // Distinguish input between having enough and not having enough cards here
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
                        // Get penalty of the last card
                        int penalty = get_penalty(pile->hand[pile->hand_size-1]);
                        
                        // No penalty
                        if (penalty == 0)
                        {
                            printf("\tTop card in pile is %i, so Player %i to put one card on the pile.\n",
                            pile->hand[0], i);
                            
                            // Distinguish between having enough cards and not
                            int insufficient_cards = 0;
                            if (players[i]->hand_size == 0)
                                insufficient_cards = 1;
                            
                            if (insufficient_cards == 0)
                                printf("\tPlayer %i puts a %i on the pile.\n", i, players[i]->hand[0]);
                            else
                                printf("\tPlayer %i attempts to put another card on the pile, but has no more.\n", i);
                        }
                        
                        // A penalty
                        else
                        {
                            printf("\tTop card in pile is %i [PENALTY], so Player %i to put %i card(s) on the pile.\n",
                            pile->hand[pile->hand_size-1], i, penalty);
                            
                            // See which iteration depth to use (penalty, or player->hand_size)
                            int iteration_depth = penalty;
                            int insufficient_cards = 0;
                            if (penalty > players[i]->hand_size)
                            {
                                iteration_depth = players[i]->hand_size;
                                insufficient_cards = 1;
                            }
                            
                            int next_player = get_next_player(players, i, Nplayers);
                            
                            // As `next_player` can output -1, meaning the game is over, we need to check the game
                            // hasn't ended.
                            if (next_player != -1)
                            {
                                // Iterate over every card in the depth, and check if it's a penalty
                                // If it is, display this and break.
                                for (int j = 0; j < iteration_depth; j++)
                                {
                                    if (get_penalty(players[i]->hand[j]) == 0)
                                        printf("\tPlayer %i puts a %i on the pile.\n", i, players[i]->hand[j]);
                                    else
                                    {
                                        printf("\tPlayer %i puts a %i [PENALTY] on the pile, passing the penalty on to Player %i.\n",
                                        i, players[i]->hand[j], next_player);
                                        break;
                                    }  
                                }

                                // Distinguish between having enough and not having enough cards
                                if (insufficient_cards == 1)
                                    printf("\tPlayer %i has no more cards to pay the rest of their penalty.\n", i);
                            }
                        
                            else
                            {
                                printf("An unexpected end-of-game occurred.\n");
                                exit(1);
                            }
                        }
                    }
                }
                
                // Take the turn, and handle the reward if there is one
                Hand *reward = take_turn(players[i], pile);
                
                // If there is a reward to be claimed
                if (reward != NULL)
                {                    
                    // Let the penaliser pick up the whole pile.
                    // Get the 2nd previous player by using get_previous_player
                    // as an argument to get_previous_player
                    int penaliser_index = 
                        get_previous_player(players, i, Nplayers);

                    // Need to check there is a previous player; if not the game is over
                    if (penaliser_index != -1)
                    {
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
                    else
                    {
                        printf("An unexpected end-of-game occurred.\n");
                        exit(1);
                    }
                }
                
                // If the player has no more cards
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
            
            // Check if the game is over
            if (finished(players, Nplayers) == 1)
            {
                // If the game is over, there must be a winner
                int winner = -1;
                for (int i = 0; i < Nplayers; i++)
                {
                    if (players[i] != NULL)
                        winner = i;
                }
                
                if (winner != -1)
                {                
                    if (talkative)
                        printf("The game is over; Player %i wins!\n", winner);
                    
                    free(players[winner]->hand);
                    free(players[winner]);
                    game_over = 1;
                    break;
                }
                else
                {
                    printf("The game has ended without a winner.\n");
                    exit(1);
                }
            }
        }
    }
    
    //~pile
    free(pile->hand);
    free(pile);
    //~players
    free(players);

    return turn_counter;
}

// Get the number of cards needing to be paid as penalty due to `card`
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

// Get the player before the current player in the `players` array
// If the previous player is NULL, keep checking for the previous player,
// until you reach the current player, meaning the current player has won.
int get_previous_player(Hand **players, int current_player, int Nplayers)
{
    if (current_player == -1)
    {
        printf("NULL player provided to get_previous_player");
        exit(1);
    }
    
    int prev_player = -1;
    int explored_player = -1;
    
    // Set initial `explored_player` to 1 before `current_player` (wrapping
    // around if necessary)
    if (current_player == 0)
        explored_player = Nplayers - 1;
    else
        explored_player = current_player - 1;
    
    // Until the `explored_player` matches `current_player` or the previous player
    // has been found, keep going 'back',
    // wrapping around to the end of the players array after going past the 0th
    // player.
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

// Get the player after the current player in the `players` array
// If the next player is NULL, keep checking for the next player,
// until you reach the current player, meaning the current player has won.
int get_next_player(Hand **players, int current_player, int Nplayers)
{
    if (current_player == -1)
    {
        printf("NULL player provided to get_next_player");
        exit(1);
    }
    
    int next_player = -1;
    int explored_player = -1;
    // Set initial `explored_player` to 1 after current_player, wrapping around
    // if necessary.
    if (current_player == Nplayers - 1)
        explored_player = 0;
    else
        explored_player = current_player + 1;

    // Until the `explored_player` matches `current_player`, or the next player has
    // been found, keep going 'forward',
    // wrapping around to the start of the players array after going past the (n-1)th
    // player.
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

// Handles a single turn of the beggar game.
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
        }
        
        return NULL;
    }
    // Otherwise, penalised move
    return take_penalty(player, pile, additional_cards);
}

// Handles a penalised move (the previous card was a penalty).
Hand *take_penalty(Hand *player, Hand *pile, int additional_cards)
{
    int talkative = 0;
    
    // additional_cards != 0 also means pile size > 0.
    // No need to check if pile->hand_size > 0 here.

    int penalty_passed_on = 0;
    // Iterate over the number of cards needing to be added, add that
    // number of cards. If any are penalties, pass the penalty on to
    // the next player.
    for (int i = 0; i < additional_cards; i++)
    {
        if (player->hand_size > 0)
        {
            int taken = take_card_from_hand(player);
            // If a card was taken, put it on the pile
            if (taken != -1)
            {
                put_card_on_hand(pile, taken);
                // If the card was a penalty, indicate this and break
                if (get_penalty(taken) != 0)
                {
                    penalty_passed_on = 1;
                    break;
                }
            }
        }
    }

    // If the penalty wasn't passed on, the reward is pile
    if (penalty_passed_on == 0)
    {
        return pile;
    }
    // Otherwise, there is no reward
    return NULL;
}

// Takes the card from the front of the hand
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
            printf("[take_card_from_hand:realloc] Out of memory!");
            exit(1);
        }

        return taken; 
    }

    // Return -1 for if the hand is empty
    return -1;
}

// Puts a card on the back of the hand
void put_card_on_hand(Hand *hand, int card)
{
    hand->hand_size++;

    // Reallocate, adding one extra int to the hand
    if (!(hand->hand = realloc(hand->hand, sizeof(int) * hand->hand_size)))
    {
        printf("[put_card_on_hand:realloc] Out of memory!");
        exit(1);
    }
    hand->hand[(hand->hand_size) - 1] = card;
}

// Outputs a hand to console neatly
void print_hand(char *name, Hand *hand)
{
    if (hand == NULL)
        printf("%s: [ELIMINATED]\n", name);
    else
    {
        // Print the hand if it isn't empty
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

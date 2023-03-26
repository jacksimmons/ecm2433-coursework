#include <stdio.h>
#include <stdlib.h>
#include "beggar.h"

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

int beggar(int Nplayers, int *deck, int talkative)
{
    // Make the deck from 1-13, not 0-51
    int *deck_normalised = get_cards(deck);
    free(deck);

    int hand_size_remainder = 52 % Nplayers;
    int hand_size_quotient = 52 / Nplayers;

    //*players
    Hand **players;
    if (!(players = malloc(sizeof(Hand *) * Nplayers)))
    {
        printf("Out of memory!");
        exit(1);
    }

    //*pile
    Hand *pile;
    if (!(pile = malloc(sizeof(Hand))))
    {
        printf("Out of memory!");
        exit(1);
    }

    // Give Players their hands
    for (int i = 0; i < Nplayers; i++)
    {
        // Make new Hand
        Hand *player;
        if (!(player = malloc(sizeof(Hand))))
        {
            printf("Out of memory!");
            exit(1);
        }
        
        int *player_hand;
        if (!(player_hand = malloc(sizeof(int) * hand_size_quotient)))
        {
            printf("Out of memory!");
            exit(1);
        }

        for (int j = 0; j < hand_size_quotient; j++)
        {
            player_hand[j] = deck_normalised[(i * hand_size_quotient + j)];
        }

        player->hand = player_hand;
        player->hand_size = hand_size_quotient;
        
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
        pile->hand[i] = deck_normalised[51 - i];
    }
    pile->hand_size = pile_size;
    
    //~deck_normalised
    free(deck_normalised);
    
    // Turn counter
    int turn_counter = 1;
    
    // Turn 0
    printf("Turn 0\n");
    print_hand("Pile", pile);
    char name[6];
    for (unsigned short i = 0; i < Nplayers; i++)
    {
        sprintf(name, "%i", i);
        print_hand(name, players[i]);
    }
    printf("\n");
    
    int cnt = 0;
    while (1)
    {
        cnt++;
        for (int i = 0; i < Nplayers; i++)
        {                                    
            if (players[i] != NULL)
            {
                // Output turn number
                printf("Turn %i [Player %i]\n", turn_counter, i);
                turn_counter++;
                
                take_turn(players, i, Nplayers, pile, talkative);
                
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
            
            // Up to 65,536 players
            print_hand("Pile", pile);
            char name[6];
            for (unsigned short i = 0; i < Nplayers; i++)
            {
                sprintf(name, "%i", i);
                print_hand(name, players[i]);
            }
            printf("\n");
                                
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

                return 1;
            }
        }
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

void take_turn(Hand **players, int current_player_index, int Nplayers, Hand *pile, int talkative)
{
    Hand *player = players[current_player_index];
    int additional_cards = 0;
    
    // If the pile isn't empty, check the last card for penalty
    if (pile->hand_size > 0)
    {
        additional_cards = get_penalty(pile->hand[(pile->hand_size)-1]);    
    }
        
    // Non-penalalised move
    if (additional_cards == 0)
    {
                // Check if (up to 5 cards ago) there was a penalty.
        // If there was, give all the cards after it to the penaliser.
        // Don't need to search further back for stacked penalties,
        // as the last penalty is the one that gets the reward.
        int penalty_found = 0;
        for (int i = 1; i <= 5; i++)
        {
            // Check we are iterating over the allocated pile
            if (pile->hand_size - i >= 0)
            {
                if (get_penalty(pile->hand[pile->hand_size - i]) != 0)
                {
                    penalty_found = 1;
                }
            }
        }
        
        // If there has been an unhandled penalty
        if (penalty_found == 1)
        {
            // Let the penaliser pick up the whole pile.
            // Get the 2nd previous player by using get_previous_player
            // as an argument to get_previous_player
            int penaliser_index = 
                get_previous_player(players, get_previous_player(players, current_player_index, Nplayers), Nplayers);
            
            for (int i = 0; i < pile->hand_size; i++)
            {
                int taken = take_card_from_hand(pile);
                put_card_on_hand(players[penaliser_index], taken);
            }
        }
        
        // Take from start
        if (talkative)
        {
            if (pile->hand_size == 0)
                printf("\tPile is empty, so Player %i to put one card on the pile.\n",
                       current_player_index);
            else
                printf("\tTop card in pile is %i, so Player %i to put one card on the pile.\n",
                   pile->hand[pile->hand_size-1], current_player_index);
        }
            
        int taken = take_card_from_hand(players[current_player_index]);
        if (taken != -1)
        {
            put_card_on_hand(pile, taken);
            if (talkative)
                printf("\tPlayer %i puts a %i on the pile.\n", current_player_index, taken);
        }
        else
        {
            if (talkative)
            {
                printf("\tPlayer %i attempts to put a card on the pile, but has none left!\n",
                       current_player_index);
            }
        }
    }

    // Current card is a penalty
    else
    {
        // additional_cards != 0 also means pile size > 0.
        // No need to check if pile->hand_size > 0 here.
        if (talkative)
        {
            printf("\tTop card in pile is %i [PENALTY], so Player %i to put %i cards on the pile.\n",
                   pile->hand[pile->hand_size-1], current_player_index, additional_cards);
        }
        
        int penalty_passed_on = 0;
        for (int i = 0; i < additional_cards; i++)
        {
            if (players[current_player_index]->hand_size > 0)
            {
                int taken = take_card_from_hand(players[current_player_index]);
                if (taken != -1)
                {
                    put_card_on_hand(pile, taken);
                    if (get_penalty(taken) != 0)
                    {
                        // The index of the player who the penalty was passed on to
                        penalty_passed_on = 1;
                        int passed_on_to = get_next_player(players, current_player_index, Nplayers);
                        if (talkative)
                            printf("\tPlayer %i puts a %i [PENALTY] on the pile, therefore passing the penalty to Player %i.\n",
                                   current_player_index, taken, passed_on_to);
                        break;
                    }
                    else
                    {
                        if (talkative)
                            printf("\tPlayer %i puts a %i on the pile.\n", current_player_index, taken);
                    }
                }
                else
                {
                    printf("\tPlayer %i has no more cards, so cannot pay the penalty.\n", current_player_index);
                }
            }
            else
            {
                printf("\tPlayer %i has no cards, so cannot pay the penalty.\n", current_player_index);
            }
        }
        
        if (penalty_passed_on == 0)
        {
            int penaliser_index = get_previous_player(players, current_player_index, Nplayers);
            printf("\tPlayer %i didn't play a penalty card during their penalty, so the whole pile goes to Player %i.\n",
                   current_player_index, penaliser_index); 
        }
    }
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
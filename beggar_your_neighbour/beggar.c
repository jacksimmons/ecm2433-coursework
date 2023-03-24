#include <stdio.h>
#include <stdlib.h>
#include "beggar.h"

const int JACK = 11;
const int QUEEN = 12;
const int KING = 13;
const int ACE = 1;

int beggar(int Nplayers, int *deck, int talkative)
{
    // Make the deck from 1-13, not 0-51
    deck = get_cards(deck);

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
        
        //*player_hand
        int *player_hand;
        if (!(player_hand = malloc(sizeof(int) * hand_size_quotient)))
        {
            printf("Out of memory!");
            exit(1);
        }

        for (int j = 0; j < hand_size_quotient; j++)
        {
            player_hand[j] = deck[(i * hand_size_quotient + j)];
        }

        player->hand = player_hand;
        player->hand_size = hand_size_quotient;

        //~player_hand
        free(player_hand);
    }

    // Put the rest of the cards on the starting pile
    for (int i = 0; i < hand_size_remainder; i++)
    {
        pile->hand[i] = deck[51 - i];
    }
    pile->hand_size = hand_size_remainder;

    int finished = 0;
    while (!finished)
    {
        for (int i = 0; i < Nplayers; i++)
        {
            pile->hand_size = take_turn(players, i, Nplayers, pile);
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

int take_turn(Hand **players, int current_player_index, int Nplayers, Hand *pile)
{
    Hand *player = players[current_player_index];
    int additional_cards = get_penalty(pile->hand[pile->hand_size-1]);

    // Place card face down
    if (additional_cards == 0)
    {
        // Take from start
        int taken = take_card_from_hand(players[current_player_index]);
        put_card_on_hand(pile, taken);
    }

    // Last card was a penalty
    else
    {
        int previous_player_index;
        if (current_player_index == 0)
        {
            previous_player_index = Nplayers - 1;
        }
        else
        {
            previous_player_index = current_player_index - 1;
        }

        
        for (int i = 0; i < additional_cards; i++)
        {
            if (players[0]->hand_size > 0)
            {
                int taken = take_card_from_hand(players[current_player_index]);
                if (get_penalty(taken) == 0)
                    put_card_on_hand(pile, taken);
                else
                    
            }
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

        // Reallocate, taking one off the hand
        if (!(hand->hand = realloc(hand->hand, sizeof(int) * hand->hand_size)))
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

    // Reallocate, adding one int to the hand
    if (!(hand = realloc(hand, sizeof(int) * hand->hand_size)))
    {
        printf("Out of memory!");
        exit(1);
    }

    hand->hand[hand->hand_size] = card;
}

int finished(Hand *players, int Nplayers)
{
    int winner = -1;
    for (int i = 0; i < Nplayers; i++)
    {
        if (players[i].hand_size > 0)
        {
            // There are multiple players with hand_size > 0
            if (winner != -1)
                return 0;
            
            // We have found a potential winner
            winner = i;
        }
    }

    // First ensure players[-1] is not performed.
    if (winner == -1)
        return 0;
    
    // Now double check that the only player with cards
    // has all 52 of them.
    if (players[winner].hand_size == 52)
        return 1;
    return 0;
}
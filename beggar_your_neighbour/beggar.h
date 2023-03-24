struct HAND {
    int *hand;
    int hand_size;
};
typedef struct HAND Hand;

int beggar(int Nplayers, int *deck, int talkative);
int *get_cards(int *deck);
int get_card(int number);
int get_penalty(int card);
int take_turn(Hand *players, int current_player_index, int Nplayers, Hand *pile)
int take_card_from_hand(Hand *hand);
void put_card_on_hand(Hand *hand, int card);
int finished(Hand *players, int Nplayers);
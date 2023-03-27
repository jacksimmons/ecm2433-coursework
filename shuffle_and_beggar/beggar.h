#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE 1

typedef struct HAND {
    int *hand;
    int hand_size;
} Hand;

int *get_cards(int *deck);
int get_card(int number);
int finished(Hand **players, int Nplayers);
int single(int Nplayers, int talkative);
int beggar(int Nplayers, int *deck, int talkative);
int get_penalty(int card);
int get_previous_player(Hand **players, int current_player_index, int Nplayers);
int get_next_player(Hand **players, int current_player_index, int Nplayers);
Hand *take_turn(Hand *player, Hand *pile);
Hand *take_penalty(Hand *player, Hand *pile, int additional_cards);
int take_card_from_hand(Hand *hand);
void put_card_on_hand(Hand *hand, int card);
void print_hand(char *name, Hand *hand);
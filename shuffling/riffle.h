void riffle_once(void *L, int len, int size, void *work);
void riffle(void *L, int len, int size, int N);
int check_shuffle(void *L, int len, int size, int (*cmp)(void *, void *));
int cmp_int(void *int_a, void *int_b);
int cmp_str(void *str_a, void *str_b);
float quality(int *numbers, int len);
float average_quality(int N, int shuffles, int trials);
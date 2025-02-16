#define MAX_KEY_SIZE (32)

void my_str_cpy(char *, char *); // strncpy impl using 32 as MAX_KEY_SIZE
int my_str_len(char *);
int my_str_cmp_opt(char *, char *); // optimized version using long comparisons instead of byte comparisons
char * my_str_dup(char *);
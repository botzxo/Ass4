#ifndef _MULTIMAP
#define _MULTIMAP

#define MAX_VALUE_LENGTH 81
#define MAX_KEY_LENGTH 31


typedef struct VALUE {
    int num;
    char str[MAX_VALUE_LENGTH];
}Value;

typedef struct NODE {
    Value value;
    struct NODE* next;
}Node;

typedef struct KEYS {
    char key[MAX_KEY_LENGTH];
    Node* values;
}Key;

typedef struct MULTIMAP {
    int max_keys;
    int size;
    Key* keys;
}Multimap;

// typedef struct KEYS Key;
// typedef struct MULTIMAP Multimap;

int validate_mm(Multimap *mm);
Multimap* mm_create(int max_keys);
int mm_insert_value(Multimap* mm, char* key, int value_num, char* value_str);
int mm_count_keys(Multimap* mm);
int mm_count_values(Multimap* mm, char* key);
int mm_get_values(Multimap* mm, char* key, Value values[], int max_values);
int mm_remove_key(Multimap* mm, char* key);
void mm_print(Multimap* mm); 
int mm_destroy(Multimap* mm);
int mm_get_first_key(Multimap *mm, char *key, int length);
int mm_get_next_key(Multimap *mm, char *key, int length);


#endif
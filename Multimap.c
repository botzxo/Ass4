#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "multimap.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif



int getkey_count = 0;


// struct NODE {
//     Value value;
//     struct NODE* next;
// };

// struct KEYS {
//     char key[MAX_KEY_LENGTH];
//     Node* values;
// };



int validate_mm(Multimap *mm){
    assert(mm->size>=0);
    assert(mm->max_keys>=0);
    assert(mm->size<=mm->max_keys);
    for(int i=0; i<mm->size; i++){
        assert(strlen(mm->keys[i].key)<MAX_KEY_LENGTH);
        Key *current_key = mm->keys;
        Node *current_value  = current_key[i].values;
        while(current_value!=NULL){
            assert(strlen(current_value->value.str)<MAX_VALUE_LENGTH);
            current_value = current_value->next;
        }
    }
    return 1;
}



Multimap* mm_create(int max_keys) {
    if (max_keys < 0) {
        return NULL;
    }

    Multimap* mm = (Multimap*)malloc(sizeof(Multimap));
    if (mm == NULL) {
        return NULL;
    }

    mm->max_keys = max_keys;
    mm->size = 0;
    mm->keys = (Key*)malloc(max_keys * sizeof(Key));

    if (mm->keys == NULL) {
        free(mm);
        return NULL;
    }

    for (int i = 0; i < max_keys; i++) {
        mm->keys[i].values = NULL;
    }
    assert(validate_mm(mm));

    return mm;
}

int compareKeys(const void* a, const void* b) {
    return strcasecmp(((Key*)a)->key, ((Key*)b)->key);
}

int mm_insert_value(Multimap* mm, char* key, int value_num, char* value_str) {
    assert(validate_mm(mm));
    if (mm == NULL || key == NULL) {
        return -1;
    }
    if (mm->size > mm->max_keys) {
        return -1;
    }

    if(strlen(key)>=MAX_KEY_LENGTH){
        return-1;
    }

    if(strlen(value_str)>=MAX_VALUE_LENGTH){
        return -1;
    }

    Key* key_value = mm->keys;
    Key key_to_search;
    strncpy(key_to_search.key, key, MAX_KEY_LENGTH);
    Key* result = (Key*)bsearch(&key_to_search, key_value, mm->size, sizeof(Key), compareKeys);

    if (result == NULL) {
        if(mm->size >= mm->max_keys){
            return -1;
        }
        // Ensure we null-terminate the key string
        strncpy(key_value[mm->size].key, key, MAX_KEY_LENGTH - 1);
        key_value[mm->size].key[MAX_KEY_LENGTH - 1] = '\0';
        mm->size++;

        qsort(key_value, mm->size, sizeof(Key), compareKeys);

        // Find the position of the inserted key after sorting
        result = (Key*)bsearch(&key_to_search, key_value, mm->size, sizeof(Key), compareKeys);
        if (result == NULL) {
            return -1; // This should not happen if sorting went well
        }
    }

    // Create a new value node and add it to the linked list for the key
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        return -1; // Memory allocation failed
    }

    new_node->value.num = value_num;
    strncpy(new_node->value.str, value_str, MAX_VALUE_LENGTH - 1);
    new_node->value.str[MAX_VALUE_LENGTH - 1] = '\0';
    new_node->next = NULL;

    Node* current = result->values;
    Node* previous = NULL;

    while (current != NULL && current->value.num < value_num) {
        previous = current;
        current = current->next;
    }

    if (previous == NULL) {
        new_node->next = result->values;
        result->values = new_node;
    } else {
        new_node->next = previous->next;
        previous->next = new_node;
    }
    
    assert(validate_mm(mm));
    return mm_count_values(mm, key);
}

int mm_count_keys(Multimap* mm) {
    assert(validate_mm(mm));
    if (mm == NULL) {
        return -1;
    }
    
    return mm->size;
}

int mm_count_values(Multimap* mm, char* key) {
    assert(validate_mm(mm));
    if (mm == NULL || key == NULL) {
        return -1;
    }

    if(strlen(key)>=MAX_KEY_LENGTH){
        return -1;
    }

    Key key_to_search;
    strncpy(key_to_search.key, key, MAX_KEY_LENGTH);
    Key* result = (Key*)bsearch(&key_to_search, mm->keys, mm->size, sizeof(Key), compareKeys);

    if (result == NULL) {
        return 0;
    } else {
        int count = 0;
        Node* current = result->values;
        while (current != NULL) {
            count++;
            current = current->next;
        }
        return count;
    }
}

int mm_get_values(Multimap* mm, char* key, Value values[], int max_values) {
    assert(validate_mm(mm));
    if (mm == NULL || key == NULL || values == NULL || max_values <= 0) {
        return -1;
    }

    if(strlen(key)>=MAX_KEY_LENGTH){
        return-1;
    }

    Key key_to_search;
    strncpy(key_to_search.key, key, MAX_KEY_LENGTH);
    Key* result = (Key*)bsearch(&key_to_search, mm->keys, mm->size, sizeof(Key), compareKeys);

    if (result == NULL) {
        return 0;
    } else {
        int count = 0;
        Node* current = result->values;
        while (current != NULL && count < max_values) {
            values[count].num = current->value.num;
            strncpy(values[count].str, current->value.str, MAX_VALUE_LENGTH - 1);
            values[count].str[MAX_VALUE_LENGTH - 1] = '\0';
            count++;
            current = current->next;
        }
        assert(validate_mm(mm));
        return count;
    }
}

void removeAndShift(Key* array, int* size, int index) {
    if (index >= 0 && index < *size) {
        // Move each key one position ahead of its current position
        for (int i = index; i < *size - 1; i++) {
            array[i] = array[i + 1];
        }
        // Zero out the last element that is now unused
        memset(&array[*size - 1], 0, sizeof(Key));
        (*size)--;
    }
}

int mm_remove_key(Multimap* mm, char* key) {
    assert(validate_mm(mm));
    if (mm == NULL || key == NULL) {
        return -1;
    }
    if(strlen(key)>=MAX_KEY_LENGTH){
        return-1;
    }

    Key* key_value = mm->keys;
    Key key_to_search;
    strncpy(key_to_search.key, key, MAX_KEY_LENGTH);
    Key* result = (Key*)bsearch(&key_to_search, key_value, mm->size, sizeof(Key), compareKeys);
    int count = 0;
    if (result == NULL) {
        return 0; // Key not found
    } else {
        // Free the linked list associated with this key
        Node* current = result->values;
        while (current != NULL) {
            Node* next = current->next;
            free(current);
            current = next;
            count++;
        }
        // Shift the remaining keys down to fill the gap
        int index = result - key_value; // Get the index of the key to remove
        removeAndShift(key_value, &mm->size, index);
        assert(validate_mm(mm));
        return count;
    }
}

void mm_print(Multimap* mm) {
    assert(validate_mm(mm));
    if (mm == NULL) {
        printf("Multimap is NULL\n");
        return;
    }
    Key* currentKey = mm->keys;
    for (int i = 0; i < mm->size; i++) {
        printf("\nKey: %s\n", currentKey[i].key);
        int count = 1;
        Node* currentValue = currentKey[i].values;
        while (NULL != currentValue) {
            printf("%d. %d %s\n", count, currentValue->value.num, currentValue->value.str);
            count++;
            currentValue = currentValue->next;
        }
        printf("\n");
    }
}

int mm_destroy(Multimap* mm) {
    assert(validate_mm(mm));
    if (mm == NULL) {
        return -1;
    }
    
    int count = 0;
    Key* current_key = mm->keys;
    for (int i = 0; i < mm->size; i++) {
        Node* current_value = mm->keys[i].values;
        Node* next;
        while (current_value != NULL) {
            next = current_value->next;
            free(current_value);
            current_value = next;
            count++;
        }
    }
    count += mm->size;
    //free(mm->keys);
    mm->size = 0; 
    free(mm);
    
    return count;
}

int mm_get_first_key(Multimap *mm, char *key, int length){
    assert(validate_mm(mm));
    if(mm==NULL || length<=0){
        return -1;
    }

    if(mm->size==0){
        return 0;
    }

    char *first_key = mm->keys[0].key;
    strncpy(key, first_key, length-1);
    key[length-1] = '\0';
    return 1;

}

int mm_get_next_key(Multimap *mm, char *key, int length){
    assert(validate_mm(mm));
    if(mm==NULL || length<=0){
        return -1;
    }
    if (getkey_count >= mm->size - 1) {
        getkey_count = 0;  // Reset count when all keys have been fetched
        return 0;          // No more keys
    }
    if(mm->size==0){
        return 0;
    }

    getkey_count++;
    char *next_key = mm->keys[getkey_count].key;
    strncpy(key, next_key, length-1);
    key[length-1] = '\0';
    return 1;

}



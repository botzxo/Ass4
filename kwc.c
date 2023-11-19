#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "multimap.h"

#define CRT_SECURE_NO_WARNINGS
#define MAX_PARA 20000
#define MAX_STOP_LENGTH 1000
#define MAX_ENTRY_LENGTH 20

typedef struct PARA_WORDS{
    char word[MAX_VALUE_LENGTH];
    int size;
}words;

int is_word(char c){
    return isalnum(c) || c == '-' || c == '\'';
}


int read_paragraph(FILE *file, char *paragraph, int *paraNum){
    if(file == NULL){
        return -1;
    }
    unsigned int index = 0;
    char line[MAX_PARA];
    
    while(fgets(line, sizeof(line), file) != NULL && (line[0] != '\n' || index == 0)){
        unsigned int line_length = strlen(line);
        if(line_length>0 && line[line_length-1]=='\n'){
            line[line_length - 1] = '\0';
            line_length--;
        }

        strcpy(paragraph + index, line);
        index += line_length;

                
        if (line_length == 0 && index > 0) {
            break;
        }
    }

    paragraph[index] = '\0';
    (*paraNum)++;
    return index > 0;
}

int compareWordListWithStopList(words word_list[], char *stop_list, char *word_found, int word_index) {
    if(word_list == NULL || stop_list == NULL){
        return -1;
    }
    
    for (int i = word_index; i < word_list->size; i++) {
        
        char *token = strtok(stop_list, " ");
        while (token != NULL) {
            if (strcmp(word_list[i].word, token) == 0) {
                strncpy(word_found, token, MAX_KEY_LENGTH);
                return i;  // Stop searching after finding a match
            }
            token = strtok(NULL, " ");
        }
    }

    return -1; //no word matches in the stop_list or no word left that matches
}



int process_word(char *stop_list, words words_list[], char* context_string, int word_index){
    char *word_found = NULL;
    int word_found_index = compareWordListWithStopList(words_list, stop_list, word_found, word_index);

    if(word_found_index==-1){
        return -1;
    }
    int context_length = (MAX_ENTRY_LENGTH - strlen(word_found)) / 2;
    
    int count_before_context = word_found_index - context_length;
    int count_after_context = word_found_index + context_length;

    char context_string_before[MAX_VALUE_LENGTH] = "";
    char context_string_after[MAX_VALUE_LENGTH] = "";

    strncat(context_string_before, word_found, MAX_VALUE_LENGTH);

    if(count_before_context <= 0){
        count_before_context=0;
    }else if(count_after_context >= words_list->size){
        count_after_context = words_list->size;
    }
    
    for(int i = count_before_context; i<word_found_index; i++){
        char temp_result[MAX_VALUE_LENGTH];
        strncpy(temp_result, words_list[i].word, MAX_VALUE_LENGTH);
        temp_result[strlen(temp_result)] = '\0';

        strncat(temp_result, context_string_before, MAX_VALUE_LENGTH);
        strncpy(context_string_before, temp_result, MAX_VALUE_LENGTH);
    }

    for(int i = word_found_index; i<count_after_context; i++){
        strncat(context_string_after, words_list[i].word, MAX_VALUE_LENGTH);
    }

    
    strncat(context_string_before, context_string_after, MAX_VALUE_LENGTH);
    strncpy(context_string, context_string_before, MAX_VALUE_LENGTH);
    context_string[strlen(context_string)] = '\0';

    return word_found_index;
      
}

int remove_words(Multimap *index, int max_frequency){
    if(index == NULL || max_frequency <=0){
        return -1;
    }
    
    int total_values;
    
    for(int i=0; i<index->size; i++){
        char *current_key = index->keys[i].key;
        total_values = mm_count_values(index, current_key);
        if(total_values >= max_frequency){
            mm_remove_key(index, current_key);
        }
    }

    return 0;
}

void print_kwc(Multimap *index){
    printf("WORD                          PARA CONTEXT\n");
    printf("====                          ==== =======\n");
    for (int i=0; i<index->size; i++){
        Key *current_key = index->keys;
        Node *current_value = current_key[i].values;
        printf("%s                        %d %s\n",current_key[i].key, current_value->value.num, current_value->value.str);
    }
}

char* read_stop_words(FILE* stoplist) {
    char stop_word[MAX_KEY_LENGTH];
    char* stopListWords = (char*)calloc(MAX_STOP_LENGTH, sizeof(char));
    if (stopListWords == NULL) {
        // Handle allocation error
        return NULL;
    }

    int num_stop_words = 0;
    while (fscanf(stoplist, "%s", stop_word) == 1 && num_stop_words < MAX_STOP_LENGTH) {
        strcat(stopListWords, stop_word);
        strcat(stopListWords, " ");
        num_stop_words++;
    }

    return stopListWords;
}

int process_paragraph(char* paragraph, Multimap* index, char* stopListWords, int para_number) {
    char context_string[MAX_VALUE_LENGTH];
    words* words_from_para = (words*)malloc(MAX_PARA * sizeof(words));
    if (words_from_para == NULL) {
        // Handle allocation error
        return -1;
    }

    // Tokenize and process words here...
    // ...
    int word_count = 0;
    char* token = strtok(paragraph, " ");

    while (token != NULL && word_count < MAX_PARA) {
        if (is_word(token[0])) {
            strncpy(words_from_para[word_count].word, token, MAX_KEY_LENGTH);
            word_count++;
            (words_from_para->size)++;
        }

        token = strtok(NULL, " ");
    }
    int word_index = 0;
    while (word_index < words_from_para->size) {
        word_index = process_word(stopListWords, words_from_para, context_string, word_index);
        if (word_index == -1) {
            return -1;
        }
        mm_insert_value(index, words_from_para[word_index].word, para_number, context_string);
        word_index++;
    }



    free(words_from_para);
    return word_index; // Or appropriate return value
}


int process_kwc(FILE* file, FILE* stoplist) {
    char* stopListWords = read_stop_words(stoplist);
    if (stopListWords == NULL) {
        // Handle error
        return -1;
    }

    Multimap* index = mm_create(MAX_ENTRY_LENGTH);
    assert(validate_mm(index));

    char* paragraph = (char*)malloc(MAX_PARA * sizeof(char));
    if (paragraph == NULL) {
        // Handle allocation error
        free(stopListWords);
        return -1;
    }

    int para_number = 0;
    while (read_paragraph(file, paragraph, &para_number)) {
        int ret = process_paragraph(paragraph, index, stopListWords, para_number);
        if (ret == -1) {
            // Handle error
            free(paragraph);
            free(stopListWords);
            return -1;
        }
        else {
            print_kwc(index);
        }

    }

    // Clean up and print results
    free(paragraph);
    free(stopListWords);

    return 0;
}




int main() {
    printf("Hello Code\n");
    FILE* para_file;
    para_file= fopen("paratest.txt", "r");
    FILE* stop_file;
    stop_file = fopen("stoplist.txt", "r");
    if (para_file == NULL || stop_file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    int ret2 = process_kwc( para_file,  stop_file);

    fclose(para_file);
    fclose(stop_file);
    return 0;

    // char paragraph[MAX_PARA]; // Assuming a maximum paragraph length of 20000 characters
    // int paragraphNumber = 0;

    // while (read_paragraph(file, paragraph, &paragraphNumber)) {
    //     // Process the paragraph as needed
    //     printf("Paragraph %d:\n%s\n\n", paragraphNumber, paragraph);
    // }

    // fclose(file);

    // return 0;


}

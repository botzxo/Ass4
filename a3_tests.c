#include <stdio.h>
#include <string.h>
#include "multimap.h"


#define VERIFY_INT(e,t) verify_int(e, t, #t)
#define VERIFY_STR(e,t) verify_str(e, t, #t)
// use these two for other types of pointers, *not* strings:
#define VERIFY_NULL(t) verify_null(t, #t)
#define VERIFY_NOT_NULL(t) verify_not_null(t, #t)

int Tests_Passed = 0;
int Tests_Failed = 0;

void verify_int(int expected, int result, char test[]) {
    if (expected == result) {
        printf("Passed: expected %d, got %d for: %s\n", expected, result, test);
        Tests_Passed++;
    }
    else {
        printf("FAILED: expected %d, got %d for: %s\n", expected, result, test);
        Tests_Failed++;
    }
}

void verify_str(char expected[], char result[], char test[]) {
    if ((NULL == expected && NULL == result) ||
        (NULL != expected && NULL != result && strcmp(expected, result) == 0)) {
        printf("Passed: expected '%s', got '%s' for: %s\n", expected, result, test);
        Tests_Passed++;
    }
    else {
        printf("FAILED: expected '%s', got '%s' for: %s\n", expected, result, test);
        Tests_Failed++;
    }
}

// type "void *" is a pointer to anything; we'll learn more soon
void verify_null(void* result, char test[]) {
    if (NULL == result) {
        printf("Passed: expected NULL, got NULL for: %s\n", test);
        Tests_Passed++;
    }
    else {
        printf("FAILED: expected NULL, got %p for: %s\n", result, test);
        Tests_Failed++;
    }
}

void verify_not_null(void* result, char test[]) {
    if (NULL != result) {
        printf("Passed: expected not NULL, got %p for: %s\n", result, test);
        Tests_Passed++;
    }
    else {
        printf("FAILED: expected not NULL, got NULL for: %s\n", test);
        Tests_Failed++;
    }
}

/*** Example tests (should pass as-is) ***/

void test_example() {
    Multimap* mm;
    Value arr[3]; // using one extra space

    printf("\n*** Example tests:\n\n");

    VERIFY_NOT_NULL(mm = mm_create(3));
    VERIFY_INT(1, mm_insert_value(mm, "hello", 123, "abc"));
    VERIFY_INT(1, mm_insert_value(mm, "world", 456, "def"));
    VERIFY_INT(2, mm_insert_value(mm, "hello", 999, "zzz"));
    // maximum value length:
    VERIFY_INT(3, mm_insert_value(mm, "hello", 1048576, "abcdefghijabcdefghij"));
    // there is no connection between this value and the one with the key "hello"
    VERIFY_INT(1, mm_insert_value(mm, "!", 123, "abc"));
    VERIFY_INT(2, mm_insert_value(mm, "!", 5, ""));

    VERIFY_INT(3, mm_count_keys(mm));
    VERIFY_INT(3, mm_count_values(mm, "hello"));
    VERIFY_INT(1, mm_count_values(mm, "world"));
    VERIFY_INT(2, mm_count_values(mm, "!"));
    VERIFY_INT(0, mm_count_values(mm, "?"));

    VERIFY_INT(2, mm_get_values(mm, "!", arr, 2));
    VERIFY_INT(5, arr[0].num);
    VERIFY_STR("", arr[0].str);
    VERIFY_INT(123, arr[1].num);
    VERIFY_STR("abc", arr[1].str);

    // no space for more keys
    VERIFY_INT(-1, mm_insert_value(mm, "xyz", 789, "ghi"));

    VERIFY_INT(2, mm_remove_key(mm, "!"));
    VERIFY_INT(0, mm_count_values(mm, "!"));
    VERIFY_INT(1, mm_insert_value(mm, "xyz", 789, "ghi"));

    // make sure we aren't copying three values when we ask for two
    arr[2].num = 0;
    arr[2].str[0] = '\0';
    VERIFY_INT(2, mm_get_values(mm, "hello", arr, 2));
    VERIFY_INT(123, arr[0].num);
    VERIFY_STR("abc", arr[0].str);
    VERIFY_INT(999, arr[1].num);
    VERIFY_STR("zzz", arr[1].str);
    VERIFY_INT(0, arr[2].num);
    VERIFY_STR("", arr[2].str);

    // this isn't a test:
    mm_print(mm);

    VERIFY_INT(8, mm_destroy(mm));

    // these could fail assertions but should execute and pass with assertions off
#ifdef NDEBUG
    VERIFY_NULL(mm_create(-1));
    VERIFY_INT(-1, mm_insert_value(NULL, "hello", 123, "abc"));
#endif
}

/*** Your test code and main program follow ***/
void test_insertion() {
    printf("\n**Testing insertions**\n\n");
    Multimap* mm = mm_create(4);
    VERIFY_INT(0, mm_count_keys(mm));
    VERIFY_INT(1, mm_insert_value(mm, "hello", 123, "abc"));;
    VERIFY_INT(1, mm_insert_value(mm, "world", 456, "def"));
    VERIFY_INT(1, mm_insert_value(mm, "example", 789, "ghi"));;
    VERIFY_INT(1, mm_count_values(mm, "hello"));
    VERIFY_INT(1, mm_count_values(mm, "world"));
    VERIFY_INT(3, mm_count_keys(mm));
    
    VERIFY_INT(-1, mm_insert_value(mm,"Helloisynewname",-20,"gg"));
    VERIFY_INT(3, mm_count_keys(mm)); 
    mm_print(mm);
    Multimap* mm2  = mm_create(3);
    
    VERIFY_INT(1, mm_insert_value(mm2, "", 123, "abc"));
    VERIFY_INT(2, mm_insert_value(mm2, "", 256, "lli"));
    VERIFY_INT(1, mm_count_keys(mm2));
    VERIFY_INT(2, mm_count_values(mm2, ""));
    VERIFY_INT(-1, mm_insert_value(mm2,"",-200,"Arethese21charactersyetIdontknow"));
    VERIFY_INT(2, mm_count_values(mm2, ""));
    mm_insert_value(mm2, "key2", 256, "xcv");
    mm_insert_value(mm2, "key3", 999, "uuu");
    VERIFY_INT(-1,mm_insert_value(mm2, "key4", 100, "no"));
    VERIFY_INT(3,mm_count_keys(mm2));
    VERIFY_INT(0, mm_count_values(mm2, "key4"));
    mm_print(mm2);
     
    mm_destroy(mm);
    mm_destroy(mm2);
}

void test_retrieve_values() {
    printf("\n**Testing value retrievals**\n\n");
    Multimap* mm = mm_create(3);
    Value values[2];

    mm_insert_value(mm, "hello", 123, "abc");
    mm_insert_value(mm, "hello", 456, "def");

    int count = mm_get_values(mm, "hello", values, 2);
    VERIFY_INT(2, count);
    VERIFY_INT(123, values[0].num);
    VERIFY_INT(456, values[1].num);
    VERIFY_INT(0, mm_get_values(mm, "no_key", values, 2));
    
    mm_insert_value(mm, "hello", 300, "www");
    VERIFY_INT(2,mm_get_values(mm, "hello", values, 2));
    VERIFY_INT(3, mm_count_values(mm, "hello"));
   
    VERIFY_INT(1,mm_get_values(mm, "hello", values, 1));
    VERIFY_INT(-1,mm_get_values(mm, "Arethese21charactersyetIdontknow", values, 5));
    mm_print(mm);
    mm_destroy(mm);
}

void test_key_removal() {
    printf("\n**Testing key removals**\n\n");
    Multimap* mm = mm_create(3);
    Value values[2];

    mm_insert_value(mm, "hello", 123, "abc");
    mm_insert_value(mm, "hello", 456, "def");
    mm_insert_value(mm, "world", 200, "gii");
    int removed_values = mm_remove_key(mm, "hello");
    VERIFY_INT(2, removed_values);  // All values are removed
    VERIFY_INT(0,mm_get_values(mm, "hello", values, 3));

    removed_values = mm_remove_key(mm, "no_key");
    VERIFY_INT(0, removed_values);  // No values removed
    VERIFY_INT(1,mm_count_keys(mm));
    VERIFY_INT(0, mm_count_values(mm, "hello"));
    mm_print(mm);
    mm_destroy(mm);
}

void test_maximum_keys_and_values() {
    printf("\n**Testing Edge cases**\n\n");
    Multimap* mm = mm_create(3);
    Value values[2]; 
    // Insert the maximum number of keys
    mm_insert_value(mm, "key1", 1, "value1");
    mm_insert_value(mm, "key2", 2, "value2");
    mm_insert_value(mm, "key3", 3, "value3");

    // Attempt to insert another key should fail
     
    VERIFY_INT(-1, mm_insert_value(mm, "key4", 4, "value4"));
    VERIFY_INT(0, mm_get_values(mm, "key4", values, 3));

    mm_destroy(mm);
}

void test_invalid_inputs() {
    printf("\n**Testing Invalid cases**\n\n");
    Multimap* mm = mm_create(3);
    #ifdef NDEBUG
        int result = mm_insert_value(mm, NULL, 123, "abc");
        VERIFY_INT(-1, result);

        int count = mm_count_values(mm, NULL);
        VERIFY_INT(-1, count);
        char test_NULL = mm_create(-1);
        VERIFY_NULL(test_NULL);
        VERIFY_INT(-1, mm_insert_value(NULL, "hello", 333, "gg"));
        VERIFY_INT(-1, mm_count_keys(NULL));
        VERIFY_NULL(mm_count_values(NULL));

    #endif
   
    mm_destroy(mm);
}



int main() {
    printf("*** Starting tests...\n");

    // Leave this:
    test_example();

    /*** Call your own tests here ***/
    test_insertion();
    test_retrieve_values();
    test_key_removal();
    test_maximum_keys_and_values();
    test_invalid_inputs();
    //test_memory_management();
    // And this:
    if (0 == Tests_Failed) {
        printf("\nAll %d tests passed.\n", Tests_Passed);
    }
    else {
        printf("\nFAILED %d of %d tests.\n", Tests_Failed, Tests_Failed + Tests_Passed);
    }

    printf("\n*** Tests complete.\n");
    return 0;
}

/******************
Name: Yarone Djebali
ID: 330242769
Assignment: ex6
*******************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/*
This function clears remaining characters from input buffer
to prevents leftover characters from interfering with next input
*/
static void clearInputBuffer() {
    int c;
    // Read and discard all characters until newline or EOF
    while ((c = getchar()) != '\n' && c != EOF);
}

int getInt(const char* prompt){
    printf("%s",prompt);
    int num;
    // Exercise assumes valid input, so we don't check return value
    scanf("%d", &num);
    // Clear remaining input buffer after reading number
    clearInputBuffer();
    // returns the integer entered by the user
    return num;
}

/*
Reads a dynamic string from stdin buffer until newline.
Memory is allocated dynamically and grows as needed
and returns pointer to the allocated string.
*/
char *getString(const char* prompt) {
    printf("%s",prompt);
    char *str = NULL;
    int len = 0;
    int capacity = 0;
    int c;

    // We read characters until newline or EOF is encountered.
    while ((c = getchar()) != '\n' && c != EOF) {
        // When (len+1 >= capacity), we need more space (including '\0'), so we resize the buffer.
        if (len + 1 >= capacity) {
            // If capacity == 0, we initialize it to 2 for at least one character and \0
            capacity = (capacity == 0) ? 2 : capacity * 2;
            char *tmp = (char *)realloc(str, capacity);
            if (!tmp) {
                free(str);
                return NULL;
            }
            // Update str only if realloc succeeded
            str = tmp;
        }
        // Append character and increment length
        str[len++] = c;
    }

    // Handle empty input line
    if (!str) {
        // Allocate space for empty string
        str = (char *)malloc(sizeof(char)*1);
        str[0] = '\0';
    } else {
        // terminate the string
        str[len] = '\0';
    }
    return str;
}

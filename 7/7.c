#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#define MAX_VALUES 16

int get_line(char **lineptr, unsigned int *n, FILE *stream);
void strip_newline(char **line);
bool valid_test(long long target, int values[], int count, int index, long long total);
bool valid_test_with_concat(long long target, int values[], int count, int index, long long total);

int main (int argc, char **argv) {
    unsigned long long successful_test_total = 0;

    if (argc != 2) {
        puts("Usage: ./7 input.txt");
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        puts("Error opening file");
        return EXIT_FAILURE;
    }

    //read through a line at a time
    char *line = NULL;
    unsigned int len = 0;
    int read;
    
    while ((read = get_line(&line, &len, f)) != -1) {
        strip_newline(&line);
        //sepate test value and numbers
        char *target_token = strtok(line, ":");
        char *numbers_token = strtok(NULL, ":");        
        if(!target_token || !numbers_token) {
            puts("Malformed line");
            continue;
        }

        //parse number values
        unsigned long long target = atoll(target_token);
        
        int values[MAX_VALUES];
        int count = 0;
        char *token = strtok(numbers_token, " ");
        while (token && count < MAX_VALUES) {
            values[count++] = atoi(token);
            token = strtok(NULL, " ");
        }

        // if (valid_test(target, values, count, 1, values[0])) {
        //     successful_test_total += target;
        // }
        if (valid_test_with_concat(target, values, count, 1, values[0])) {
            successful_test_total += target;
        }
        
        
    }
    printf("Successful test total: %lld\n", successful_test_total);
    
    free(line);
    fclose(f);
    return EXIT_SUCCESS;
}

//returns number of chars read on success, -1 on failure
int get_line(char **lineptr, unsigned int *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;
    }

    unsigned int pos = 0;
    int c;

    if (*lineptr == NULL || *n == 0) {
        *n = 128; //reasonable default size
        *lineptr = malloc(*n);
        if (*lineptr == NULL) {
            return -1;
        }
    }

    while ((c = fgetc(stream)) != EOF) {
        //resize lineptr if needed
        if (pos + 1 >= *n) {
            unsigned int new_size = *n * 2;
            char *new_ptr = realloc(*lineptr, new_size);
            if (!new_ptr) return -1;
            *lineptr = new_ptr;
            *n = new_size;
        }
        //add character to string
        (*lineptr)[pos++] = (char)c;
        //check for end of line
        if (c == '\n') break;
    }

    if (pos == 0 && c == EOF) return -1;

    //add null char terminator
    (*lineptr)[pos] = '\0';
    return pos;
}

void strip_newline(char **line) {
    int l = strlen(*line);
    if (l > 0 && (*line)[l - 1] == '\n') {
        (*line)[l - 1] = '\0';
    }
}

bool valid_test(long long target, int values[], int count, int index, long long total) {
    //end case
    if (index == count -1) {        
        if (total * values[index] == target || total + values[index] == target) {
            return true;
        } else {
            return false;
        }
    }
    //addition
    if(valid_test(target, values, count, index + 1, total + values[index])) {
        return true;
    }
    //multiplication
    if(valid_test(target, values, count, index + 1, total * values[index])) {
        return true;
    }
    return false;
}

bool valid_test_with_concat(long long target, int values[], int count, int index, long long total) {
    //get left side of concat
    long long concat_total = total;
    //get digits of right side
    char str[10];
    sprintf(str, "%d", values[index]);
    int digits = strlen(str);
    //add zeroes to left side
    for (int i = 0; i < digits; i++) {
        concat_total *= 10;
    }

    //end case
    if (index == count -1) {
        if (total * values[index] == target || total + values[index] == target || concat_total + values[index] == target) {
            return true;        
        } else {
            return false;
        }
    }
    //addition
    if(valid_test_with_concat(target, values, count, index + 1, total + values[index])) {
        return true;
    }
    //multiplication
    if(valid_test_with_concat(target, values, count, index + 1, total * values[index])) {
        return true;
    }
    
    if (valid_test_with_concat(target, values, count, index + 1, concat_total + values[index])) {
        return true;
    }

    return false;
}


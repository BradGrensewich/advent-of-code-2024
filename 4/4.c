#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024
#define INITIAL_CAPACITY 100

void strip_newline(char *s);
int check_for_word(char **wordsearch, int i, int j, int rows, int columns);
int check_word_in_direction(char **wordsearch, int i, int j, int v_move, int h_move);
int check_xmas(char **wordsearch, int i, int j, int rows, int columns);

int main(int argc, char **argv) {    
    int length_of_row;
    int number_of_rows = 0;

    if (argc < 2) {
        printf("Usage: ./3 input.txt\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");      
    if (f == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    int capacity = INITIAL_CAPACITY;
    int rows = 0;    
    
    //allocate space for a ton of pointers
    char **wordsearch = malloc(capacity * sizeof(char *));
    if (wordsearch == NULL) {
        printf("Memory allocation failure\n");
        fclose(f);
        return 1;
    }

    //read input
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), f)) {
        strip_newline(line);

        //there are more lines than intial memory allocated
        if (rows > capacity) {
            //double number of pointers
            capacity *= 2;
            char **tmp = realloc(wordsearch, capacity * sizeof(char *));
            if (tmp == NULL) {
                perror("Memory reallocation failed");
                //free memory
                for (int i = 0; i < rows; i++) free(wordsearch[i]);
                free(wordsearch);
                fclose(f);
                return 1;
            }
            wordsearch = tmp;
        }
       
        //automatically allocate memory for a line and copy read line to it
        wordsearch[rows] = strdup(line);
        //check if success
        if (wordsearch[rows] == NULL) {
            perror("Memory allocation for line failed");
            for (int i = 0; i < rows; i++) free(wordsearch[i]);
            free(wordsearch);
            fclose(f);
            return 1;
        }
        rows++;
    }
    
    int columns = strlen(wordsearch[0]);
    //find and count xmas
    unsigned int count = 0;
    unsigned int xmas_count = 0;
    for (int i = 0; i < rows; i++) {
        char c = 'a';
        for (int j = 0; j < columns; j++) {
            c = wordsearch[i][j];
            if (c == 'X') {
               count += check_for_word(wordsearch, i, j, rows, columns);
            }
            if (c == 'M' || c == 'S') {
                xmas_count += check_xmas(wordsearch, i, j, rows, columns);
            }
        }
    }
    printf("XMAS count: %d and %d\n", count, xmas_count);

    //free memory
    for (int i = 0; i < rows; i++) free(wordsearch[i]);
    free(wordsearch);
    fclose(f);
    return 0;
}

void strip_newline(char *s) {
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len -1] == '\r')) {
        s[--len] = '\0';
    }
}

int check_for_word(char **wordsearch, int i, int j, int rows, int columns) {
    int count = 0;
    // 8 directions: {v_move, h_move}
    int directions[8][2] = {
        {0, 1},   // →
        {0, -1},  // ←
        {1, 0},   // ↓
        {-1, 0},  // ↑
        {1, 1},   // ↘
        {-1, -1}, // ↖
        {1, -1},  // ↙
        {-1, 1}   // ↗
    };

    //check all directions
    for (int d = 0; d < 8; d++) {
        int v = directions[d][0];
        int h = directions[d][1];

        int end_i = i + 3 * v;
        int end_j = j + 3 * h;

        if (end_i >= 0 && end_i < rows && end_j >= 0 && end_j < columns) {
            count += check_word_in_direction(wordsearch, i, j, v, h);
        }
    }

    return count; 

}

int check_word_in_direction(char **wordsearch, int i, int j, int v_move, int h_move) {
    if (wordsearch[i + v_move][j + h_move] == 'M' &&
        wordsearch[i + v_move * 2][j + h_move * 2] == 'A' && 
        wordsearch[i + v_move * 3][j + h_move * 3] == 'S') {
        return 1;
    }
    return 0;
}

int check_xmas(char **wordsearch, int i, int j, int rows, int columns) {
    //check bounds
    int end_i = i + 2;
    int end_j = j + 2;
    if (end_i >= rows || end_j >= columns) {
        return 0;
    }
    //if m
    if (wordsearch[i][j] == 'M') {
        if((wordsearch[i + 2][j] == 'M' && 
            wordsearch[i][j + 2] == 'S' && 
            wordsearch[i + 2][j + 2] == 'S' && 
            wordsearch[i + 1][j + 1] == 'A') ||
            (wordsearch[i + 2][j] == 'S' && 
                wordsearch[i][j + 2] == 'M' && 
                wordsearch[i + 2][j + 2] == 'S' && 
                wordsearch[i + 1][j + 1] == 'A'))
            

            {
            return 1;
        }
    }   

    //if s
    if (wordsearch[i][j] == 'S') {
        if((wordsearch[i + 2][j] == 'S' && 
            wordsearch[i][j + 2] == 'M' && 
            wordsearch[i + 2][j + 2] == 'M' && 
            wordsearch[i + 1][j + 1] == 'A') ||
            (wordsearch[i + 2][j] == 'M' && 
                wordsearch[i][j + 2] == 'S' && 
                wordsearch[i + 2][j + 2] == 'M' && 
                wordsearch[i + 1][j + 1] == 'A'))
             {
            return 1;
        }
        
    }
    return 0;
}
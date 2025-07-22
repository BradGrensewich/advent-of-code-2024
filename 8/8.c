#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct antenna_node_ {
    int v;
    int h;
    struct antenna_node_ *next;
} antenna_node;

#define GRID_DIMENSION_LENGTH 50
#define ASCII_OFFSETTER 48
#define MAX_ASCII_NEEDED (122 - ASCII_OFFSETTER)

int get_line(char **lineptr, unsigned int *n, FILE *stream);
void strip_newline(char **line);
void insert_into_hash_table(char c, int v, int h);
void free_table();
int get_unique_antinode_count();
int antinode_placed(int v, int h);
int place_antinodes(antenna_node *first_ptr, antenna_node *second_ptr);

//hash table for antenna coordinates
antenna_node *table[MAX_ASCII_NEEDED] = {NULL};
//unique antinode locations
int antinode_coordinates[GRID_DIMENSION_LENGTH][GRID_DIMENSION_LENGTH] = {0};

int main (int argc, char **argv) {
   

    if (argc != 2) {
        puts("Usage: ./8 input.txt");
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
    int line_count = 0;
    while ((read = get_line(&line, &len, f)) != -1) {
        strip_newline(&line);        
        //track antennas
        for (int i = 0; i < GRID_DIMENSION_LENGTH; i++) {
            if (line[i] != '.') {
                insert_into_hash_table(line[i], line_count, i);
            }
        }
        line_count++;
    }

    printf("Unique antinodes: %d\n", get_unique_antinode_count());

    free(line);
    free_table();
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

void insert_into_hash_table(char c, int v, int h) {
    int index = (int)(c - ASCII_OFFSETTER);
    antenna_node *new_antenna = malloc(sizeof(antenna_node));
    new_antenna->v = v;
    new_antenna->h = h;
    new_antenna->next = table[index];
    table[index] = new_antenna;    
}

void free_table() {
    for (int i = 0; i < MAX_ASCII_NEEDED; i++) {
        antenna_node *ptr = table[i];
        while (ptr != NULL) {
            antenna_node *tmp = ptr;
            ptr = ptr->next;
            free(tmp);
        }
    }
}
int get_unique_antinode_count() {
    int count = 0;
    for (int i = 0; i < MAX_ASCII_NEEDED; i++) {
        antenna_node *first_ptr = table[i];
        while(first_ptr != NULL) {
            antenna_node *second_ptr = first_ptr->next;
            while (second_ptr != NULL) {
                // //get distances of potential antinodes
                // int v_distance = second_ptr->v - first_ptr->v;
                // int h_distance = second_ptr->h - first_ptr->h;
                
                // //check coordinates "before" antennas 
                // count += antinode_placed(first_ptr->v - v_distance, first_ptr->h - h_distance);
                // //check "after" antennas
                // count += antinode_placed(second_ptr->v + v_distance, second_ptr->h + h_distance);
                count += place_antinodes(first_ptr, second_ptr);

                second_ptr = second_ptr->next;
            }
            first_ptr = first_ptr->next;
        }
    }
    return count;
}

int antinode_placed(int v, int h) {
    //coodinates are in map
    if (v >= 0 &&
        v < GRID_DIMENSION_LENGTH && 
        h >= 0 && 
        h < GRID_DIMENSION_LENGTH && 
        antinode_coordinates[v][h] == 0
    ) {
        antinode_coordinates[v][h] = 1;
        return 1;
    }
    return 0;
}

int place_antinodes(antenna_node *first_ptr, antenna_node *second_ptr) {
    int count = 0;
    int v_distance = second_ptr->v - first_ptr->v;
    int h_distance = second_ptr->h - first_ptr->h;
    //working backwards
    for (int i = 0;;i++) {
        int v = first_ptr->v - i * v_distance;
        int h = first_ptr->h - i * h_distance; 
        if (
            v >= 0 &&
            v < GRID_DIMENSION_LENGTH && 
            h >= 0 && 
            h < GRID_DIMENSION_LENGTH
        ) {
            if (antinode_coordinates[v][h] == 1) continue;
            //place antinode
            antinode_coordinates[v][h] = 1;
            count++;
        } else {
            break;
        }
    }

    //working forwards
    for (int i = 0;;i++) {
        int v = first_ptr->v + i * v_distance;
        int h = first_ptr->h + i * h_distance; 
        if (
            v >= 0 &&
            v < GRID_DIMENSION_LENGTH && 
            h >= 0 && 
            h < GRID_DIMENSION_LENGTH
        ) {
            if (antinode_coordinates[v][h] == 1) continue;
            //place antinode
            antinode_coordinates[v][h] = 1;
            count++;
        } else {
            break;
        }
    }
    return count;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct peak_node_ {
    int trailhead_v;
    int trailhead_h;
    struct peak_node_ *next;
} peak_node;

#define MAP_DIMENSION_LENGTH 45
#define HASH_COUNT 4545

unsigned char map[MAP_DIMENSION_LENGTH][MAP_DIMENSION_LENGTH];
peak_node *reached_peak_hash_table[HASH_COUNT];


int get_line(char **lineptr, unsigned int *n, FILE *stream);
void strip_newline(char **line);
unsigned long long get_trailhead_scores_total();
int count_reachable_peaks(int v, int h, int current_height, int trailhead_v, int trailhead_h);
bool peak_found_by_trailhead_already(int hash, int trailhead_v, int trailhead_h);
unsigned int get_hash(int v, int h);
void free_table();

int main (int argc, char **argv) {    
    if (argc != 2) {
        puts("Usage ./10 input.txt");
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        puts("Error opening file");
        return EXIT_FAILURE;
    }

    //read through file to save map to memory
    char *line;
    int len;
    int row = 0;
    while (get_line(&line, &len, f) > 1) {
        strip_newline(&line);
        //save line to map as int
        for (int i = 0; i < MAP_DIMENSION_LENGTH; i++) {
            map[row][i] = (int)line[i] - '0';
        }
        row++;
    }
    fclose(f);
    free(line);

    printf("Trailhead scores total: %lld\n", get_trailhead_scores_total());
    
    free_table();    
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

unsigned long long get_trailhead_scores_total() {
    unsigned long long total = 0;
    //read through map
    for (int i = 0; i < MAP_DIMENSION_LENGTH; i++) {
        for (int j = 0; j < MAP_DIMENSION_LENGTH; j++) {
            if (map[i][j] == 0) {
                total += count_reachable_peaks(i, j, 0, i, j);
            }
        }
    }   
    return total;
}

int count_reachable_peaks(int v, int h, int current_height, int trailhead_v, int trailhead_h) {
    int count = 0;
    //found a reachable peak
    if (map[v][h] == 9) {
        // //check if already reched from this trailhead
        // unsigned int hash = get_hash(v, h);
        // if (peak_found_by_trailhead_already(hash, trailhead_v, trailhead_h)) {
        //     return 0;
        // }

        // //mark in table
        // peak_node *found = malloc(sizeof(peak_node));
        // found->trailhead_v = trailhead_v;
        // found->trailhead_h = trailhead_h;
        // found->next = reached_peak_hash_table[hash];
        // reached_peak_hash_table[hash] = found;
        
        return 1;
    }
    //up
    if (v - 1 >= 0 && map[v - 1][h] == current_height + 1) {
        count += count_reachable_peaks(v - 1, h, current_height + 1, trailhead_v, trailhead_h);
    }
    //down
    if (v + 1 < MAP_DIMENSION_LENGTH && map[v + 1][h] == current_height + 1) {
        count += count_reachable_peaks(v + 1, h, current_height + 1, trailhead_v, trailhead_h);
    }
    //left
    if (h - 1 >= 0 && map[v][h -1] == current_height + 1) {
        count += count_reachable_peaks(v, h - 1, current_height + 1, trailhead_v, trailhead_h);
    }
    //right
    if (h + 1 < MAP_DIMENSION_LENGTH && map[v][h + 1] == current_height + 1) {
        count += count_reachable_peaks(v, h + 1, current_height + 1, trailhead_v, trailhead_h);
    }

    return count;
}
unsigned int get_hash(int v, int h) {
    v *= 100;
    return v + h;
}

bool peak_found_by_trailhead_already(int hash, int trailhead_v, int trailhead_h) {
    peak_node * ptr = reached_peak_hash_table[hash];
    while (ptr) {
        if (ptr->trailhead_v == trailhead_v && ptr->trailhead_h == trailhead_h) {
            return true;
        }
        ptr = ptr->next;
    }
    return false;
}

void free_table() {
    peak_node *ptr;
    peak_node *to_delete;
    for (int i = 0; i < HASH_COUNT; i++) {
        ptr = reached_peak_hash_table[i];
        while (ptr) {
            to_delete = ptr;
            ptr = ptr->next;
            free(to_delete);
        }
    }

}
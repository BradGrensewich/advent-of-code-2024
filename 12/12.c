#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct region_ {
    unsigned int area;
    unsigned int perimeter;
} region;

#define MAP_DIMENSION 140

char map[MAP_DIMENSION][MAP_DIMENSION + 1];
bool checked[MAP_DIMENSION][MAP_DIMENSION];

void get_map(FILE *f);
unsigned long get_total_fence_cost();
void find_region_info(region *r, int v, int h);
int get_line(char **lineptr, unsigned int *n, FILE *stream);
void strip_newline(char **line);

int main(int argc, char **argv) {
    if (argc !=2) {
        puts("Usage: ./a.out input.txt");
        return EXIT_FAILURE;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        puts("Error opening file");
        return EXIT_FAILURE;
    }

    get_map(f);

    printf("Total fence cost: %ld\n", get_total_fence_cost());
    fclose(f);
    return EXIT_SUCCESS;
}


void get_map(FILE *f) {
     //read through a line at a time
    char *line = NULL;
    unsigned int len = 0;
    int read;
    int line_count = 0;
    while ((read = get_line(&line, &len, f)) != -1) {
        strip_newline(&line);
        strcpy(map[line_count], line);
        line_count++;
    }
    free(line);
}

unsigned long get_total_fence_cost() {    
    unsigned long total = 0;
    region *new = malloc(sizeof(region));
    for (int i = 0; i < MAP_DIMENSION; i++) {
        for (int j = 0; j < MAP_DIMENSION; j++) {
            if (!checked[i][j]) {                
                new->area = 0;                
                new->perimeter = 0;
                find_region_info(new, i, j);
                total += (new->area * new->perimeter);
            }
        }
    }
    free(new);
    return total;
}

void find_region_info(region *r, int v, int h) {    
    checked[v][h] = true;
    r->area += 1;
    //left
    if (h - 1 >= 0 && map[v][h] == map[v][h - 1]) {
        if (!checked[v][h - 1]) {
            find_region_info(r, v, h - 1);
        }        
    } else {
        r->perimeter++;
    }
    //right
    if (h + 1 < MAP_DIMENSION && map[v][h] == map[v][h + 1]) {
        if (!checked[v][h + 1]) {
            find_region_info(r, v, h + 1);
        }        
    } else {
        r->perimeter++;
    }
    //up
    if (v - 1 >= 0 && map[v][h] == map[v - 1][h]) {
        if (!checked[v - 1][h]) {
            find_region_info(r, v - 1, h);
        }        
    } else {
        r->perimeter++;
    }
    //down
    if (v + 1 < MAP_DIMENSION && map[v][h] == map[v + 1][h]) {
         if (!checked[v + 1][h]) {
            find_region_info(r, v + 1, h);
        }        
    } else {
        r->perimeter++;
    }
    
}
//returns number of chars read on success, -1 on failure
int get_line(char **lineptr, unsigned int *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;
    }

    unsigned int pos = 0;
    int c;

    if (*lineptr == NULL || *n == 0) {
        *n = MAP_DIMENSION + 1; //reasonable default size
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
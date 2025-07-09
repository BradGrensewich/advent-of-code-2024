#include <stdio.h>
#include <string.h>

typedef struct {
    int v_position;
    int h_position;
    int direction;
} position;

#define CHARS_IN_LINE 132
char map[CHARS_IN_LINE][CHARS_IN_LINE] = {0};

void strip_newline(char *s);
int creates_loop(position guard_position, int real_dimensions, int obs_v, int obs_h);

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: ./6 input.txt\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    
    int visited[CHARS_IN_LINE][CHARS_IN_LINE] = {0};
    int checked_obstacles[CHARS_IN_LINE][CHARS_IN_LINE] = {0};
    int distinct_locations_visited = 0;
    int viable_obstacles = 0;

    //read file into map
    for (int i = 0; i < CHARS_IN_LINE && fgets(map[i], (CHARS_IN_LINE), f); i++) {
        strip_newline(map[i]);
    }

    int real_dimensions = strlen(map[0]);
    

    //get guard_postion
    position guard_position;
    for (int i = 0; i < real_dimensions; i++) {
        for (int j = 0; j < real_dimensions; j++) {
            if (map[i][j] == '^') {
                guard_position.v_position = i;
                guard_position.h_position = j;
                guard_position.direction = 0;
                visited[i][j] = 1;
                distinct_locations_visited++;
            }
        }
    }

    //simulate patrol route
    int directions[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};   
    while(1) {
        int direction = guard_position.direction;
        //get next coordinates
        int new_v = guard_position.v_position + directions[direction][0];
        int new_h = guard_position.h_position + directions[direction][1]; 
        //check leaving map       
        if (new_v >= real_dimensions || new_v < 0 || new_h >= real_dimensions || new_h < 0 ) {
            break;
        }
        
        //check if direction change needed
        if (map[new_v][new_h] == '#') {
            if (direction == 3) {
                guard_position.direction = 0;
            } else {
                guard_position.direction++;
            }
            continue;            
        }
        
        //check if obstacle is viable
        if (!checked_obstacles[new_v][new_h]) {
            if (creates_loop(guard_position, real_dimensions, new_v, new_h)) {                
                viable_obstacles++;
            }
            checked_obstacles[new_v][new_h] = 1;
        }
               
        //move 
        guard_position.v_position = new_v;
        guard_position.h_position = new_h;

        //track movement
        if (visited[new_v][new_h] == 0) {
            visited[new_v][new_h] = 1;
            distinct_locations_visited++;
        }
    }
    printf("Distinct location visited: %d\nObstacles: %d\n", distinct_locations_visited, viable_obstacles);
    fclose(f);    
}

void strip_newline(char *s) {
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len -1] == '\r')) {
        s[--len] = '\0';
    }
}

int creates_loop(position guard_position, int real_dimensions, int obs_v, int obs_h) {
    int position_count = 0;
    position positions[9999] = {0};
    positions[position_count].v_position = guard_position.v_position;
    positions[position_count].h_position = guard_position.h_position;
    positions[position_count].direction = guard_position.direction;
    position_count++;
    //simulate patrol route
    int directions[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};     
     
    while(1) {
        int direction = guard_position.direction;
        //get next coordinates
        int new_v = guard_position.v_position + directions[direction][0];
        int new_h = guard_position.h_position + directions[direction][1]; 
        //check leaving map       
        if (new_v >= real_dimensions || new_v < 0 || new_h >= real_dimensions || new_h < 0 ) {
            break;
        }
        
        //check if direction change needed
        if (map[new_v][new_h] == '#' || (new_v == obs_v && new_h == obs_h)) {
            if (direction == 3) {
                guard_position.direction = 0;
            } else {
                guard_position.direction++;
            }
            continue;            
        }        
               
        //move 
        guard_position.v_position = new_v;
        guard_position.h_position = new_h;        

        //check if will create loop
        for (int i = 0; i < position_count; i++) {
            if (positions[i].v_position == guard_position.v_position && 
                positions[i].h_position == guard_position.h_position &&
                positions[i].direction == guard_position.direction) {
                    return 1;
                }
        }

        //track position
        positions[position_count].v_position = guard_position.v_position;
        positions[position_count].h_position = guard_position.h_position;
        positions[position_count].direction = guard_position.direction;
        position_count++;

    }
    return 0;

}

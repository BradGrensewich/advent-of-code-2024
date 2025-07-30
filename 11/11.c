#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUCKETS 2147483647

typedef struct stone_node_ {
    unsigned long long value;
    struct stone_node_ *next;
    struct stone_node_ *previous;
} stone_node;

stone_node *stones = NULL;
unsigned long long count = 0;

bool initialize_stones();
bool insert_into_list(unsigned long long value);
bool blink();
unsigned int get_digits(unsigned long long value);
unsigned long long get_left_stone(unsigned long long value, unsigned int digits);
unsigned long long get_right_stone(unsigned long long value, unsigned int digits);
unsigned long long get_stone_count();
void free_list();

int main() {
    //store intial stones in memory
    if (!initialize_stones()) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 25; i++) {
        if(!blink()) {
            return EXIT_FAILURE;
        }
    }
    printf("Stone count: %lld\n", get_stone_count());

    free_list();
    return EXIT_SUCCESS;
}

bool initialize_stones() {
    char initial_stones[] = "8435 234 928434 14 0 7 92446 8992692";
    char *stone_ptr = strtok(initial_stones, " ");    
    while (stone_ptr) {
        unsigned long long stone = atol(stone_ptr);
        if (!insert_into_list(stone)) {
            return false;
        }     
        //find next stone
        stone_ptr = strtok(NULL, " ");
    }
    return true;
}

bool insert_into_list(unsigned long long value) {
    stone_node *new = malloc(sizeof(stone_node));
    if (!new) {
        puts("malloc error");
        return false;
    }
    new->value = value;
    new->previous = stones;
    new->next = NULL;
    stone_node *ptr = stones;
    //empty list
    if (!ptr) {
        stones = new;
        return true;
    }
    //single stone
    if (!ptr->next) {
        stones->next = new;
        return true;
    }
    //find end of list
    while (ptr->next) {
        ptr = ptr->next;
    }
    //insert at end
    new->previous = ptr;
    ptr->next = new;
    return true;
}

bool blink() {
    //go through all stones
    stone_node *curr = stones;
    while (curr) {
        unsigned long long curr_value = curr->value;
        unsigned int curr_digits = get_digits(curr_value);
        //0
        if (curr_value == 0) {
            curr->value = 1;
        //even digits
        } else if (curr_digits % 2 == 0) {            
            //split stone value into 2 stones
            curr->value = get_right_stone(curr_value, curr_digits);
            stone_node *left_stone = malloc(sizeof(stone_node));
            if (!left_stone) {
                puts("malloc error spliting stones");
                return false;
            }
            
            left_stone->value = get_left_stone(curr_value, curr_digits);
            //insert left stone
            if (curr->previous) {
                curr->previous->next = left_stone;
            } else {
                stones = left_stone;
            }            
            left_stone->previous = curr->previous;
            curr->previous = left_stone;
            left_stone->next = curr;
        //no other rules
        } else {
            curr->value = curr_value * 2024;
        }
        curr = curr->next;
    }
    return true;
}

unsigned int get_digits(unsigned long long value) {
    unsigned int digits = 0;
    while (value > 0) {
        value /= 10;
        digits++;
    }
    return digits;
}
unsigned long long get_left_stone(unsigned long long value, unsigned int digits) {

    for (int i = 0, k = digits / 2; i < k; i++ ) {
        value /= 10;
    }
    return value;

}
unsigned long long get_right_stone(unsigned long long value, unsigned int digits) {
    unsigned int modolo_number = 1;
    for (int i = 0; i < digits / 2; i++) {
        modolo_number *= 10;
    }    
    return value % modolo_number;    
}

unsigned long long get_stone_count() {
    stone_node *ptr = stones;
    unsigned long long count = 0;
    while (ptr) {
        count ++;
        ptr = ptr->next;
    }
    return count;
}

void free_list() {
    stone_node *ptr = stones;
    stone_node *to_delete;
    while(ptr) {
        // printf("%lld ", ptr->value);
        to_delete = ptr;
        ptr = ptr->next;
        free(to_delete);
    }
}

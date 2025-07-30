#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITIAL_BUCKET_COUNT 10253
#define LOAD_FACTOR 0.75

typedef struct stone_node_ {
    unsigned long long value;
    unsigned long long curr_count;
    unsigned long long unfinished_transforms;
    unsigned long long turns_into[2];
    bool creates_two;
    struct stone_node_ *next;
} stone_node;

typedef struct {
    stone_node **buckets;
    unsigned int bucket_count;
    unsigned int size;
} hash_table;

hash_table *table = NULL;
// stone_node *stones[INITIAL_BUCKET_COUNT] = {NULL};
unsigned long long total_stone_count = 0;

bool create_table(); 
unsigned long long hash(unsigned long long value, unsigned int hash);
bool resize_table();
bool insert_into_table_or_add_to_count(unsigned long long value, unsigned long long count);
bool initialize_stones();
void initialize_stone(stone_node *s);
unsigned int get_digits(unsigned long long value);
unsigned long long get_left_stone(unsigned long long value, unsigned int digits);
unsigned long long get_right_stone(unsigned long long value, unsigned int digits);
bool blink();
void set_counts_to_zero_and_resize();
void free_table();

int main() {    
    create_table();
    //store intial stones in memory
    if (!initialize_stones()) {
        puts("sf");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 75; i++) {
        if(!blink()) {
            return EXIT_FAILURE;
        }
    }
    printf("%lld\n", total_stone_count);
    
    free_table();
    return EXIT_SUCCESS;
}

bool create_table() {
    table = malloc(sizeof(hash_table));
    if (!table) {
        puts("Error mallocing table");
        return false;
    }
    table->bucket_count = INITIAL_BUCKET_COUNT;
    table->size = 0;
    table->buckets = calloc(table->bucket_count, sizeof(stone_node *));
    if (!table->buckets) {
        puts("error callocing table buckets");
        return false;
    }
    return true;
}

unsigned long long hash(unsigned long long value, unsigned int bucket_count) {
    value = (~value) + (value << 21); // mix
    value = value ^ (value >> 24);
    value = (value + (value << 3)) + (value << 8); // mix
    value = value ^ (value >> 14);
    value = (value + (value << 2)) + (value << 4); // mix
    value = value ^ (value >> 28);
    value = value + (value << 31);
    return value % bucket_count;
}

bool resize_table() {
    unsigned int new_bucket_count = table->bucket_count * 2;
    stone_node **new_buckets = calloc(new_bucket_count, sizeof(stone_node *));
    if (!new_buckets) {
        puts("error callocing new buckets");
        return false;
    }

    for (int i = 0; i < table->bucket_count; i++) {
        stone_node *curr = table->buckets[i];
        while (curr) {
            stone_node *next = curr->next;
            unsigned int new_index = hash(curr->value, new_bucket_count);
            curr->next = new_buckets[new_index];
            new_buckets[new_index] = curr;
            curr = next;
        }
    }
    free(table->buckets);
    table->buckets = new_buckets;
    table->bucket_count = new_bucket_count;
    return true;
}

bool insert_into_table_or_add_to_count(unsigned long long value, unsigned long long count) {
    if ((double)table->size / table->bucket_count > LOAD_FACTOR) {
        resize_table();
    }
    unsigned long long index = hash(value, table->bucket_count);
    stone_node *head = table->buckets[index];

    stone_node *ptr = head;
    //check for already tracked
    while(ptr) {
        if (ptr->value == value) {
            ptr->curr_count += count;
            total_stone_count += count;
            return true;
        }
        ptr = ptr->next;
    }

    stone_node *new = malloc(sizeof(stone_node));
    if (!new) {
        puts("error mallocing new stone");
        return false;
    }
    new->value = value;
    
    initialize_stone(new);
    new->curr_count = count;
    new->unfinished_transforms = 0;
    new->next = head;    
    table->buckets[index] = new;
    table->size++;
    total_stone_count += count;

    
    return true;
}

bool initialize_stones() {
    char initial_stones[] = "8435 234 928434 14 0 7 92446 8992692";
    char *stone_ptr = strtok(initial_stones, " ");    
    while (stone_ptr) {
        unsigned long long stone = atol(stone_ptr);
        if (!insert_into_table_or_add_to_count(stone, 1)) {
            return false;
        }     
        //find next stone
        stone_ptr = strtok(NULL, " ");
    }
    return true;
}


void initialize_stone(stone_node *s) {    
    unsigned long long value = s->value;
    unsigned int curr_digits = get_digits(value);
    if (value == 0) {
        s->creates_two = false;
        s->turns_into[0] = 1;        
    } else if (curr_digits % 2 == 0) {
        s->creates_two = true;
        s->turns_into[0] = get_left_stone(value, curr_digits);
        s->turns_into[1] = get_right_stone(value, curr_digits);
    } else {
        s->creates_two = false;
        s->turns_into[0] = value * 2024;
    }
    return;
}


unsigned int get_digits(unsigned long long value) {
    if (value == 0) return 1;
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

bool blink() {
    set_counts_to_zero_and_resize();
    for (unsigned int i = 0; i < table->bucket_count; i++) {
        stone_node *curr = table->buckets[i];
        while (curr) {
            unsigned long long unfinished_transforms = curr->unfinished_transforms;
            total_stone_count -= unfinished_transforms;
            if(!insert_into_table_or_add_to_count(curr->turns_into[0], unfinished_transforms)) {
                puts("error during blink");
                return false;
            }
            if (curr->creates_two) {                
                if(!insert_into_table_or_add_to_count(curr->turns_into[1], unfinished_transforms)) {
                    puts("error during blink");
                    return false;
                }
            }
            curr->unfinished_transforms = 0;
            curr = curr->next;
        }
    }
    return true;
}
//resize here if needed to prevent resizing during blink
void set_counts_to_zero_and_resize() {
    unsigned long long new_stones = 0;
    for (unsigned int i = 0; i < table->bucket_count; i++) {
        stone_node *curr = table->buckets[i];
        while (curr) {
            curr->unfinished_transforms = curr->curr_count;
            curr->curr_count = 0;
            if (curr->creates_two) {
                new_stones += curr->unfinished_transforms;
            }
            curr = curr->next;
        }
    }
   
    // while(((double)(table->size + new_stones) / table->bucket_count) > LOAD_FACTOR) {
    //     resize_table();
    // }
}

void free_table() {
    for (unsigned int i = 0; i < table->bucket_count; i++) {
        stone_node *curr = table->buckets[i];
        while (curr) {
            stone_node *tmp = curr;
            curr = curr->next;
            free(tmp);
        }    
    }
    free(table->buckets);
    free(table);

}


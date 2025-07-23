#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct block_ {
    int id;
    struct block_ *next;
    struct block_ *prev;
} block;

block *list = NULL;
block *tail = NULL;
unsigned int free_space = 0;

bool insert_into_disc(int id);
bool write_to_disc(FILE *f);
void reformat_disc();
void reformat_disc_new(); 
void free_list();
unsigned long long get_checksum();
int get_needed_block_size(int id, block *reverse_ptr);
bool has_enough_free_space(int needed, block *forward_ptr); 
void move_block(int block_size, block *empty_space, block *block_location);

int main(int argc, char **argv) {
     if (argc != 2) {
        puts("Usage: ./8 input.txt");
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        puts("Error opening file");
        return EXIT_FAILURE;
    }
    if(!write_to_disc(f)) {
        puts("Error writing to disc");
        return EXIT_FAILURE;
    }   
    reformat_disc_new();
    printf("checksum: %lld\n", get_checksum());

    free_list();
    fclose(f);
    return EXIT_SUCCESS;
} 

bool write_to_disc(FILE *f) {
    //read through file
    int c;
    unsigned int blocks;
    int id = 0;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        //insert blocks of id
        blocks = c - '0';
        for (int i = 0; i < blocks; i++) {
            if(!insert_into_disc(id)) return false;            
        }
        //get empty block number
        if((c = fgetc(f)) == EOF || c == '\n') {
            break;
        }
        
        //insert empty blocks
        blocks = c - '0';
        for (int i = 0; i < blocks; i++) {
           if(!insert_into_disc(-1)) return false;
           free_space++;
        }
        //increment id
        id++;      
    }
    return true;

}

bool insert_into_disc(int id) {
    
    block *new_block = malloc(sizeof(block));
    if (!new_block) {
        puts("Malloc error");
        return false;
    }
    new_block->id = id;
    new_block->next = NULL;
    new_block->prev = NULL;
    
    //empty list    
    if (list == NULL) {
        list = new_block;
        tail = new_block;
        return true;        
    }
    
    //insert new block at end
    new_block->prev = tail;
    tail->next = new_block;
    tail = new_block;
    return true;
}

void reformat_disc() {
    block *reverse_ptr = tail;
    block *forward_ptr = list;
    unsigned int distance_from_back = 0;
    while(1) {
        while(reverse_ptr && reverse_ptr->id == -1) {
            reverse_ptr = reverse_ptr->prev;
            distance_from_back++;
            if (distance_from_back >= free_space) {
                return;
            }
        }
        while(forward_ptr && forward_ptr->id != -1) {
            forward_ptr = forward_ptr->next;
        }
        forward_ptr->id = reverse_ptr->id;
        reverse_ptr->id = -1;
    }
}

//need to avoid reprocessing already moved files
void reformat_disc_new() {
    block *reverse_ptr = tail;
    int smallest_file_attempted_to_move = 10000;
    while (reverse_ptr) {
        //found block
        if(reverse_ptr->id != -1 && reverse_ptr->id < smallest_file_attempted_to_move) {
            int id = reverse_ptr->id;
            smallest_file_attempted_to_move = id;
            //get block size
            int needed_block_size = get_needed_block_size(id, reverse_ptr);     
            
            //search for appropriate free space
            block *forward_ptr = list;
            while(forward_ptr && forward_ptr->id != id) {
                //found free space
                if (forward_ptr->id == -1 && has_enough_free_space(needed_block_size, forward_ptr)) {
                    //move block
                    move_block(needed_block_size, forward_ptr, reverse_ptr);
                    
                    //continue passed empty space -1 to take end of loop step into account
                    for (int i = 0; i < needed_block_size -1; i++) {
                        reverse_ptr = reverse_ptr->prev;
                    }
                    break;    
                }
                forward_ptr = forward_ptr->next;
            }
        }
        reverse_ptr = reverse_ptr->prev;
    } 
}

int get_needed_block_size(int id, block *reverse_ptr) {
    //get block size
    int needed_block_size = 0;
    block *size_ptr = reverse_ptr;
    while (size_ptr && size_ptr->id == id) {
        needed_block_size++;
        size_ptr = size_ptr->prev;
    }
    return needed_block_size;
}

bool has_enough_free_space(int needed, block *forward_ptr) {
    int found = 0;
    block *size_ptr = forward_ptr;
    while (size_ptr && size_ptr->id == -1) {
        found++;
        if (found == needed) return true;
        size_ptr = size_ptr->next;
    }
    return false;
}

void move_block(int block_size, block *forward_ptr, block *reverse_ptr) {
    block *block_location = reverse_ptr;
    block *empty_space = forward_ptr;
    int id = block_location->id;
    for (int i = 0; i < block_size; i++) {
        empty_space->id = id;
        block_location->id = -1;
        empty_space = empty_space->next;
        block_location = block_location->prev;
    }
}



void free_list() {
    block *ptr = list;
    while (ptr != NULL) {        
        block *tmp = ptr;        
        ptr = ptr->next;
        free(tmp);
    }
}

unsigned long long get_checksum() {
    unsigned long long total = 0;
    block *ptr = list;
    for (int i = 0; ptr != NULL; i++) {
        if (ptr->id != -1) {
            total += (i * ptr->id);    
        }        
        ptr = ptr->next;        
    }
    return total;
}
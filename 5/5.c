#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_BUCKETS 100
#define RULE_LINE_LENGTH 7

typedef struct node {
    struct node *next;
    int num;    
} node;

void insert_into(node **bucket, int number);
void strip_newline(char *s);
int get_middle_page_of_valid(int update[], int numbers_in_update, node *rules[]);
int is_legal(int after, node *rules);
int get_illegal_mid(int update[], int number_of_instructions, node* rules[]); 

int main(int argc, char **argv) {  
    //hash table of rules
    //each bucket contains a list of numbers that cannot be AFTER index number
    node *table[NUM_BUCKETS] = {NULL};
    unsigned long mid_sum = 0;
    unsigned long illegal_mid_sum = 0;
    
    if (argc < 2) {
        printf("Usage: ./3 input.txt\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");      
    if (f == NULL) {
        printf("Error opening input file\n");
        return 1;
    }
    
    //read through all rules, stopping short of update
    char line[RULE_LINE_LENGTH];
    while(fgets(line, sizeof(line), f)) {
        //end of rules
        if(line[0] == '\n') {
            break;
        }
        int first, second;
        sscanf(line, "%d|%d", &first, &second);
        insert_into(&table[second], first);
    }

    //read through updates
    char update[96];
    int numbers_in_update[24];
    while (fgets(update, sizeof(update), f)) {
        strip_newline(update);
        int i = 0;

        //tokenize line and parse ints
        char* token = strtok(update, ",");
        for (; token && i < 24; i++) {
            //store to array
            numbers_in_update[i] = atoi(token);
            token = strtok(NULL, ",");
        }
        int legal_mid = get_middle_page_of_valid(numbers_in_update, i, table);
        mid_sum += legal_mid;
        if (!legal_mid) {
            illegal_mid_sum += get_illegal_mid(numbers_in_update, i, table);
        }
    }

    printf("Mid sum: %ld\nIllegals: %ld\n", mid_sum, illegal_mid_sum);

    //clear memory
    for (int i = 0; i < NUM_BUCKETS; i++) {
        node *ptr = table[i];
        while (ptr != NULL) {
            node *tmp = ptr;
            ptr = ptr->next;
            free(tmp);
        }
    }
    fclose(f);

    return 0;
}

void insert_into(node **bucket, int number) {
    node *n = malloc(sizeof(node));
    n->num = number;
    n->next = *bucket;
    *bucket = n;    
}

void strip_newline(char *s) {
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len -1] == '\r')) {
        s[--len] = '\0';
    }
}

int get_middle_page_of_valid(int update[], int number_of_instructions, node *rules[]) {
    for (int i = 0; i < number_of_instructions - 1; i++) {
        for (int j = (i + 1); j < number_of_instructions; j++) {
            if (!is_legal(update[j], rules[update[i]])) {
                return 0;
            }
        }
    }
    //return middle instruction    
    return update[number_of_instructions / 2];
}

int is_legal(int after, node *rules) {
    node *ptr = rules;
    while (ptr != NULL) {
        if (ptr->num == after) {
            return 0;
        }
        ptr = ptr->next;
    }
    return 1;
}

int get_illegal_mid(int update[], int number_of_instructions, node* rules[]) {
    for (int i = 0; i < number_of_instructions - 1; i++) {
        int before = update[i];
        for (int j = (i + 1); j < number_of_instructions; j++) {
            int swapped = 0;
            int after = update[j];
           
            node *ptr = rules[before];
            while(ptr != NULL) {
                //breaks rules
                if (ptr->num == after) {
                    //swap
                    update[i] = after;
                    update[j] = before;
                    swapped++;
                    break;
                }
                ptr = ptr->next;
            }
            //if swapped check stop moving to next index to check new i value legality
            if (swapped) {
                i--;
                break;
            }
        }
    }
    return update[number_of_instructions / 2];
}





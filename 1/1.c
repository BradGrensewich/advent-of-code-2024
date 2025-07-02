#include <stdio.h>
#include <stdlib.h>


typedef struct node {
    struct node *next;
    int number;
} node;

int insert(int number, node **list);
void free_list(node *list);
unsigned long long get_similarity_score(node *left, node * right); 

int main (int argc, char **argv){
    FILE *f = NULL;
    node *list1 = NULL;
    node *list2 = NULL;
    unsigned long long difference = 0;    
    
    if (argc != 2) {
        printf("usage: ./1 input\n");
        return 1;
    }

    f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    //add lists into memory
    unsigned int left, right;
    while (fscanf(f, "%d %d", &left, &right) == 2) {
        insert(left, &list1);
        insert(right, &list2);
    } 

    //add up differences
    node *left_pointer = list1;
    node *right_pointer = list2;
    while (left_pointer != NULL) {
        int curr = abs(left_pointer->number - right_pointer->number);
        difference += curr;
        left_pointer = left_pointer->next;
        right_pointer = right_pointer->next;
    }
    printf("Total difference: %lld\n", difference);
    printf("Similarity score: %lld\n", get_similarity_score(list1, list2));


    free_list(list1);
    free_list(list2);
    free(f);
}

//returns 1 on success
int insert(int number, node **list){
    //create new node
    node *n = malloc(sizeof(node));
    if (n == NULL) {
        printf("Error finding memory for new node\n");
        return 0;
    }
    n->number = number;
    n->next = NULL;
    //empty list or smallest value
    if (*list == NULL || n->number < (*list)->number) {
        n->next = *list;
        *list = n;
        return 1;        
    }

    //iterate through list
    node *ptr = *list;
    while (ptr != NULL) {
        //new node is largest
        if (ptr->next == NULL) {
            ptr->next = n;
            return 1;
        //node is somewhere in middle
        } else if (number < ptr->next->number) {
            n->next = ptr->next;
            ptr->next = n;
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

unsigned long long get_similarity_score(node *left, node * right) {
    unsigned long long score = 0;
    node *left_pointer = left;
    while (left_pointer != NULL) {
        int num = left_pointer->number;
        int multiplier = 0;
        node *right_pointer = right;
        while (right_pointer != NULL) {
            if (right_pointer->number > num) {
                break;
            }
            if (right_pointer->number == num) {
                multiplier++;
            }
            right_pointer = right_pointer->next;
        }
        score += (num * multiplier);
        left_pointer = left_pointer->next;
    } 
    return score;
}

void free_list(node *list) {
    node *tmp = list;
    while (list != NULL) {
        list = list->next;
        free(tmp);
        tmp = list;
    }
}
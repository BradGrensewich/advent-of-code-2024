#include <stdio.h>
#include <regex.h>
#include <string.h>

#define MAX_INSTRUCTION_LENGTH 11
#define MAX_ENABLER_LENGTH 6

unsigned long long check_mul(regex_t r, FILE *f);
int check_enable(FILE *f); 

int main (int argc, char **argv) {
    unsigned long long results = 0;
    FILE *f = NULL;
    char c;
    int instructions_enabled = 1;

    if (argc < 2) {
        printf("Usage: ./3 input.txt\n");
        return 1;
    }

    f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    //create regex
    regex_t regex;
    if (regcomp(&regex, "^ul\\([0-9]{1,3},[0-9]{1,3}\\)$", REG_EXTENDED) != 0) {
        printf("Error compiling regex\n");
        return 1;
    }

    //read through file
    while (fread(&c, 1, 1, f) > 0) {
        if (c == 'm' && instructions_enabled) {
            //save file ptr distance from start if file
            long pos = ftell(f);
            //check for valid instruction
            results += check_mul(regex, f);
            //return to poistion relative to start of file
            fseek(f, pos, SEEK_SET);
        } else if (c == 'd') {
            //save file ptr distance from start if file
            long pos = ftell(f);
            //check for enable or disabler
            int enabler = check_enable(f);
            if (enabler == -1) {
                instructions_enabled = 0;
            } else if (enabler == 1) {
                instructions_enabled = 1;
            } else if (enabler == 2) {
                return 1;
            }
            
            //return to poistion relative to start of file
            fseek(f, pos, SEEK_SET);

        }
    }

    printf("Results: %lld\n", results);

    fclose(f);
}

unsigned long long check_mul(regex_t r, FILE *f) {
    char possible_instruction[MAX_INSTRUCTION_LENGTH + 1] = {'\0'};
    char c;
    int read = 0;
    //get instruction
    while (fread(&c, 1, 1, f) > 0 && read < MAX_INSTRUCTION_LENGTH) {        
        if (c == 'm') {
            return 0;
        }
        possible_instruction[read++] = c;
        if (c == ')') {
            break;
        }
    }    
    //check against pattern
    if (regexec(&r, possible_instruction, 0, NULL, 0) == 0) {
        //calculate product
        int n1, n2;
        sscanf(possible_instruction, "ul(%d,%d)", &n1, &n2);
        //printf("n1: %d, n2: %d\n", n1, n2);
        return n1 * n2;
    }
   
    return 0;
}
int check_enable(FILE *f) {
    char possible_enabler[MAX_ENABLER_LENGTH + 1] = {'\0'};
    if (fread(possible_enabler, 1, MAX_ENABLER_LENGTH, f) < 1) {
        printf("Error reading enabler");
        return 2;
    }
   
    if (strcmp(possible_enabler, "on't()") == 0) {
        return -1;
    }

    //prepare for enabler check
    for (int i = 3; i < MAX_ENABLER_LENGTH; i++) {
        possible_enabler[i] = '\0';
    }
       
    if (strcmp(possible_enabler, "o()") == 0) {
        return 1;
    }
    return 0;

}
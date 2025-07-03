#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024
#define MAX_LEVELS_PER_REPORT 8

int report_is_safe(int level_count, int *report);
int dampened_report_is_safe(int level_count, int *report);

int main(int argc, char **argv) {
    FILE *f = NULL;
    char buffer[MAX_LINE_LENGTH];
    unsigned int safe_reports = 0;   
    
    if (argc < 2) {
        printf("Usage: ./2 input.txt\n");
        return 1;
    }

    f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    while(fgets(buffer, sizeof(buffer), f) != NULL) {
        char *ptr = buffer;
        int report[8];
        int level_count = 0;
        int n;      

        while(sscanf(ptr, " %d", &n) > 0) {
            report[level_count++] = n;
          
            // Advance the pointer past the integer and any trailing whitespace           
            while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t')) {
                ptr++;
            }
            while (*ptr != '\0' && *ptr >= '0' && *ptr <= '9') {
                ptr++;
            }
        }  
        if (report_is_safe(level_count, report) || dampened_report_is_safe(level_count, report)) {
            safe_reports++;                    
        }    
       
    }

    printf("Safe reports: %d\n", safe_reports);
    fclose(f);
    return 0;
}

int report_is_safe(int level_count, int *report) {
    int direction = 0;    
    for (int i = 1; i < level_count; i++) {
        int difference = report[i] - report[i - 1];
        //note direction
        if (difference > 0) {
            direction++;
        } else if (difference < 0) {
            direction--;
        } else {
            return 0;
        } 
        //check direction change
        if (abs(direction) != (i)) {           
            return 0;
        }
        //check magnitude
        if (abs(difference) > 3) {
            return 0;
        }
    }
    return 1;
}

int dampened_report_is_safe(int level_count, int *report) {
    for (int i = 0; i < level_count; i++) {
        int dampened[level_count - 1];
        int offset = 0;
        for (int j = 0; j < level_count; j++) {
            if (j == i) {
                offset--;
                continue;
            }
            dampened[j + offset] = report[j];
        }
        if (report_is_safe(level_count - 1, dampened)) {
            return 1;
        } 
    }
    return 0;
}
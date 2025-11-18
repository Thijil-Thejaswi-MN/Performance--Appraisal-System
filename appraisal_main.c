#include "appraisal_head.h"
#include <string.h> 
#include <limits.h> 

extern RankEntry *heap; // Declare heap from appraisal_function.c

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <max_employees>\n", argv[0]);
        return 1;
    }

    
    char *endptr;
    long val = strtol(argv[1], &endptr, 10);

    if (endptr == argv[1] || *endptr != '\0' || val <= 0 || val > INT_MAX) {
        printf("Error: <max_employees> must be a positive integer.\n");
        return 1;
    }
    MAX_EMPLOYEES_GLOBAL = (int)val;

    // Dynamically allocate heap based on MAX_EMPLOYEES_GLOBAL
    heap = (RankEntry*)malloc(sizeof(RankEntry) * MAX_EMPLOYEES_GLOBAL);
    if (heap == NULL) {
        printf("Memory allocation failed for heap.\n");
        return 1;
    }

    Employee *root = NULL;
    Queue q;
    init_queue(&q);

    int choice;
    int employee_count = 0; 

    while (1) {
        print_menu();
        printf("Enter choice: ");
        
        char choice_buffer[10];
        if (fgets(choice_buffer, sizeof(choice_buffer), stdin) == NULL) {
            printf("Error reading choice. Exiting.\n");
            break;
        }
        if (sscanf(choice_buffer, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            // Clear stdin buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }


        switch (choice) {

            case 1: {
                if (employee_count >= MAX_EMPLOYEES_GLOBAL) {
                    printf("Maximum number of employees (%d) reached. Cannot add more.\n", MAX_EMPLOYEES_GLOBAL);
                    break;
                }
                int id;
                char name[NAME_LEN], dept[DEPT_LEN];

                id = get_positive_integer("Enter Employee ID: ");
                
               
                if (search_employee(root, id) != NULL) {
                    printf("Employee with ID %d already exists. Please use a different ID.\n", id);
                    break;
                }

                printf("Enter Name: ");
                // Use fgets for safer string input
                if (fgets(name, sizeof(name), stdin) == NULL) {
                    printf("Error reading name.\n");
                    break;
                }
                name[strcspn(name, "\n")] = 0;

                printf("Enter Department: ");
                
                if (fgets(dept, sizeof(dept), stdin) == NULL) {
                    printf("Error reading department.\n");
                    break;
                }
                dept[strcspn(dept, "\n")] = 0; // Remove newline

                root = insert_employee(root, id, name, dept);
                employee_count++;
                printf("Employee added successfully.\n");
                break;
            }

            case 2: {
                int id;
                float score;
                printf("Available Employee ids:\n");
                display_employee_ids(root);
                printf("\n");
                
                id = get_positive_integer("Enter Employee ID: ");

                if (search_employee(root, id) == NULL) {
                    printf("Employee ID %d not found. Cannot add appraisal.\n", id);
                    break;
                }

                score = get_valid_rating();

                enqueue(&q, id, score);
                printf("Appraisal Request Added to Queue.\n");
                break;
            }

            case 3:
                process_appraisal(&q, root);
                break;

            case 4:
                display_inorder(root);
                break;

            case 5: {
                int id;
                printf("Available Employee ids:\n");
                display_employee_ids(root);
                printf("\n");
                id = get_positive_integer("Enter Employee ID to search: ");

                Employee *e = search_employee(root, id);
                if (e)
                    printf("Found: %s | Dept: %s | Avg: %.2f | Reviews: %d\n",
                           e->name, e->dept, e->avg_score, e->reviews);
                else
                    printf("Employee not found.\n");
                break;
            }

            case 6: {
                int k;
                k = get_positive_integer("Enter number of top performers to display: ");
                display_top_performers(k);
                break;
            }

            case 7:
                printf("Exiting program.\n");
                free(heap); 
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    free(heap); 
    return 0;
}

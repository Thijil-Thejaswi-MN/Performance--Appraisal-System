#include "appraisal_head.h"

int main() {
    Employee *root = NULL;
    Queue q;
    init_queue(&q);

    int choice;

    while (1) {
        print_menu();
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1: {
                int id;
                char name[NAME_LEN], dept[DEPT_LEN];

                printf("Enter Employee ID: ");
                scanf("%d", &id);

                printf("Enter Name: ");
                scanf(" %[^\n]", name);

                printf("Enter Department: ");
                scanf(" %[^\n]", dept);

                root = insert_employee(root, id, name, dept);
                break;
            }

            case 2: {
                int id;
                float score;

                printf("Enter Employee ID: ");
                scanf("%d", &id);

                if (search_employee(root, id) == NULL) {
                    printf("Employee ID %d not found. Cannot add appraisal.\n", id);
                    break;
                }

                printf("Enter Score (0-5): ");
                scanf("%f", &score);

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
                printf("Enter Employee ID to search: ");
                scanf("%d", &id);

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
                printf("Enter number of top performers to display: ");
                scanf("%d", &k);

                if (k <= 0) k = 5;
                display_top_performers(k);
                break;
            }

            case 7:
                printf("Exiting program.\n");
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}

#include "header.h"

int main() {
    Employee *root = NULL;
    Queue q;
    init_queue(&q);

    int choice;

    while (1) {
        print_menu();
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            int id;
            char name[NAME_LEN], dept[DEPT_LEN];
            printf("Enter Employee ID: ");
            scanf("%d", &id);
            printf("Enter Name: ");
            scanf(" %[^\n]", name);
            printf("Enter Department: ");
            scanf(" %[^\n]", dept);

            root = insert_employee(root, id, name, dept);
            printf("Employee added successfully!\n");
        }

        else if (choice == 2) {
            int id;
            float score;
            printf("Enter Employee ID: ");
            scanf("%d", &id);
            printf("Enter Score (0-5): ");
            scanf("%f", &score);

            enqueue(&q, id, score);
            printf("✅ Appraisal Request Added to Queue!\n");
        }

        else if (choice == 3) {
            process_appraisal(&q, root);
        }

        else if (choice == 4) {
            printf("\n--- Department Wise Report (Simplified) ---\n");
            // Basic (print employees by department manually)
            display_inorder(root);
        }

        else if (choice == 5) {
            printf("\n--- Employee List (In-Order BST) ---\n");
            display_inorder(root);
        }

        else if (choice == 6) {
            int id;
            printf("Enter Employee ID to search: ");
            scanf("%d", &id);

            Employee *e = search_employee(root, id);
            if (e)
                printf("Found: %s | Dept: %s | Avg: %.2f | Reviews: %d\n",
                       e->name, e->dept, e->avg_score, e->reviews);
            else
                printf("Employee not found!\n");
        }

        else if (choice == 7) {
            printf("Exited\n");
            break;
        }

        else if (choice == 8) {
            int k;
            printf("Enter how many top performers to display (default 5): ");
            scanf("%d", &k);
            if (k <= 0) k = 5;
            display_top_performers(k);
        }

        else {
            printf(" Invalid choice! Try again.\n");
        }
    }

    return 0;
}

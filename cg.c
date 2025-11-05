#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------- LINEAR DS 1: Array for KPIs ----------
#define MAX_KPI 5
char *kpiNames[MAX_KPI] = {"Punctuality", "Teamwork", "Productivity", "Creativity", "Communication"};
int kpiWeights[MAX_KPI] = {20, 20, 30, 15, 15}; // total = 100

// ---------- LINEAR DS 2: Linked List for Appraisals ----------
typedef struct Appraisal {
    int scores[MAX_KPI];
    float finalScore;
    struct Appraisal *next;
} Appraisal;

// ---------- NON-LINEAR DS: BST for Employees ----------
typedef struct Employee {
    int empID;
    char name[50];
    Appraisal *appraisals;  // linked list of appraisals
    struct Employee *left, *right;
} Employee;

// Function to create a new Employee node
Employee* createEmployee(int id, char *name) {
    Employee *newEmp = (Employee*)malloc(sizeof(Employee));
    newEmp->empID = id;
    strcpy(newEmp->name, name);
    newEmp->appraisals = NULL;
    newEmp->left = newEmp->right = NULL;
    return newEmp;
}

// Insert employee into BST
Employee* insertEmployee(Employee* root, int id, char *name) {
    if (root == NULL) return createEmployee(id, name);
    if (id < root->empID)
        root->left = insertEmployee(root->left, id, name);
    else if (id > root->empID)
        root->right = insertEmployee(root->right, id, name);
    return root;
}

// Search employee in BST
Employee* searchEmployee(Employee* root, int id) {
    if (root == NULL || root->empID == id) return root;
    if (id < root->empID) return searchEmployee(root->left, id);
    return searchEmployee(root->right, id);
}

// Add appraisal (linked list) for an employee
void addAppraisal(Employee *emp, int scores[]) {
    if (emp == NULL) {
        printf("Employee not found!\n");
        return;
    }
    Appraisal *newApp = (Appraisal*)malloc(sizeof(Appraisal));
    int total = 0;
    for (int i = 0; i < MAX_KPI; i++) {
        newApp->scores[i] = scores[i];
        total += (scores[i] * kpiWeights[i]) / 100;
    }
    newApp->finalScore = total;
    newApp->next = emp->appraisals;
    emp->appraisals = newApp;
    printf("Appraisal added for Employee %s (ID %d)\n", emp->name, emp->empID);
}

// Display employee performance history
void displayEmployee(Employee *emp) {
    if (emp == NULL) {
        printf("Employee not found!\n");
        return;
    }
    printf("\nEmployee ID: %d, Name: %s\n", emp->empID, emp->name);
    Appraisal *app = emp->appraisals;
    if (app == NULL) {
        printf("No appraisal records.\n");
        return;
    }
    int appraisalCount = 1;
    while (app != NULL) {
        printf("Appraisal %d:\n", appraisalCount++);
        for (int i = 0; i < MAX_KPI; i++)
            printf("   %s = %d\n", kpiNames[i], app->scores[i]);
        printf("   Final Score = %.2f\n", app->finalScore);
        app = app->next;
    }
}

// In-order traversal of BST to display all employees
void displayAllEmployees(Employee *root) {
    if (root == NULL) return;
    displayAllEmployees(root->left);
    printf("Employee ID: %d, Name: %s\n", root->empID, root->name);
    displayAllEmployees(root->right);
}

// --------------------- MAIN ---------------------
int main() {
    Employee *root = NULL;
    int choice, id, scores[MAX_KPI];
    char name[50];

    while (1) {
        printf("\n=== Automated Performance Appraisal System ===\n");
        printf("1. Add Employee\n");
        printf("2. Add Appraisal\n");
        printf("3. View Employee Details\n");
        printf("4. View All Employees\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                printf("Enter Employee ID: ");
                scanf("%d", &id);
                printf("Enter Employee Name: ");
                scanf(" %[^\n]s", name);
                root = insertEmployee(root, id, name);
                printf("Employee added successfully!\n");
                break;

            case 2:
                printf("Enter Employee ID to add appraisal: ");
                scanf("%d", &id);
                for (int i = 0; i < MAX_KPI; i++) {
                    printf("Enter score for %s (0-%d): ", kpiNames[i], kpiWeights[i]);
                    scanf("%d", &scores[i]);
                }
                addAppraisal(searchEmployee(root, id), scores);
                break;

            case 3:
                printf("Enter Employee ID to view details: ");
                scanf("%d", &id);
                displayEmployee(searchEmployee(root, id));
                break;

            case 4:
                printf("\n--- Employee List (In-order Traversal) ---\n");
                displayAllEmployees(root);
                break;

            case 5:
                printf("Exiting...\n");
                exit(0);

            default:
                printf("Invalid choice! Try again.\n");
        }
    }

    return 0;
}

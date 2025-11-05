#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------- LINEAR DS 1: Array for KPI Weights ----------------
#define MAX_KPI 5
#define MAX_EMP 50

char *kpiNames[MAX_KPI] = {"Punctuality", "Teamwork", "Productivity", "Creativity", "Communication"};
int kpiWeights[MAX_KPI] = {20, 20, 30, 15, 15}; // total = 100

// ---------------- LINEAR DS 2: Linked List for Appraisals ----------------
typedef struct Appraisal {
    int scores[MAX_KPI];
    float finalScore;
    struct Appraisal *next;
} Appraisal;

// ---------------- NON-LINEAR DS 1: BST for Employees ----------------
typedef struct Employee {
    int empID;
    char name[50];
    Appraisal *appraisals;
    struct Employee *left, *right;
} Employee;

// ---------------- NON-LINEAR DS 2: Heap for Ranking ----------------
typedef struct {
    int empID;
    char name[50];
    float score;
} RankEntry;

RankEntry heap[MAX_EMP];
int heapSize = 0;

// ---------------- BST OPERATIONS ----------------
Employee* createEmployee(int id, char *name) {
    Employee *newEmp = (Employee*)malloc(sizeof(Employee));
    newEmp->empID = id;
    strcpy(newEmp->name, name);
    newEmp->appraisals = NULL;
    newEmp->left = newEmp->right = NULL;
    return newEmp;
}

Employee* insertEmployee(Employee* root, int id, char *name) {
    if (root == NULL) return createEmployee(id, name);
    if (id < root->empID)
        root->left = insertEmployee(root->left, id, name);
    else if (id > root->empID)
        root->right = insertEmployee(root->right, id, name);
    return root;
}

Employee* searchEmployee(Employee* root, int id) {
    if (root == NULL || root->empID == id) return root;
    if (id < root->empID) return searchEmployee(root->left, id);
    return searchEmployee(root->right, id);
}

// ---------------- HEAP (MAX HEAP) OPERATIONS ----------------
void swap(RankEntry *a, RankEntry *b) {
    RankEntry temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(int index) {
    while (index > 0 && heap[index].score > heap[(index - 1) / 2].score) {
        swap(&heap[index], &heap[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

void heapifyDown(int index) {
    int largest = index;
    int left = 2 * index + 1, right = 2 * index + 2;

    if (left < heapSize && heap[left].score > heap[largest].score)
        largest = left;
    if (right < heapSize && heap[right].score > heap[largest].score)
        largest = right;

    if (largest != index) {
        swap(&heap[index], &heap[largest]);
        heapifyDown(largest);
    }
}

void insertHeap(int empID, char *name, float score) {
    heap[heapSize].empID = empID;
    strcpy(heap[heapSize].name, name);
    heap[heapSize].score = score;
    heapifyUp(heapSize);
    heapSize++;
}

void displayTopPerformers(int n) {
    if (heapSize == 0) {
        printf("No performance data available.\n");
        return;
    }

    printf("\n--- Top %d Performers ---\n", n);
    int limit = n < heapSize ? n : heapSize;
    for (int i = 0; i < limit; i++) {
        printf("%d) %s (ID: %d) - Score: %.2f\n",
               i + 1, heap[i].name, heap[i].empID, heap[i].score);
    }
}

// ---------------- APPRAISAL MANAGEMENT ----------------
void addAppraisal(Employee *emp, int scores[]) {
    if (emp == NULL) {
        printf("Employee not found!\n");
        return;
    }

    Appraisal *newApp = (Appraisal*)malloc(sizeof(Appraisal));
    float total = 0;
    for (int i = 0; i < MAX_KPI; i++) {
        newApp->scores[i] = scores[i];
        total += (scores[i] * kpiWeights[i]) / 100.0;
    }
    newApp->finalScore = total;
    newApp->next = emp->appraisals;
    emp->appraisals = newApp;

    insertHeap(emp->empID, emp->name, total); // add to ranking heap

    printf("\n✅ Appraisal added for %s (ID: %d) | Final Score = %.2f\n", emp->name, emp->empID, total);
}

void displayEmployee(Employee *emp) {
    if (emp == NULL) {
        printf("Employee not found!\n");
        return;
    }

    printf("\nEmployee ID: %d | Name: %s\n", emp->empID, emp->name);
    Appraisal *app = emp->appraisals;

    if (app == NULL) {
        printf("No appraisal records found.\n");
        return;
    }

    int appraisalCount = 1;
    while (app != NULL) {
        printf("Appraisal #%d:\n", appraisalCount++);
        for (int i = 0; i < MAX_KPI; i++)
            printf("   %s = %d\n", kpiNames[i], app->scores[i]);
        printf("   Final Score = %.2f\n", app->finalScore);
        app = app->next;
    }
}

void displayAllEmployees(Employee *root) {
    if (root == NULL) return;
    displayAllEmployees(root->left);
    printf("Employee ID: %d | Name: %s\n", root->empID, root->name);
    displayAllEmployees(root->right);
}

// ---------------- MAIN MENU ----------------
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
        printf("5. View Top Performers\n");
        printf("6. Exit\n");
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
                printf("\n--- Employee List (In-order Traversal of BST) ---\n");
                displayAllEmployees(root);
                break;

            case 5:
                displayTopPerformers(5); // top 5 performers
                break;

            case 6:
                printf("Exiting system... Goodbye!\n");
                exit(0);

            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}

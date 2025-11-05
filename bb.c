#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GOALS 5
#define MAX_NAME_LEN 50
#define MAX_COMMENT_LEN 200

// Goal stored in an array (linear data structure #1)
typedef struct {
    char description[100];
    int progress; // 0-100%
} Goal;

// Review stored in a linked list (linear data structure #2)
typedef struct ReviewNode {
    int rating; // 1-5
    char comments[MAX_COMMENT_LEN];
    struct ReviewNode *next;
} ReviewNode;

// Employee stored in BST node (non-linear data structure)
typedef struct EmployeeNode {
    int id;
    char name[MAX_NAME_LEN];
    char role[MAX_NAME_LEN];
    Goal goals[MAX_GOALS];
    int goal_count;
    ReviewNode *reviews_head;
    struct EmployeeNode *left;
    struct EmployeeNode *right;
} EmployeeNode;

EmployeeNode *root = NULL;

// --- BST functions for employees ---

EmployeeNode* createEmployee(int id, const char *name, const char *role) {
    EmployeeNode *newNode = (EmployeeNode*)malloc(sizeof(EmployeeNode));
    newNode->id = id;
    strncpy(newNode->name, name, MAX_NAME_LEN);
    strncpy(newNode->role, role, MAX_NAME_LEN);
    newNode->goal_count = 0;
    newNode->reviews_head = NULL;
    newNode->left = newNode->right = NULL;
    return newNode;
}

EmployeeNode* insertEmployee(EmployeeNode *node, int id, const char *name, const char *role) {
    if (node == NULL) {
        return createEmployee(id, name, role);
    }
    if (id < node->id) {
        node->left = insertEmployee(node->left, id, name, role);
    } else if (id > node->id) {
        node->right = insertEmployee(node->right, id, name, role);
    } else {
        printf("Employee with ID %d already exists.\n", id);
    }
    return node;
}

EmployeeNode* searchEmployee(EmployeeNode *node, int id) {
    if (node == NULL || node->id == id) return node;
    if (id < node->id) return searchEmployee(node->left, id);
    else return searchEmployee(node->right, id);
}

// --- Goal functions ---

void addGoal(EmployeeNode *e) {
    if (e->goal_count >= MAX_GOALS) {
        printf("Max goals reached for this employee.\n");
        return;
    }
    printf("Enter goal description: ");
    fgets(e->goals[e->goal_count].description, 100, stdin);
    e->goals[e->goal_count].description[strcspn(e->goals[e->goal_count].description, "\n")] = 0;
    e->goals[e->goal_count].progress = 0;
    e->goal_count++;
    printf("Goal added.\n");
}

void updateGoalProgress(EmployeeNode *e) {
    if (e->goal_count == 0) {
        printf("No goals set for this employee.\n");
        return;
    }
    printf("Goals:\n");
    for (int i = 0; i < e->goal_count; i++) {
        printf("%d. %s (Progress: %d%%)\n", i + 1, e->goals[i].description, e->goals[i].progress);
    }
    int goal_num;
    printf("Enter goal number to update: ");
    scanf("%d", &goal_num);
    getchar();
    if (goal_num < 1 || goal_num > e->goal_count) {
        printf("Invalid goal number.\n");
        return;
    }
    int progress;
    printf("Enter new progress (0-100): ");
    scanf("%d", &progress);
    getchar();
    if (progress < 0 || progress > 100) {
        printf("Invalid progress value.\n");
        return;
    }
    e->goals[goal_num - 1].progress = progress;
    printf("Goal progress updated.\n");
}

// --- Review functions (linked list) ---

void addReview(EmployeeNode *e) {
    ReviewNode *newReview = (ReviewNode*)malloc(sizeof(ReviewNode));
    printf("Enter rating (1-5): ");
    scanf("%d", &newReview->rating);
    getchar();
    if (newReview->rating < 1 || newReview->rating > 5) {
        printf("Invalid rating.\n");
        free(newReview);
        return;
    }
    printf("Enter comments: ");
    fgets(newReview->comments, MAX_COMMENT_LEN, stdin);
    newReview->comments[strcspn(newReview->comments, "\n")] = 0;
    newReview->next = NULL;

    // Insert at end of linked list
    if (e->reviews_head == NULL) {
        e->reviews_head = newReview;
    } else {
        ReviewNode *temp = e->reviews_head;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newReview;
    }
    printf("Review added.\n");
}

void printReviews(ReviewNode *head) {
    int count = 1;
    while (head != NULL) {
        printf("  Review %d: Rating: %d, Comments: %s\n", count, head->rating, head->comments);
        head = head->next;
        count++;
    }
}

// --- Report ---

void printEmployeeReport(EmployeeNode *e) {
    if (!e) {
        printf("Employee not found.\n");
        return;
    }
    printf("\n--- Performance Report for %s (ID: %d) ---\n", e->name, e->id);
    printf("Role: %s\n", e->role);
    printf("Goals:\n");
    for (int i = 0; i < e->goal_count; i++) {
        printf("  %d. %s - Progress: %d%%\n", i + 1, e->goals[i].description, e->goals[i].progress);
    }
    printf("Reviews:\n");
    if (e->reviews_head == NULL) {
        printf("  No reviews yet.\n");
    } else {
        printReviews(e->reviews_head);
    }
    printf("----------------------------------------\n");
}

// --- Utility functions ---

void addEmployee() {
    int id;
    char name[MAX_NAME_LEN];
    char role[MAX_NAME_LEN];
    printf("Enter employee ID: ");
    scanf("%d", &id);
    getchar();
    printf("Enter employee name: ");
    fgets(name, MAX_NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;
    printf("Enter role: ");
    fgets(role, MAX_NAME_LEN, stdin);
    role[strcspn(role, "\n")] = 0;

    root = insertEmployee(root, id, name, role);
    printf("Employee added.\n");
}

void menu() {
    printf("\nPerformance Appraisal System\n");
    printf("1. Add Employee\n");
    printf("2. Set Goals for Employee\n");
    printf("3. Update Goal Progress\n");
    printf("4. Add Performance Review\n");
    printf("5. View Employee Report\n");
    printf("6. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int choice;
    while (1) {
        menu();
        scanf("%d", &choice);
        getchar(); // consume newline
        int id;
        EmployeeNode *e;
        switch (choice) {
            case 1:
                addEmployee();
                break;
            case 2:
                printf("Enter employee ID to set goals: ");
                scanf("%d", &id);
                getchar();
                e = searchEmployee(root, id);
                if (e) addGoal(e);
                else printf("Employee not found.\n");
                break;
            case 3:
                printf("Enter employee ID to update goal progress: ");
                scanf("%d", &id);
                getchar();
                e = searchEmployee(root, id);
                if (e) updateGoalProgress(e);
                else printf("Employee not found.\n");
                break;
            case 4:
                printf("Enter employee ID to add review: ");
                scanf("%d", &id);
                getchar();
                e = searchEmployee(root, id);
                if (e) addReview(e);
                else printf("Employee not found.\n");
                break;
            case 5:
                printf("Enter employee ID to view report: ");
                scanf("%d", &id);
                getchar();
                e = searchEmployee(root, id);
                printEmployeeReport(e);
                break;
            case 6:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid option.\n");
        }
    }
    return 0;
}

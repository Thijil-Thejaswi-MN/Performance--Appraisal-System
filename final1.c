/*
 Automated Performance Appraisal System
 - Linear DS: Linked List (Queue) for appraisal requests
 - Non-linear DS: Binary Search Tree (BST) for employee records

 Build:
   gcc -std=c11 -O2 -Wall appraisal.c -o appraisal

 Author: (you can put your name)
 Date: (put date)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ------------------------
   Basic constants & types
   ------------------------ */

#define NAME_LEN 64
#define DEPT_LEN 32

typedef struct Employee {
    int id;                         // key
    char name[NAME_LEN];
    char dept[DEPT_LEN];
    int num_reviews;
    double total_score;             // sum of scores
    double avg_score;               // convenience: total_score / num_reviews
    char rating[16];                // "Excellent", "Good", ...
    struct Employee *left, *right;  // BST pointers
} Employee;

/* Appraisal request stored in queue */
typedef struct AppraisalReq {
    int emp_id;
    double score;
} AppraisalReq;

/* Queue node (linked list) */
typedef struct QNode {
    AppraisalReq req;
    struct QNode *next;
} QNode;

typedef struct Queue {
    QNode *front;
    QNode *rear;
} Queue;

/* ------------------------
   Utility / Validation
   ------------------------ */

void clear_input_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* read integer with validation; returns 1 on success */
int read_int(const char *prompt, int *out) {
    char buf[128];
    long val;
    char *endptr;
    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    val = strtol(buf, &endptr, 10);
    if (endptr == buf || (*endptr != '\n' && *endptr != '\0')) {
        return 0;
    }
    *out = (int)val;
    return 1;
}

/* read double with validation; returns 1 on success */
int read_double(const char *prompt, double *out) {
    char buf[128];
    double val;
    char *endptr;
    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    val = strtod(buf, &endptr);
    if (endptr == buf || (*endptr != '\n' && *endptr != '\0')) {
        return 0;
    }
    *out = val;
    return 1;
}

/* read trimmed string */
void read_string(const char *prompt, char *out, size_t maxlen) {
    printf("%s", prompt);
    if (!fgets(out, (int)maxlen, stdin)) {
        out[0] = '\0';
        return;
    }
    size_t ln = strlen(out);
    if (ln > 0 && out[ln-1] == '\n') out[ln-1] = '\0';
}

/* compute rating based on avg */
void compute_rating(Employee *e) {
    if (e->num_reviews == 0) {
        strcpy(e->rating, "None");
        e->avg_score = 0.0;
        return;
    }
    e->avg_score = e->total_score / e->num_reviews;
    double a = e->avg_score;
    if (a >= 4.5) strcpy(e->rating, "Excellent");
    else if (a >= 3.5) strcpy(e->rating, "Good");
    else if (a >= 2.5) strcpy(e->rating, "Average");
    else strcpy(e->rating, "Poor");
}

/* ------------------------
   BST: Employee functions
   ------------------------ */

/* create new employee node (heap alloc) */
Employee* new_employee(int id, const char *name, const char *dept) {
    Employee *e = (Employee*)malloc(sizeof(Employee));
    if (!e) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    e->id = id;
    strncpy(e->name, name, NAME_LEN-1); e->name[NAME_LEN-1] = '\0';
    strncpy(e->dept, dept, DEPT_LEN-1); e->dept[DEPT_LEN-1] = '\0';
    e->num_reviews = 0;
    e->total_score = 0.0;
    e->avg_score = 0.0;
    strcpy(e->rating, "None");
    e->left = e->right = NULL;
    return e;
}

/* insert into BST by id; returns root */
Employee* bst_insert(Employee *root, Employee *node, int *inserted) {
    if (root == NULL) {
        *inserted = 1;
        return node;
    }
    if (node->id < root->id) {
        root->left = bst_insert(root->left, node, inserted);
    } else if (node->id > root->id) {
        root->right = bst_insert(root->right, node, inserted);
    } else {
        // duplicate id not allowed
        *inserted = 0;
        // free node to avoid leak
        free(node);
    }
    return root;
}

/* search by id */
Employee* bst_search(Employee *root, int id) {
    while (root) {
        if (id == root->id) return root;
        else if (id < root->id) root = root->left;
        else root = root->right;
    }
    return NULL;
}

/* inorder print */
void bst_inorder_print(Employee *root) {
    if (!root) return;
    bst_inorder_print(root->left);
    printf("ID: %d | Name: %s | Dept: %s | Reviews: %d | Avg: %.2f | Rating: %s\n",
           root->id, root->name, root->dept, root->num_reviews, root->avg_score, root->rating);
    bst_inorder_print(root->right);
}

/* free BST */
void bst_free(Employee *root) {
    if (!root) return;
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

/* ------------------------
   Queue (Linked List) functions
   ------------------------ */

void init_queue(Queue *q) {
    q->front = q->rear = NULL;
}

int is_queue_empty(Queue *q) {
    return q->front == NULL;
}

/* enqueue O(1) */
int enqueue(Queue *q, AppraisalReq req) {
    QNode *node = (QNode*)malloc(sizeof(QNode));
    if (!node) return 0; // fail
    node->req = req;
    node->next = NULL;
    if (q->rear) q->rear->next = node;
    q->rear = node;
    if (!q->front) q->front = node;
    return 1;
}

/* dequeue O(1); returns 1 if succeeded */
int dequeue(Queue *q, AppraisalReq *out) {
    if (is_queue_empty(q)) return 0;
    QNode *tmp = q->front;
    *out = tmp->req;
    q->front = tmp->next;
    if (!q->front) q->rear = NULL;
    free(tmp);
    return 1;
}

/* free all nodes */
void queue_clear(Queue *q) {
    AppraisalReq dummy;
    while (dequeue(q, &dummy)) {}
}

/* ------------------------
   Functional Modules
   ------------------------ */

/* Easy #1: add_employee */
void add_employee(Employee **root_ptr) {
    int id;
    char name[NAME_LEN], dept[DEPT_LEN];
    if (!read_int("Enter Employee ID (positive integer): ", &id) || id <= 0) {
        printf("Invalid ID input.\n");
        clear_input_line();
        return;
    }
    read_string("Enter name: ", name, NAME_LEN);
    if (strlen(name) == 0) { printf("Name cannot be empty.\n"); return; }
    read_string("Enter department: ", dept, DEPT_LEN);
    if (strlen(dept) == 0) { printf("Dept cannot be empty.\n"); return; }

    Employee *node = new_employee(id, name, dept);
    int inserted = 0;
    *root_ptr = bst_insert(*root_ptr, node, &inserted);
    if (inserted) printf("Employee added successfully.\n");
    else printf("Employee with ID %d already exists. Not added.\n", id);
}

/* Easy #2: enqueue_appraisal */
void enqueue_appraisal(Queue *q) {
    int id;
    double score;
    if (!read_int("Employee ID for appraisal: ", &id) || id <= 0) {
        printf("Invalid ID.\n"); return;
    }
    if (!read_double("Score (0.0 - 5.0): ", &score) || score < 0.0 || score > 5.0) {
        printf("Invalid score. Must be between 0 and 5.\n"); return;
    }
    AppraisalReq req = { .emp_id = id, .score = score };
    if (enqueue(q, req)) printf("Appraisal request queued.\n");
    else printf("Failed to queue appraisal.\n");
}

/* Medium #1: process_next_appraisal */
void process_next_appraisal(Queue *q, Employee *root) {
    if (is_queue_empty(q)) {
        printf("No pending appraisal requests.\n");
        return;
    }
    AppraisalReq req;
    if (!dequeue(q, &req)) { printf("Failed to dequeue.\n"); return; }
    Employee *e = bst_search(root, req.emp_id);
    if (!e) {
        printf("Employee with ID %d not found. Appraisal discarded.\n", req.emp_id);
        return;
    }
    // Update employee record
    e->num_reviews += 1;
    e->total_score += req.score;
    compute_rating(e);
    printf("Processed appraisal for ID %d | Name: %s | New Avg: %.2f | Rating: %s\n",
           e->id, e->name, e->avg_score, e->rating);
}

/* Medium #2: generate_department_report
   This will traverse BST and compute:
     - number of employees per dept
     - total score and average per dept
     - print top performer(s) (highest avg) in each dept
*/
typedef struct DeptStat {
    char dept[DEPT_LEN];
    int count;
    double total_avg_sum;   // sum of avg scores for avg calculation or we can compute differently
    double top_avg;
    int top_emp_id;
    char top_emp_name[NAME_LEN];
    struct DeptStat *next;
} DeptStat;

/* find dept node in linked list; create if not exist */
DeptStat* find_or_create_dept(DeptStat **head, const char *dept) {
    DeptStat *cur = *head;
    while (cur) {
        if (strcmp(cur->dept, dept) == 0) return cur;
        cur = cur->next;
    }
    DeptStat *n = (DeptStat*)malloc(sizeof(DeptStat));
    if (!n) { fprintf(stderr, "Memory fail\n"); exit(EXIT_FAILURE); }
    strncpy(n->dept, dept, DEPT_LEN-1); n->dept[DEPT_LEN-1] = '\0';
    n->count = 0;
    n->total_avg_sum = 0.0;
    n->top_avg = -1.0;
    n->top_emp_id = -1;
    n->top_emp_name[0] = '\0';
    n->next = *head;
    *head = n;
    return n;
}

/* traverse bst and accumulate dept stats */
void dept_traverse_accumulate(Employee *root, DeptStat **head) {
    if (!root) return;
    dept_traverse_accumulate(root->left, head);
    DeptStat *ds = find_or_create_dept(head, root->dept);
    ds->count += 1;
    ds->total_avg_sum += root->avg_score;
    if (root->avg_score > ds->top_avg) {
        ds->top_avg = root->avg_score;
        ds->top_emp_id = root->id;
        strncpy(ds->top_emp_name, root->name, NAME_LEN-1);
        ds->top_emp_name[NAME_LEN-1] = '\0';
    }
    dept_traverse_accumulate(root->right, head);
}

/* print and free dept stat list */
void print_and_free_deptstats(DeptStat *head) {
    if (!head) { printf("No employees in system yet.\n"); return; }
    printf("Department-wise report:\n");
    DeptStat *cur = head;
    while (cur) {
        double avg_of_avgs = (cur->count > 0) ? (cur->total_avg_sum / cur->count) : 0.0;
        printf("Dept: %s | Employees: %d | Dept Avg Score: %.2f | Top: ID %d (%s) with %.2f\n",
               cur->dept, cur->count, avg_of_avgs, cur->top_emp_id, cur->top_emp_name, cur->top_avg);
        DeptStat *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
}

void generate_department_report(Employee *root) {
    DeptStat *head = NULL;
    dept_traverse_accumulate(root, &head);
    print_and_free_deptstats(head);
}

/* ------------------------
   Extra helpers: search & delete (simple delete by merging)
   ------------------------ */

/* find min in right subtree */
Employee* find_min(Employee *root) {
    while (root && root->left) root = root->left;
    return root;
}

Employee* bst_delete(Employee *root, int id, int *deleted) {
    if (!root) return NULL;
    if (id < root->id) root->left = bst_delete(root->left, id, deleted);
    else if (id > root->id) root->right = bst_delete(root->right, id, deleted);
    else {
        *deleted = 1;
        // Node found
        if (!root->left) {
            Employee *r = root->right;
            free(root);
            return r;
        } else if (!root->right) {
            Employee *l = root->left;
            free(root);
            return l;
        } else {
            Employee *succ = find_min(root->right);
            // copy succ data (not pointers)
            root->id = succ->id;
            strncpy(root->name, succ->name, NAME_LEN);
            strncpy(root->dept, succ->dept, DEPT_LEN);
            root->num_reviews = succ->num_reviews;
            root->total_score = succ->total_score;
            root->avg_score = succ->avg_score;
            strncpy(root->rating, succ->rating, sizeof(root->rating));
            // delete successor
            root->right = bst_delete(root->right, succ->id, deleted);
            // note: deleted flag was set twice but that's fine
        }
    }
    return root;
}

/* ------------------------
   Main menu
   ------------------------ */

void print_menu() {
    printf("\n=== Automated Performance Appraisal System ===\n");
    printf("1. Add employee (BST insert) [Easy]\n");
    printf("2. Submit appraisal (enqueue) [Easy]\n");
    printf("3. Process next appraisal (dequeue + update) [Medium]\n");
    printf("4. Generate department report [Medium]\n");
    printf("5. Display all employees (inorder)\n");
    printf("6. Search employee by ID\n");
    printf("7. Delete employee by ID\n");
    printf("8. Exit\n");
}

int main(void) {
    Employee *root = NULL;
    Queue q;
    init_queue(&q);
    int choice;

    while (1) {
        print_menu();
        if (!read_int("Select option: ", &choice)) {
            printf("Invalid selection.\n");
            continue;
        }
        switch (choice) {
            case 1:
                add_employee(&root);
                break;
            case 2:
                enqueue_appraisal(&q);
                break;
            case 3:
                process_next_appraisal(&q, root);
                break;
            case 4:
                generate_department_report(root);
                break;
            case 5:
                printf("Employees (inorder):\n");
                bst_inorder_print(root);
                break;
            case 6: {
                int id;
                if (!read_int("Enter employee ID to search: ", &id)) { printf("Invalid ID.\n"); break; }
                Employee *e = bst_search(root, id);
                if (!e) printf("Employee not found.\n");
                else printf("Found - ID %d | Name %s | Dept %s | Reviews %d | Avg %.2f | Rating %s\n",
                            e->id, e->name, e->dept, e->num_reviews, e->avg_score, e->rating);
                break;
            }
            case 7: {
                int id;
                if (!read_int("Enter employee ID to delete: ", &id)) { printf("Invalid ID.\n"); break; }
                int deleted = 0;
                root = bst_delete(root, id, &deleted);
                if (deleted) printf("Employee %d deleted.\n", id);
                else printf("Employee %d not found.\n", id);
                break;
            }
            case 8:
                printf("Exiting... freeing memory.\n");
                queue_clear(&q);
                bst_free(root);
                return 0;
            default:
                printf("Invalid option.\n");
        }
    }

    return 0;
}

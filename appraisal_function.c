#include "appraisal_head.h"
#include <ctype.h> // For isdigit

int MAX_EMPLOYEES_GLOBAL; // Defined here

// Dynamically allocated heap for top performers
RankEntry *heap = NULL;
static int heapSize = 0;

// Function to get a valid rating between 0 and 5 (float)
float get_valid_rating() {
    float score;
    char input_buffer[100];
    int valid_input;

    do {
        printf("Enter Score (0-5): ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            // Remove newline character if present
            input_buffer[strcspn(input_buffer, "\n")] = 0;

            // Check if input is empty
            if (strlen(input_buffer) == 0) {
                printf("Error: Input cannot be empty. Please enter a number between 0 and 5.\n");
                valid_input = 0;
                continue;
            }

            // Check if input contains non-digit characters (except for a single decimal point)
            int decimal_count = 0;
            int non_digit_found = 0;
            for (int i = 0; input_buffer[i] != '\0'; i++) {
                if (!isdigit(input_buffer[i])) {
                    if (input_buffer[i] == '.' && decimal_count == 0) {
                        decimal_count++;
                    } else {
                        non_digit_found = 1;
                        break;
                    }
                }
            }

            if (non_digit_found) {
                printf("Error: Invalid input. Please enter a numeric value between 0 and 5.\n");
                valid_input = 0;
                continue;
            }

            // Attempt to convert to float
            if (sscanf(input_buffer, "%f", &score) == 1) {
                if (score >= 0.0 && score <= 5.0) {
                    valid_input = 1;
                } else {
                    printf("Error: Rating must be between 0 and 5. Please try again.\n");
                    valid_input = 0;
                }
            } else {
                printf("Error: Invalid input. Please enter a numeric value between 0 and 5.\n");
                valid_input = 0;
            }
        } else {
            printf("Error reading input. Please try again.\n");
            valid_input = 0;
            // Clear stdin buffer in case of error
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    } while (!valid_input);
    return score;
}

// Function to get a positive integer
int get_positive_integer(const char *prompt) {
    int value;
    char input_buffer[100];
    int valid_input;

    do {
        printf("%s", prompt);
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            // Remove newline character if present
            input_buffer[strcspn(input_buffer, "\n")] = 0;

            // Check if input is empty
            if (strlen(input_buffer) == 0) {
                printf("Error: Input cannot be empty. Please enter a positive integer.\n");
                valid_input = 0;
                continue;
            }

            // Check if input contains non-digit characters
            int non_digit_found = 0;
            for (int i = 0; input_buffer[i] != '\0'; i++) {
                if (!isdigit(input_buffer[i])) {
                    non_digit_found = 1;
                    break;
                }
            }

            if (non_digit_found) {
                printf("Error: Invalid input. Please enter a positive integer.\n");
                valid_input = 0;
                continue;
            }

            // Attempt to convert to integer
            if (sscanf(input_buffer, "%d", &value) == 1) {
                if (value > 0) {
                    valid_input = 1;
                } else {
                    printf("Error: Value must be a positive integer. Please try again.\n");
                    valid_input = 0;
                }
            } else {
                printf("Error: Invalid input. Please enter a positive integer.\n");
                valid_input = 0;
            }
        } else {
            printf("Error reading input. Please try again.\n");
            valid_input = 0;
            // Clear stdin buffer in case of error
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    } while (!valid_input);
    return value;
}

// ---------- BST FUNCTIONS ----------
static Employee* create_node(int id, char name[], char dept[]) {
    Employee *e = (Employee*)malloc(sizeof(Employee));
    e->id = id;
    strcpy(e->name, name);
    strcpy(e->dept, dept);
    e->reviews = 0;
    e->total_score = 0;
    e->avg_score = 0;
    e->left = e->right = NULL;
    return e;
}

Employee* insert_employee(Employee *root, int id, char name[], char dept[]) {
    if (root == NULL)
        return create_node(id, name, dept);
    if (id < root->id)
        root->left = insert_employee(root->left, id, name, dept);
    else if (id > root->id)
        root->right = insert_employee(root->right, id, name, dept);
    else
        printf("Employee with ID %d already exists.\n", id);
    return root;
}

Employee* search_employee(Employee *root, int id) {
    while (root) {
        if (id == root->id) return root;
        else if (id < root->id) root = root->left;
        else root = root->right;
    }
    return NULL;
}

void display_inorder(Employee *root) {
    if (!root) return;
    display_inorder(root->left);
    printf("ID:%d | Name:%s | Dept:%s | Reviews:%d | Avg:%.2f\n",
           root->id, root->name, root->dept, root->reviews, root->avg_score);
    display_inorder(root->right);
}

// ---------- QUEUE FUNCTIONS ----------
void init_queue(Queue *q) {
    q->front = q->rear = NULL;
}

void enqueue(Queue *q, int id, float score) {
    QNode *n = (QNode*)malloc(sizeof(QNode));
    n->data.emp_id = id;
    n->data.score = score;
    n->next = NULL;
    if (q->rear) q->rear->next = n;
    q->rear = n;
    if (!q->front) q->front = n;
}

int dequeue(Queue *q, AppraisalReq *req) {
    if (!q->front) return 0;
    QNode *temp = q->front;
    *req = temp->data;
    q->front = temp->next;
    if (!q->front) q->rear = NULL;
    free(temp);
    return 1;
}

// ---------- HEAP FOR TOP PERFORMERS ----------
static void heap_swap(int i, int j) {
    RankEntry t = heap[i];
    heap[i] = heap[j];
    heap[j] = t;
}

static void heapify_up(int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap[i].score <= heap[p].score) break;
        heap_swap(i, p);
        i = p;
    }
}

static void heapify_down(int i) {
    while (1) {
        int l = 2*i + 1, r = 2*i + 2, largest = i;
        if (l < heapSize && heap[l].score > heap[largest].score) largest = l;
        if (r < heapSize && heap[r].score > heap[largest].score) largest = r;
        if (largest == i) break;
        heap_swap(i, largest);
        i = largest;
    }
}

void heap_insert(int id, char name[], float score) {
    int i;
    for (i = 0; i < heapSize; i++)
        if (heap[i].id == id) break;

    if (i < heapSize) {
        heap[i].score = score;
        strcpy(heap[i].name, name);
        heapify_up(i);
        heapify_down(i);
        return;
    }
    
    if (heapSize < MAX_EMPLOYEES_GLOBAL) { // Check against global max employees
        heap[heapSize].id = id;
        strcpy(heap[heapSize].name, name);
        heap[heapSize].score = score;
        heapify_up(heapSize);
        heapSize++;
    } else {
        printf("Heap is full. Cannot add more top performers.\n");
    }
}

void display_top_performers(int k) {
    if (heapSize == 0) {
        printf("No performance data available.\n");
        return;
    }
    if (k > heapSize) k = heapSize;

    // Create a temporary array for sorting, size based on MAX_EMPLOYEES_GLOBAL
    RankEntry *temp = (RankEntry*)malloc(sizeof(RankEntry) * MAX_EMPLOYEES_GLOBAL);
    if (temp == NULL) {
        printf("Memory allocation failed for temporary heap display.\n");
        return;
    }
    int ts = heapSize;
    memcpy(temp, heap, sizeof(RankEntry) * heapSize); // Copy only active elements

    // Build max-heap from temporary array
    for (int i = ts / 2 - 1; i >= 0; i--) {
        int l, r, largest, idx = i;
        while (1) {
            l = 2 * idx + 1;
            r = 2 * idx + 2;
            largest = idx;
            if (l < ts && temp[l].score > temp[largest].score) largest = l;
            if (r < ts && temp[r].score > temp[largest].score) largest = r;
            if (largest == idx) break;
            RankEntry t = temp[idx]; temp[idx] = temp[largest]; temp[largest] = t;
            idx = largest;
        }
    }

    for (int rank = 1; rank <= k; rank++) {
        RankEntry top = temp[0];
        printf("%d) %s (ID:%d) - Avg:%.2f\n", rank, top.name, top.id, top.score);
        temp[0] = temp[ts - 1];
        ts--;

        int idx = 0;
        while (1) {
            int l = 2 * idx + 1, r = 2 * idx + 2, largest = idx;
            if (l < ts && temp[l].score > temp[largest].score) largest = l;
            if (r < ts && temp[r].score > temp[largest].score) largest = r;
            if (largest == idx) break;
            RankEntry t = temp[idx]; temp[idx] = temp[largest]; temp[largest] = t;
            idx = largest;
        }
    }
    free(temp); // Free the temporary array
}

// ---------- PROCESS APPRAISAL ----------
void process_appraisal(Queue *q, Employee *root) {
    if (!q->front) {
        printf("No pending appraisal requests.\n");
        return;
    }

    AppraisalReq req;
    dequeue(q, &req);

    Employee *e = search_employee(root, req.emp_id);
    if (!e) {
        printf("Employee ID %d not found. Appraisal discarded.\n", req.emp_id);
        return;
    }

    e->reviews++;
    e->total_score += req.score;
    e->avg_score = e->total_score / e->reviews;

    heap_insert(e->id, e->name, e->avg_score);

    printf("Processed appraisal for %s (ID:%d) | New Avg: %.2f | Reviews: %d\n",
           e->name, e->id, e->avg_score, e->reviews);
}



void display_employee_ids(Employee *root) {
    if (!root) return;

    display_employee_ids(root->left);
    printf("%d ", root->id);
    display_employee_ids(root->right);
}

// ---------- MENU ----------
void print_menu() {
    printf("\n==============================\n");
    printf("  Automated Appraisal System  \n");
    printf("==============================\n");
    printf("1. Add Employee\n");
    printf("2. Submit Appraisal (Enqueue)\n");
    printf("3. Process Next Appraisal\n");
    printf("4. Display All Employees\n");
    printf("5. Search Employee by ID\n");
    printf("6. Top Performers\n");
    printf("7. Exit\n");
}

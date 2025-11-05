#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* =========================================================
   SIMPLE HELPERS (local / file-scope)
   ========================================================= */
static Employee* create_node(int id, char name[], char dept[]) {
    Employee *e = (Employee*)malloc(sizeof(Employee));
    e->id = id;
    strcpy(e->name, name);
    strcpy(e->dept, dept);
    e->total_score = 0.0f;
    e->reviews = 0;
    e->avg_score = 0.0f;
    e->left = e->right = NULL;
    return e;
}

/* -------------------- HEAP (file-scope) -------------------- */
static RankEntry heap[MAX_EMP];
static int heapSize = 0;

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

static int heap_find_index_by_id(int id) {
    for (int i = 0; i < heapSize; i++)
        if (heap[i].id == id) return i;
    return -1;
}

/* =========================================================
   BST FUNCTIONS
   ========================================================= */

Employee* insert_employee(Employee *root, int id, char name[], char dept[]) {
    if (root == NULL) {
        return create_node(id, name, dept);
    }
    if (id < root->id) {
        root->left = insert_employee(root->left, id, name, dept);
    } else if (id > root->id) {
        root->right = insert_employee(root->right, id, name, dept);
    } else {
        // Duplicate ID – just ignore insert (keep existing)
        printf("ID %d already exists. Skipping insert.\n", id);
    }
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

/* =========================================================
   QUEUE (LINKED LIST) FUNCTIONS
   ========================================================= */

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

/* =========================================================
   HEAP INTERFACE
   ========================================================= */

void heap_insert(int id, char name[], float score) {
    // If employee already in heap, update score and fix heap
    int idx = heap_find_index_by_id(id);
    if (idx != -1) {
        heap[idx].score = score;
        // Keep latest name (in case user edited spelling)
        strcpy(heap[idx].name, name);
        heapify_up(idx);
        heapify_down(idx);
        return;
    }
    // If not present and capacity available, add new
    if (heapSize < MAX_EMP) {
        heap[heapSize].id = id;
        strcpy(heap[heapSize].name, name);
        heap[heapSize].score = score;
        heapify_up(heapSize);
        heapSize++;
    } else {
        // Heap full: replace worst if this one is better
        int worst = 0;
        for (int i = 1; i < heapSize; i++)
            if (heap[i].score < heap[worst].score) worst = i;
        if (score > heap[worst].score) {
            heap[worst].id = id;
            strcpy(heap[worst].name, name);
            heap[worst].score = score;
            heapify_up(worst);
            heapify_down(worst);
        }
    }
}

void display_top_performers(int k) {
    if (heapSize == 0) {
        printf("No performance data available.\n");
        return;
    }
    if (k <= 0) k = 5;
    if (k > heapSize) k = heapSize;

    // Non-destructive: copy heap and pop max k times
    RankEntry temp[MAX_EMP];
    for (int i = 0; i < heapSize; i++) temp[i] = heap[i];
    int ts = heapSize;

    // local helpers for temp
    auto void temp_swap(int i, int j) {
        RankEntry t = temp[i]; temp[i] = temp[j]; temp[j] = t;
    }
    auto void temp_heapify_down(int i) {
        while (1) {
            int l = 2*i + 1, r = 2*i + 2, largest = i;
            if (l < ts && temp[l].score > temp[largest].score) largest = l;
            if (r < ts && temp[r].score > temp[largest].score) largest = r;
            if (largest == i) break;
            temp_swap(i, largest);
            i = largest;
        }
    }

    // Build heap property in temp if needed (simple bottom-up)
    for (int i = ts/2 - 1; i >= 0; i--) temp_heapify_down(i);

    printf("\n--- Top %d Performer(s) ---\n", k);
    for (int rank = 1; rank <= k; rank++) {
        RankEntry top = temp[0];
        printf("%d) %s (ID:%d) - Avg: %.2f\n", rank, top.name, top.id, top.score);
        temp[0] = temp[ts - 1];
        ts--;
        temp_heapify_down(0);
    }
}

/* =========================================================
   PROCESS APPRAISAL (DEQUEUE + UPDATE BST + HEAP)
   ========================================================= */

void process_appraisal(Queue *q, Employee *root) {
    if (!q->front) {
        printf("No pending appraisal requests.\n");
        return;
    }
    AppraisalReq req;
    if (!dequeue(q, &req)) {
        printf("Failed to read from queue.\n");
        return;
    }
    Employee *e = search_employee(root, req.emp_id);
    if (!e) {
        printf("Employee ID %d not found. Appraisal discarded.\n", req.emp_id);
        return;
    }
    // Update totals and average
    e->reviews += 1;
    e->total_score += req.score;
    e->avg_score = (e->reviews > 0) ? (e->total_score / e->reviews) : 0.0f;

    // Push/update into heap for ranking
    heap_insert(e->id, e->name, e->avg_score);

    printf("Processed appraisal for %s (ID:%d) | New Avg: %.2f | Reviews: %d\n",
           e->name, e->id, e->avg_score, e->reviews);
}

/* =========================================================
   MENU
   ========================================================= */

void print_menu() {
    printf("\n==============================\n");
    printf("  Automated Appraisal System  \n");
    printf("==============================\n");
    printf("1. Add Employee (BST Insert)\n");
    printf("2. Submit Appraisal (Enqueue)\n");
    printf("3. Process Next Appraisal\n");
    printf("4. Department-wise Report (Simple)\n");
    printf("5. Display All Employees (BST In-order)\n");
    printf("6. Search Employee by ID\n");
    printf("7. Exit\n");
    printf("8. View Top Performers (Heap)\n");
}

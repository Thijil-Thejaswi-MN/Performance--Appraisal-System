#include "appraisal_head.h"

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

    heap[heapSize].id = id;
    strcpy(heap[heapSize].name, name);
    heap[heapSize].score = score;
    heapify_up(heapSize);
    heapSize++;
}

void display_top_performers(int k) {
    if (heapSize == 0) {
        printf("No performance data available.\n");
        return;
    }
    if (k > heapSize) k = heapSize;

    RankEntry temp[MAX_EMP];
    int ts = heapSize;
    memcpy(temp, heap, sizeof(heap));

    for (int i = ts/2 - 1; i >= 0; i--) {
        int l, r, largest, idx = i;
        while (1) {
            l = 2*idx + 1;
            r = 2*idx + 2;
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
        temp[0] = temp[ts-1];
        ts--;

        int idx = 0;
        while (1) {
            int l = 2*idx + 1, r = 2*idx + 2, largest = idx;
            if (l < ts && temp[l].score > temp[largest].score) largest = l;
            if (r < ts && temp[r].score > temp[largest].score) largest = r;
            if (largest == idx) break;
            RankEntry t = temp[idx]; temp[idx] = temp[largest]; temp[largest] = t;
            idx = largest;
        }
    }
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

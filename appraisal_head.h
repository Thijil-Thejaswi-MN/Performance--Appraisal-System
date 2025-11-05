#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMP 50
#define NAME_LEN 50
#define DEPT_LEN 30

// Employee node (BST)
typedef struct Employee {
    int id;
    char name[NAME_LEN];
    char dept[DEPT_LEN];
    int reviews;
    float total_score;
    float avg_score;
    struct Employee *left, *right;
} Employee;

// Appraisal request (Queue)
typedef struct {
    int emp_id;
    float score;
} AppraisalReq;

typedef struct QNode {
    AppraisalReq data;
    struct QNode *next;
} QNode;

typedef struct {
    QNode *front, *rear;
} Queue;

// Rank heap entry
typedef struct {
    int id;
    char name[NAME_LEN];
    float score;
} RankEntry;

// Function declarations
Employee* insert_employee(Employee *root, int id, char name[], char dept[]);
Employee* search_employee(Employee *root, int id);
void display_inorder(Employee *root);

void init_queue(Queue *q);
void enqueue(Queue *q, int id, float score);
int dequeue(Queue *q, AppraisalReq *req);

void process_appraisal(Queue *q, Employee *root);

void heap_insert(int id, char name[], float score);
void display_top_performers(int k);

void print_menu();

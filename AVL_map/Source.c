#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Structura unui nod AVL
typedef struct AVLNode {
    int key;
    int height;
    struct AVLNode* left, * right;
} AVLNode;

// Functie pentru crearea unui nod nou
AVLNode* createNode(int key) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->key = key;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

// Functie pentru obtinerea inaltimii unui nod
int height(AVLNode* node) {
    return node ? node->height : 0;
}

// Functie pentru calcularea factorului de echilibru
int getBalance(AVLNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

// Rotire dreapta pentru reechilibrare
AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));
    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));

    return x;
}

// Rotire stanga pentru reechilibrare
AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));
    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));

    return y;
}

// Inserarea unui nod intr-un arbore AVL
AVLNode* insert(AVLNode* node, int key) {
    if (!node)
        return createNode(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else
        return node;

    node->height = 1 + (height(node->left) > height(node->right) ? height(node->left) : height(node->right));
    int balance = getBalance(node);

    // Reechilibrare
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Cautarea unui nod intr-un arbore AVL
int search(AVLNode* root, int key) {
    if (!root)
        return 0;
    if (root->key == key)
        return 1;
    if (key < root->key)
        return search(root->left, key);
    return search(root->right, key);
}

// Stergerea unui nod dintr-un arbore AVL
AVLNode* minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current->left)
        current = current->left;
    return current;
}

AVLNode* deleteNode(AVLNode* root, int key) {
    if (!root)
        return root;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        if (!root->left || !root->right) {
            AVLNode* temp = root->left ? root->left : root->right;

            if (!temp) {
                temp = root;
                root = NULL;
            }
            else
                *root = *temp;

            free(temp);
        }
        else {
            AVLNode* temp = minValueNode(root->right);
            root->key = temp->key;
            root->right = deleteNode(root->right, temp->key);
        }
    }

    if (!root)
        return root;

    root->height = 1 + (height(root->left) > height(root->right) ? height(root->left) : height(root->right));
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Functie pentru evaluarea performantelor
void evaluatePerformance(const char* filename) {
    AVLNode* root = NULL;
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Nu s-a putut deschide fisierul %s.\n", filename);
        return;
    }

    int value;
    clock_t start, end;

    // Masurarea timpului pentru inserare
    start = clock();
    while (fscanf(file, "%d", &value) != EOF) {
        root = insert(root, value);
    }
    end = clock();
    printf("Timp pentru inserare: %lf secunde\n", (double)(end - start) / CLOCKS_PER_SEC);

    rewind(file);

    // Masurarea timpului pentru cautare
    start = clock();
    while (fscanf(file, "%d", &value) != EOF) {
        search(root, value);
    }
    end = clock();
    printf("Timp pentru cautare: %lf secunde\n", (double)(end - start) / CLOCKS_PER_SEC);

    rewind(file);

    // Masurarea timpului pentru stergere
    start = clock();
    while (fscanf(file, "%d", &value) != EOF) {
        root = deleteNode(root, value);
    }
    end = clock();
    printf("Timp pentru stergere: %lf secunde\n", (double)(end - start) / CLOCKS_PER_SEC);

    fclose(file);
}

int main() {
    // Generarea fisierului random.txt cu valori aleatorii
    FILE* file = fopen("random.txt", "w");
    for (int i = 0; i < 100000; i++) {
        fprintf(file, "%d\n", rand() % 1000000);  // Genereaza valori intre 0 si 999999
    }
    fclose(file);

    // Generarea fisierului sorted_asc.txt cu valori crescatoare
    file = fopen("sorted_asc.txt", "w");
    for (int i = 0; i < 100000; i++) {
        fprintf(file, "%d\n", i);
    }
    fclose(file);

    // Generarea fisierului sorted_desc.txt cu valori descrescatoare
    file = fopen("sorted_desc.txt", "w");
    for (int i = 100000; i > 0; i--) {
        fprintf(file, "%d\n", i);
    }
    fclose(file);

    // Evaluarea performantelor pentru diferite fisiere
    printf("Evaluare pentru fisier cu valori aleatorii:\n");
    evaluatePerformance("random.txt");

    printf("\nEvaluare pentru fisier cu valori crescatoare:\n");
    evaluatePerformance("sorted_asc.txt");

    printf("\nEvaluare pentru fisier cu valori descrescatoare:\n");
    evaluatePerformance("sorted_desc.txt");

    return 0;
}

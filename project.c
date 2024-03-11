#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

#define MAX 512
#define COMMAND_NEW_STATION "aggiungi-stazione"
#define COMMAND_DELETE_STATION "demolisci-stazione"
#define COMMAND_NEW_CAR "aggiungi-auto"
#define COMMAND_DELETE_CAR "rottama-auto"
#define COMMAND_PATH "pianifica-percorso"

/*
 *  ---------------------------------------------------------------
 *  ---------------- Data Type Definitions ------------------------
 *  ---------------------------------------------------------------
 */
typedef struct elem {
    int dist;
    struct elem *next;
} Elem;

typedef struct data {
    int dist;
    int max;
} Data;

typedef struct {
    int distance;
    int numberCar;
    int maxCar;
    int* cars;
} Station;

typedef struct node {
    Station station;
    struct node *father;
    struct node *left;
    struct node *right;
} Node;

typedef Node* PtrNode;


/*
 *  ---------------------------------------------------------------
 *  ---------------- Function Declarations ------------------------
 *  ---------------------------------------------------------------
 */

//Parking handling
void add_car(int distance, int autonomy, Node *root);
void scrap_car(int distance, int autonomy, Node *root);
void create_parking(int *cars, Station *station);

//Station handling
Station* find_station(int distance, Node *root);
Node* create_station(int distance, int numberCar, int *cars, Node *root);
Node* demolish_station(int distance, Node *root);

//Path
void shortest_path_dx(Node *root, int start, int end);
void shortest_path_sx(Node *root, int start, int end);
void plan_route(Node *root, int start, int end);

//BST functions
Node* tree_insert(Node *newNode, Node *root);
Node* transplant(Node *root, Node *u, Node *v);
Node* tree_delete(Node *root, Node *z);
Node* tree_minimum(Node *root);
Node* tree_search(Node *root, int distance);
Data* get_inorder_vector(Node *root, int *size, int min, int max);
Data* get_desc_order_vector(Node *root, int *size, int min, int max);

//List Function
Elem* add_element(Elem *l, int data);
Elem* delete_list(Elem *l);
void print_list(Elem *l);


/*
 *  ---------------------------------------------------------------
 *  ---------------------------  Main  ----------------------------
 *  ---------------------------------------------------------------
 */  

int main() {
    FILE *file_input;
    file_input = stdin;
    if(file_input == NULL){
        printf("File not found \n");
        exit(1);
    }
    char command[20];
    int distance, numberCars, autonomy, s, d;
    int cars[512];
    Node *root = NULL;

    while(fscanf(file_input, "%s", command) != EOF){
        if(strcmp(command, COMMAND_NEW_STATION) == 0) {
            if(fscanf(file_input, "%d %d", &distance, & numberCars) != EOF){
                for(int i = 0; i < numberCars; i++){
                    if(fscanf(file_input, "%d", &cars[i]))
                        ;
                }
                root = create_station(distance, numberCars, cars, root);
            }

        } else if (strcmp(command, COMMAND_DELETE_STATION) == 0) {
            if(fscanf(file_input, "%d", &distance) != EOF){
                root = demolish_station(distance, root);
            }

        } else if (strcmp(command, COMMAND_NEW_CAR) == 0) {
            if(fscanf(file_input, "%d %d", &distance, &autonomy) != EOF){
                add_car(distance, autonomy, root);
            }

        } else if (strcmp(command, COMMAND_DELETE_CAR) == 0) {
            if(fscanf(file_input, "%d %d", &distance, &autonomy) != EOF){
                scrap_car(distance, autonomy, root);
            }

        } else if (strcmp(command, COMMAND_PATH) == 0) {
            if(fscanf(file_input, "%d %d", &s, &d) != EOF){
                plan_route(root, s, d);
            }

        }
    }

    fclose(file_input);
    return 0;
}



/*
 *  ---------------------------------------------------------------
 *  ------------------- Station Handling --------------------------
 *  ----------------- Function Definitions ------------------------
 *  ---------------------------------------------------------------
*/

Node* create_station(int distance, int numberCar, int *cars, Node *root){
    if(tree_search(root, distance)!=NULL || numberCar < 0){
        printf("non aggiunta\n");
        fflush(stdout);
        return root;
    }
    Node *newNode = malloc(sizeof(Node));
    if(newNode != NULL) {
        newNode->station.distance = distance;
        newNode->station.maxCar = 0;
        newNode->station.numberCar = numberCar;
        newNode->father = NULL;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->station.cars = NULL;
        if (numberCar != 0) {
            create_parking(cars, &(newNode->station));
        }
        root = tree_insert(newNode, root);
        printf("aggiunta\n");
        fflush(stdout);
    }
    return root;
}

Node* demolish_station(int distance, Node *root){
    Node *node = tree_search(root, distance);
    if(distance <= 0 || node == NULL){
        printf("non demolita\n");
        fflush(stdout);
        return root;
    }
    root = tree_delete(root, node);
    printf("demolita\n");
    fflush(stdout);
    return root;
}

Station* find_station(int distance, Node *root){
    return &(tree_search(root, distance)->station);
}



/*
 *  ---------------------------------------------------------------
 *  ------------------- Parking Handling --------------------------
 *  ----------------- Function Definitions ------------------------
 *  ---------------------------------------------------------------
*/

void create_parking(int *cars, Station *station){
    int max;
    station->cars = malloc(sizeof(int) * MAX);
    if (station->cars != NULL) {
        max = 0;
        for (int i = 0; i < station->numberCar; i++) {
            if (max < cars[i])
                max = cars[i];
            station->cars[i] = cars[i];
        }
        station->maxCar = max;
    }
}

void add_car(int distance, int autonomy, Node *root) {
    Station *station = find_station(distance, root);

    if(station==NULL || autonomy < 0 || station->numberCar == MAX){
        printf("non aggiunta\n");
        fflush(stdout);
    } else if (station->cars != NULL && station->numberCar < MAX) {
        station->numberCar++;
        (station->cars)[station->numberCar - 1] = autonomy;
        if (station->maxCar < autonomy) {
            station->maxCar = autonomy;
        }
        printf("aggiunta\n");
        fflush(stdout);
    } else if (station->cars == NULL) {
        station->numberCar = 1;
        station->maxCar = autonomy;
        create_parking(&autonomy, station);
        printf("aggiunta\n");
        fflush(stdout);
    }
}

void scrap_car(int distance, int autonomy, Node *root){
    Station *station = find_station(distance, root);

    if(station == NULL || station->cars == NULL || autonomy < 0){
        printf("non rottamata\n");
        fflush(stdout);
        return;
    }

    int max = 0;
    bool flag = 0;
    for(int i = 0; i < station->numberCar; i++){
        if(station->cars[i] == autonomy && !flag){
            flag = 1;
            for(int j = i; j < station->numberCar - 1; j++) {
                station->cars[j] = station->cars[j + 1];
                if(station->cars[j] > max) max = station->cars[j];
            }
            station->numberCar--;
        }
        if(station->cars[i] > max && !flag)
            max = station->cars[i];
    }
    station->maxCar = max;
    if(flag == 1) {
        printf("rottamata\n");
        fflush(stdout);
    } else {
        printf("non rottamata\n");
        fflush(stdout);
    }
}


/*
 *  ---------------------------------------------------------------
 *  --------------------- Shortest Path ---------------------------
 *  ----------------- Function Definitions ------------------------
 *  ---------------------------------------------------------------
*/

void shortest_path_dx(Node *root, int start, int end){
    int size;
    Elem *p = NULL;
    Data *arr = get_inorder_vector(root, &size, start, end);
    int last = size - 1;
    bool stop;
    p = add_element(p, end);
    while(last > 0){
        stop = 0;
        for(int i = 0; i < last && !stop; i++){
            if(arr[i].dist + arr[i].max >= arr[last].dist){
                stop = 1;
                p = add_element(p, arr[i].dist);
                last = i;
            }
        }
        if(!stop){
            last = -1;
            printf("nessun percorso\n");
            fflush(stdout);
        }
    }
    if(last == 0) print_list(p);
    free(arr);
    delete_list(p);
    p = NULL;
    arr = NULL;
}


void shortest_path_sx(Node *root, int start, int end){
    int *dist, *p;
    int size = 0;
    Data *arr = get_desc_order_vector(root, &size, end, start);
    if(size == 0 && arr == NULL){
        printf("nessun percorso\n");
    } else {
        if(arr[0].dist - arr[0].max > arr[1].dist){
            printf("nessun percorso\n");
            return;
        }
        dist = malloc(sizeof(int)*size);
        p = malloc(sizeof(int)*size);
        dist[0] = 0;
        p[0] = arr[0].dist;
        for(int i = 1; i < size; i++){
            dist[i]= INT_MAX;
            p[i] = INT_MAX;
        }
        bool stop;
        for(int i = 0; i < size; i++){
            stop = 0;
            for(int j = i+1; j < size && !stop; j++){
                if(arr[i].dist - arr[i].max <= arr[j].dist){
                    if((dist[j] > dist[i] + 1 && dist[i] != INT_MAX) || (dist[j] == dist[i] + 1 && p[j] > arr[i].dist)){
                        dist[j] = dist[i] + 1;
                        p[j] = arr[i].dist;
                    }
                } else {
                    stop = 1;
                }
            }
        }
        if(dist[size-1] == INT_MAX){
            printf("nessun percorso\n");
        } else { // da rivedere il ritrovamento del percorso
            int len = dist[size-1] + 1, j = size - 1;
            int res[len];
            res[0] = start;
            res[len - 1] = end;
            for(int i = len - 2; i > 0; i--){
                while(arr[j].dist != res[i+1])
                    j--;
                res[i] = p[j];
            }

            printf("%d", res[0]);
            for(int i = 1; i < len; i++){
                printf(" %d", res[i]);
            }
            printf("\n");
        }
        free(dist);
        free(p);
        dist = NULL;
        p = NULL;
    }
    free(arr);
    arr = NULL;
}


void plan_route(Node *root, int start, int end){
    if(start == end){
        printf("%d\n", start);
        fflush(stdout);
    } else if (start < end){
        shortest_path_dx(root, start, end);
    } else {
        shortest_path_sx(root, start, end);
    }
}



/*
 *  ---------------------------------------------------------------
 *  --------------------- BST Handling ----------------------------
 *  ----------------- Function Definitions ------------------------
 *  ---------------------------------------------------------------
*/

Node* tree_insert(Node *newNode, Node *root){
    if(root == NULL) //if bst is empty
        return newNode;

    Node *res, *f = NULL;
    res = root;
    while(root != NULL){
        f = root;
        if(newNode->station.distance < root->station.distance)
            root = root->left;
        else root = root->right;
    }
    newNode->father = f;
    if(f == NULL)
        root = newNode;
    else if (newNode->station.distance < f->station.distance)
        f->left = newNode;
    else f->right = newNode;
    return res;
}


Node* tree_minimum(Node *root){
    while(root->left != NULL)
        root = root->left;
    return root;
}


Node* transplant(Node *root, Node *u, Node *v){
    if(u->father == NULL)
        root = v;
    else if (u == u->father->left)
        u->father->left = v;
    else u->father->right = v;
    if (v != NULL)
        v->father = u->father;
    return root;
}


Node* tree_delete(Node *root, Node *z){
    if(z->left == NULL)
        root = transplant(root, z, z->right);
    else if(z->right == NULL)
        root = transplant(root, z, z->left);
    else {
        Node *y = tree_minimum(z->right);
        if(y->father != z) {
            root = transplant(root, y, y->right);
            y->right = z->right;
            y->right->father = y;
        }
        root = transplant(root, z, y);
        y->left = z->left;
        y->left->father = y;
    }
    free(z);
    z = NULL;
    return root;
}


Node* tree_search(Node *root, int distance){
    while(root != NULL && distance != root->station.distance){
        if (distance < root->station.distance)
            root = root->left;
        else root = root->right;
    }
    return root;
}

/*
 * A BST in-order traversal without recursion or a stack (just for fun!)
 */
Data* get_inorder_vector(Node *root, int *size, int min, int max){ 
    Node *pre, * current;
    Data *v = NULL;
    int added;
    if(root == NULL) {
        *size = 0;
        return NULL;
    }
    v = malloc(sizeof(Data) * 100);
    *size = 0;
    added = 100;
    current = root;
    while (current != NULL){
        if(current->left == NULL){
            if(current->station.distance <= max && min <= current->station.distance) {
                if (*size < added) {
                    v[*size].dist = current->station.distance;
                    v[*size].max = current->station.maxCar;
                    *size += 1;
                } else {
                    Data *tmp = realloc(v, sizeof(Data) * (*size + 100));
                    if (tmp != NULL) {
                        tmp[*size].dist = current->station.distance;
                        tmp[*size].max = current->station.maxCar;
                        v = tmp;
                        *size += 1;
                        added += 100;
                    }
                }
            }
            current = current->right;
        } else {
            pre = current->left;
            while(pre->right != NULL && pre->right != current)
                pre = pre->right;

            if(pre->right == NULL){
                pre->right = current;
                current = current->left;
            } else {
                pre->right = NULL;
                if(current->station.distance <= max && min <= current->station.distance) {
                    if (*size < added) {
                        v[*size].dist = current->station.distance;
                        v[*size].max = current->station.maxCar;
                        *size += 1;
                    } else {
                        Data *tmp = realloc(v, sizeof(Data) * (*size + 100));
                        if (tmp != NULL) {
                            tmp[*size].dist = current->station.distance;
                            tmp[*size].max = current->station.maxCar;
                            v = tmp;
                            *size += 1;
                            added += 100;
                        }
                    }
                }
                current = current->right;
            }
        }
    }
    return v;
}

/*
 * A BST inverse in-order traversal without recursion or a stack (just for fun!)
 */
Data* get_desc_order_vector(Node *root, int *size, int min, int max) {
    Node *pre, * current;
    Data *v = NULL;
    int added;
    if(root == NULL) {
        *size = 0;
        return NULL;
    }
    v = malloc(sizeof(Data) * 100);
    *size = 0;
    added = 100;
    *size = 0;
    current = root;
    while (current != NULL) {
        if (current->right == NULL) {
            if (current->station.distance <= max && min <= current->station.distance) {
                if (*size < added) {
                    v[*size].dist = current->station.distance;
                    v[*size].max = current->station.maxCar;
                    *size += 1;
                } else {
                    Data *tmp = realloc(v, sizeof(Data) * (*size + 100));
                    if (tmp != NULL) {
                        tmp[*size].dist = current->station.distance;
                        tmp[*size].max = current->station.maxCar;
                        v = tmp;
                        *size += 1;
                        added += 100;
                    }
                }
            }
            current = current->left;
        } else {
            pre = current->right;
            while (pre->left != NULL && pre->left != current)
                pre = pre->left;

            if (pre->left == NULL) {
                pre->left = current;
                current = current->right;
            } else {
                pre->left = NULL;
                if (current->station.distance <= max && min <= current->station.distance) {
                    if (*size < added) {
                        v[*size].dist = current->station.distance;
                        v[*size].max = current->station.maxCar;
                        *size += 1;
                    } else {
                        Data *tmp = realloc(v, sizeof(Data) * (*size + 100));
                        if (tmp != NULL) {
                            tmp[*size].dist = current->station.distance;
                            tmp[*size].max = current->station.maxCar;
                            v = tmp;
                            *size += 1;
                            added += 100;
                        }
                    }
                }
                current = current->left;
            }
        }
    }
    return v;
}


/*
 *  ---------------------------------------------------------------
 *  -------------------- List Handling ----------------------------
 *  ----------------- Function Definitions ------------------------
 *  ---------------------------------------------------------------
*/

Elem* add_element(Elem *l, int data) {
    Elem *tmp;
    tmp = malloc(sizeof(Elem));
    if (tmp != NULL) {
        tmp->dist = data;
        if (l == NULL)
            tmp->next = NULL;
        else tmp->next = l;
        return tmp;
    } else {
        printf("Error in malloc add_element\n");
        return l;
    }
}


Elem* delete_list(Elem *l) {
    Elem *tmp;
    while(l != NULL){
        tmp = l->next;
        free(l);
        l = tmp;
    }
    return NULL;
}


void print_list(Elem *l){
    if(l == NULL) {
        printf("NULL\n");
        fflush(stdout);
    } else {
        while(l->next != NULL){
            printf("%d ", l->dist);
            fflush(stdout);
            l = l->next;
        }
        printf("%d\n", l->dist);
        fflush(stdout);
    }
}

//
// Created by Peter Oo on 07/01/2023
//
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "node.h"

/**
 * DATA STURCTURE DECLARATIONS
*/
typedef struct node{
    char *label;
    int idx;
    int ht;
    struct node *left;
    struct node *right;
}Node;

Node *phoneBook = NULL;
Node *emailBook = NULL;
Node *nrcBook = NULL;
int *node_indexes = NULL;
int nIdx = 0;

/**
 * return max number
*/
int max(int a, int b){
    return (a > b)? a : b;
}
/**
 * string comparason: RETURN 1 if target string is greater than source string, -1 if less, 0 if same
*/
int compare_str(char *src, char *target){
    int sLen = stringLen(src);
    int tLen = stringLen(target);
    if(tLen > sLen)
        return 1;
    else if(tLen < sLen)
        return -1;
    else{
        for(int i=0; i<tLen; i++){
            if(target[i] > src[i])
                return 1;
            else if(target[i] < src[i])
                return -1;
        }
        return 0;
    }
}
/**
 * compare two string and return max string
*/
char * max_string(char *a, char *b){
    if(compare_str(a, b) == 1)
        return b;
    else 
        return a;
}
/**
 * compare two string and return min string
*/
char * min_string(char *a, char *b){
    if(compare_str(a, b) == -1)
        return b;
    else
        return a;
}
/**
 * get height of a node, and return
*/
int height(Node *root){
    if(root == NULL)
        return 0;
    else
        return root->ht;
}
/**
 * check balance of left's height and right's height, and return the balance factor
*/
int balance_factor(Node *root){
    int k = height(root->left) - height(root->right);
    return k;
}
/**
 * create a node for the key, and return the node
*/
Node *createNode(char *key, int index){
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->label = stringCopy(key);
    newNode->idx = index;
    newNode->ht = 1;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
/**
 * pivot on the node and perform left rotation
*/
void left_rotate(Node **a){
    Node *b = (*a)->right;
    (*a)->right = b->left;
    b->left = (*a);
    (*a)->ht = 1 + max(height((*a)->left), height((*a)->right));
    b->ht = 1 + max(height(b->left), height(b->right));
    (*a) = b;
}
/**
 * pivot on the node and perform right rotation
*/
void right_rotate(Node **b){
    Node *a = (*b)->left;
    (*b)->left = a->right;
    a->right = (*b);
    (*b)->ht = 1 + max(height((*b)->right), height((*b)->left));
    a->ht = 1 + max(height(a->left), height(a->right));
    (*b) = a;
}
/**
 * inserting a node to the tree and perform balance of the tree
*/
void insertNode(Node **root, char *key, int index){
    if((*root) == NULL){
        (*root) = createNode(key, index);
    }
    else{
        if(compare_str((*root)->label, key) == 1)
            insertNode(&(*root)->right, key, index);
        else if(compare_str((*root)->label, key) == -1)
            insertNode(&(*root)->left, key, index);
        else if(compare_str((*root)->label, key) == 0){
            printf(RED"[!] same key cannot be inserted to tree\n"G_RESET);
            exit(-1);
        }
        (*root)->ht = 1 + max(height((*root)->left), height((*root)->right));
        int k = balance_factor((*root));
        if(k < -1 && compare_str((*root)->right->label, key) == 1)
            left_rotate(&(*root));
        else if(k < -1 && compare_str((*root)->right->label, key) == -1){
            right_rotate(&(*root)->right);
            left_rotate(&(*root));
        }
        else if(k > 1 && compare_str((*root)->left->label, key) == -1)
            right_rotate(&(*root));
        else if(k > 1 && compare_str((*root)->left->label, key) == 1){
            left_rotate(&(*root)->left);
            right_rotate(&(*root));
        }
    }
}
/**
 * find the lowest node in the tree, and return the node
*/
Node * left_min_node(Node *root){
    while(root->right != NULL)
        root = root->right;
    return root;
}
/**
 * find a key similar and if found, delete that node, and balance the tree
*/
void deleteNode(Node **root, char *key){
    if((*root) != NULL){
        if(compare_str((*root)->label, key) == 1)
            deleteNode(&(*root)->right, key);
        else if(compare_str((*root)->label, key) == -1)
            deleteNode(&(*root)->left, key);
        else if(compare_str((*root)->label, key) == 0){
            if((*root)->left == NULL || (*root)->right == NULL){
                (*root) = ((*root)->left)? (*root)->left : (*root)->right;
            }
            else{
                Node *temp = left_min_node((*root)->left);
                (*root)->label = stringCopy(temp->label);
                (*root)->idx = temp->idx;
                deleteNode(&(*root)->left, temp->label);
            }
        }
        if((*root) != NULL){
            (*root)->ht = 1 + max(height((*root)->left), height((*root)->right));
            int k = balance_factor((*root));
            if(k < -1 && balance_factor((*root)->right) <= 0)
                left_rotate(&(*root));
            else if(k < -1 && balance_factor((*root)->right) > 0){
                right_rotate(&(*root)->right);
                left_rotate(&(*root));
            }
            else if(k > 1 && balance_factor((*root)->left) >= 0)
                right_rotate(&(*root));
            else if(k > 1 && balance_factor((*root)->left) < 0){
                left_rotate(&(*root)->left);
                right_rotate(&(*root));
            }
        }
    }
}
/**
 *  Search for the given key, and return the index, if not found, return -1
*/
int search_index(Node *root, char *key){
    while(root != NULL){
        if(compare_str(root->label, key) == 1)
            root = root->right;
        else if(compare_str(root->label, key) == -1)
            root = root->left;
        else
            return root->idx;
    }
    return -1;
}
void insertByEmail(char *email, int idx){
    insertNode(&emailBook, email, idx);
}
void insertByPhone(char *phone, int idx){
    insertNode(&phoneBook, phone, idx);
}
void insertByNRC(char *nrc, int idx){
    insertNode(&nrcBook, nrc, idx);
}
void deleteByEmail(char *email){
    deleteNode(&emailBook, email);
}
void deleteByPhone(char *phone){
    deleteNode(&phoneBook, phone);
}
void deleteByNRC(char *nrc){
    deleteNode(&nrcBook, nrc);
}
int searchByEmail(char *email){
    return search_index(emailBook, email);
}
int searchByPhone(char *phone){
    return search_index(phoneBook, phone);
}
int searchByNRC(char *nrc){
    return search_index(nrcBook, nrc);
}
void preOrder(Node *root){
    if(root != NULL){
        if(node_indexes == NULL)
            node_indexes = malloc(sizeof(int));
        node_indexes[nIdx++] = root->idx;
        node_indexes = realloc(node_indexes, (nIdx+1)*sizeof(int));
        node_indexes[nIdx] = -1;
        preOrder(root->left);
        preOrder(root->right);
    }
}
void traverseTree(){
    nIdx = 0;
    preOrder(emailBook);
    nIdx = 0;
}
int getIndexes(){
    if(node_indexes != NULL){
        if(node_indexes[nIdx] != -1)
            return node_indexes[nIdx++];
        else{
            nIdx = 0;
            free(node_indexes);
            node_indexes = NULL;
            return -1;
        }
    }
    else{
        return -1;
    }
}
/********************************************************************************************************/
/********************************************************************************************************/
/********************************************************************************************************/ 

/**
 * main function for testing purposes
*/
// int main(){
//     insertByEmail("peter@gmail.com", 11);
//     insertByEmail("merry@gmail.com", 10);
//     insertByEmail("amily@gmail.com", 2);
//     insertByEmail("magi@gmail.com", 6);
//     insertByEmail("penta@gmail.com", 4);
//     traverseTree();
//     int i = 0;
//     while((i = getIndexes()) != -1)
//         printf("%d ", i);
//     deleteByEmail("merry@gmail.com");
//     printf("\n");
//     traverseTree();
//     while((i = getIndexes()) != -1)
//         printf("%d ", i);
//     return 0;
// }
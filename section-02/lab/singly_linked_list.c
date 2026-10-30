#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node *create_node(int data)
{
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

void delete_node(Node **head, int data)
{
    Node *cur = *head, *prev = NULL;

    /* Return if the list is empty */
    if(cur == NULL) return;

    /* Delete if the first node matches */
    if(cur->data == data) 
    {
        *head = cur->next;
        free(cur);
        return;
    }

    /* Search for the node to delete */
    while(cur != NULL && cur->data != data)
    {
        prev = cur;
        cur = cur->next;
    }

    /* Delete the node if found */
    if(cur != NULL && cur->data == data)
    {
        prev->next = cur->next;
        free(cur);
    }
}


void insert_front(Node** head, int data)
{
    Node* new_node = create_node(data);
    new_node->next = *head;
    *head = new_node;
}

void insert_back(Node** head, int data)
{   
    if(*head == NULL) 
    {
        *head = create_node(data);
        return;
    }
    Node* cur = *head;
    
    /* Travel to the end of the linked list */
    while(cur->next != NULL) 
    {
        cur = cur->next;
    }
    /* Connect the end of the linked list to the new node */
    cur->next = create_node(data);
}

typedef void (*tranverse_func)(Node*); /* Function pointer type for traversing nodes */

void traverse_node(Node* head, tranverse_func func) 
{
    Node* cur = head;
    while(cur != NULL) 
    {
        func(cur);
        cur = cur->next;
    }
}

void print_node(Node* node)
{
    printf("%d\n", node->data);
}

int main()
{
    Node* head = NULL;
    insert_back(&head, 1);
    insert_back(&head, 2);
    insert_back(&head, 3);
    insert_back(&head, 4);
    insert_back(&head, 5);
    insert_front(&head, 1000);

    delete_node(&head, 3);
    traverse_node(head, print_node); // Output: 1000 1 2 4 5
}
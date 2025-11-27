#include <stdlib.h>
#include <stdio.h>
#include "list.h"

Node *head = NULL;
Node *cursor = NULL;
int list_size = 0;

static void prune_head() {
    if (head == NULL) return;

    Node *to_delete = head;

    if (cursor == to_delete) {
        cursor = NULL;
    }

    head = to_delete->next;
    if (head != NULL) {
        head->prev = NULL;
    }

    free(to_delete);
    list_size--;
}

void clear_list() {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
    cursor = NULL;
    list_size = 0;
}

void insert_char(char c) {
    if (list_size >= MAX_LIST_SIZE) {
        prune_head();
    }

    Node *new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) return;

    new_node->data = c;

    if (cursor == NULL) {
        new_node->prev = NULL;
        new_node->next = head;

        if (head != NULL) {
            head->prev = new_node;
        }
        head = new_node;
    } else {
        new_node->prev = cursor;
        new_node->next = cursor->next;

        if (cursor->next != NULL) {
            cursor->next->prev = new_node;
        }
        cursor->next = new_node;
    }

    cursor = new_node;
    list_size++;
}

void delete_char() {
    if (cursor == NULL) return;

    Node *to_delete = cursor;
    cursor = to_delete->prev;

    if (to_delete->prev != NULL) {
        to_delete->prev->next = to_delete->next;
    } else {
        head = to_delete->next;
    }

    if (to_delete->next != NULL) {
        to_delete->next->prev = to_delete->prev;
    }

    free(to_delete);
    list_size--;
}

void delete_char_forward() {
    Node *to_delete;

    if (cursor == NULL) {
        if (head == NULL) return;
        to_delete = head;
        head = to_delete->next;
        if (head != NULL) head->prev = NULL;
    } else {
        if (cursor->next == NULL) return;
        to_delete = cursor->next;
        cursor->next = to_delete->next;
        if (to_delete->next != NULL) to_delete->next->prev = cursor;
    }

    free(to_delete);
    list_size--;
}

void print_list() {
    Node *curr = head;
    
    printf("\rBuffer: "); 

    if (cursor == NULL) printf("|");

    while (curr != NULL) {
        printf("%c", curr->data);
        if (curr == cursor) printf("|");
        curr = curr->next;
    }

    printf("                ");
    fflush(stdout);
}
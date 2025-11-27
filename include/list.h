#ifndef LIST_H
#define LIST_H

#define MAX_LIST_SIZE 32

typedef struct Node {
    char data;
    struct Node *prev;
    struct Node *next;
} Node;

extern Node *head;
extern Node *cursor;
extern int list_size;

void insert_char(char c);
void delete_char();
void delete_char_forward();
void clear_list();
void print_list(); 

#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "macro_list.h"

list_node *macro_head = NULL;

list_node *create_list_node(char *name, char *value)
{
	list_node *new_node;
	new_node = (list_node *)malloc(sizeof(list_node));
	new_node->name = (char *)malloc(NAMELEN);
	new_node->value= (char *)malloc(BUFSIZ);
	new_node->next = NULL;
	strcpy(new_node->name, name);
	strcpy(new_node->value, value);
	return new_node;
}

void insert_tail(list_node *head, char *name, char *value)
{
	list_node *cur = head;

	if(head == NULL){
		return;
	}

	while(cur->next)
		cur = cur->next;

	cur->next = create_list_node(name, value);
}

list_node *find_node_from_name(list_node *head, char *name){
	list_node *cur = head;
	if(head == NULL)
		return NULL;

	while(cur){
		if(strcmp(cur->name, name) == 0)
			return cur;

		cur = cur->next;
	}
	return NULL;
}

void list_set_value(list_node *node, char *value){
	strcpy(node->value, value);
}

void print_list(list_node *head){
	list_node *cur;

	cur = head;
	while(cur){
		printf("%s->%s\n", cur->name, cur->value);
		cur = cur->next;
	}
}

void get_macro_value(char *macro, char *buf){
	list_node *cur = macro_head;

	while(cur){
		if(strcmp(cur->name, macro)==0){
			strcpy(buf, cur->value);
			return;
		}

		cur = cur->next;
	}
	strcpy(buf, "");
}

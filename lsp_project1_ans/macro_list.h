#ifndef MACRO_LIST_H_
#define MACRO_LIST_H_

#define NAMELEN 255
#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif

typedef struct list_node{
	char *name;
	char *value;
	struct list_node *next;
}list_node;


void remove_newline(char *str);
void insert_tail(list_node *head, char *name, char *value);
list_node *create_list_node(char *name, char *value);
list_node *find_node_from_name(list_node *head, char *name);
void list_set_value(list_node *node, char *value);
void print_list(list_node *node);
void get_macro_value(char *macro, char *buf);

#endif

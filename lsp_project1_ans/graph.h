#ifndef GRAPH_H_
#define GRAPH_H_

#define FILELEN 255
#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#ifndef unuse
	#define unuse 0
#endif

#define HAVE_DEPENDENCY(node) (strcmp(node->name, "") != 0)

typedef struct graph_node{
	struct graph_node *parent;
	struct child *child_list_head;
	struct command *command_list_head;
	char *name;
}graph_node;

typedef struct child{
	struct graph_node *node;
	struct child *next;
	struct child *prev;
}child;

typedef struct command{
	struct command *next;
	char *command;
}command;

typedef struct ancestor{
	char *name;
	int is_last_child;
} ancestor;

graph_node *create_graph_node(char *name);
void print_graph(graph_node *root_node, int len, int edge_type, int print_node);
void insert_child(graph_node *parent, graph_node *child);
child *create_child_node(graph_node *child_node);
void insert_command(graph_node *node, char *str);
void print_command();
void set_macro(graph_node *node);
int has_macro(char *command);
void set_user_macro(char *str);
void str_replace(char *str, char *old, char *new);
char *remove_extension(char *target_name);
char *remove_macro(char *str);
void copy_node(graph_node *dest, graph_node *org);
graph_node *get_root_child(char *node_name);
void push_ancestor(ancestor **ancestor_tbl, int *position, char *str, int is_last_child);
void pop_ancestor(ancestor **ancestor_tbl, int *position);
int check_ancestor(ancestor **ancestor_tbl, int *position);
void run_command(graph_node *node, int check_print_command, int is_first_child);
time_t get_child_recent_time(graph_node *node);
int has_next_node(graph_node *node);

#endif

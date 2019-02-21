#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include "graph.h"
#include "macro_list.h"

graph_node *root;
ancestor *target_ancestor[100];
ancestor *macro_ancestor[100];
ancestor *include_ancestor[100];
int target_position = 0;
int macro_position = 0;
int include_position = 0;

graph_node *create_graph_node(char *name){
	graph_node *new_node;

	new_node = (graph_node *)malloc(sizeof(graph_node));
	new_node->name = (char *)malloc(FILELEN);
	strcpy(new_node->name, name);
	new_node->parent = NULL;
	new_node->child_list_head = NULL;
	new_node->command_list_head = NULL;

	return new_node;
}

void print_graph(graph_node *node, int len, int edge_type, int print_node){
	char buf[BUFSIZ];
	graph_node *cur_node, *parent_node;
	child *cur;
	int next_edge_type;
	int i;
	int len2;

	memset(buf, 0, sizeof(buf));
	if(edge_type == 3 || edge_type == 4){
		memset(buf, ' ', len);
	}

	switch(edge_type){
		case 1:
			sprintf(buf+strlen(buf), "┳");
			len++;
			break;
		case 2:
			sprintf(buf+strlen(buf), "━");
			len++;
			break;
		case 3:
			sprintf(buf+strlen(buf), "┗");
			len++;
			break;
		case 4:
			sprintf(buf+strlen(buf), "┣");
			len++;
			break;
	}
	if(print_node)
		sprintf(buf+strlen(buf), "%s", node->name);

	len2 = 0;
	for(i = 0; target_ancestor[i] != NULL; i++){
		if((target_ancestor[i]->is_last_child ==false)&& buf[len2+1] == ' '){
			char pre[BUFSIZ], next[BUFSIZ];
			memset(pre, 0, sizeof(pre));
			memset(next, 0, sizeof(next));
			strncpy(pre, buf, len2);
			strcpy(next, buf+len2+1);
			sprintf(buf, "%s┃%s",pre, next);
			len2+=3;
		}
		if(i>0 && target_ancestor[i]->is_last_child==true)
			len2++;
		len2 += strlen(target_ancestor[i]->name);
	}

	printf("%s", buf);

	cur = node->child_list_head;
	if(cur == NULL || cur->node->name[0] == 0){//no child
		graph_node *root_child = get_root_child(node->name);
		if(node->parent != root && root_child != NULL){
			push_ancestor(target_ancestor, &target_position, node->name, !has_next_node(node));
			print_graph(root_child, len, next_edge_type, 0);
			pop_ancestor(target_ancestor, &target_position);
		}
		else
			printf("\n");
	}
	else{
		if(print_node)
			push_ancestor(target_ancestor, &target_position, node->name, !has_next_node(node));
		while(cur != NULL){
			next_edge_type = -1;
			if(cur->prev == NULL && cur->next != NULL)
				next_edge_type = 1;
			if(cur->prev == NULL && cur->next == NULL)
				next_edge_type = 2;
			if(cur->prev != NULL && cur->next == NULL)
				next_edge_type = 3;
			if(cur->prev != NULL && cur->next != NULL)
				next_edge_type = 4;
		
			if(check_ancestor(target_ancestor, &target_position) == false){
				printf("\n");
				break;
			}
			print_graph(cur->node, len+strlen(node->name), next_edge_type, true);
			cur = cur->next;
		}
		if(print_node)
			pop_ancestor(target_ancestor, &target_position);
	}
}

void insert_child(graph_node *parent_node, graph_node *child_node){
	child *cur;
	cur = parent_node->child_list_head;

	//if parent has no child
	if(cur == NULL){
		parent_node->child_list_head = create_child_node(child_node);
		parent_node->child_list_head->node->parent = parent_node;
		return;
	}

	while(cur->next)
		cur = cur->next;

	cur->next = create_child_node(child_node);
	cur->next->prev = cur;
	cur->next->node->parent = parent_node;
}

child *create_child_node(graph_node *child_node){
	child *new_node;

	new_node = (child *)malloc(sizeof(child));
	new_node->node = child_node;
	new_node->next = NULL;
	new_node->prev = NULL;
	return new_node;
}

void insert_command(graph_node *node, char *str){
	command *new_command, *cur;

	new_command = (command *)malloc(sizeof(command));
	new_command->command = malloc(BUFSIZ);
	strcpy(new_command->command, str);
	new_command->next = NULL;

	cur = node->command_list_head;

	if(cur == NULL)
		node->command_list_head = new_command;
	else{
		while(cur->next){
			cur = cur->next;
		}
		cur->next = new_command;
	}
}

void print_command(){
	child *cur = root->child_list_head;
	command *cur_command;

	while(cur){
		printf("target : %s\n", cur->node->name);

		cur_command = cur->node->command_list_head;
		while(cur_command){
			printf("\tcommand : %s\n", cur_command->command);
			cur_command = cur_command->next;
		}

		cur = cur->next;
	}
}

void set_macro(graph_node *node){
	child *cur = root->child_list_head;
	command *cur_command;
	list_node *macro_node;
	char cur_node_name[BUFSIZ];

	while(cur){
		//target
		set_user_macro(cur->node->name);

		//command
		cur_command = cur->node->command_list_head;
		while(cur_command){
			strcpy(cur_node_name, cur->node->name);
			//internal macro
			str_replace(cur_command->command, "$@", cur_node_name);
			str_replace(cur_command->command, "$*", remove_extension(cur_node_name));

			set_user_macro(cur_command->command);
			cur_command = cur_command->next;
		}

		cur = cur->next;
	}
}

void str_replace(char *str, char *old, char *new){
	char tmp[BUFSIZ];
	char *ptr;

	ptr = strstr(str, old);

	if(ptr == NULL)
		return;

	strcpy(tmp, ptr+strlen(old));
	strcpy(ptr, new);
	strcpy(str+strlen(str), tmp);
}

int has_macro(char *command){
	int i, j;

	for(i=0; i<strlen(command); i++){
		if(command[i] == '$' && command[i+1] == '('){
			for(j=i; j<strlen(command); j++)
				if(command[j] == ')')
					return true;
		}
	}

	return false;
}

void set_user_macro(char *str){
	int i;
	int start_index;
	char macro[BUFSIZ];
	char macro_name[BUFSIZ];
	char value[BUFSIZ];

	while(has_macro(str)){
		push_ancestor(macro_ancestor, &macro_position, str, unuse);
		//check circular macro
		if(check_ancestor(macro_ancestor, &macro_position) == false){
			fprintf(stderr, "'%s' has recursive macro\n", str);
			exit(1);
		}

		start_index = 0;
		for(i = 0; i < strlen(str); i++){
			if(str[i] == '$' && str[i+1] == '(')
				start_index = i+2;

			if(start_index != 0 && str[i] == ')'){
				memset(macro_name, 0, sizeof(macro_name));
				memcpy(macro_name, str+start_index, i-start_index);
				get_macro_value(macro_name, value);
				sprintf(macro, "$(%s)", macro_name);
				str_replace(str, macro, value);
				break;
			}
		}
	}
}

char *remove_extension(char *target_name){
	int i;

	for(i = strlen(target_name); i > 0; i--){
		if(target_name[i] == '.'){
			target_name[i] = 0;
			return target_name;
		}
	}
}

void copy_node(graph_node *dest, graph_node *org){
	dest->child_list_head = org->child_list_head;
	dest->command_list_head = org->command_list_head;
}

graph_node *get_root_child(char *node_name){
	child *cur;

	cur = root->child_list_head;

	if(cur == NULL)
		return NULL;
	while(cur != NULL){
		if(strcmp(node_name, cur->node->name) == 0)
			return cur->node;
		cur = cur->next;
	}

	return NULL;
}

void push_ancestor(ancestor **ancestor_tbl, int *position, char *str, int is_last_child){
	ancestor *new_ancestor;

	new_ancestor = (ancestor *)malloc(sizeof(ancestor));
	new_ancestor->name = malloc(BUFSIZ);
	strcpy(new_ancestor->name, str);
	new_ancestor->is_last_child = is_last_child;
	ancestor_tbl[(*position)++] = new_ancestor;
}

void pop_ancestor(ancestor **ancestor_tbl, int *position){
	(*position)--;
	free(ancestor_tbl[*position]->name);
	free(ancestor_tbl[*position]);
	ancestor_tbl[*position] = NULL;
}

int check_ancestor(ancestor **ancestor_tbl, int *position){
	char *str = ancestor_tbl[*position-1]->name;
	int i;

	for(i = 0; i < *position-1; i++){
		if(strcmp(ancestor_tbl[i]->name, str) == 0){
			return false;
		}
	}
	return true;
}

void run_command(graph_node *node, int check_print_command, int is_first_child){
	struct stat statbuf;
	time_t parent_time, child_time;
	command *cur;
	child *cur_child;
	graph_node *root_child;
	
	push_ancestor(target_ancestor, &target_position, node->name, false);
	if(check_ancestor(target_ancestor, &target_position) == false){
		pop_ancestor(target_ancestor, &target_position);
		printf("make: Circular %s <- %s dependency dropped.\n", target_ancestor[target_position-1]->name, node->name);
		return;
	}
	//run child command
	cur_child = node->child_list_head;
	while(cur_child != NULL){
		root_child = get_root_child(cur_child->node->name);
		if(root_child != NULL)
			run_command(root_child, check_print_command, false);
		else if(cur_child->node->name[0] != 0 && access(cur_child->node->name, F_OK) < 0){
			fprintf(stderr, "make: *** No rule to make target '%s', needed by '%s'.  Stop\n", cur_child->node->name, node->name);
			exit(1);
		}
		cur_child = cur_child->next;
	}
	pop_ancestor(target_ancestor, &target_position);

	//parent
	if(access(node->name, F_OK) == 0){//target is file
		if(stat(node->name, &statbuf) < 0){
			fprintf(stderr, "stat error for %s\n", node->name);
			exit(1);
		}
		parent_time = statbuf.st_mtime;
	}else{//target is not file
		parent_time = 0;
	}

	child_time = get_child_recent_time(node);

	//if child file is more recent than parent file, run command
	if(parent_time < child_time){
		cur = node->command_list_head;
		while(cur != NULL){
			if(check_print_command)
				printf("%s\n", cur->command);
			system(cur->command);
			cur = cur->next;
		}
	}
	else
		if(is_first_child)
			printf("make: '%s' is up to date.\n", node->name);
}

time_t get_child_recent_time(graph_node *node){
	struct stat statbuf;
	child *cur;
	time_t ret_time = 0;

	cur = node->child_list_head;
	if(cur == NULL)
		return INT_MAX;
	while(cur != NULL){
		if(access(cur->node->name, F_OK) == 0){
			stat(cur->node->name, &statbuf);
			if(ret_time < statbuf.st_mtime)
				ret_time = statbuf.st_mtime;
		}
		else
			ret_time = INT_MAX;
		cur = cur->next;
	}
	return ret_time;
}

int has_next_node(graph_node *node){
	graph_node *parent = node->parent;
	child *cur;

	if(parent == NULL)
		return false;

	cur = parent->child_list_head;
	while(cur != NULL){
		if(strcmp(cur->node->name, node->name) == 0)
			break;
		cur = cur->next;
	}

	if(cur->next != NULL)
		return true;
	else
		return false;
}

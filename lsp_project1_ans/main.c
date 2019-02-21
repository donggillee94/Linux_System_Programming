#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include "main.h"
#include "macro_list.h"
#include "graph.h"

int check_print_command = true;
int check_print_tree = false;
int check_print_macro = false;
extern list_node *macro_head;
extern graph_node *root;
regex_t regex1, regex2, regex3;
char *make_file = "Makefile";
char *cur_makefile;
char target_arr[TARGET_NUM][TARGET_LEN];
char macro_arr[MACRO_NUM][MACRO_LEN];
int macro_count = 0;
int target_count = 0;
int line_count = 0;
extern ancestor *include_ancestor[100];
extern int include_position;

int main(int argc, char*argv[])
{
	graph_node *node;
	char c;
	int ret;
	int i;

	init();

	//option
	while((c = getopt(argc, argv, "stmhf:c:")) != -1){
		switch(c){
			case 'f': //Makefile to optarg
				make_file = optarg;
				break;
			case 's':
				check_print_command = false;
				break;
			case 't':
				check_print_tree = true;
				break;
			case 'm':
				check_print_macro = true;
				break;
			case 'c':
				if(chdir(optarg) < 0){
					fprintf(stderr, "make: *** %s: No search file or directory. Stop\n", optarg);
					exit(1);
				}
				break;
			case 'h':
				print_usage();
				exit(0);
				break;
			case '?':
				if(optopt == 'f' || optopt == 'c'){
					exit(1);
				}
				else{
					printf("Unkonw flag : %c\n", optopt);
				}
				break;
		}
	}

	setNread_makefile(make_file, make_file, 0);

	if(HAS_TARGET() == false){
		fprintf(stderr, "make: *** No targets.  Stop.\n");
		exit(1);
	}
	save_targetNmacro(argc, argv);
	set_macro(root);

	if(check_print_tree){
		printf("---------------------graph----------------------\n");
		print_graph(root, 0, 0, true);
	}
	if(check_print_macro){
		printf("-----------------macro list---------------------\n");
		print_list(macro_head);
	}

	//run command whene t and m options are not set
	if(check_print_tree == false && check_print_macro == false){
		for(i = 0; i < target_count; i++){
			node = get_root_child(target_arr[i]);
			if(node != NULL){
				run_command(node, check_print_command, true);
			}
			if(node == NULL && access(target_arr[i], F_OK) < 0){
				fprintf(stderr, "make: *** No rule to make target '%s'.  Stop.\n", target_arr[i]);
				exit(0);
			}
		}
	}
}

void read_makefile(char *fname){
	graph_node *node;
	FILE *fp;
	char buf[BUFSIZ];
	
	if((fp = fopen(fname, "r")) == NULL){
		fprintf(stderr, "make: %s: No such file or directory\n", make_file);
		exit(1);
	}

	while(fgets(buf, BUFSIZ, fp) != NULL){
		line_count++;
		standardise_str(buf, fp);

		if(is_blank(buf) || IS_COMMENT(buf))//공백 | 주석
			continue;

		//target : <dependency 1> <dependency 2> ... <depepdency N>
		if(is_target(buf)){
			if(has_inner_macro(buf)){
				fprintf(stderr, "%s:%d: target and dependency can not use internal macro\n", cur_makefile, line_count);
				exit(1);
			}

			node = save_target(buf);
			save_command(node, fp);
		}
		//macro ?= value
		else if(is_ifexistmacro(buf)){
			save_macro(buf, true);
		}
		//macro = value
		else if(is_macro(buf)){
			save_macro(buf, false);
		}
		//include <filename1> <filename2> ... <filenameN>
		else if(is_include(buf)){
			char *mk_fname;
			char *str;

			//read included makefile
			mk_fname = strtok_r(buf+8, " \t", &str);
			setNread_makefile(fname, mk_fname, line_count);
			while((mk_fname = strtok_r(NULL, " \t", &str)) != NULL){
				setNread_makefile(fname, mk_fname, line_count);
			}
		}
		else{
			fprintf(stderr, "%s:%d: *** missing separator.  Stop.\n", cur_makefile, line_count);
			exit(1);
		}
	}
}

void standardise_str(char *buf, FILE *fp){
	char tmp_str[BUFSIZ];

	//remove_newline(buf); //remove \n
	remove_newline(buf);

	//remove \ and convert one line
	if(buf[strlen(buf)-1] == '\\'){
		fgets(tmp_str, BUFSIZ, fp);
		line_count++;
		sprintf(buf+strlen(buf)-1, " %s", tmp_str);

		standardise_str(buf, fp);
	}
}

void init(void)
{
	root = create_graph_node("root");

	//target : <dependency 1> <dependency 2> ... <depepdency N>
	if(regcomp(&regex1, "^[^:= ]+[ \t]*:[ \t]*[^:=]*$", REG_EXTENDED) < 0){
		fprintf(stderr, "regex1 error\n");
		exit(1);
	}

	//macro ?= value
	if(regcomp(&regex2, "^[^?:= \t]+[ \t]*[?]=[ \t]*[^?:= \t]+[^?:=]*$", REG_EXTENDED) < 0){
		fprintf(stderr, "regex2 error\n");
		exit(1);
	}

	//macro = value
	if(regcomp(&regex3, "^[^?:= \t]+[ \t]*=[ \t]*[^?:= \t]+[^?:=]*$", REG_EXTENDED) < 0){
		fprintf(stderr, "regex3 error\n");
		exit(1);
	}
}

void trim(char *str){
	int i;

	while(str[strlen(str)-1] == ' ' || str[strlen(str)-1] == '\t')
		str[strlen(str)-1] = 0;

	while(str[0] == ' ' || str[0] == '\t'){
		strcpy(str, str+1);
		str[strlen(str)] = 0;
	}
}

void save_macro(char *buf, int is_ifcond){
	char *l_str, *r_str;
 	list_node *node;

	if(is_ifcond)
		l_str = strtok(buf, "?");
	else
		l_str = strtok(buf, "=");
	r_str = strtok(NULL, "\n");

	trim(l_str);
	trim(r_str);

	node = find_node_from_name(macro_head, l_str);
	if(node == NULL){
		if(macro_head == NULL)
			macro_head = create_list_node(l_str, r_str);
		else
			insert_tail(macro_head, l_str, r_str);
	}
	else if(is_ifcond == false){
		list_set_value(node, r_str);
	}
 }

graph_node *save_target(char *buf){
	graph_node *child_node, *dependency_node;
	char *l_str, *r_str;
	char *dependency;

	l_str = strtok(buf, ":");
	trim(l_str);
	r_str = strtok(NULL, ":");

	if(get_root_child(l_str) != NULL){
		fprintf(stderr, "%s:%d: overlapping target error\n", cur_makefile, line_count);
		exit(1);
	}
	child_node = create_graph_node(l_str);

	//non dependency
	if(r_str == NULL || is_blank(r_str))
		dependency = "";
	else
		dependency = strtok(r_str, " \t");
	
	
	//save child node
	dependency_node = create_graph_node(dependency);
	insert_child(child_node, dependency_node);
	while((dependency = strtok(NULL, " \t")) != NULL){
		dependency_node = create_graph_node(dependency);
		insert_child(child_node, dependency_node);
	}
	insert_child(root, child_node);

	return child_node;
}

void save_targetNmacro(int argc, char *argv[]){
	int i;
	char str[BUFSIZ];

	str[0] = 0;
	for(i = optind; i < argc; i++){
		strcat(str, argv[i]);

		if(is_macro(str)){//macro
			if(macro_count >= MACRO_NUM){
				fprintf(stderr, "macro count error\n");
				exit(1);
			}
			strcpy(macro_arr[macro_count++], str);
			save_macro(str, false);
		}
		else{
			if(target_count >= TARGET_NUM){
				fprintf(stderr, "target count error\n");
				exit(1);
			}
			strcpy(target_arr[target_count++], str);
		}

		str[0] = 0;
	}
	if(target_count == 0){
		strcpy(target_arr[target_count++], root->child_list_head->node->name);
	}

}

int is_target(char *buf){
	return regexec(&regex1, buf,  0, NULL, 0) == 0;
}

int is_ifexistmacro(char *buf){
	return regexec(&regex2, buf, 0, NULL, 0) == 0;
}

int is_macro(char *buf){
	return regexec(&regex3, buf, 0, NULL, 0) == 0;
}

int is_include(char *buf){
	return strncmp(buf, "include ", 7) == 0;
}

int save_command(graph_node *node, FILE *fp){
	char buf[BUFSIZ];

	while(fgets(buf, sizeof(buf), fp) != NULL){
		line_count++;
		standardise_str(buf, fp);
		remove_newline(buf);
		if(is_blank(buf) || IS_COMMENT(buf))//공백 | 주석
			continue;

		//command end
		if(is_target(buf) || is_ifexistmacro(buf) || is_macro(buf) || is_include(buf)){
			line_count--;
			if(fseek(fp, -strlen(buf)-1, SEEK_CUR) < 0){
				fprintf(stderr, "fseek error\n");
				exit(1);
			}
			return 0;
		}

		//error
		if(buf[0] != '\t'){
			fprintf(stderr, "%s:%d: *** missing separator.  Stop.\n", cur_makefile, line_count);
			exit(1);
		}

		//remove \t
		trim(buf);

		insert_command(node, buf);
	}

	return 0;
}

void print_usage(){
	printf("Usage : ssu_make [Target] [Option] [Macro]\n");
	printf("Option:\n");
	printf(" -c <directory>\tChange to <directory> before doing anything.\n");
	printf(" -f <file>\tRead <file> as a makefile.\n");
	printf(" -h\t\tPrint this message and exit.\n");
	printf(" -s\t\tDo not print the commands as they are executed.\n");
	printf(" -m\t\tPrint macro list\n");
	printf(" -t\t\tPrint tree\n");
}

int is_blank(char *str){
	int i;

	for(i = 0 ; i< strlen(str); i++){
		if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
			return false;
	}
	
	return true;
}

void remove_newline(char *str){
	if(strlen(str) <= 0)
		return;
	if(str[strlen(str)-1] == '\n')
		str[strlen(str)-1] = 0;
}

void setNread_makefile(char *cur_name, char *include_name, int line){
	push_ancestor(include_ancestor, &include_position, include_name, unuse);
	if(check_ancestor(include_ancestor, &include_position) == false){
		fprintf(stderr, "recursive include : %s\n", include_name);
		exit(1);
	}

	cur_makefile = include_name;
	line_count = 0;
	read_makefile(include_name);
	line_count = line;
	cur_makefile = cur_name;
}

int has_inner_macro(char *str){
	if(strstr(str, "$*") == NULL && strstr(str, "$@") == NULL)
		return false;
	return true;
}

#ifndef MAIN_H_
#define MAIN_H

#include "graph.h"
#include "macro_list.h"

#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#define TARGET_NUM 5
#define MACRO_NUM 5
#define TARGET_LEN 100
#define MACRO_LEN 100

#define IS_COMMENT(str) (str[0]=='#'?true:false)
#define IS_EMPTY_STRING(str) (str[0]=='\n'||str[0] == 0?true:false)
#define HAS_TARGET() (root->child_list_head==NULL?false:true)

void init(void);
void trim(char *str);
void save_macro(char *buf, int is_ifcond);
graph_node *save_target(char *buf);
void save_targetNmacro(int argc, char *argv[]);
void setNread_makefile(char *cur_name, char *include_name, int line_count);
void read_makefile(char *fname);
void standardise_str(char *buf, FILE *fp);
int is_target(char *buf);
int is_macro(char *buf);
int is_ifexistmacro(char *buf);
int is_include(char *buf);
int save_command(graph_node *node, FILE *fp);
void print_usage();
int is_blank(char *str);
void remove_newline(char *str);
int has_inner_macro(char *str);

#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define BUFFER_SIZE 1024
#define MACRO_TABLE 128
#define TABLE_SIZE 128

typedef struct classify{
	int circular;
	char target[BUFFER_SIZE];
	char dependancy[BUFFER_SIZE];
	char command[BUFFER_SIZE];
}classify;

typedef struct macro{
	char macro_total[100];
	char macro_Lv[100];
	int macro_center;
	char macro_Rv[100];
}macro;

int total_line = 0, macro_line = 0, st_cnt = 0;
int line_read(int , char buf[]);
void target_input(macro *, classify *, char *, int *, int *);
void macro_classify(macro *, int);
void macro_transfer(macro *, classify *, int, int);
void doubled_macro(macro *, int);
void Question_macro(macro *, int *);
void overlap_macro(macro *, int *);
void inner_macro(classify *, int);
void command_call(classify, classify *, int);
void tokenized(classify *);
char *replaceMacro(char *, char *, char *);

int main(int argc, char *argv[])
{
	int i = 0, j = 0, k = 0, h = 0;
	int handle = 0, handle1 = 0;
	char target[TABLE_SIZE][BUFFER_SIZE];
	char input_macro[TABLE_SIZE][BUFFER_SIZE];
	char fname[BUFFER_SIZE] = "Makefile";
	char dir_path[BUFFER_SIZE];
	int flag_h = 0, flag_c = 0, flag_f = 0, flag_s = 0, flag_m = 0;
	int param_opt;
	char *dir_name;
	macro macro[TABLE_SIZE];
	classify line_st[TABLE_SIZE];

	while((param_opt = getopt(argc, argv, "mshc:f:")) != -1) {
		switch(param_opt) {
			case 'h':
				flag_h = 1;
				break;
			case 'f':
				flag_f = 1;
				memcpy(fname, optarg, 100);
				break;
			case 'c':
				flag_c = 1;
				dir_name = optarg;
				break;
			case 's':
				flag_s = 1;
				break;
			case 'm':
				flag_m = 1;
				break;
			case '?':
				printf("Unknown flag : %c\n", optopt);
				break;
		}
	}

	for(; optind < argc; optind++) {
		if(strstr(argv[optind], "=") == NULL) {
			strcpy(target[i], argv[optind]);
			i++;
			handle++;
		}
		else {
			char buf[BUFFER_SIZE];
			char *ptr;
			
			strcpy(input_macro[j], argv[optind]);
			j++;
			if(j > 5) {
				fprintf(stderr, "macro is more than 5.  .Stop.\n");
				exit(1);
			}
		}
	}
	if(flag_c) {
		chdir(dir_name);
	}
	if(flag_h) {
		printf("Usage : ssu_make [Target] [Option] [Macro]\n");
		printf("Option:\n");
		printf(" -f <file>	Use <file> as a makefile.\n");
		printf(" -c <directory>	Change to <directory> before reading the makefiles.\n");
		printf(" -s		Do not print the commands as they are excuted\n");
		printf(" -h		print usage\n");
		printf(" -m		print macro list\n");
		printf(" -t		print tree\n");
		exit(1);
	}
	if(flag_f) {
		if(0 == access(fname, F_OK)) {
			;
		}
		else {
			fprintf(stderr, "make: %s: No such file or directory\n", fname);
			exit(1);
		}
	}
	else if(!flag_f) {
		if(0 != access(fname, F_OK)) {
			fprintf(stderr, "make: %s: No such file or directory\n", fname);
			exit(1);
		}
	}

	target_input(macro ,line_st, fname,  &st_cnt, &macro_line);
	st_cnt++;
	macro_line++;
	macro_classify(macro, macro_line);
	Question_macro(macro, &macro_line);
	overlap_macro(macro, &macro_line);
	doubled_macro(macro, macro_line);
	inner_macro(line_st, st_cnt);
	tokenized(line_st);
	if(flag_m) {
		printf("-------------macro list--------------\n");
		for(int val11 = 1; val11 < macro_line; val11++){
			printf("%s -> %s\n", macro[val11].macro_Lv, macro[val11].macro_Rv);
		}
		exit(1);
	}
	if(st_cnt == 1) {
		fprintf(stderr, "make: *** No targets.   Stop\n");
		exit(1);
	}
	for(int val22 = 0; val22 < j; val22++) {
		char *ptr;
		ptr = strtok(input_macro[val22], "=");
		strcpy(macro[macro_line].macro_Lv, ptr);
		while(ptr = strtok(NULL, "=")) {
			strcpy(macro[macro_line].macro_Rv,ptr);
		}
		macro[macro_line].macro_center = 0;
		macro_line++;
	}
	overlap_macro(macro, &macro_line);
	macro_transfer(macro, line_st, macro_line, st_cnt);
	if(handle == 0) {
		command_call(line_st[1], line_st, flag_s);
	}
	else{
		for(k = 0; k < i; k++) {
			for(h = 1; h < st_cnt; h++){
				if(strcmp(line_st[h].target, target[k]) == 0) {
					command_call(line_st[h], line_st, flag_s);
					handle1++;
				}
			}
			if(handle1 == 0) {
				if(access(target[k], F_OK) == 0) {
					;
				}
				else {
					fprintf(stderr, "make: *** No rule to make target '%s'.\n", target[k]);
					exit(1);
				}
			}
			handle1 = 0;
		}
	}
}
void tokenized(classify *sp)
{
	int i, j;
	int k=0;
	for(i = 1; i < st_cnt; i++) {
		for(j = 0; j < (int)strlen(sp[i].dependancy); j++) {
			if(sp[i].dependancy[j] == '\t')
				sp[i].dependancy[j] = ' ';
		}
	}
	for(i = 1; i < st_cnt; i++) {
		char *ptr;
		ptr = sp[i].command;
		memcpy(sp[i].command, ptr+1, strlen(ptr));
	}
}

void command_call(classify sp, classify *sp2, int flag_s)
{	
	struct stat statbuf;
	struct stat statbuf1;
	int i, j; 
	int handle = 0, handle1 = 0;
	char *ptr; 
	char buf[BUFFER_SIZE];
	char buf1[BUFFER_SIZE];
	char command[TABLE_SIZE][BUFFER_SIZE];
	char dependancy[TABLE_SIZE][BUFFER_SIZE];
	time_t target_time, dependancy_time;
	int k = 0;
	for(int z = 1; z < st_cnt; z++) {
		if((strstr(sp.dependancy, sp2[z].dependancy)) != NULL && (strstr(sp.target, sp2[z].target) != NULL)) {
			sp2[z].circular = 1;
		}
	}

	if(strlen(sp.dependancy) > 0) {
		strcpy(buf, sp.dependancy);
		ptr = strtok(buf, " ");
		sprintf(dependancy[k], "%s",  ptr);
		k++;
		while(ptr = strtok(NULL, " ")){
			sprintf(dependancy[k], "%s", ptr);
			k++;
		}
	}
	for(i = k-1; i >= 0; i--) {
		if(!stat(sp.target, &statbuf)){
			target_time = statbuf.st_mtime;
		}
		if(!stat(dependancy[i], &statbuf1)){
			dependancy_time = statbuf1.st_mtime;
		}
		if(dependancy_time != 0) {
			if(target_time >= dependancy_time) {
				fprintf(stderr, "make:  '%s' is up to date.\n", sp.target);
				handle++;
			}
			else
				;
		}
	}
	for(i = 0; i < k; i++){
		for(j = 1; j < st_cnt; j++){
			if(strcmp(dependancy[i], sp2[j].target) == 0) {
				handle1++;
				if(sp2[j].circular == 1) {
					fprintf(stderr, "make: Circular %s <- %s dependancy dropped.\n", sp.target, dependancy[i]);
				}
				else{
					if(handle == 0)
						command_call(sp2[j], sp2, flag_s);
				}
			}
		}
		if(handle1 == 0){
			if(access(dependancy[i], F_OK) != 0) {
				fprintf(stderr, "make: *** No rule to make target '%s', needed by '%s'. Stop\n", dependancy[i], sp.target);
				exit(1);
			}
		}
	}
	for(i = 1; i < st_cnt; i++) {
		sp2[i].circular = 0;
	}
	int vl = 0;
	if(strlen(sp.command) > 0) {
		strcpy(buf1, sp.command);
		ptr = strtok(buf1, "'\t");
		sprintf(command[vl], "%s", ptr);
		vl++;
		while(ptr = strtok(NULL, "\t")) {
			sprintf(command[vl], "%s", ptr);
			vl++;
		}
	}
	for(i = 0; i < vl; i++) {
		if(handle == 0) {
			if(flag_s == 0)
				printf("%s\n", command[i]);
			system(command[i]);
		}
	}

}
void inner_macro(classify *sp, int st_cnt)
{
	int i;
	char *pattern1 = "$*";
	char *pattern2 = "$@";
	
	for(i = 1; i < st_cnt; i++) {
		if(strstr(sp[i].command, pattern2) != NULL) {
			strcpy(sp[i].command, replaceMacro(sp[i].command, pattern2, sp[i].target));
		}
		else if(strstr(sp[i].command, pattern1) != NULL) {
			char buf[100];
			for(int j = 0; j < strlen(sp[i].target); j++) {
				buf[j] = sp[i].target[j];
				if(sp[i].target[j] == '.')
					buf[j] = '\0';
			}
			strcpy(sp[i].command, replaceMacro(sp[i].command, pattern1, buf));

		}
	}
}
void overlap_macro(macro *macro, int *macro_line)
{
	int i, j;
	int handle = 0;
		for(i = 1; i < *macro_line; i++) {
			for(j = i+1; j < *macro_line; j++) {
				if(i == j) {
					;
				}
				else {
					if(strcmp(macro[i].macro_Lv, macro[j].macro_Lv) == 0) {
						strcpy(macro[i].macro_Rv, macro[j].macro_Rv);
						handle = 1;
					}
				}
				if(handle == 1) {
					for(int k = j+1; k < *macro_line; k++) {
						macro[k-1].macro_center = macro[k].macro_center;
						strcpy(macro[k-1].macro_Lv, macro[k].macro_Lv);
						strcpy(macro[k-1].macro_Rv, macro[k].macro_Rv);
					}
					(*macro_line)--;
					handle = 0;
					j--;
				}
			}
		}
}
void Question_macro(macro *macro, int *macro_line)
{
	int i, j;
	int handle = 0;
	for(i = 1; i < *macro_line; i++) {
		if(macro[i].macro_center == 1) {
			for(j = 1; j < *macro_line; j++) {
				if(i == j) {
					;
				}
				else{
					if(strcmp(macro[j].macro_Lv, macro[i].macro_Lv) == 0) {
						handle = 1;
					}
					else {
						macro[i].macro_center = 0;
					}
				}
			}
			if(handle == 1) {
				for(int k = i+1; k < *macro_line; k++) {
					macro[k-1].macro_center = macro[k].macro_center;
					strcpy(macro[k-1].macro_Lv, macro[k].macro_Lv);
					strcpy(macro[k-1].macro_Rv, macro[k].macro_Rv);
				}
				(*macro_line)--;
				handle = 0;
			}
		}
	}
}

void doubled_macro(macro *macro, int macro_line)
{	
	int i, j;
	int handle = 0;
	char changed[macro_line][1024];
	for(i = 1; i < macro_line; i++) {
		sprintf(changed[i], "$(%s)", macro[i].macro_Lv);
	}
	while(1) {
		handle = 0;
		for(i = 1; i < macro_line; i++) {
			for(j = 1; j < macro_line; j++) {
				if(strstr(macro[j].macro_Lv, changed[i]) != NULL) {
					strcpy(macro[j].macro_Lv, replaceMacro(macro[j].macro_Lv, changed[i], macro[i].macro_Rv));
					handle++;
				}
				if(strstr(macro[j].macro_Rv, changed[i]) != NULL) {
					strcpy(macro[j].macro_Rv, replaceMacro(macro[j].macro_Rv, changed[i], macro[i].macro_Rv));
					handle++;
				}
			}
		}
		if(handle == 0)
			break;
	}	
}

char *replaceMacro(char *input, char *target, char *change)
{
	char *result;
	char *strTemp;
	int i = 0, nCount = 0;
	int targetLength = strlen(target);

	if(targetLength < 1)
		return input;

	int changeLength = strlen(change);

	if(changeLength != targetLength) {
		for(i = 0; input[i] != '\0';) {
			if(memcmp(&input[i], target, targetLength) == 0) {
				nCount++;
				i +=targetLength;
			}
			else
				i++;
		}
	}
	else
		i = strlen(input);

	result = (char *) malloc(i+1+nCount+(changeLength-targetLength));
	if(result == NULL)
		return NULL;

	strTemp = result;

	while(*input) {
		if(memcmp(input, target, targetLength) == 0) {
			memcpy(strTemp, change, changeLength);
			strTemp += changeLength;
			input += targetLength;
		}
		else
			*strTemp++ = *input++;
	}
	*strTemp = '\0';

	return result;
}

void macro_transfer(macro *macro, classify *line, int macro_line, int st_cnt)
{
	int i, j;
	int handle = 0;
	char changed[macro_line][1024];
	for(i = 1; i < macro_line ; i++) {
		sprintf(changed[i], "$(%s)", macro[i].macro_Lv);
	}
	while(1) {
		handle = 0;
		for(i = 1; i < macro_line; i++) {
			for(j = 1; j < st_cnt; j++) {
				if(strstr(line[j].target, changed[i]) != NULL) {
					strcpy(line[j].target,replaceMacro(line[j].target, changed[i], macro[i].macro_Rv));
					handle++;
				}
				if(strstr(line[j].dependancy, changed[i]) != NULL) {
					strcpy(line[j].dependancy, replaceMacro(line[j].dependancy, changed[i], macro[i].macro_Rv));
					handle++;
				}
				if(strstr(line[j].command, changed[i]) != NULL) {
					strcpy(line[j].command, replaceMacro(line[j].command, changed[i], macro[i].macro_Rv));
					handle++;
				}
			}
		}
		if(handle == 0)
			break;
	}
}

void macro_classify(macro *macro, int macro_line)
{
	int i = 0, handle = 0, macro_cnt = 1, j = 0;
	char ch;
	int length;

	for(macro_cnt = 1; macro_cnt < macro_line; macro_cnt++){
		length = strlen(macro[macro_cnt].macro_total);
		while(1){
			ch = macro[macro_cnt].macro_total[i];
			i++;
			if(!handle){
				j++;
				macro[macro_cnt].macro_Lv[j-1] = ch;
				if(ch == ' ')
					if(i < length)
					macro[macro_cnt].macro_Lv[j-1] = 0;
				if(ch == '='){
					macro[macro_cnt].macro_Lv[j-1] = 0;
					handle = 1;
					j = 0;
				}
				if(ch == '?'){
					macro[macro_cnt].macro_Lv[j] = 0;
					macro[macro_cnt].macro_center = 1;
				}
			}
			else
			{
				j++;
				macro[macro_cnt].macro_Rv[j-1] = ch;
				if(ch == ' ' && j > 1)
					;
				else if(ch == ' ')
					j = 0;
			}
			if(i == length){
				handle = 0;
				j = 0;
				i = 0;
				break;
			}
		}
	}
}

void target_input(macro *macro, classify *sp, char *file, int *st_cnt, int *macro_line)
{	
	regex_t state_colon;
	regex_t state_tab;
	regex_t state_equal;
	regex_t state_include;
	regex_t state_sh;
	int kkk = 1, long_sh = 1;
	int line = 0;
	int handle = 1, handle2 = 0, handle3 = 0, enter_cnt = 0, handle4 = 0;
	int loop = 0;
	int fd, fd2;
	int command_line = 0;
	int is_macro = 0;
	int target_in = 0, dependancy_in = 0;
	int buf_count = 0;
	char ch, buf[BUFFER_SIZE], macro_buf[BUFFER_SIZE];
	
	char *pattern_sh = "^#";
	char *pattern_tab = "^	";
	char *pattern_colon = ":+";
	char *pattern_equal = "=+";
	char *pattern_include = "^include";

	if((fd = open(file, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", file);
		exit(1);
	}
	if((fd2 = open(file, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", file);
		exit(1);
	}
	

	while(1) {
		line++;
		total_line++;
		regcomp(&state_sh, pattern_sh, REG_EXTENDED);
		regcomp(&state_equal, pattern_equal, REG_EXTENDED);
		regcomp(&state_tab, pattern_tab, REG_EXTENDED);
		regcomp(&state_colon, pattern_colon, REG_EXTENDED);
		regcomp(&state_include, pattern_include , REG_EXTENDED);

		if(line_read(fd2, buf) == 0)
			break;
		
		int buf_size = strlen(buf);
		int status_colon = regexec(&state_colon, buf, 0, NULL, 0);
		int status_tab = regexec(&state_tab, buf, 0, NULL, 0);
		int status_equal = regexec(&state_equal, buf, 0 , NULL, 0);
		int status_include = regexec(&state_include, buf, 0, NULL, 0);
		int status_sh = regexec(&state_sh, buf, 0, NULL, 0);
		if(status_sh == 0){
			while(1) {
				read(fd, &ch, sizeof(ch));
				if(ch == '\n') {
					break;
				}
			}
		}
		else {
			if(status_include == 0) {
				char include_fname[TABLE_SIZE][BUFFER_SIZE];
				char *ptr = strtok(buf, " ");
				ptr = strtok(NULL, " ");
				int include_cnt = 0;
				while(ptr != NULL) {
					strcpy(include_fname[include_cnt], ptr);	
					for(int val_1 = 0; val_1 < strlen(include_fname[include_cnt]); val_1++) {
						if(include_fname[include_cnt][val_1] == '#'){
							include_fname[include_cnt][val_1] = '\0';
						}
					}
					target_input(macro, sp, include_fname[include_cnt], st_cnt, macro_line);
					ptr = strtok(NULL, " ");

					include_cnt++;
				}
				is_macro = 0;
			}
			else {
				if(status_equal == 0)
					handle4 = 0;
				if(status_colon == 0) {
					regex_t state_empty;
					char *pattern_empty = "^ ";
					regcomp(&state_empty, pattern_empty, REG_EXTENDED);
					int status_empty = regexec(&state_empty, buf, 0, NULL, 0);
					if(status_empty == 0) {
						fprintf(stderr, "%s:%d: *** missing separator.  .Stop.\n", file, line);
						exit(1);
					}
					is_macro = 0;
					buf_count = 0;
					target_in = 0;
					dependancy_in = 0;
					command_line = 0;
					(*st_cnt)++;
				}
				else if(status_equal == 0) { 
					if(handle4 != 1){
						buf_count = 0;
						(*macro_line)++;
						for(int j = 0;j < BUFFER_SIZE; j++)
							macro_buf[j] = '\0';
					}
					target_in = 0;
					dependancy_in = 0;
					command_line = 0;
					is_macro = 1;
				}
				else if(status_tab == 0) {
					if(handle4 != 1) {
						is_macro = 0;
					}
					target_in = 1;
					dependancy_in = 1;
					if(command_line == 0) {
						if(handle3 != 1){
							buf_count = 0;
							command_line++;
						}
					}
				}
				else {
					if(buf[0] == '\0')
						;
					else{
						fprintf(stderr, "%s:%d: *** missing separator.  .Stop.\n", file, line);
						exit(1);
					}
					if(handle3 != 0)
						status_tab = 0;
				}
			}
			while(1) {
				read(fd, &ch, sizeof(ch));
				loop++;
				buf_count++;
				if(status_include == 0) {
					if(loop > buf_size) {
						loop = 0;
						break;
					}
				}
				else{
					if(ch == '#' || handle2 == 1){
						handle2 = 1;
						if(ch == '\\')
							long_sh++;
						if(ch == '\n') {
							kkk++;
							if(kkk > long_sh){
								kkk = 1;
								long_sh = 1;
								handle2 = 0;
								lseek(fd, -1L, SEEK_CUR);
								break;
							}
						}
					}
					else {
						if(ch == '\n'){
							buf_count--;
							if(handle3 == 1 || handle4 == 1) {
								enter_cnt++;
							}
						}
						if(loop > buf_size) {
							loop = 0;
							break;
						}
						if(ch == '\\') {
							handle3 = 1;
							if(status_equal == 0 ){
								handle4 = 1;
							}
						}
						if(handle3 == 1 || handle4 == 1) {
							if(enter_cnt >= 2) {
								enter_cnt = 0;
								buf_count = 1;
								handle3 = 0;
								handle4 = 0;
								command_line++;
							}
							if(enter_cnt <= 1) {
								dependancy_in = 0;
							}
							if(status_tab == 0 && dependancy_in == 1) {
								status_tab = 1;
								dependancy_in = 0;
								target_in = 1;
							}
						}
						if(is_macro == 1) {
							regex_t state_empty;
							char *pattern_empty = "^ ";
							regcomp(&state_empty, pattern_empty, REG_EXTENDED);
							int status_empty = regexec(&state_empty, buf, 0, NULL, 0);
							
							if((status_tab == 0 || status_empty == 0) && handle4 != 1) {
								fprintf(stderr, "%s:%d: *** missing separator.  .Stop.\n", file, line);
								exit(1);
							}
							macro_buf[buf_count-1] = ch;
							if(ch == '\\')
								buf_count--;
							if(ch == '\n')
								enter_cnt++;
							strcpy(macro[*macro_line].macro_total, macro_buf);
						}

						else if(target_in == 0 && dependancy_in == 0 && is_macro == 0) {
							regex_t state_empty;
							char *pattern_empty = "^ ";
							regcomp(&state_empty, pattern_empty, REG_EXTENDED);
							int status_empty = regexec(&state_empty, buf, 0, NULL, 0);
							if(status_tab == 0) {
								fprintf(stderr, "%s:%d: *** missing separator.  .Stop.\n", file, line);
								exit(1);
							}
							if(ch == ' ' || ch == ':') {
								target_in = 1;
								buf_count = 0;
							}
							sp[*st_cnt].target[buf_count-1] = ch;
						}
						else if(target_in == 1 && dependancy_in == 0 && is_macro == 0 && command_line == 0) {
							if(ch == ' ') {
								if(buf_count == 1)
									buf_count--;
								else
									sp[*st_cnt].dependancy[buf_count-1] = ' ';
							}
							else if(ch == ':') {
								buf_count--;
							}
							else {
								sp[*st_cnt].dependancy[buf_count-1] = ch;
							}
							if(ch == '\\')
								buf_count--;
							if(ch == '\n') {
								dependancy_in = 1;
								enter_cnt++;
							}
						}
						else if(( status_tab == 0 && command_line != 0)) {
							sp[*st_cnt].command[buf_count-1] = ch;
						}
					}
				}
			}
		}
	}
}
int line_read(int fd2, char buf[])
{
	char ch;
	int i=0;
	while(1) {
		if(read(fd2, &ch, sizeof(ch)) > 0) {
			if(ch == '\n') {
				buf[i] = '\0';
				return 1;
			}
			else {
				buf[i] = ch;
				i++;
			}
		}
		else
			return 0;
	}
}

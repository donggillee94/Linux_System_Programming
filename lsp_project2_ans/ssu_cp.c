#include "ssu_cp.h"

PATH_T from = {from.p_path, ""};
PATH_T to = {to.p_path, ""};
PATH_T froms[OPT_MAX_NUM] =  {{NULL,""}, {NULL,""},{NULL,""}, {NULL,""},{NULL,""}};


char *flags = "asdipfunrl";
uid_t myuid;
int exit_val, myumask;
int (*statfcn)();
char *buf,*progname;
char *old_to, *p;
int d_count;
/** flags  **/
flag_t s_flag;
flag_t d_flag;
flag_t i_flag;
flag_t p_flag;
flag_t n_flag;
flag_t r_flag;
flag_t l_flag;

flag_t is_opt=0;

int ssu_cp(int argc, char *argv[])
{
	int ch;
	int i;
	int is_dir=0;
	struct stat to_stat;
	init_variable(TRUE); 	// var initiate.
	//called program name

	if(argc > 1)
	{
		if (get_opt(argc, argv)==ERROR)
		{
			fprintf(stderr, "option error!\n");
			return ERROR;
		}
	}

	buf = (char *)malloc(MAX_SIZE);
	if (!buf){
		fprintf(stderr, "%s : out of space.\n", progname);
		return ERROR;
	}
	progname = ( p = strrchr(*argv, '/')) ? ++p : *argv;
	// copy the umask for explicit mode setting
	myuid = getuid();
	myumask = umask(0);
	umask(myumask);

	if (path_set(&to,argv[--argc]) == FALSE)
		return ERROR;

	for( i = 0 ; i < strlen(flags) ; i++)
	{
		if(check_flag(flags[i]) == TRUE)
			break;
	}
	statfcn = s_flag ? lstat : stat;

	if ( i == strlen(flags))
	{
		if ( argc != 2)
		{
			error("not enough argments!\n");
			return ERROR;
		}
	}
	else
	{
		is_opt = TRUE;
	}

	printf("*******************target : %s************************\n", to.p_path);
	is_dir = stat(to.p_path, &to_stat);
	if (is_dir == ERROR && errno != ENOENT)
	{
		fprintf(stderr, "There is no destination path!\n");
		return ERROR;
	}
	if ( is_dir == ERROR || !S_ISDIR(to_stat.st_mode ))// target  is existing 
	{
		if(!is_opt && argc >1) 
		{	if (!path_set(&from, argv[1]))// src setting!
			{
				return ERROR;
			}
			printf("*************************src : %s **************************\n " , from.p_path);
			return copy();
		}
		else
		{
			if(!path_set(&from, argv[argc-1]))
			{
				return ERROR;
			}
			printf("src : %s    \n", from.p_path);
			return copy();
		}
	}
	else 
	{
		if (!is_opt) 
		{
			for (;; ++argv)
			{
				if (!path_set(&from, *argv))
				{
					exit_val = TRUE;
					continue;
				}
				old_to = path_append(&to, path_basename(&from), -1);
				printf("*******************************src : %s***************************\n", from.p_path); 
				if (!old_to)
				{
					exit_val = TRUE;
					continue;
				}
				copy();
				if (!--argc)
					break;

				path_restore(&to, old_to);
				printf("*****************************src : %s*************************** \n", from.p_path); 
			}
		}
		else 
		{
			for(;; ++argv)
			{
				if (!path_set(&from, argv[1]))
				{
					exit_val = TRUE;
					continue;
				}
				printf("**************************************src : %s *******************************\n" , from.p_path);
				old_to = path_append(&to, path_basename(&from), -1);
				if (!old_to)
				{
					exit_val = TRUE;
					continue;
				}
				copy();
				if (--argc == 1 )
					break;
				path_restore (&to, old_to);
				printf("**************************************src : %s *******************************\n", from.p_path);
			}
		}
	}
	free(buf);
	return exit_val;
}

/*copy file or directory func for each options.*/
// copy file or directory at "from", to "to"
int copy()
{
	struct stat from_stat, to_stat;
	int dne, statval; 
	statval = statfcn(from.p_path, &from_stat);
	char filepath[MAX_SIZE] ;

	if (statval == ERROR)
	{
		error(from.p_path);
		return ERROR;
	}
	if (stat(to.p_path, &to_stat) == ERROR)
	{
		dne = TRUE;
	}
	else 
	{
		if (to_stat.st_dev == from_stat.st_dev && to_stat.st_ino == from_stat.st_ino) 
		{
			fprintf(stderr, "%s : %s and %s are identical (not copied).\n",progname,to.p_path, from.p_path);
			exit_val = TRUE;
			return ERROR;
		}
		dne = FALSE;
	}
	if( s_flag)
		copy_tolink();

	switch(from_stat.st_mode & S_IFMT) 
	{
		case S_IFLNK: 
			if (!s_flag)
			{
				copy_tolink();
			}
			return TRUE;

		case S_IFDIR: 
			if (!r_flag && !d_flag) 
			{
				fprintf(stderr, "%s: %s must be a directory (not copied).\n",progname, from.p_path);
				exit_val = TRUE;
				return ERROR;
			}
			if (dne) 
			{ 
				if (mkdir(to.p_path, from_stat.st_mode|S_IRWXU) <0) 
				{
					error(to.p_path);
					return ERROR;
				}			
			}
			else if (!S_ISDIR(to_stat.st_mode) != S_IFDIR )
			{
				fprintf(stderr, "%s: %s: not a directory. \n",progname, to.p_path);
				return ERROR;
			}
			if (d_flag)
			{
				copy_dir_fork();
				goto frontp_flag; 
			}
			copy_dir(); 
			if (l_flag)
			{
				setfile(&from_stat, 0);
			}
			else if (dne) 
				chmod(to.p_path, from_stat.st_mode); 
			return TRUE;
	}
	copy_file(&from_stat, dne);
frontp_flag:	
	if (p_flag) 
	{
		print_fileinfo(&from , &from_stat);
	}
	return TRUE;
}

void copy_file(struct stat *fs, int dne)
{
	int from_fd, to_fd, rcount, wcount;
	struct stat to_stat;

	if ((from_fd = open(from.p_path, O_RDONLY, 0)) == ERROR)
	{
		error(from.p_path);
		return ;
	}
	if (!dne)
	{
		if (i_flag && !n_flag) 
		{
			int check_overwrite, ch;
			fprintf(stderr, "overwrite %s (y/n)? ", to.p_path);
			check_overwrite = ch = getchar();
			while(ch != '\n' && ch != EOF)
				ch = getchar();
			if (check_overwrite != 'y')
			{
				close(from_fd);
				return ;
			}
		}
		else if(!i_flag && n_flag)
		{
			close(from_fd);
			return ;
		}
		else if(i_flag&& n_flag) 
		{
			error("i_option & n_option is exclusive");
			close(from_fd);
			return ;
		}
		to_fd = open(to.p_path, O_WRONLY | O_TRUNC, 0);
	}
	else
		to_fd = open(to.p_path, O_WRONLY|O_CREAT|O_TRUNC, fs->st_mode & ~(S_ISUID|S_ISGID));

	if (to_fd == ERROR)
	{
		error(to.p_path);
		close(from_fd);
		return ;
	}

	while ((rcount = read(from_fd,buf, MAX_SIZE)) > 0 ) 
	{
		wcount = write(to_fd, buf, rcount);
		if (rcount != wcount || wcount == ERROR) 
		{
			error(to.p_path);
			return ;
		}
	}
	if (rcount < 0)
		error(from.p_path);

	if (l_flag)
		setfile(fs, to_fd);
	
	else if (fs->st_mode & (S_ISUID|S_ISGID) && fs->st_uid == myuid)
		if (fstat(to_fd, &to_stat))
			error(to.p_path);

		else if (fs->st_gid == to_stat.st_gid && fchmod(to_fd, fs->st_mode & RETAINBITS & ~myumask))
			error (to.p_path);
	close(from_fd);
	if (close(to_fd))
		error(to.p_path);
}

void copy_dir()
{
	struct stat from_stat;
	struct dirent *dp, **dir_list;
	int dir_cnt, i;
	char *old_from, *old_to;

	dir_cnt = scandir(from.p_path, &dir_list, NULL, NULL); 
	if (dir_cnt == ERROR)
	{
		fprintf(stderr, "%s: can't read directory %s.\n", progname, from.p_path);
		exit_val = TRUE;
	}
	/*copy files*/
	for (i =0 ; i< dir_cnt ; i++)
	{
		dp = dir_list[i];
		if ( strcmp(dir_list[i]->d_name, ".")==0)
			goto done;
		else if (strcmp(dir_list[i]->d_name, "..") == 0)
			goto done;

		old_from = path_append(&from, dp->d_name, (int)dp->d_reclen); 
		if (!old_from)
		{
			exit_val = TRUE;
			goto done;
		}
		if (statfcn(from.p_path, &from_stat) < 0)
		{
			error(dp->d_name);
			path_restore(&from, old_from); 
			goto done;
		}
		if (S_ISDIR(from_stat.st_mode)) 
		{
			path_restore(&from, old_from);
			continue;
		}
		old_to = path_append(&to, dp->d_name, (int)dp->d_reclen); 
		if (old_to) 
		{
			printf("*********************** copy start from %s -> to :%s \n", from.p_path, to.p_path);
			copy(); 
			path_restore(&to, old_to);
		}
		else
			exit_val = TRUE;
		path_restore(&from, old_from); 
done:	dir_list[i] = NULL;
		free(dp);
	}

	/*copy directories*/
	for (i = 0; i < dir_cnt; ++i)
	{
		dp = dir_list[i];
		if (!dp)
			continue;
		old_from = path_append(&from, dp->d_name, (int) dp->d_reclen); 
		if (!old_from) 
		{
			exit_val = TRUE;
			free(dp);
			continue;
		}
		old_to = path_append(&to, dp->d_name, (int) dp->d_reclen);  
		if (!old_to)
		{
			exit_val = TRUE;
			free(dp);
			path_restore(&from, old_from);
			continue;
		}
		printf("****** do copy start from %s -> to : %s \n", from.p_path, to.p_path);
		copy();
		free(dp);
		path_restore(&from, old_from);
		path_restore(&to, old_to);
	}
	free(dir_list);

}

void copy_dir_fork()
{
	struct stat from_stat;
	int statval;
	struct dirent *dp, **dir_list;
	int dir_cnt, i, j;
	char *old_from, *old_to;
	dir_cnt = scandir(from.p_path, &dir_list, NULL, NULL); 
	if (dir_cnt == ERROR)
	{
		fprintf(stderr, "%s: can't read directory %s.\n", progname, from.p_path);
		exit_val = TRUE;
	}
	/*copy files*/
	for (i =0 ; i< dir_cnt ; i++)
	{
		dp = dir_list[i];
		if ( strcmp(dir_list[i]->d_name, ".")==0)
			goto done2;
		else if (strcmp(dir_list[i]->d_name,"..")==0)
			goto done2;
		old_from = path_append(&from, dp->d_name, (int)dp->d_reclen);
		if (!old_from)
		{
			exit_val = TRUE;
			goto done2;
		}
		if (statfcn(from.p_path, &from_stat) < 0)
		{
			error(dp->d_name);
			path_restore(&from, old_from);
			goto done2;
		}
		if (S_ISDIR(from_stat.st_mode))
		{
			path_restore(&from, old_from);
			continue;
		}
		old_to = path_append(&to, dp->d_name, (int)dp->d_reclen);
		if (old_to)
		{
			copy();
			path_restore(&to, old_to);
		}
		else
			exit_val = TRUE;
		path_restore(&from, old_from);
done2:	dir_list[i] = NULL;
		free(dp);
	}
	/*copy directories*/
	int state;
	pid_t pid , child[OPT_MAX_NUM];
	for( j = 0 ; j < dir_cnt ; j++)
	{
		dp = dir_list[j];
		if (!dp)
			continue;
		old_from = path_append(&from, dp->d_name, (int) dp->d_reclen);
		if (!old_from) {
			exit_val =TRUE;
			free(dp);
			path_restore(&from, old_from);
			continue;
		}
		old_to = path_append(&to, dp->d_name, (int) dp->d_reclen); 
		if(!old_to)
		{
			exit_val = TRUE;
			free(dp);
			path_restore(&from, old_from);
			continue;
		}
		statval = statfcn(from.p_path,&from_stat );
		if (statval == ERROR)
		{
			path_restore(&from, old_from);
			path_restore(&to, old_to);
			return ;
		}
		d_count--;
		if (d_count >= 0 ){ 
		pid = fork();
		if ( pid == ERROR) 
		{
			fprintf(stderr, "error on fork\n");
			exit(1);
		}
		if (pid == 0) {
			printf("child process pid : %d \n", getpid());
			printf("copy %s \n" , from.p_path);
		copy();
		free(dp);
		path_restore(&from, old_from);
		path_restore(&to, old_to);
		}
		else
			wait(&state);
		}
		else{
			copy();
			free(dp);
			path_restore(&from, old_from);
			path_restore(&to, old_to);
		}
	}
		free(dir_list);
}
		
void copy_link(int exists)
{
	int len;
	char link[MAX_SIZE];

	if ((len = readlink(from.p_path, link , sizeof(link))) == ERROR)
	{
		error(from.p_path);
		return;
	}
	link[len] = '\0';
	if (exists && unlink(to.p_path))
	{
		error(to.p_path);
		return ;
	}
	if (symlink(link, to.p_path))
	{
		error(link);
		return;
	}
}

void copy_tolink()
{
	char link[MAX_SIZE];
	if (able_access(from.p_path, 1) == TRUE)
		if (symlink(from.p_path, to.p_path)== ERROR)
		{
			printf("from : %s , to : %s \n", from.p_path, to.p_path); 
			error(link);
			return ;
		}
}

void setfile(struct stat *fs, int fd)
{
	static struct timeval tv[2];
	char path[MAX_SIZE];

	fs->st_mode &= S_ISUID|S_ISGID|S_IRWXU|S_IRWXG|S_IRWXO;

	tv[0].tv_sec = fs->st_atime; 
	tv[1].tv_sec = fs->st_mtime; 
	if (utimes(to.p_path, tv))   
	{
		snprintf(path, sizeof(path), "utimes :%s", to.p_path);
		error(path);
	}

	if (fd ? fchown(fd, fs->st_uid, fs->st_gid): chown(to.p_path, fs->st_uid, fs->st_gid)) 
	{
		if (errno != EPERM) 
		{
			snprintf(path, sizeof(path), "chown : %s", to.p_path);
			error(path);
		}
		fs->st_mode &= ~(S_ISUID|S_ISGID); 
	}

	if (fd ? fchmod(fd, fs->st_mode) : chmod(to.p_path, fs->st_mode))
	{
		snprintf(path, sizeof(path), "chown : %s", to.p_path);
		error(path);
	}
}
void init_variable(flag_t flag)
{
	if(flag)
	{
		s_flag = FALSE;
		d_flag = FALSE;
		i_flag = FALSE;
		p_flag = FALSE;
		n_flag = FALSE;
		r_flag = FALSE;
		l_flag = FALSE;
	}
	for(int i = 0 ; i < OPT_MAX_NUM ; i++)
	{
		froms[i].p_end  = froms[i].p_path;
	}

}

int check_flag(char c)
{ 
	int is = FALSE;
	switch(c)
	{
		case 's':
			if(s_flag)
				is =TRUE;
			break;
		case 'd':
			if(d_flag)
				is =TRUE;
			break;
		case 'i':
			if(i_flag)
				is =TRUE;
			break;
		case 'p':
			if(p_flag)
				is =TRUE;
			break;
		case 'n':
			if(n_flag)
				is =TRUE;
			break;
		case 'r':
			if(r_flag)
				is =TRUE;
		case 'l':
			if(l_flag)
				is =TRUE;
			break;
	}
	return is;
}

//s 옵션의 경우 mutaully 한지 확인하는 함수
int check_s_onlyopt()
{
	if(s_flag &&  ( d_flag || i_flag || p_flag || n_flag || r_flag || l_flag))
	{
		fprintf(stderr , "s option is mutually exclusive.\n");
		return ERROR;
	}
	return OK;
}

int get_opt(int argc, char *argv[])
{
	char *cur_optarg;
	int i;
	char c;
	while ((c = getopt(argc ,argv, "d:D:iIpPrRsSnNlL")) !=EOF)
	{
		switch ((char)c){
			case 'd':
			case 'D':
				if(check_flag('d'))
					return ERROR;
				d_flag =TRUE;
				cur_optarg = optarg;
				for( i = 0 ; i < strlen(cur_optarg) ; i++)
					if(!isdigit(cur_optarg[i]))
					{
						fprintf(stderr, "a option error :  you should enter number [N]\n");
						return ERROR;
					}
				d_flag =TRUE;
				int d_num = atoi(optarg);

				if(d_num == 0)
				{
					fprintf(stderr, " you should enter number [N]\n");
					return ERROR;
				}
				if (d_num >= 2 && d_num <= OPT_MAX_NUM)
				{
					d_count = d_num;
				}
				else if (d_num > OPT_MAX_NUM)
				{
					fprintf(stderr, "invalid number. your number should be under OPT_MAX_NUM\n");
					return ERROR;
				}
				else
				{
					fprintf(stderr, "invalid number . your number should be over 1! \n");
					return ERROR;
				}
				optind++;
				break;
			case 'i':
			case 'I':
				if(check_flag('i'))
				{
					fprintf(stderr,"you already use i options.\n");
					return ERROR;
				}
				i_flag = TRUE;
				printf(" i option is on\n");  
				break;
			case 'p':
			case 'P':
				if(check_flag('p'))
				{
					fprintf(stderr, "you already use p options.\n");
					return ERROR;
				}
				p_flag = TRUE;
				break;
			case 'L':
			case 'l':
				if(check_flag('l'))
				{
					fprintf(stderr, "you already use l options.\n");
					return ERROR;
				}
				l_flag = TRUE;
				printf(" l option is on\n");
				break;	
			case 'R':
			case 'r':
				if(check_flag('r'))
				{
					fprintf(stderr, "you already use r options.\n");
					return ERROR;
				}
				r_flag = TRUE;
				printf(" r option is on\n");
				break;
			case 's':
			case 'S':
				if(check_flag('s'))
				{
					fprintf(stderr, "you already use s options.\n");
					return ERROR;
				}
				if(check_s_onlyopt()== OK)
				{
					s_flag = TRUE;
					printf(" s option is on\n");
				}
				else
					return ERROR;
				break;
			case 'n':
			case 'N':
				if(check_flag('n'))
				{
					fprintf(stderr, "you already use n options.\n");
					return ERROR;
				}
				n_flag = TRUE;
				printf("n option is on\n");
				break;
			case '?':
			default:
				usage();
				break;
		}
	}
	return TRUE;
}

/*path 관련 함수*/
int path_set(PATH_T *p, char *string)
{
	if (strlen(string) > PATH_MAX)
	{
		fprintf(stderr, "%s: %s: name too long.\n", progname, string);
		return FALSE;
	}

	strcpy(p->p_path, string);
	p->p_end = p->p_path + strlen(p->p_path);

	if (p->p_path == p->p_end)
	{
		*p->p_end++ = '.';
		*p->p_end = 0;
	}

	STRIP_TRAILING_SLASH(p);
	return OK;
}

char *path_append(PATH_T *p,char *name,int len)
{
	char *old;

	old = p->p_end;
	if (len == ERROR)
		len = strlen(name);
	if ((len + p->p_end - p->p_path +1) > PATH_MAX)
	{
		fprintf(stderr, "%s: %s/%s: name too long.\n", progname, p->p_path, name);
		printf("name len is %ld .\n", len + (p->p_end) -(p->p_path) +1 );
		return FALSE;
	}

	if (p->p_end[-1] != '/')
	{
		*p->p_end++ = '/';
		*p->p_end = 0;
	}

	strncat(p->p_end, name, len);
	//p->p_end += len;
	p->p_end += strlen(name);
	*p->p_end = 0;


	STRIP_TRAILING_SLASH(p);
	return old;

}

void path_restore(PATH_T *p, char *old)
{
	p->p_end = old;
	*p->p_end = 0;
}

char *path_basename(PATH_T *p)
{
	char *basename;

	basename = strrchr(p->p_path, '/');
	return basename ? basename +1 : p->p_path;
}

int able_access(char *path_name, int flag)
{
	if (access(path_name, F_OK) < 0)
	{
		if(flag)
		{
			printf("%s doesn't exist \n", path_name);
			return ERROR;
		}
	}
	else if (access(path_name, R_OK) < 0)
	{
		if(flag)
		{
			printf("%s can't be read\n", path_name);
			return ERROR;
		}
	}
	else //success to access 
		return TRUE;
}
void print_fileinfo(PATH_T *p, struct stat *from_stat)
{
	char buffer[MAX_SIZE];
	struct passwd *fromfile_passwd = getpwuid(from_stat->st_uid);
	struct group *fromfile_group= getgrgid(from_stat->st_gid);
	printf("****************file info**********************\n");
	printf("파일 이름 : %s\n", p->p_path);
	printf("데이터의 마지막 읽은 시간 : %s \n", formatdate(buffer, from_stat->st_atime));
	printf("데이터의 마지막 수정 시간 : %s \n", formatdate(buffer,from_stat->st_mtime));
	printf("데이터의 마지막 변경 시간 : %s \n", formatdate(buffer, from_stat->st_ctime));
	printf("OWNER : %s\n", fromfile_passwd->pw_name);
	printf("GROUP : %s\n", fromfile_group->gr_name);
	printf("file size : %ld\n", from_stat->st_size);
}

char *formatdate(char* str, time_t val)
{
	strftime(str, MAX_SIZE, "%Y.%m.%d %H:%M:%S", localtime(&val));
	return str;
}

void error(char *s)
{
	exit_val = TRUE;
	fprintf(stderr,"%s: %s: %s\n", progname, s, strerror(errno));
}

void usage()
{
	fprintf(stderr,"usage : in case of file\n cp [-i/n][-l][-p][-d][N] [source][target]\n or cp [-s][source][target] \n in case of directory cp [-i/n][-l][-p][-r][-d][N]\n");
	exit(1);
}

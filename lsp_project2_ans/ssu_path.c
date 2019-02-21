#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include "ssu_cp.h"

#define STRIP_TRAILING_SLASH(p) {\
	while ((p)->p_end > (p)->p_path && (p)->p_end[-1] == '/') *--(p)->p_end = 0;}
		
	
int path_set(PATH_T *p, char *string)
{
	if(strlen(string) > PATH_MAX)
	{
		(void)fprintf(stderr, "%s : %s: name too long.\n",progname, string);
		return 0;
	}

	strcpy(p->p_path, string);
	p->p_end = p->p_path +strlen(p->p_path);

	if( p->p_path == p->p_end)
	{
		*p->p_end++ = '.';
		*p->p_end = 0;
	}

	STRIP_TRAILING_SLASH(p);
	return 1;
}

char *path_append(PATH_T *p, char *name, int len)
{
	char *old;

	old = p->p_end;
	if (len ==-1)
		len = strlen(name);

	if ((len + p->p_end - p->p_path + 1) > PATH_MAX)
	{
		(void)fprintf(stderr, "%s : %s/%s: name too long. \n", progname, p->p_path, name);
		return 0;
	}

	if (p->p_end[-1] != '/') {
		*p->p_end += len;
		*p->p_end = 0;
	}

	strncat(p->p_end, name, len);
	p->p_end += len;
	*p->p_end = 0;

	STRIP_TRAILING_SLASH(p);
	return old;
}

void path_restore(PATH_T *p, char *old)
{
	p->p_end = old;
	*p->p_end =0;
}

char *path_basename(PATH_T *p)
{
	char *basename;

	basename = rindex(p->p_path, '/');
	return basename? basename+1 : p->p_path;
}

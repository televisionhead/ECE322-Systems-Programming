#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>

struct file {
	char* name;
	char type;
	char* path;
	int size;
	char* owner;
	char* group;
	time_t modified;
};

int timesort = 0;

int compare(const void* a, const void* b) {
	const struct file* file1 = a;
	const struct file* file2 = b;

	if(timesort) {
		return difftime(file2->modified, file1->modified);
	} else {
		return strcmp(file1->name, file2->name);
	}
}

int main(int argc, char* argv[]) {
	if(argc < 2 || argc > 4) {
		fprintf(stderr, "usage: %s [-FLAG(S)] <pathname>\n", argv[0]);
		printf("flags: -a to show hidden files, -t to sort by time modified instead of name\n");
		printf("tip: the path should always be your last argument (flags must come before it)\n");
		return -1;
	}	

	int patharg = argc - 1;
	int hidden = 0;
	char* currflag;

	if(argc > 2) {
		for(int i = 1; i < patharg; i++) {
			currflag = argv[i];
			if(strcmp(currflag, "-a") == 0) {
				hidden = 1;
			}
			else if(strcmp(currflag, "-t") == 0) {
				timesort = 1;
			}
			else {
				fprintf(stderr, "Unrecognized flag '%s': options are -a or -t and must come before your path\n", currflag);
				return -1;
			}
		}	
	}

	char* path = argv[patharg];
	DIR* dirp = opendir(path);

	if(dirp == NULL) {
		fprintf(stderr,"Bad path: %s\n", path);
		return -1;
	}

	struct dirent* d;
	struct file* files = NULL;
	int index = -1;
	int structsize = sizeof(struct file);
	struct file curr;
	struct stat finfo;
	char* fullpath;
	char* dname;

	while((d = readdir(dirp)) != NULL) {
		curr.name = d->d_name;

		if(hidden || curr.name[0] != '.') {
			index++;

			fullpath = (char*)malloc(sizeof(char)*strlen(path)+strlen(curr.name)+2);
			sprintf(fullpath,"%s/%s", path, curr.name);

			if(lstat(fullpath, &finfo) == 0) {				
				dname = (char*)malloc(sizeof(char)*strlen(curr.name)+2);

				curr.type = ' ';

				if(S_ISDIR(finfo.st_mode)) {
					curr.type = '/';
				}
		 		else if(S_ISREG(finfo.st_mode)) {
		 			if((finfo.st_mode & S_IXUSR) == S_IXUSR) {
						curr.type = '*';
					}
		 		}
		 		else if(S_ISLNK(finfo.st_mode)) {
						curr.type = '@';
		 		}
		 		else if(S_ISFIFO(finfo.st_mode)) {
						curr.type = '|';
		 		}

				sprintf(dname,"%s%c", curr.name, curr.type);

				curr.path = dname;
				curr.size = finfo.st_size;
		 		curr.owner = getpwuid(finfo.st_uid)->pw_name;
				curr.group = getgrgid(finfo.st_gid)->gr_name;
				curr.modified = finfo.st_mtim.tv_sec;

				files = (struct file*)realloc(files, (index+1) * structsize);
				files[index] = curr;

				free(fullpath);
			} else {
				fprintf(stderr,"Error! lstat did not return 0 on file or directory '%s'\n", fullpath);
				return -1;
			}
		}
	}

  qsort(files, index+1, structsize, compare);

	printf("\nFile listing for %s is:\n\n", path);

	printf("[FILENAME] \t     [SIZE] \t[OWNER,GROUP] \t[LAST MODIFIED]\n\n");

	for(int i = 0; i <= index; i++) {
		printf("%-20s ", files[i].path);
		printf("%-5d ", files[i].size);
		printf("\t[%s,%s] ", files[i].owner, files[i].group);
		printf("\t%s", ctime(&files[i].modified));
	}

	free(dname);
	free(files);

	return 0;
}
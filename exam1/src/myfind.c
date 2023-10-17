#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_finfo(char* fpath, struct stat finfo) {
	mode_t st_mode = finfo.st_mode;
	uid_t st_uid = finfo.st_uid;
	gid_t st_gid = finfo.st_gid;

	printf("\nPath: %s\n", fpath);

	if(S_ISDIR(st_mode)) {
		printf("Type: directory\n");
	} else {
		if(S_ISREG(st_mode)) {
			if((st_mode & S_IXUSR) == S_IXUSR) {
				printf("Type: executable file\n");
			} else {
				printf("Type: regular file\n");
			}
		}
		else if(S_ISLNK(st_mode)) {
			printf("Type: symbolic link\n");
		}
		else if(S_ISFIFO(st_mode)) {
			printf("Type: fifo (named pipe)\n");
		}
		else if(S_ISSOCK(st_mode)) {
			printf("Type: socket\n");
		}
		else if(S_ISCHR(st_mode)) {
			printf("Type: character device\n");
		}
		else if(S_ISBLK(st_mode)) {
			printf("Type: block device\n");
		}
		else {
			printf("Type: unknown (st_mode: %d)\n", st_mode);
		}
	}

	printf("Size: %d bytes\n", finfo.st_size);
	printf("Blocks: %d blocks\n", finfo.st_blocks);
	printf("Block Size: %d bytes\n", finfo.st_blksize);
	printf("Owner (uid): %s (%d)\n", getpwuid(st_uid)->pw_name, st_uid);
	printf("Group (gid): %s (%d)\n", getgrgid(st_gid)->gr_name, st_gid);
	printf("Access Time: %s", ctime(&finfo.st_atim.tv_sec));
	printf("Modify Time: %s", ctime(&finfo.st_mtim.tv_sec));
	printf("Change Time: %s\n", ctime(&finfo.st_ctim.tv_sec));
	
	free(fpath);

	return;
}

int recursive_search(char* path, char* str, int count) {
	DIR* dirp = opendir(path);

	if(dirp == NULL) {
		fprintf(stderr, "opendir failed to open '%s' (maybe try running as root?)\n", path);
		return 0;
	}

	struct dirent* item;
	char* fname;
	char* fpath;
	struct stat finfo;

	while((item = readdir(dirp)) != NULL) {
		fname = item->d_name;

		if(strcmp(fname, ".") != 0 && strcmp(fname, "..") != 0) {
			fpath = (char*)malloc(strlen(path)+strlen(fname)+2);
			sprintf(fpath, "%s/%s", path, fname);
			
			if(!lstat(fpath, &finfo)) {
				if(strstr(fname, str)) {
					count++;
					print_finfo(fpath, finfo);
				}
				if(S_ISDIR(finfo.st_mode)) {
					count = recursive_search(fpath, str, count);
				}
			} else {
				fprintf(stderr, "lstat did not return 0 on '%s'\n", fpath);
				return 0;
			}
		}
	}
	
	closedir(dirp);

	return count;
}

int main(int argc, char* argv[]) {
	if(argc < 3) {
		fprintf(stderr, "usage: %s <directory> <string>\n", argv[0]);
		return -1;
	}

	char* path = argv[1];
	char* str = argv[2];

	printf("\nSearching for '%s' in '%s'\n", str, path);

	printf("Found %d occurrences of '%s'\n", recursive_search(path, str, 0), str);

	
	return 0;
}
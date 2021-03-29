#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/inotify.h>
#include "handleFile.h"

int inotify(int sockfd, const char *dirname)
{
	int length, i = 0;
	int fd;
	int wd;
	char buffer[BUF_LEN];
	char filename[MAX_LENGTH] = {0};
	char command[MAX_LENGTH] = {0};

	fd = inotify_init();

	if (fd < 0)
	{
		perror("inotify_init");
	}
	wd = inotify_add_watch(fd, dirname, IN_MODIFY | IN_CREATE | IN_DELETE);
	length = read(fd, buffer, BUF_LEN);

	if (length < 0)
	{
		perror("read");
	}

	while (i < length)
	{
		struct inotify_event *event =
				(struct inotify_event *)&buffer[i];
		if (event->len)
		{
			if (event->mask & IN_CREATE || event->mask & IN_MODIFY)
			{
				strcat(filename, event->name);
				/* strcat(command, "ADD");
				send(sockfd, command, strlen(command), 0); // send command
				send(sockfd, filename, strlen(filename), 0); */

				char aux[MAX_LENGTH] = {0};
				strcat(aux, dirname);
				strcat(aux, "/");
				strcat(aux, filename);

				printf("notify: file \"%s\" was created \n", aux);

				/* send_file(sockfd, aux); */
			}
			else if (event->mask & IN_DELETE)
			{
				strcat(filename, event->name);
				send(sockfd, "REMOVE", strlen("REMOVE"), 0); // send remove command
				send(sockfd, filename, strlen(filename), 0); // send filename to server
				printf("The file \"%s\" was deleted.\n", filename);
			}
		}
		i += EVENT_SIZE + event->len;
	}

	(void)inotify_rm_watch(fd, wd);
	(void)close(fd);
}

int send_upfiles(int sockfd, const char path[])
{
	int count = 0;
	DIR *dir;
	struct dirent *ent;
	char filename[MAX_LENGTH] = {0};

	if ((dir = opendir(path)) != NULL)
	{
		/* print all the files and directories within directory */
		printf("Sending files to the client...\n");
		while ((ent = readdir(dir)) != NULL)
		{
			strcat(filename, ent->d_name);
			if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0)
			{
				send(sockfd, filename, strlen(filename), 0); //first send file name

				char aux[MAX_LENGTH] = {0};
				strcat(aux, DSK_SERVER);
				strcat(aux, filename);

				if (send_file(sockfd, aux) == 0)
				{
					printf("File \"%s\" send successfully\n", filename);
				}
				else
				{
					printf("Can not send file \"%s\"\n", filename);
				}
				count++;
				sleep(2);
			}
			bzero(filename, MAX_LENGTH);
		}

		if(count > 0){
			printf("%d files was sent to client\n", count);
		}else{
			printf("No files to send\n");
		}
		send(sockfd, "START", strlen("START"), 0); // Send command to client notify about your dirname
		closedir(dir);
	}
	else
	{ // could not open directory
		perror("list files");
		return -1;
	}
	return 0;
}

int list_files(const char path[])
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path)) != NULL)
	{
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL)
		{
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
			{
				printf("%s\n", ent->d_name);
			}
		}
		closedir(dir);
	}
	else
	{ // could not open directory
		perror("list files");
		return -1;
	}
	return 0;
}

int remove_file(const char filename[]){
	int ret;
	FILE *fp;

	fp = fopen(filename, "w");
	ret = remove(filename);

	if(ret == 0) {
		printf("File \"%s\" deleted successfully\n", filename);
	} else {
		printf("Unable to delete the file \"%s\"\n", filename);
		return -1;
	}

	return 0;
}

void remove_dir(const char path[])
{
	size_t path_len;
	char *full_path;
	DIR *dir;
	struct stat stat_path, stat_entry;
	struct dirent *entry;

	// stat for the path
	stat(path, &stat_path);

	// if path does not exists or is not dir - exit with status -1
	if (S_ISDIR(stat_path.st_mode) == 0)
	{
		fprintf(stderr, "%s: %s\n", "Is not directory", path);
		exit(-1);
	}

	// if not possible to read the directory for this user
	if ((dir = opendir(path)) == NULL)
	{
		fprintf(stderr, "%s: %s\n", "Can`t open directory", path);
		exit(-1);
	}

	// the length of the path
	path_len = strlen(path);

	// iteration through entries in the directory
	while ((entry = readdir(dir)) != NULL)
	{

		// skip entries "." and ".."
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;

		// determinate a full path of an entry
		full_path = calloc(path_len + strlen(entry->d_name) + 1, sizeof(char));
		strcpy(full_path, path);
		strcat(full_path, "/");
		strcat(full_path, entry->d_name);

		// stat for the entry
		stat(full_path, &stat_entry);

		// recursively remove a nested directory
		if (S_ISDIR(stat_entry.st_mode) != 0)
		{
			remove_dir(full_path);
			continue;
		}

		// remove a file object
		if (unlink(full_path) == 0)
			printf("Removed a file: %s\n", full_path);
		else
			printf("Can`t remove a file: %s\n", full_path);
		free(full_path);
	}

	// remove the devastated directory and close the object of it
	if (rmdir(path) == 0)
		printf("Removed a directory: %s\n", path);
	else
		printf("Can`t remove a directory: %s\n", path);

	closedir(dir);
}

void make_dir(char *path)
{
	char *sep = strrchr(path, '/');
	if (sep != NULL)
	{
		*sep = 0;
		make_dir(path);
		*sep = '/';
	}
	if (mkdir(path, 0777) && errno != EEXIST)
		printf("error while trying to create '%s'\n", path);
}

int send_file(int socket, char *fname)
{
	char buffer[MAX_LENGTH] = {0};

	FILE *file = fopen(fname, "r");

	if (file == NULL)
	{
		printf("File %s not found.\n", fname);
		return -1;
	}

	bzero(buffer, MAX_LENGTH);

	int fs_block_sz;
	while ((fs_block_sz = fread(buffer, sizeof(char), MAX_LENGTH, file)) > 0)
	{
		if (send(socket, buffer, fs_block_sz, 0) < 0)
		{
			fprintf(stderr, "Failed to send file \"%s\" (errno = %d)\n", fname, errno);
			return -1;
		}
		bzero(buffer, MAX_LENGTH);
	}
	//printf("File data sent successfully\n");
	return 0;
}

int recv_file(int socket, char *fname, char *DSK)
{
	char buffer[MAX_LENGTH];
	char fpath[MAX_LENGTH];

	strcpy(fpath, DSK);
	strcat(fpath, fname);
	FILE *out_file = fopen(fpath, "wb");

	if (out_file == NULL)
	{
		printf("File %s cannot create file on your pc.\n", fname);
		return -1;
	}	
	else
	{
		bzero(buffer, MAX_LENGTH);
		int out_file_block_sz = 0;
		while ((out_file_block_sz = recv(socket, buffer, MAX_LENGTH, 0)) > 0)
		{
			int write_sz = fwrite(buffer, sizeof(char), out_file_block_sz, out_file);
			if (write_sz < out_file_block_sz)
			{
				perror("File write failed on your pc.");
				return -1;
			}

			bzero(buffer, MAX_LENGTH);
			if (out_file_block_sz == 0 || out_file_block_sz != MAX_LENGTH)
			{
				break;
			}
		}
		if (out_file_block_sz < 0)
		{
			if (errno == EAGAIN)
			{
				printf("recv() timed out.\n");
			}
			else
			{
				fprintf(stderr, "recv() failed due to errno = %d\n", errno);
				return -1;
			}
		}
		//printf("File recived successfully\n");
		fclose(out_file);
	}

	return 0;
}
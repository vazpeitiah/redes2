#define MAX_LENGTH 1024
#define DSK_SERVER "dsk_server/"
#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

int remove_file(const char filename[]);
int inotify(int sockfd, const char *dirname);
int send_upfiles(int sockfd, const char path[]);
int list_files(const char path[]);
void remove_dir(const char path[]);
void make_dir(char *path);
int send_file(int sockfd, char *filename);
int recv_file(int sockfd, char *filename, char* DSK);
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "util.h"

#define MAXSIZE 1024

int swToPasv(int csockfd);
int swToPort(int csockfd, int localport);
bool setType(int csockfd, bool type);
void ftp_list(int csockfd, char *path, bool type, bool mode);
void ftp_list_pasv(int csockfd, int dsockfd, char *path);
void ftp_list_port(int csockfd, int dsockfd, int localport, char *path);
void ftp_pwd(int csockfd);
void ftp_changdir(int csockfd, char *path);
void ftp_mkdir(int csockfd, char *path);
void ftp_quit(int csockfd);
void ftp_del(int csockfd, char *path);
void ftp_rename(int csockfd, char *path1, char *path2);
void ftp_up(int csockfd, char *path1, char *path2, bool type, bool mode, int speed);
void ftp_up_pasv(int csockfd, int dsockfd, char *path1, char *path2, int speed);
void ftp_up_port(int csockfd, int dsockfd, int localport, char *path1, char *path2, int speed);
void ftp_down(int csockfd, char *path1, char *path2, bool type, bool mode);
void ftp_down_pasv(int csockfd, int dsockfd, char *path1, char *path2);
void ftp_down_port(int csockfd, int dsockfd, int localport, char *path1, char *path2);

void rest_down(int csockfd, char *path1, char *path2, bool type, bool mode);
void rest_down_pasv(int csockfd, int dsockfd, char *path1, char *path2);
void ftp_append_pasv(int csockfd, int dsockfd, char *path1, char *path2);
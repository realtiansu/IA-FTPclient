#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#define MAXSIZE 1024

void help();
void trim(char *strIn, char *strOut);
int file_size(char* filename);
bool replace(char strRes[],char from[], char to[]);
char *strrpc(char *str,char *oldstr,char *newstr);
int random(int x);
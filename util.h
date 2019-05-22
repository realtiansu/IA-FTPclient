#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXSIZE 1024
#define random(x) (rand()%x)

void help();
void zeromery(char *a,int len);
void trim(char *strIn, char *strOut);
int file_size(char* filename);
bool replace(char strRes[],char from[], char to[]);
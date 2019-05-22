#include "util.h"


void help()
{
    printf("Usage: ftp [-46pinegvtd] [hostname]");
}


void zeromery(char *a,int len)
{
    int i;
    len=sizeof(a);
    for(i=0;i<len;i++)
    {
        a[i]=0;
    }
}


void trim(char *strIn, char *strOut)
{
    if(strIn == NULL) {
        return;
    }
    
    int i, j ;

    i = 0;

    j = strlen(strIn) - 1;

    while(strIn[i] == ' ')
        ++i;

    while(strIn[j] == ' ')
        --j;
    strncpy(strOut, strIn + i , j - i + 1);
    strOut[j - i + 1] = '\0';
}


char *itoa(int value, char *string, int radix)
{
    char tmp[33];
    char *tp = tmp;
    int i;
    unsigned v;
    int sign;
    char *sp;
 
    sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (unsigned)value;
    while (v || tp == tmp)
    {
        i = v % radix;
        v = v / radix;
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }
 
    if (string == 0)
        string = (char *)malloc((tp-tmp)+sign+1);
    sp = string;
 
    if (sign)
        *sp++ = '-';
    while (tp > tmp)
        *sp++ = *--tp;
    *sp = 0;
    return string;
}

bool replace(char strRes[],char from[], char to[]) {
    int i;
    bool flag = false;
    char *p,*q,*ts;
    for(i = 0; strRes[i]; ++i) {
        if(strRes[i] == from[0]) {
            p = strRes + i;
            q = from;
            while(*q && (*p++ == *q++));
            if(*q == '\0') {
                ts = (char *)malloc(strlen(strRes) + 1);
                strcpy(ts,p);
                strRes[i] = '\0';
                strcat(strRes,to);
                strcat(strRes,ts);
                free(ts);
                flag = true;
            }
        }
    }
    return flag;
}


int file_size(char* filename)  
{  
    FILE *fp=fopen(filename,"r");  
    if(!fp) return -1;  
    fseek(fp,0L,SEEK_END);  
    int size=ftell(fp);  
    fclose(fp);  
      
    return size;  
}
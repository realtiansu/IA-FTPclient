#include "util.h"


void help()
{
    printf("Usage: ftp [-46pinegvtd] [hostname]");
}


int random(int x) {
    srand((unsigned)time(NULL));
    return rand() % x;
}


void trim(char *strIn, char *strOut)    //去掉首尾空格
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


char *itoa(int value, char *string, int radix)      //int转ascii
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

bool replace(char strRes[],char from[], char to[]) {        //替换
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


char *strrpc(char *str,char *oldstr,char *newstr){
    char bstr[strlen(str)];//转换缓冲区
    memset(bstr,0,sizeof(bstr));
 
    for(int i = 0;i < strlen(str);i++){
        if(!strncmp(str+i,oldstr,strlen(oldstr))){//查找目标字符串
            strcat(bstr,newstr);
            i += strlen(oldstr) - 1;
        }else{
        	strncat(bstr,str + i,1);//保存一字节进缓冲区
	    }
    }
 
    strcpy(str,bstr);
    return str;
}


int file_size(char* filename)       //计算文件大小
{  
    FILE *fp=fopen(filename,"r");  
    if(!fp) return -1;  
    fseek(fp,0L,SEEK_END);  
    int size=ftell(fp);  
    fclose(fp);  
      
    return size;  
}
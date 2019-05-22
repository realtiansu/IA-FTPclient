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
#include <curses.h>
#include "function.h"
#include "util.h"

#define MAXSIZE 1024


int cliopen(char *host,int port);           //准备连接
bool login(int csockfd);                    //输入用户名，密码，登录
void cmd(int csockfd);                      //开始监听用户输入的命令

int main(int argc,char *argv[])
{
    int csockfd, port = 21;
    bool loginresult;
    struct hostent *host;

// start:
    if(argc < 2)
    {
        help();
        exit(0);
    }
    char *hostname = argv[1];
    if(argc == 3)
    {
        port = atoi(argv[2]);
    }
    
    host = gethostbyname(hostname);//argv[1]就是hostname
    if(!host)
    {
        printf("Invalid hostname");
        exit(0);
    }

    csockfd = cliopen(hostname,port);
    loginresult = login(csockfd);
    if(loginresult) {
        cmd(csockfd);
    }
    // else {
    //     sleep(1);
    //     goto start;
    // }

}


bool login(int csockfd)
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result==-1)
    {
        printf("fail to connet host!");
        return false;
    }
    else if(strncmp(recvline,"220",3)==0)
    {
        char name[MAXSIZE],password[MAXSIZE];
    repass:
        memset(name, 0, MAXSIZE);
        memset(password, 0, MAXSIZE);
        memset(sendline, 0, MAXSIZE);
        memset(recvline, 0, MAXSIZE);

        printf("username: ");
        scanf("%s",name);
        sprintf(sendline, "USER %s\r\n", name);
        // printf("--->%s\n",sendline);

        result = send(csockfd,sendline,strlen(sendline),0);
        if(result < 0)
        {
            printf("send username error\n");
        }

        result = recv(csockfd,recvline,sizeof(recvline),0);
        if(strncmp(recvline,"331",3)==0)
        {
            memset(sendline, 0, MAXSIZE);
            memset(recvline, 0, MAXSIZE);
            printf("password: ");                                       
            
            initscr();
            scanf("%s",password);       //依赖 libncurses5-dev
            endwin();
            
            getchar();
            sprintf(sendline, "PASS %s\r\n", password);
            // printf("--->%s\n",sendline);

            result = send(csockfd,sendline,strlen(sendline),0);
            if(result < 0)
            {
                printf("send password error\n");
            }
            result = recv(csockfd,recvline,sizeof(recvline),0);
            if(strncmp(recvline,"230",3)==0) {
                return true;
            } else if(strncmp(recvline,"530",3)==0) {
                printf("username or password error\n");
                goto repass;
            }
        }
        else
        {
            printf("username response error, %s\n", recvline);
        }
    }
    else 
    {
        printf("220 connect is error, %s\n", recvline);
    }

    close(csockfd);

    return false;
}


void cmd(int csockfd)
{
    char cmd[MAXSIZE],origCmd[MAXSIZE];
    char delims[] = " ";
    char *str = NULL;
    bool type = false, mode = false;
    int speed = 2000;
    
    bool r = setType(csockfd, type);


    while (true)
    {
        printf("ftp-> ");
        memset(cmd, 0, MAXSIZE);            //memset清空这个变量的内存
        memset(origCmd, 0, MAXSIZE);

        fgets(origCmd, 1024, stdin);
        origCmd[strcspn(origCmd, "\n")] = 0;
        trim(origCmd, cmd);
        printf("\n the input cmmond is <%s>\n\n", cmd);

        str = strtok(cmd, delims);      //把cmd 以空格为界拆成子串
        if(str == NULL) {
            continue;
        }

        if(strncmp(cmd,"ls",2)==0)      //ls
        {
            // printf("%s\n", str);
            str = strtok( NULL, delims );
            ftp_list(csockfd, str, type, mode);
        }
        else if(strncmp(cmd,"pwd",3)==0)        //pwd
        {
            ftp_pwd(csockfd);
        }
        else if(strncmp(cmd,"rename",6)==0)        //rename
        {
            char path1[MAXSIZE], path2[MAXSIZE];
            memset(path1, 0, MAXSIZE);
            memset(path2, 0, MAXSIZE);

            str = strtok( NULL, delims );
            if(str != NULL) {
                // printf("%s\n", str);
                strncpy(path1,str,MAXSIZE);
            }

            str = strtok( NULL, delims );
            if(str != NULL) {
                // printf("%s\n", str);
                strncpy(path2,str,MAXSIZE);
            }
            
            if(strlen(path1)>0 && strlen(path2)>0 ) {
                // printf("%s,%s\n", path1, path2);
                ftp_rename(csockfd, path1, path2);
            } else {
                printf("Invalid useage\n");
            }
        }
        else if(strncmp(cmd,"put",3) == 0)        //put
        {
            char path1[MAXSIZE], path2[MAXSIZE];
            memset(path1, 0, MAXSIZE);
            memset(path2, 0, MAXSIZE);

            str = strtok( NULL, delims );
            if(str != NULL) {
                // printf("%s\n", str);
                strncpy(path1,str,MAXSIZE);
            }

            str = strtok( NULL, delims );
            if(str != NULL) {
                // printf("%s\n", str);
                strncpy(path2,str,MAXSIZE);
            }
            
            if(strlen(path1)>0 && strlen(path2)>0 ) {
                // printf("%s,%s\n", path1, path2);
                ftp_up(csockfd, path1, path2, type, mode, speed);
            } else {
                printf("Invalid useage\n");
            }
        }
        else if(strncmp(cmd,"get",3) == 0)        //get
        {
            char path1[MAXSIZE], path2[MAXSIZE];
            memset(path1, 0, MAXSIZE);
            memset(path2, 0, MAXSIZE);

            str = strtok( NULL, delims );
            if(str != NULL) {
                // printf("%s\n", str);
                strncpy(path1,str,MAXSIZE);
            }

            str = strtok( NULL, delims );
            if(str != NULL) {
                // printf("%s\n", str);
                strncpy(path2,str,MAXSIZE);
            }
            
            if(strlen(path1)>0 && strlen(path2)>0 ) {
                // printf("%s,%s\n", path1, path2);
                ftp_down(csockfd, path1, path2, type, mode);
            } else {
                printf("Invalid useage\n");
            }

        }
        else if(strncmp(cmd,"rest",4) == 0)        //rest
        {
            char path1[MAXSIZE], path2[MAXSIZE];
            memset(path1, 0, MAXSIZE);
            memset(path2, 0, MAXSIZE);

            str = strtok( NULL, delims );
            if(str != NULL) {
                // printf("%s\n", str);
                strncpy(path1,str,MAXSIZE);
            }

            str = strtok( NULL, delims );
            if(str != NULL) {
                // printf("%s\n", str);
                strncpy(path2,str,MAXSIZE);
            }
            
            if(strlen(path1)>0 && strlen(path2)>0 ) {
                // printf("%s,%s\n", path1, path2);
                rest_down(csockfd, path1, path2, type, mode);
            } else {
                printf("Invalid useage\n");
            }

        }
        else if(strncmp(cmd,"cd",2)==0)     //cd
        {
            str = strtok( NULL, delims );
            if(str != NULL) {
                ftp_changdir(csockfd, str);
            } else {
                printf("Invalid useage\n");
            }
        }
        else if(strncmp(cmd,"mkdir",5)==0)      //mkdir
        {
            str = strtok( NULL, delims );
            if(str != NULL) {
                ftp_mkdir(csockfd, str);
            } else {
                printf("Invalid useage\n");
            }
        }
        else if(strncmp(cmd,"rm",2)==0)     //rm, delete, 文件和文件夹都能删
        {
            str = strtok( NULL, delims );
            if(str != NULL) {
                ftp_del(csockfd, str);
            } else {
                printf("Invalid useage\n");
            }
        }
        else if(strncmp(cmd,"binary",6)==0)     //to binary
        {
            if(type == true){
                bool r = setType(csockfd, false);
            } else {
                printf("Already binary\n");
            }
            
        }
        else if(strncmp(cmd,"ascii",5)==0)      //to ascii
        {
            if(type == false){
                bool r = setType(csockfd, true);
            } else {
                printf("Already ascii\n");
            }
        }
        else if(strncmp(cmd,"pasv",4)==0)     //to passive mode
        {
            mode = false;
        }
        else if(strncmp(cmd,"port",4)==0)     //to port mode
        {
            mode = true;
        }
        else if(strncmp(cmd,"speed",5)==0)     //set speed
        {
            str = strtok( NULL, delims );
            if(str != NULL) {
                speed = atoi(str);
            } else {
                printf("Invalid useage\n");
            }
        }
        else if(strncmp(cmd,"quit",4)==0)       //quit
        {
            ftp_quit(csockfd);
        }
        else
        {
            if(strlen(cmd)==0)
            {
                continue;
            }
            printf("no this command\n");
        }
        
    }

}


int cliopen(char *host,int port)
{
    int control_sock;
    struct sockaddr_in servaddr;
    printf("host is %s\n",host);
    
    control_sock = socket(AF_INET,SOCK_STREAM,0);
    if(control_sock < 0)
    {
        printf("Fail to open the socket");
        exit(0);
    }

    bzero(&servaddr,sizeof(servaddr));//初始化结构体
    servaddr.sin_family=AF_INET;//设置地址家族
    servaddr.sin_port=htons(port);//设置端口
    servaddr.sin_addr.s_addr=inet_addr(host);//设置地址

    if(connect(control_sock,(struct sockaddr*)&servaddr,sizeof(struct sockaddr)) == -1)
    {
        printf("Fail to open the socket");
        exit(0);
    }

    return control_sock;
}


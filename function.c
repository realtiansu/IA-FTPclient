#include "function.h"


void ftp_rename(int csockfd, char *path1, char *path2)      //handler of rename
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);

    sprintf(sendline, "RNFR %s\r\n", path1);
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send rnfr error\n");
        return;
    }
    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result < 0 || strncmp(recvline,"350",3)!=0)
    {
        printf("receive rnfr error, %s\n", recvline);
        return;
    }

    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);

    sprintf(sendline, "RNTO %s\r\n", path2);
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send rnto error\n");
        return;
    }
    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result < 0 || strncmp(recvline,"250",3)!=0)
    {
        printf("receive rnto error, %s\n", recvline);
        return;
    }

}


void ftp_rm(int csockfd, char *path)       //handler of remove
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);

    sprintf(sendline, "RMD %s\r\n", path);

    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send rmd error\n");
        return;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result < 0 || strncmp(recvline,"250",3)!=0)
    {
        printf("%s\n", recvline);
        printf("No this file or directory\n");
    }
}


void ftp_del(int csockfd, char *path)       //handler of remove
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);

    sprintf(sendline, "DELE %s\r\n", path);

    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send rmd error\n");
        return;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    // printf("%s\n", recvline);
    if(result < 0 || strncmp(recvline,"250",3)!=0)
    {
        printf("No this file or directory\n");
    }
}


void ftp_quit(int csockfd)          //handler of quit
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);

    sprintf(sendline, "QUIT\r\n");

    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send quit error\n");
        return;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result < 0 || strncmp(recvline,"221",3)!=0)
    {
        printf("receive quit error, %s\n", recvline);
        return;
    }
    else
    {
        printf("\n");
        exit(0);
    }
    
}


void ftp_mkdir(int csockfd, char *path)         //handler of make directory
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    sprintf(sendline, "MKD %s\r\n", path);

    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send mkd error\n");
        return;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result < 0 || strncmp(recvline,"257",3)!=0)
    {
        printf("receive mkd error, %s\n", recvline);    //报错550 Permission denied的话，去把/etc/vsftpd.conf的 write_enable=YES的注释删掉
        return;
    }
}


void ftp_changdir(int csockfd, char *path)          //handler of change directory
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    sprintf(sendline, "CWD %s\r\n", path);

    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send cd error\n");
        return;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result < 0 || strncmp(recvline,"250",3)!=0)
    {
        printf("receive cd error, %s\n", recvline);
        return;
    }
    // else
    // {
    //     printf("%s\n", recvline);
    // }
}

void ftp_list(int csockfd, char *path, bool type, bool mode)        //handler of list directory
{
    int dsockfd, localport;
    localport = random(30000) + 1025;

    if(!mode)
    {
        dsockfd = swToPasv(csockfd);
        if(dsockfd < 0) {
            return;
        }

        ftp_list_pasv(csockfd, dsockfd, path);
    }
    else
    {
        dsockfd = swToPort(csockfd, localport);
        if(dsockfd < 0) {
            return;
        }

        ftp_list_port(csockfd, dsockfd, localport, path);
    }
    
    // if(dsockfd < 0)
    // {
    //     return;
    // }
    // else
    // {
    //     bool r = setType(csockfd, type);
    //     if(!r)
    //     {
    //         return;
    //     }
    // }
    
}


void ftp_pwd(int csockfd)           //handler of pwd
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    sprintf(sendline, "PWD\r\n");
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("error when sending pwd\n");
    }
    result = recv(csockfd,recvline,sizeof(recvline),0);
    // printf("%s\n", recvline);
    if(strncmp(recvline,"257",3)==0)
    {
        int i=0;
        char *ptr;
        char currendir[1024];
        memset(currendir, 0, 1024);
        ptr=recvline+5;
        while(*(ptr)!='"')
        {
            currendir[i++]=*(ptr);
            ptr++;
        }
        currendir[i]='\0';
        printf("%s\n",currendir);
 
    }
    else
    {
        printf("pwd,recv is error, %s\n", recvline);
    }
}


int swToPasv(int csockfd)               //switch to passive mode 
{
    int i,j,result,dsockfd;
    int ip1,ip2,ip3,ip4;
    struct sockaddr_in servaddr;
    char sendline[MAXSIZE],recvline[MAXSIZE], ip[MAXSIZE];
    bzero(&servaddr,sizeof(servaddr));
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    sprintf(sendline, "PASV\r\n");

    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send pasv error\n");
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    printf(",, %s\n", recvline);
    if(result < 0 || strncmp(recvline,"227",3)!=0)
    {
        printf("receive pasv error, %s\n", recvline);
        return -1;
    }

    //处理ftp server 端口
    char *ptr1,*ptr2;
    char num[MAXSIZE];
    memset(num, 0, MAXSIZE);

    //取低位字节，是i
    ptr1=recvline+strlen(recvline);
    while(*(ptr1)!=')') { ptr1--; }
    ptr2=ptr1;
    while(*(ptr2)!=',') { ptr2--; }
    strncpy(num,ptr2+1,ptr1-ptr2-1);
    i=atoi(num);//将字符串转换成整数

    //取高位字节, 是j
    memset(num, 0, MAXSIZE);
    ptr1=ptr2;
    ptr2--;
    while(*(ptr2)!=',') { ptr2--; }
    strncpy(num,ptr2+1,ptr1-ptr2-1);
    j=atoi(num);

    memset(num, 0, MAXSIZE);
    ptr1=ptr2;
    ptr2--;
    while(*(ptr2)!=',') { ptr2--; }
    strncpy(num,ptr2+1,ptr1-ptr2-1);
    ip4=atoi(num);

    memset(num, 0, MAXSIZE);
    ptr1=ptr2;
    ptr2--;
    while(*(ptr2)!=',') { ptr2--; }
    strncpy(num,ptr2+1,ptr1-ptr2-1);
    ip3=atoi(num);

    memset(num, 0, MAXSIZE);
    ptr1=ptr2;
    ptr2--;
    while(*(ptr2)!=',') { ptr2--; }
    strncpy(num,ptr2+1,ptr1-ptr2-1);
    ip2=atoi(num);

    memset(num, 0, MAXSIZE);
    ptr1=ptr2;
    ptr2--;
    while(*(ptr2)!='(') { ptr2--; }
    strncpy(num,ptr2+1,ptr1-ptr2-1);
    ip1=atoi(num);

    // printf("%d.%d.%d.%d\n", ip1, ip2, ip3, ip4);
    sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);       //得到ip

    int data_serviceport;
    data_serviceport=j*256+i;           //得到端口
    
    dsockfd=socket(AF_INET,SOCK_STREAM,0);

    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(ip);
    servaddr.sin_port=htons(data_serviceport);
    
    if(connect(dsockfd,(struct sockaddr*)&servaddr,sizeof(struct sockaddr))==-1)
    {
        printf("pasv port connect error!\n");
        return -1;
    }
    else
    {
        printf("pasv remote port %d connected\n", data_serviceport);
    }

    return dsockfd;
}


int swToPort(int csockfd, int localport)                //switch to active mode 
{
    int i,j,result,dsockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXSIZE],recvline[MAXSIZE],ip[MAXSIZE];
    bzero(&servaddr,sizeof(servaddr));
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    memset(ip, 0, MAXSIZE);
    
    dsockfd = socket(AF_INET,SOCK_STREAM,0);
    if(dsockfd < 0)
    {
        printf("data port create error\n");
        return -1;
    }

    int ret = sizeof(struct sockaddr_in);
    getsockname(csockfd, (struct sockaddr*)&servaddr, &ret);
    inet_ntop(AF_INET,&(servaddr.sin_addr),ip,sizeof(ip));      //得到了自己的ip
    
    bool r = replace(ip,".",",");

    j=localport/256;
    i=localport%256;
    sprintf(sendline, "PORT %s,%d,%d\r\n", ip, j, i);
    printf("sendline is %s\n", sendline);
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("port send is error!\n");
        return -1;
    }
    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result < 0 || strncmp(recvline,"200",3) != 0)
    {
        printf("port recv is error, %s\n", recvline);
        return -1;
    }
    else
    {
        printf("port listening at local %d, %s\n", localport, recvline);
        return dsockfd;
    }

    return -1;
}

bool setType(int csockfd, bool type)                //set binary or ascii
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);

    if(!type)
    {
        sprintf(sendline, "TYPE I\r\n");
    }
    else
    {
        sprintf(sendline, "TYPE A\r\n");
    }
    
    
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send type error\n");
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(strncmp(recvline,"200",3)==0)
    {
        printf("%s\n", recvline);
        return true;
    }
    else
    {
        printf("receive type error, %s\n", recvline);
    }

    return false;
}


void ftp_list_pasv(int csockfd, int dsockfd, char *path)            //handler of list in pasv mode
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE],data[32767];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    memset(data, 0, 32767);

    if(path == NULL) {
        sprintf(sendline, "LIST\r\n");
    } else {
        sprintf(sendline, "LIST %s\r\n", path);
    }

    result = send(csockfd,sendline,strlen(sendline),0);
    // printf("%s\n", sendline);
    if(result < 0)
    {
        printf("send list error\n");
        goto pasvEnd;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    // printf("%s\n", recvline);
    if(result < 0 || strncmp(recvline,"150",3)!=0)
    {
        printf("receive list error, %s\n", recvline);
        goto pasvEnd;
    }
    else
    {
        result = recv(dsockfd,data,sizeof(data),0);
        // printf("recv %d\n", strlen(data));
        if(result < 0) {
            printf("receive list error\n");
            goto pasvEnd;
        }
        else {
            if(strlen(data)>0) {
                printf("\n");
                printf("%s\n", data);
            } else {
                printf("No such directory\n");
            }
        }

        result = recv(csockfd,recvline,sizeof(recvline),0);
        if(result < 0 || strncmp(recvline,"226",3)!=0)
        {
            printf("receive list error, %s\n", recvline);
            goto pasvEnd;
        }
    }

    pasvEnd:
    close(dsockfd);
    return;
}


void ftp_list_port(int csockfd, int dsockfd, int localport, char *path)              //handler of list in active mode
{
    int result;
    char sendline[MAXSIZE],recvline[MAXSIZE],data[32767];
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    memset(sendline, 0, MAXSIZE);
    memset(sendline, 0, MAXSIZE);
    memset(data, 0, 32767);

    if(path == NULL)
    {
        sprintf(sendline, "LIST\r\n");
    }
    else
    {
        // printf("%s\n", path);
        sprintf(sendline, "LIST %s\r\n", path);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(localport);
    if (bind(dsockfd,(struct sockaddr*)&servaddr,sizeof(struct sockaddr)) == -1) {
        printf("bind data port failed!\n");
        goto portEnd;
    }
    if(listen(dsockfd,10) == -1) {
        printf("listen data port failed!\n");
        goto portEnd;
    }

    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0)
    {
        printf("send list error\n");
        goto portEnd;
    }
    
    int struct_len = sizeof(struct sockaddr_in);
    int new_dfd = accept(dsockfd,(struct sockaddr*)&servaddr, &struct_len);
    if(new_dfd < 0)
    {
        printf("accept data connect error\n");
        goto portEnd;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    // printf("%s\n", recvline);
    if(result < 0 || strncmp(recvline,"150",3)!=0)
    {
        printf("receive list error, %s\n", recvline);
        goto portEnd;
    }
    else
    {
        result = recv(new_dfd, data, 32766, 0);
        if(result < 0)
        {
            printf("receive list data error\n");
            goto portEnd;
        }
        else 
        {
            printf("\n%s\n", data);
        }

        result = recv(csockfd,recvline,sizeof(recvline),0);
        if(result < 0 || strncmp(recvline,"226",3)!=0)
        {
            printf("receive list data error, %s\n", recvline);
            goto portEnd;
        }
    }

    portEnd:
    close(dsockfd);
    return;
}

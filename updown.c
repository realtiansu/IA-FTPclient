#include "function.h"


void ftp_up(int csockfd, char *path1, char *path2, bool type, bool mode, int speed) {
    int dsockfd, localport;
    localport = random(30000) + 1025;
    // printf("%d,%d\n", type, mode);
    
    if(!mode) {
        dsockfd = swToPasv(csockfd);
        if(dsockfd < 0) {
            return;
        }

        ftp_up_pasv(csockfd, dsockfd, path1, path2, speed);
    }
    else {
        dsockfd = swToPort(csockfd, localport);
        if(dsockfd < 0) {
            return;
        }

        ftp_up_port(csockfd, dsockfd, localport, path1, path2, speed);
    }

}


void ftp_up_pasv(int csockfd, int dsockfd, char *path1, char *path2, int speed) {            //handler of upload in pasv mode
    int result, rc;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    unsigned char buf[20];

    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    memset(buf, 0, 20);

    FILE *fp, *fp2;
    fp = fopen(path1, "rb");

    if(fp == NULL) {
        printf("%s no such file\n", path1);
        goto pasvEnd2;
    } 
    // else {
    //     size = fread(data, 1, sizeof(data), fp);
    //     fclose(fp);
    // }

    sprintf(sendline, "STOR %s\r\n", path2);
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0) {
        printf("send stor error\n");
        goto pasvEnd2;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result < 0 || strncmp(recvline,"150",3)!=0) {
        printf("receive stor error, %s\n", recvline);
        goto pasvEnd2;
    } else {
        while( (rc = fread(buf,sizeof(unsigned char), 20, fp)) != 0 ) {
            result = send(dsockfd, buf, 20, 0);
            if(speed < 2000){
                usleep(1000000/speed);
            }
        }
        // char drc[1];
        // float com;
        // for(int i=0; i<strlen(data); i++) {
        //     drc[0] = data[i];
        //     result = send(dsockfd, drc, 1, 0);
        //     
        //     com = 100*(i+1)/strlen(data);
        //     printf("%%%.f\n", com);
        // }

        close(dsockfd);
        if(result < 0) {
            return;
        } else {
            result = recv(csockfd,recvline,sizeof(recvline),0);
            if(result < 0 || strncmp(recvline,"226",3)!=0)
            {
                printf("receive list error, %s\n", recvline);
                return;
            }
        }
    }

    pasvEnd2:
    close(dsockfd);
    fclose(fp);
    return;
}


void ftp_up_port(int csockfd, int dsockfd, int localport, char *path1, char *path2, int speed) {         //handler of upload in active mode
    int result, rc;
    char sendline[MAXSIZE],recvline[MAXSIZE];
    unsigned char buf[20];
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    memset(buf, 0, 20);

    FILE *fp;
    fp = fopen(path1, "r");
    if(fp == NULL) {
        printf("%s no such file\n", path1);
        goto portEnd2;
    }
    //  else {
    //     size = fread(data, 1, sizeof(data), fp);
    //     fclose(fp);
    // }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(localport);
    if (bind(dsockfd,(struct sockaddr*)&servaddr,sizeof(struct sockaddr)) == -1) {
        printf("bind data port failed!\n");
        goto portEnd2;
    }
    if(listen(dsockfd,10) == -1) {
        printf("listen data port failed!\n");
        goto portEnd2;
    }

    sprintf(sendline, "STOR %s\r\n", path2);
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0) {
        printf("send stor error\n");
        goto portEnd2;
    }
    
    int struct_len = sizeof(struct sockaddr_in);
    int new_dfd = accept(dsockfd,(struct sockaddr*)&servaddr, &struct_len);
    if(new_dfd < 0)
    {
        printf("accept data connect error\n");
        goto portEnd2;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);
    if(result < 0 || strncmp(recvline,"150",3)!=0) {
        printf("receive stor error, %s\n", recvline);
        goto portEnd2;
    } else {
        while( (rc = fread(buf,sizeof(unsigned char), 20, fp)) != 0 ) {
            result = send(dsockfd, buf, 20, 0);
            if(speed < 2000){
                usleep(1000000/speed);
            }
        }

        close(new_dfd);
        if(result < 0) {
            printf("receive list error\n");
            close(dsockfd);
            return;
        } else {
            result = recv(csockfd,recvline,sizeof(recvline),0);
            if(result < 0 || strncmp(recvline,"226",3)!=0)
            {
                printf("receive list error, %s\n", recvline);
                close(dsockfd);
                return;
            }
        }
    }

    portEnd2:
    close(new_dfd);
    close(dsockfd);
    return;
}




void ftp_down(int csockfd, char *path1, char *path2, bool type, bool mode) {        
    int dsockfd, localport;
    localport = random(30000) + 1025;
    // printf("%d,%d\n", type, mode);
    
    if(!mode) {
        dsockfd = swToPasv(csockfd);
        if(dsockfd < 0) {
            return;
        }

        ftp_down_pasv(csockfd, dsockfd, path1, path2);
    }
    else {
        dsockfd = swToPort(csockfd, localport);
        if(dsockfd < 0) {
            return;
        }

        ftp_down_port(csockfd, dsockfd, localport, path1, path2);
    }
}


void ftp_down_pasv(int csockfd, int dsockfd, char *path1, char *path2) {             //handler of download in pasv mode
    int result, size;
    char sendline[MAXSIZE],recvline[MAXSIZE],data[32767];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    memset(data, 0, 32767);

    sprintf(sendline, "RETR %s\r\n", path1);
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0) {
        printf("send retr error\n");
        goto pasvEnd;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);     //printf("retr, %s\n", recvline);
    if(result < 0 || strncmp(recvline,"150",3)!=0) {
        printf("receive retr error, %s\n", recvline);
        goto pasvEnd;
    } else {
        result = recv(dsockfd, data, sizeof(data), 0);
        if(result < 0) {
            printf("receive retr data error\n");
            goto pasvEnd;
        } else {
            // printf("%s\n", data);
            FILE *fp;
            fp = fopen(path2, "w");
            if(fp == NULL) {
                printf("%s fail to open\n", path2);
                goto pasvEnd;
            } else {
                // size = fread(data, 1, sizeof(data), fp);
                fwrite(data,1,result,fp);
                fclose(fp);
            }

        }

        result = recv(csockfd,recvline,sizeof(recvline),0);
        if(result < 0 || strncmp(recvline,"226",3)!=0)
        {
            printf("receive retr error, %s\n", recvline);
            goto pasvEnd;
        }
        
    }

    pasvEnd:
    close(dsockfd);
    return;
}


void ftp_down_port(int csockfd, int dsockfd, int localport, char *path1, char *path2) {              //handler of download in active mode
    int result, size;
    char sendline[MAXSIZE],recvline[MAXSIZE],data[32767];
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    memset(data, 0, 32767);

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

    sprintf(sendline, "RETR %s\r\n", path1);
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0) {
        printf("send retr error\n");
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
    if(result < 0 || strncmp(recvline,"150",3)!=0) {
        // printf("receive stor error, %s\n", recvline);
        goto portEnd;
    } else {
        result = recv(new_dfd, data, sizeof(data), 0);
        // printf("%s,%s\n", recvline, data);
        if(result < 0) {
            printf("receive retr data error\n");
            goto portEnd;
        } else {
            // printf("%s\n", data);
            FILE *fp;
            fp = fopen(path2, "w");
            if(fp == NULL) {
                printf("%s fail to open\n", path2);
                goto portEnd;
            } else {
                // size = fread(data, 1, sizeof(data), fp);
                fwrite(data,1,result,fp);
                fclose(fp);
            }

        }

        result = recv(csockfd,recvline,sizeof(recvline),0);
        if(result < 0 || strncmp(recvline,"226",3)!=0)
        {
            printf("receive retr error, %s\n", recvline);
            goto portEnd;
        }
        
        
    }

    portEnd:
    close(new_dfd);
    close(dsockfd);
    return;
}


void rest_down(int csockfd, char *path1, char *path2, bool type, bool mode) {
    int dsockfd, localport;
    // localport = random(30000) + 1025;
    // printf("%d,%d\n", type, mode);
    
    // if(!mode) {
    //     dsockfd = swToPasv(csockfd);
    //     if(dsockfd < 0) {
    //         return;
    //     }

    //     ftp_down_pasv(csockfd, dsockfd, path1, path2);
    // }
    // else {
    //     dsockfd = swToPort(csockfd, localport);
    //     if(dsockfd < 0) {
    //         return;
    //     }

    //     ftp_down_port(csockfd, dsockfd, localport, path1, path2);
    // }
    dsockfd = swToPasv(csockfd);
    if(dsockfd < 0) {
        return;
    }

    rest_down_pasv(csockfd, dsockfd, path1, path2);
}


void rest_down_pasv(int csockfd, int dsockfd, char *path1, char *path2) {
    int result, size = file_size(path2);
    char sendline[MAXSIZE],recvline[MAXSIZE],data[32767];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    memset(data, 0, 32767);

    sprintf(sendline, "REST %d\r\n", size);
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0) {
        printf("send retr error\n");
        goto end;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);     //printf("retr, %s\n", recvline);
    // printf("receive, %s\n", recvline);
    if(result < 0 || strncmp(recvline,"350",3)!=0) {
        printf("rest error, %s\n", recvline);
        goto end;
    } else {
        ftp_append_pasv(csockfd, dsockfd, path1, path2);
    }

    end:
    close(dsockfd);
    return;
}


void ftp_append_pasv(int csockfd, int dsockfd, char *path1, char *path2) {
    int result, size;
    char sendline[MAXSIZE],recvline[MAXSIZE],data[32767];
    memset(sendline, 0, MAXSIZE);
    memset(recvline, 0, MAXSIZE);
    memset(data, 0, 32767);

    sprintf(sendline, "RETR %s\r\n", path1);
    result = send(csockfd,sendline,strlen(sendline),0);
    if(result < 0) {
        printf("send retr error\n");
        goto end;
    }

    result = recv(csockfd,recvline,sizeof(recvline),0);     //printf("retr, %s\n", recvline);
    if(result < 0 || strncmp(recvline,"150",3)!=0) {
        printf("receive retr error, %s\n", recvline);
        goto end;
    } else {
        result = recv(dsockfd, data, sizeof(data), 0);
        if(result < 0) {
            printf("receive retr data error\n");
            goto end;
        } else {
            // printf("%s\n", data);
            FILE *fp;
            fp = fopen(path2, "a");
            if(fp == NULL) {
                printf("%s fail to open\n", path2);
                goto end;
            } else {
                // size = fread(data, 1, sizeof(data), fp);
                fwrite(data,1,result,fp);
                fclose(fp);
            }

        }

        result = recv(csockfd,recvline,sizeof(recvline),0);
        if(result < 0 || strncmp(recvline,"226",3)!=0)
        {
            printf("receive retr error, %s\n", recvline);
            goto end;
        }
        
    }

    end:
    close(dsockfd);
    return;
}
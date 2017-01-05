/*************************************************************************
    > File Name: client.c
    > Author: RyanLee
    > Mail: chason0207@163.com
    > Created Time: 2017-01-04 15:24:49
 ************************************************************************/

/*******   http客户端程序   httpclient.c   ************/
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <sys/types.h>
#include   <sys/socket.h>
#include   <errno.h>
#include   <unistd.h>
#include   <netinet/in.h>
#include   <limits.h>
#include   <netdb.h>
#include   <arpa/inet.h>
#include   <ctype.h>


#define TEXT_BUFFSIZE 1024
//////////////////////////////httpclient.c   开始///////////////////////////////////////////

/********************************************
功能：搜索字符串右边起的第一个匹配字符
********************************************/
char* Rstrchr(char* s, char x)
{
    int i = strlen(s);
    if (!(*s))
    {
        return 0;
    }
    while (s[i - 1])
    {
        if (strchr(s + (i - 1), x))
        {
            return (s + (i - 1));
        }
        else
        {
            i--;
        }
    }
    return 0;
}

/********************************************
功能：把字符串转换为全小写
********************************************/
void   ToLowerCase(char* s)
{
    while (*s && *s != '\0' )
    {
        *s = tolower(*s++);
    }
    *s = '\0';
}

/**************************************************************
功能：从字符串src中分析出网站地址和端口，并得到用户要下载的文件
***************************************************************/
void GetHost(char* src, char* web, char* file, int* port)
{
    char* pA;
    char* pB;

    memset(web, 0, sizeof(web));
    memset(file, 0, sizeof(file));
    *port = 0;
    if (!(*src)){
        return;
    }

    pA = src;
    if (!strncmp(pA, "http://", strlen("http://"))){
        pA = src + strlen("http://");
    }else if (!strncmp(pA, "https://", strlen( "https://"))){
        pA = src + strlen( "https://");
    }

    pB = strchr(pA, '/');
    if (pB){
        memcpy(web, pA, strlen(pA) - strlen(pB));
        if (pB + 1){
            memcpy(file, pB + 1, strlen(pB) - 1);
            file[strlen(pB) - 1] = 0;
        }
    }else{
        memcpy(web, pA, strlen(pA));
    }

    if (pB){
        web[strlen(pA) - strlen(pB)] = 0;
    }else{
        web[strlen(pA)] = 0;
    }

    pA = strchr(web, ':');
    if (pA){
        *port = atoi(pA + 1);
    }else{
        *port = 80;
    }
}

/*********************************************************************
*filename:   httpclient.c
*purpose:   HTTP协议客户端程序，可以用来下载网页
*********************************************************************/
int   main(int   argc,   char   *argv[])
{
    int sockfd = 0;
    char buffer[2049] = "";
    struct sockaddr_in   server_addr;
    struct hostent   *host;
    int port = 0;
    int nbytes = 0;
    char host_addr[256] = "";
    char host_file[1024] = "";
    FILE *fp;
    char request[1024] = "";
    // int send = 0;
    // int totalsend = 0;
    int i = 0;
    char *pt;
	int  running = 1;
	int ret;


    if (argc != 2)
    {
        fprintf(stderr, "Usage:%s   web-address\a\n ", argv[0]);
        exit(1);
    }
    printf( "parameter.1 is: %s\n ", argv[1]);
    //ToLowerCase(argv[1]);/*将参数转换为全小写*/
    //printf( "lowercase   parameter.1   is:   %s\n ",   argv[1]);

    GetHost(argv[1], host_addr, host_file, &port);/*分析网址、端口、文件名等*/
    printf( "webhost:%s\n ", host_addr);
    printf( "hostfile:%s\n ", host_file);
    printf( "port:%d\n\n ", port);

    if ((host = gethostbyname(host_addr)) == NULL) /*取得主机IP地址*/
    {
        fprintf(stderr, "Gethostname   error,   %s\n ",   strerror(errno));
        exit(1);
    }

    /*   客户程序开始建立   sockfd描述符   */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) /*建立SOCKET连接*/
    {
        fprintf(stderr, "Socket   Error:%s\a\n ", strerror(errno));
        exit(1);
    }

    /*   客户程序填充服务端的资料   */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr*)host->h_addr);

    printf("start connect to %s...\n", inet_ntoa(*((struct in_addr*)host->h_addr)));
    /*   客户程序发起连接请求   */
    if (connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1) /*连接网站*/
    {
        fprintf(stderr, "Connect   Error:%s\a\n ", strerror(errno));
        exit(1);
    }

    sprintf(request, "GET /%s HTTP/1.1\r\nHost:%s\r\nConnection: Keep-Alive\r\n\r\n", host_file, host_addr);

    printf( "%s\n", request);/*准备request，将要发送给主机*/

    /*取得真实的文件名*/
    if (host_file && *host_file){
        pt = Rstrchr(host_file, '/');
    }else{
        pt = 0;
    }

	if (send(sockfd, request, strlen(request) + 1, 0) == -1) {
		printf( "send error!%s\n ", strerror(errno));
		exit(1);
	}

    printf( "\nThe following is the response header:\n ");

    while (running) {
		memset(buffer, 0, sizeof(buffer));
		int bufflen = recv(sockfd, buffer, TEXT_BUFFSIZE, 0);
		
		if (bufflen < 0) {
			printf("接收数据流出错\n");
			close(sockfd);
			return 0;
		}

		if (bufflen > 0) {
			printf("%s\n", buffer);
		} else {
			running = 0;
		}
	}
    /*   结束通讯   */
    close(sockfd);
    return 0;
}
//////////////////////////////httpclient.c   结束///////////////////////////////////////////

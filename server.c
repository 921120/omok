#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
//#include "Omok.h"
int clnt_sock;
int serv_sock;
struct sockaddr_in serv_addr;
struct sockaddr_in clnt_addr;
int clnt_addr_size;
int connect_server(int port, char* ipaddr){
	int socket1;
	int ret;
	struct sockaddr_in staddr;
	if((socket1 = socket(PF_INET, SOCK_STREAM,0)) ==-1){
		perror("socket()");
		return -1;
	}

	memset(&staddr, 0, sizeof(staddr));
	staddr.sin_family   = AF_INET;
	staddr.sin_port    = htons(port);
	staddr.sin_addr.s_addr    = inet_addr(ipaddr);

	if((ret = connect(socket1, (struct sockaddr*)&staddr, sizeof(staddr))) < 0){
		perror("connect()");
		close(socket1);
		return -1;
	}
	return socket1;
}

int acceptt(){
	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	if(clnt_sock == -1)
		perror("accept error");

	return clnt_sock;
}

int open_server(int port){

	if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket open error");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		perror("bind error");

	if(listen(serv_sock, 5) == -1)
		perror("listen error"); 

	return acceptt();
}

void sendXY(int x, int y, int clnt_sock){
	char t1[3], t2[3];
	sprintf(t1, "%d", x);
	sprintf(t2, "%d", y);
	write(clnt_sock, t1, sizeof(t1));
	write(clnt_sock, t2, sizeof(t2));
}

void receiveXY(int *x, int *y, int clnt_sock){
	char t1[3], t2[3];
	int str_len;

	str_len = read(clnt_sock, t1, sizeof(t1));
	if(str_len == -1)
		perror("read error");
	str_len = read(clnt_sock, t2, sizeof(t2));
	if(str_len == -1)
		perror("read error");

	*x = atoi(t1);
	*y = atoi(t2);
}

void close_sock(int clnt_sock){
	close(clnt_sock);
}


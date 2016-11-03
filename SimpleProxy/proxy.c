#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>

#define MAXBUFF 1024

void getWeb(int clientSock, char* uri, char* version){
	struct sockaddr_in host_addr;
	struct hostent* host;
	int port=80, hostSock, newSock, nbytes;
	char* temp;
	char hostName[MAXBUFF], buffer[MAXBUFF];
	
	temp=strtok(uri, "//");
	temp=strtok(NULL, "/");
	sprintf(hostName, "%s", temp);
	host=gethostbyname(hostName);
	temp=strtok(NULL, "");
	
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(port);
	bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
	
	if((hostSock=socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("Unable to create socket; closing socket\n");
		exit(0);
	}
	
	if((newSock=connect(hostSock,(struct sockaddr*)&host_addr,sizeof(struct sockaddr)))<0){
		printf("Error connecting to remote server\n");
	}
	
	if(temp==NULL){
		sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n", version, hostName);
	}
	else{
		sprintf(buffer, "GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n", temp, version, hostName);
	}
	
	if((nbytes=send(hostSock, buffer, strlen(buffer), 0))<0){
		printf("Error writing to socket\n");
		write(clientSock, "HTTP/1.1 500 Internal Server Error: cannot allocate memory\n\n", 60);
		exit(0);
	}
	
	bzero(buffer, sizeof(buffer));
	nbytes=recv(hostSock, buffer, MAXBUFF, 0);
	while(nbytes>0){
		send(clientSock, buffer, nbytes, 0);
	}
	bzero(buffer, sizeof(buffer));
	close(newSock);
	close(hostSock);
}

int main(int argc, char* argv[]){
	int serverSock, clientSock, nbytes;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len;
	char buf[MAXBUFF], method[MAXBUFF], uri[MAXBUFF], version[MAXBUFF];
	
	if (argc<2){
		printf("Please provide port number (./proxy <port#>)\n");
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	if((serverSock=socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("Unable to create socket; closing socket\n");
		exit(0);
	}
	
	if (bind(serverSock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("Unable to bind socket; closing socket\n");
		close(serverSock);
		exit(1);
	}
	
	if(listen(serverSock, 100)!=0){
		printf("Unable to listen; closing socket\n");
		close(serverSock);
		exit(1);
	}
	
	printf("Accepting requests.\n");
	addr_len=sizeof(client_addr);
	
	while(1){
		
		bzero(method, sizeof(method));
		bzero(uri, sizeof(uri));
		bzero(version, sizeof(version));
		bzero(buf, sizeof(buf));
		
		if((clientSock = accept(serverSock, (struct sockaddr *) &client_addr, &addr_len)) < 0) {
			printf("Error on attempting to accept\n");
			close(clientSock);
			exit(0);
		}
		
		if(fork()==0){
			if ((nbytes = recv(clientSock, &buf, MAXBUFF, 0)) > 0) {
				sscanf(buf, "%s %s %s %*s", method, uri, version);
				printf("%s\n", method);
				printf("%s\n", uri);
				printf("%s\n", version);
				if(strncmp(method, "GET", 3)==0){
					if(strncmp(uri, "/http://", 8)==0){
						if(strncmp(version, "HTTP/1.1", 8)==0 || strncmp(version, "HTTP/1.0", 8)==0){
							printf("Successfull\n");
							getWeb(clientSock, uri, version);
						}
						else{
							printf("Invalid Version\n");
							write(clientSock, "HTTP/1.1 404 Bad Request\n", 26);
							write(clientSock, "Content-length: 50\n", 19);
							write(clientSock, "Content-Type: text/html\n\n", 25);
							write(clientSock, "<html><body><H1>Invalid Version</H1></body></html>", 50);
						}
					}
					else{
						printf("Invalid URI\n");
						write(clientSock, "HTTP/1.1 404 Bad Request\n", 26);
						write(clientSock, "Content-length: 46\n", 19);
						write(clientSock, "Content-Type: text/html\n\n", 25);
						write(clientSock, "<html><body><H1>Invalid URI</H1></body></html>", 46);
					}
				}
				else{
					printf("Invalid Method\n");
					write(clientSock, "HTTP/1.1 404 Bad Request\n", 26);
					write(clientSock, "Content-length: 49\n", 19);
					write(clientSock, "Content-Type: text/html\n\n", 25);
					write(clientSock, "<html><body><H1>Invalid Method</H1></body></html>", 49);
				}
			}
			else if((nbytes = recv(clientSock, &buf, MAXBUFF, 0)) == -1){
				write(clientSock, "HTTP/1.1 500 Internal Server Error: cannot allocate memory\n\n", 60);
			}
		}
		close(clientSock);	
		printf("Listening again\n");
	}
	close(clientSock);
	close(serverSock);
	return(0);
}
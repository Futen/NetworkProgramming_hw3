#include "Header.h"

void* Command(void *arg);
void* WaitForConnect(void *arg);
void* FileUpload(void *arg);
vector<string> ListDir();

int main(int argc,char** argv){
    int service_port;
    int sockfd;
	struct sockaddr_in servaddr;
    pthread_t tid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
 	bzero(&servaddr, sizeof(servaddr));
 	servaddr.sin_family = AF_INET;
 	servaddr.sin_port = htons(atoi(argv[2]));
    service_port = atoi(argv[3]);	
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	

	exit(0);
}
void* WaitForConnect(void *arg){
    int service_port = *((int*)arg);
    int server_sockfd;
    int *connfd;
    int nbytes;
    char recvline[MAXLINE];
    string recvData;
    struct sockaddr_in server_addr, client_addr;
    socklen_t clilen;
    pthread_t tid;

    printf("Port: %d\n", service_port);
    service_port = htons(service_port);
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = service_port;

    bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sockfd, LISTENQ);
    while(true){
        clilen = sizeof(client_addr);
        connfd = (int*)malloc(sizeof(int));
        *connfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &clilen);
        nbytes = read(*connfd, recvline, MAXLINE);
        recvData = recvline;
        if(recvData == "FILE"){
            pthread_create(&tid, NULL, &FileUpload, connfd);
        }
    }
    return NULL;
}
void* FileUpload(void *arg){
    int nbytes;
    int connfd = *((int*)arg);
    free(arg);
    pthread_detach(pthread_self());
    return NULL;
}
vector<string> ListDir(){
    vector<string> ls;
    DIR *dp;
    dirent *d;
    system("mkdir -p data");

    if((dp = opendir("data/")) == NULL){
        printf("Error Dir\n");
        exit(0);
    }
    while((d = readdir(dp)) != NULL){
        if(!(!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))){
            ls.push_back(string(d->d_name));
        }
    }
    return ls;
}


#include "ClientHeader.h"

void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen);

int main(int argc, char* argv[]){
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    //dg_cli(stdin, sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    ConnectProcess(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    return 0;
}
void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen){
    int n;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    while (fgets(sendline, MAXLINE, fp) != NULL){
        sendline[strlen(sendline)-1] = '\0';
        sendto(sockfd, sendline, strlen(sendline) + 1, 0, pservaddr, servlen);
        
        //n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        //printf("%s\n", recvline);    
    }
    return;
}














#ifndef ClientHeader
#define ClientHeader
#include "Header.h"


void ConnectProcess(int sockfd, struct sockaddr *server_addr, socklen_t servlen);
int Login(int sockfd, struct sockaddr *server_addr, socklen_t servlen);
void ShowCommand();
string GetCommandString(string input);
void ProcessCommand(int command, string sendData, int sockfd, struct sockaddr *server_addr, socklen_t servlen);

#endif

#include "Header.h"

string user_account;
string user_password;
string user_nickname;
string user_birthday;

void ShowCommand();
string GetCommandString(string input);
void ProcessCommand(int sockfd, string sendData);
void* Command(void *arg);
void* WaitForConnect(void *arg);
void* FileUpload(void *arg);
void* RecvFromServer(void* arg);
vector<string> ListDir();
int Login(int sockfd, vector<string> file_lst);

int main(int argc,char** argv){
    int service_port;
    int sockfd;
	struct sockaddr_in servaddr;
    string sendData;
    vector<string> file_lst;
    pthread_t tid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
 	bzero(&servaddr, sizeof(servaddr));
 	servaddr.sin_family = AF_INET;
 	servaddr.sin_port = htons(atoi(argv[2]));
    service_port = atoi(argv[3]);	
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    file_lst = ListDir();
    //for(int i=0; i<file_lst.size(); i++)
    //    cout << file_lst[i]<< endl;
	
	connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    Login(sockfd, file_lst);
    pthread_create(&tid, NULL, &RecvFromServer, &sockfd);

    while(true){
        ShowCommand();
        cin >> sendData;
        sendData = GetCommandString(sendData);
        ProcessCommand(sockfd, sendData);
    }

	exit(0);
}
void* RecvFromServer(void* arg){
    int sockfd = *((int*)arg);
    int nbytes;
    char recvline[MAXLINE];
    Packet *packet;
    while((nbytes=read(sockfd, recvline, sizeof(Packet))) > 0){
       packet = (Packet*)recvline;
       cout << packet->artical << endl;
    }
    return NULL;
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
int Login(int sockfd, vector<string> file_lst){
    char recvline[MAXLINE];
    string login_mode = "LOGIN";
    string create_mode = "CREATEACCOUNT";
    string recvData;
    string command;
    Packet *packet = NewPacket(0);
    cout << "$ Create?" << endl;
    cout << "$ Login?" << endl;
    cin >> command;
    if(command != "Login" && command != "Create")
        return 0;
    cout << "Account: ";
    cin >> user_account;
    cout << "Password: ";
    cin >> user_password;
    if(command == "Login")
        PacketPush(packet, login_mode);
    else if(command == "Create"){
        cout << "Nickname: ";
        cin >> user_nickname;
        cout << "Birthday(yy/mm/dd): ";
        cin >> user_birthday;
        PacketPush(packet, create_mode);
    }
    else
        return 0;
    PacketPush(packet, user_account);
    PacketPush(packet, user_password);
    if(command == "Create"){
        PacketPush(packet, user_nickname);
        PacketPush(packet, user_birthday);
    }
    write(sockfd, (char*)packet, sizeof(Packet));
    delete packet;
    read(sockfd, recvline, MAXLINE);
    packet = (Packet*)recvline;
    if(string("success") == packet->buf[0]){
        user_nickname = packet->buf[1];
        user_birthday = packet->buf[2];
        cout << "Login Success " << user_nickname  << "!!" << endl;
        packet = NewPacket(0);
        packet->number = file_lst.size();
        write(sockfd, (char*)packet, sizeof(Packet));
        delete packet;
        for(int i=0; i<file_lst.size(); i++){
            packet = NewPacket(0);
            PacketPush(packet, file_lst[i]);
            cout << file_lst[i] << endl;
            write(sockfd, (char*)packet, sizeof(Packet));
            delete packet;
        }
        return 1;
    }
    else{
        cout << "Error account or password" << endl;
        return 0;
    }
}
void ShowCommand(){
    cout << "***************" << endl;
    cout << "$ ShowMyInfo" << endl;
    cout << "$ DeleteMyAccount" << endl;
    cout << "$ ModifyMyAccount" << endl;
    cout << "$ ShowMyInvite" << endl;
    cout << "$ InviteWho" << endl;
    cout << "$ AcceptMyInvite" << endl;
    cout << "$ RemoveMyInvite" << endl;
    cout << "$ SearchWho" << endl;
    cout << "$ Logout" << endl;
    cout << "***************" << endl;
}
string GetCommandString(string input){
    if(input == "ShowMyInfo")   return string("SHOWMYINFO");
    else if(input == "DeleteMyAccount") return string("DELETEMYACCOUNT");
    else if(input == "Logout")  return string("LOGOUT");
    else if(input == "ModifyMyAccount") return string("MODIFYACCOUNT");
    else if(input == "ShowMyInvite") return string("SHOWINVITE");
    else if(input == "RemoveMyInvite") return string("REMOVEINVITE");
    else if(input == "AcceptMyInvite") return string("ACCEPTINVITE");
    else if(input == "InviteWho") return string("INVITE");
    else if(input == "SearchWho") return string("SEARCH");
    else    return string("UNKNOWN");
}
void ProcessCommand(int sockfd, string sendData){
    int command = CommandChoose(sendData);
    Packet *packet;
    switch(command){
        case SHOWMYINFO:
            cout << "My Account:{" << endl;
            cout << "\taccount: " << user_account << endl;
            cout << "\tpassword: " << user_password << endl;
            cout << "\tnickname: " << user_nickname << endl;
            cout << "\tbirthday: " << user_birthday << endl;
            cout << "}" << endl;
            break;
        case LOGOUT:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            write(sockfd, (char*)packet, sizeof(Packet));
            delete packet;
            exit(0);
            break;
    }
}

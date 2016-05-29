#include "Header.h"

string user_account;
string user_password;
string user_nickname;
string user_birthday;
struct owner_info{
    string file_name;
    string IP;
    int port;
};
void ShowCommand();
string GetCommandString(string input);
void ProcessCommand(int sockfd, string sendData);
void* Command(void *arg);
void* WaitForConnect(void *arg);
void FileUpload(int sockfd, string file_name, int start_byte, int total_byte);
void FileDownload(int sz, vector<owner_info> info);
void* RecvFromServer(void* arg);
vector<string> ListDir();
int Login(int sockfd, vector<string> file_lst);
bool CheckLst(vector<string> a, vector<string> b);

int service_port;


int main(int argc,char** argv){
    int sockfd;
    struct sockaddr_in servaddr;
    string sendData;
    vector<string> file_lst;
    vector<string> last_lst;
    pthread_t tid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    service_port = atoi(argv[3]);	
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    file_lst = ListDir();
    last_lst = file_lst;
    //for(int i=0; i<file_lst.size(); i++)
    //    cout << file_lst[i]<< endl;

    connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    Login(sockfd, file_lst);
    pthread_create(&tid, NULL, &RecvFromServer, &sockfd);
    pthread_create(&tid, NULL, &WaitForConnect, &service_port);
    int t = 0;
    while(true){
        file_lst = ListDir();
        ShowCommand();
        if(!CheckLst(file_lst, last_lst) && t != 0){
            cin >> sendData;
            sendData = GetCommandString(sendData);
            ProcessCommand(sockfd, sendData);
        }
        else
            ProcessCommand(sockfd, string("UPDATELST"));
        t = 1;
        last_lst = file_lst;
    }

    exit(0);
}
void* RecvFromServer(void* arg){
    int sockfd = *((int*)arg);
    int nbytes;
    int number;
    char recvline[MAXLINE];
    string show = "show";
    string transfer = "transfer";
    string file_name;
    string owner;
    string IP;
    int port;
    int sz;
    Packet *packet;
    pthread_detach(pthread_self());
    while((nbytes=read(sockfd, recvline, sizeof(Packet))) > 0){
        packet = (Packet*)recvline;
        if(show == packet->buf[0])
            cout << packet->artical << endl;
        else if(transfer == packet->buf[0]){
            vector<owner_info> info;
            number = packet->number;
            file_name = packet->buf[1];
            sz = packet->number2;
            //cout << file_name << endl;
            for(int i=0; i<number; i++){
                nbytes = read(sockfd, recvline, sizeof(Packet));
                packet = (Packet*)recvline;
                owner = packet->buf[0];
                IP = packet->buf[1];
                port = packet->number;
                owner_info owner_if;

                owner_if.file_name = file_name;
                owner_if.IP = IP;
                owner_if.port = port;
                info.push_back(owner_if);
                //cout << owner << endl;
                //cout << IP << endl;
                //cout << port << endl;
            }
            FileDownload(sz, info);
        }
    }
    return NULL;
}
void FileDownload(int sz, vector<owner_info> info){
    int sockfd;
    struct sockaddr_in servaddr;
    Packet *packet;
    int start_b = 0;
    int end_b = 0;
    int nbytes;
    int leave;
    int dis = sz / info.size();
    char line[BUFFSIZE];
    sprintf(line, "data/%s", info[0].file_name.c_str());
    FILE *fp = fopen(line, "wb");
    
    int leave_sz = sz;
    cout << "GGGGGGGGGGGGGGGGGGGGGGG\n";
    for(int i=0; i<info.size(); i++){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(info[i].port);
        inet_pton(AF_INET, info[i].IP.c_str(), &servaddr.sin_addr);
        //printf("i %d port %d\n", i, info[i].port);
        connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
        packet = NewPacket(0);
        PacketPush(packet, info[i].file_name);
        packet->number = start_b;
        //cout << "START~~~~~~~~~  " << start_b << endl;
        if(start_b + dis -1 <= sz - 1){
            packet->number2 = dis;
            leave = packet->number2;
        }
        else{
            packet->number2 = sz - start_b - 1;
            leave = packet->number2;
        }
        start_b += dis;
        write(sockfd, (char*)packet, sizeof(Packet));
        delete packet;
        while(leave > 0){
            if(leave >= BUFFSIZE)
                nbytes = read(sockfd, line, BUFFSIZE);
            else
                nbytes = read(sockfd, line, leave);
            fwrite(line, sizeof(char), nbytes, fp);
            leave -= nbytes;
        }
        cout << "File Dowwnload Finish !!!" << endl;
        close(sockfd);
    }
    fclose(fp);
}
void* WaitForConnect(void *arg){
    int service_port = *((int*)arg);
    int server_sockfd;
    int connfd;
    int nbytes;
    char recvline[MAXLINE];
    string recvData;
    struct sockaddr_in server_addr, client_addr;
    socklen_t clilen;
    Packet *packet;

    string IP;
    int port;
    pthread_detach(pthread_self());
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
        connfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &clilen);
        IP = inet_ntoa(((struct sockaddr_in*)&client_addr)->sin_addr);
        port = port = ((struct sockaddr_in*)&client_addr)->sin_port;
        printf("%s %d connect\n", IP.c_str(), port);
        nbytes = read(connfd, recvline, sizeof(Packet));
        packet = (Packet*)recvline;
        string file_name = packet->buf[0];
        int start_b = packet->number;
        int end_b = packet->number2;
        FileUpload(connfd, file_name, start_b, end_b);
        close(connfd);
    }
    return NULL;
}
void FileUpload(int sockfd, string file_name, int start_byte, int total_byte){
    int nbytes;
    int connfd = sockfd;
    char line[BUFFSIZE];
    int leave = total_byte;
    sprintf(line, "data/%s", file_name.c_str());
    FILE *fp = fopen(line, "rb");
    fseek(fp, start_byte, SEEK_SET);
    while(leave > 0){
        if(leave >= BUFFSIZE)
            nbytes = fread(line, sizeof(char), BUFFSIZE, fp);
        else
            nbytes = fread(line, sizeof(char), leave, fp);
        nbytes = write(sockfd, line, nbytes);
        leave -= nbytes;
    }
    cout << "File Upload Finish!!!" << endl;
    fclose(fp);
}
bool CheckLst(vector<string> a, vector<string> b){
    int check1, check2;
    check1 = 1;
    check2 = 1;
    for(int i=0; i<a.size(); i++){
        int check = 0;
        for(int j=0; j<b.size(); j++){
            if(a[i] == b[j])
                check = 1;
        }
        check1 *= check;
    }
    for(int i=0; i<b.size(); i++){
        int check = 0;
        for(int j=0; j<a.size(); j++){
            if(b[i] == a[j])
                check = 1;
        }
        check2 *= check;
    }
    if(check1 == 1 && check2 == 1){
        return false;
    }
    else
        return true;
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

    packet->number = service_port;
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
    cout << "$ ShowAllFile" << endl;
    cout << "$ ShowMyInfo" << endl;
    cout << "$ DeleteMyAccount" << endl;
    cout << "$ ModifyMyAccount" << endl;
    cout << "$ UpdateLst" << endl;
    cout << "$ ShowMyInvite" << endl;
    cout << "$ InviteWho" << endl;
    cout << "$ AcceptMyInvite" << endl;
    cout << "$ RemoveMyInvite" << endl;
    cout << "$ SearchWho" << endl;
    cout << "$ Logout" << endl;
    cout << "$ DownloadFile" << endl;
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
    else if(input == "UpdateLst") return string("UPDATELST");
    else if(input == "ShowAllFile") return string("SHOWFILELST");
    else if(input == "DownloadFile") return string("REQUESTFILE");
    else    return string("UNKNOWN");
}
void ProcessCommand(int sockfd, string sendData){
    int command = CommandChoose(sendData);
    vector<string> ls;
    char line[MAXLINE];
    int sz;
    FILE *fp;
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
        case DELETEMYACCOUNT:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            write(sockfd, (char*)packet, sizeof(Packet));
            delete packet;
            sleep(2);
            exit(0);
            break;
        case UPDATELST:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            ls = ListDir();
            //packet = NewPacket(0);
            packet->number = ls.size();
            write(sockfd, (char*)packet, sizeof(Packet));
            delete packet;
            for(int i=0; i<ls.size(); i++){
                packet = NewPacket(0);
                PacketPush(packet, ls[i]);
                sprintf(line, "data/%s", ls[i].c_str());
                fp = fopen(line, "rb");
                fseek(fp, 0L, SEEK_END);
                sz = ftell(fp);
                fclose(fp);
                packet->number = sz;
                write(sockfd, (char*)packet, sizeof(Packet));
                delete packet;
            }
            break;
        case SHOWFILELST:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            write(sockfd, (char*)packet, sizeof(Packet));
            delete packet;
            break;
        case REQUESTFILE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cin >> sendData;
            PacketPush(packet, sendData);
            write(sockfd, (char*)packet, sizeof(Packet));
            delete packet;
            break;
    }
}

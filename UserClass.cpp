#include "DataBaseHeader.h"

void UserClass::Init(){
    first_user =  NULL;
    last_user = NULL;
    user_index = 0;
    ifstream file;
    file.open("USER_LST.txt", fstream::in);
    if(file.is_open()){
        int count = 1;
        string line[6];
        while(getline(file, line[count-1])){
            //printf("%d %s\n", count-1, line[count-1].c_str());
            if (count<6)
                count++;
            else{
                this->CreateUser(line[0], line[1], line[2], line[3], line[4], line[5]);
                count = 1;
            }
        }
        file.close();
    }
    this->FriendInit();
    this->ArticalInit();
}
void UserClass::SaveUserList(){
    char reg_buf[100];
    char last_buf[100];
    ofstream file;
    file.open("USER_LST.txt", fstream::out);
    if(file.is_open()){
        for(User *tmp=first_user; tmp!=NULL; tmp=tmp->next){
            sprintf(reg_buf, "%ld", (long int)tmp->reg_time_t);
            sprintf(last_buf, "%ld", (long int)tmp->last_time_t);
            file << (tmp->account + "\n");
            file << (tmp->password + "\n");
            file << (tmp->nickname + "\n");
            file << (tmp->birthday + "\n");
            file << (string(reg_buf) + "\n");
            file << (string(last_buf) + "\n");
        }
        file.close();
    }
}
User* UserClass::CreateUser(string account, string password, string nickname, string birthday){
    time_t ticks;
    char buf[MAXLINE];
    ticks = time(NULL);
    snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
    //User *tmp = (User*)malloc(sizeof(User));
    User *tmp = new User;
    tmp->account = account;
    tmp->password = password;
    tmp->nickname = nickname;
    tmp->birthday = birthday;
    tmp->index = user_index;
    tmp->islogin = 0;
    tmp->artical_index = 0;
    user_index++;
    tmp->port = 0;
    tmp->IP = "";
    tmp->last_time_t = ticks;
    tmp->reg_time_t = ticks;
    tmp->reg_time = buf;
    tmp->last_time = buf;
    tmp->previous = NULL;
    tmp->next = NULL;
    tmp->first_artical = NULL;
    tmp->last_artical = NULL;
    if(last_user == NULL){
        first_user = tmp;
        last_user = tmp;
    }
    else{
        tmp->previous = last_user;
        last_user->next = tmp;
        last_user = tmp;
    }
    return last_user;
}
bool UserClass::DeleteUser(string IP, int port){
    User* who = this->FindUserFromIPAndPort(IP, port);
    if (who == NULL)
        return false;
    else{
        if(who->previous != NULL && who->next != NULL){
            who->previous->next = who->next;
            who->next->previous = who->previous;
        }
        else if(who->previous == NULL && who->next != NULL){
            this->first_user = who->next;
            who->next->previous = NULL;
        }
        else if(who->previous != NULL && who->next == NULL){
            this->last_user = who->previous;
            who->previous->next = NULL;
        }
        delete who;
        return true;
    }
}
User* UserClass::FindUser(string whatkind, string info){
    for(User* tmp = first_user; tmp != NULL; tmp = tmp->next){
        if(whatkind == "account" && tmp->account == info)
            return tmp;
        else if(whatkind == "nickname" && tmp->nickname == info)
            return tmp;
    }
    return NULL;
}
User* UserClass::FindUserFromIPAndPort(string IP, int port){
    User* tmp;
    for(tmp = this->first_user; tmp != NULL; tmp = tmp->next){
        if(tmp->islogin == 1){
            if(tmp->IP == IP && tmp->port == port)
                return tmp;
        }
    }
    return NULL;
}
User* UserClass::UserLogin(string account, string password, string IP, int port){
    time_t ticks;
    char buf[MAXLINE];
    ticks = time(NULL);
    //snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
    sprintf(buf, "%ld", (long int)ticks);
    int shutdown = 0;
    User *answer, *tmp;
    for(tmp = first_user, answer = NULL; tmp != NULL && shutdown == 0; tmp = tmp->next){
        //cout << tmp <<endl;
        if(tmp->account == account && tmp->password == password){
            answer = tmp;
            shutdown = 1;
        }
    }
    if(answer != NULL){
        answer->islogin = 1;
        answer->IP = IP;
        answer->port = port;
        answer->last_time_t = ticks;
        return answer;
    }
    return NULL;
}
User* UserClass::UserLogout(string IP, int port){
    time_t ticks = time(NULL);
    User *who = this->FindUserFromIPAndPort(IP, port);
    if(who != NULL){
        who->islogin = 0;
        who->last_time_t = ticks;
        return who;
    }
    else
        return NULL;
}
User* UserClass::UserModify(string IP, int port, string password, string nickname, string birthday){
    User *who = this->FindUserFromIPAndPort(IP, port);
    if(who != NULL){
        who->password = password;
        who->nickname = nickname;
        who->birthday = birthday;
        return who;
    }
    else
        return NULL;
}
void UserClass::ArticalInit(){
    DIR *dp;
    dirent *d;

    system("mkdir -p Artical/");
    dp = opendir("Artical/");
    while((d = readdir(dp)) != NULL){
        if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, "..")){
            continue;
        }
        ifstream file;
        file.open((string("Artical/") + string(d->d_name)).c_str());
        if(file.is_open()){
            string line;
            string account;
            string artical;
            string message;
            string who;
            string artical_time;
            string message_time;
            string IP;
            int port;
            User* user_tmp;
            Artical* artical_tmp;
            Message* message_tmp;
            while(getline(file, line)){
                if(line == "New Artical:"){
                    getline(file, line);
                    artical = line;
                }
                else if(line == "Author:"){
                    getline(file, line);
                    account = line;
                    user_tmp = this->FindUser(string("account"), account);
                    //artical_tmp = this->CreateArtical(user_tmp, artical);
                }
                else if(line == "Artical_Time:"){
                    getline(file, line);
                    artical_time = line;
                    //artical_tmp = this->CreateArtical(user_tmp, artical, artical_time);
                }
                else if(line == "IP:"){
                    getline(file, line);
                    IP = line;
                }
                else if(line == "port:"){
                    getline(file, line);
                    port = atoi(line.c_str());
                    artical_tmp = this->CreateArtical(user_tmp, artical, IP, port, artical_time);
                }
                else if(line == "A message:"){
                    getline(file, line);
                    message = line;
                }
                else if(line == "Who:"){
                    getline(file, line);
                    who = line;
                    //message_tmp = this->CreateMessage(artical_tmp, who, message);
                }
                else if(line == "Message_Time:"){
                    getline(file, line);
                    message_time = line;
                    message_tmp = this->CreateMessage(artical_tmp, who, message, message_time);
                }
                else if(line == "Like:"){
                    getline(file, line);
                    artical_tmp->like_lst.push_back(line);
                }
            }
        }
    }
    closedir(dp);
}
User* UserClass::CreateUser(string account, string password, string nickname, string birthday, string reg_time, string log_time){
    time_t reg_ticks;
    time_t last_ticks;
    char reg_buf[100];
    char last_buf[100];
    sscanf(reg_time.c_str(), "%ld", (long int*)&reg_ticks);
    sscanf(log_time.c_str(), "%ld", (long int*)&last_ticks);
    sprintf(reg_buf, "%s", ctime(&reg_ticks));
    sprintf(last_buf, "%s", ctime(&last_ticks));
    User* user = this->CreateUser(account, password, nickname, birthday);
    user->reg_time_t = reg_ticks;
    user->last_time_t = last_ticks;
    user->reg_time = reg_buf;
    user->last_time = last_buf;
    return user;
}
Message* UserClass::CreateMessage(Artical *artical, string who, string message, string time){
    Message* tmp = this->CreateMessage(artical, who, message);
    sscanf(time.c_str(), "%ld", (long int*)&tmp->time_tar);
    return tmp;
}
Artical* UserClass::CreateArtical(User* user, string artical, string IP, int port, string time){
    time_t ticks;
    sscanf(time.c_str(), "%ld", (long int*)&ticks);
    Artical* tmp = this->CreateArtical(user, artical);
    tmp->IP = IP;
    tmp->port = port;
    tmp->time_tar = ticks;
    return tmp;
}
Artical* UserClass::CreateArtical(User *user, string artical){
    time_t ticks;
    char buf[MAXLINE];
    ticks = time(NULL);
    snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
    Artical* tmp = new Artical;
    tmp->author = user->account;
    tmp->artical = artical;
    tmp->index = user->artical_index;
    tmp->message_count = 0;
    tmp->time = buf;
    tmp->time_tar = ticks;
    user->artical_index++;
    tmp->previous = NULL;
    tmp->next = NULL;
    tmp->first_message = NULL;
    tmp->last_message = NULL;
    if(user->first_artical == NULL && user->last_artical == NULL){
        user->first_artical = tmp;
        user->last_artical = tmp;
    }
    else{
        tmp->previous = user->last_artical;
        user->last_artical->next = tmp;
        user->last_artical = tmp;
    }
    return tmp;
}
Message* UserClass::CreateMessage(Artical* artical, string who, string message){
    time_t ticks;
    char buf[MAXLINE];
    ticks = time(NULL);
    snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
    Message* tmp = new Message;
    tmp->who = who;
    tmp->message = message;
    tmp->index = artical->message_count;
    tmp->time = buf;
    tmp->time_tar = ticks;
    artical->message_count++;
    tmp->previous = NULL;
    tmp->next = NULL;
    if(artical->first_message == NULL && artical->last_message == NULL){
        artical->first_message = tmp;
        artical->last_message = tmp;
    }
    else{
        tmp->previous = artical->last_message;
        artical->last_message->next = tmp;
        artical->last_message = tmp;
    }
    return tmp;
}
void UserClass::ShowArtical(User* user){
    Artical* artical_tmp;
    Message* message_tmp;
    for(Artical* artical_tmp=user->first_artical; artical_tmp!=NULL; artical_tmp=artical_tmp->next){
        cout << "Artical ID: " << artical_tmp->index << "\tAuthor: " << artical_tmp->author << endl;
        cout << "\tBegin\n"; 
        cout << "\t\t"  << artical_tmp->artical << endl;
        cout << "\tEnd\n";
        for(Message* message_tmp = artical_tmp->first_message; message_tmp != NULL; message_tmp = message_tmp->next){
            cout << "\tMessage ID: " << message_tmp->index << "\twho: " << message_tmp->who << endl;
            cout << "\t\tBegin" << endl;
            cout << "\t\t\t" << message_tmp->message << endl;
            cout << "\t\tEnd" << endl;
        }
    }
}
Artical* UserClass::FindArticalFromIndex(string account, int artical_index){
    User *user_tmp = this->FindUser(string("account"), account);
    Artical* tmp;
    if(user_tmp != NULL){
        for(tmp = user_tmp->first_artical; tmp != NULL; tmp = tmp->next){
            if(tmp->index == artical_index)
                return tmp;
        }
    }
    return NULL;
}
Message* UserClass::FindMessageFromIndex(string account, int artical_index, int message_index){
    Artical* art_tmp = this->FindArticalFromIndex(account, artical_index);
    Message* tmp;
    if(art_tmp != NULL){
        for(tmp = art_tmp->first_message; tmp != NULL; tmp = tmp->next){
            if(tmp->index == message_index)
                return tmp;
        }
    }
    return NULL;
}
void UserClass::SaveArtical(){
    time_t ticks;
    char t_buf[MAXLINE];
    system("rm Artical/*");
    int count;
    User* user_tmp;
    Artical* art_tmp;
    Message* mess_tmp;
    for(user_tmp = this->first_user, count = 0; user_tmp != NULL; user_tmp = user_tmp->next){
        for(art_tmp = user_tmp->first_artical ; art_tmp != NULL; art_tmp = art_tmp->next, count++){
            char buf[256];
            sprintf(buf, "%d", count);
            string file_name = string("Artical/artical_") + string(buf) + string(".txt");
            ofstream file;
            file.open(file_name.c_str(), fstream::out);
            file << "New Artical:\n";
            file << (art_tmp->artical + "\n");
            file << "Author:\n";
            file << (art_tmp->author + "\n");
            file << "Artical_Time:\n";
            sprintf(t_buf, "%ld", (long int) art_tmp->time_tar);
            file << (string(t_buf) + "\n");
            file << "IP:\n";
            file << (art_tmp->IP + "\n");
            sprintf(buf, "%d", art_tmp->port);
            file << "port:\n";
            file << (string(buf) + "\n");
            for(mess_tmp = art_tmp->first_message; mess_tmp != NULL; mess_tmp = mess_tmp->next){
                file << "A message:\n";
                file << (mess_tmp->message + "\n");
                file << "Who:\n";
                file << (mess_tmp->who + "\n");
                file << "Message_Time:\n";
                sprintf(t_buf, "%ld", (long int) mess_tmp->time_tar);
                file << (string(t_buf) + "\n");
            }
            for(int i=0; i<art_tmp->like_lst.size(); i++){
                file << "Like:\n";
                file << (art_tmp->like_lst[i] + "\n");
            }
            file.close();
        }
    }
}
Artical* UserClass::ModifyArtical(string IP, int port, int artical_index, string artical){
    User* user = this->FindUserFromIPAndPort(IP, port);
    if(user != NULL){
        Artical* tmp = this->FindArticalFromIndex(user->account, artical_index);
        if(tmp != NULL){
            tmp->artical = artical;
            return tmp;
        }
    }
    return NULL;
}
Message* UserClass::ModifyMessage(string IP, int port, string artical_author, int artical_index, int message_index, string message){
    User *user = this->FindUserFromIPAndPort(IP, port);
    User *author = this->FindUser(string("account"), artical_author);
    if(user != NULL && author != NULL){
        Message* tmp = this->FindMessageFromIndex(author->account, artical_index, message_index);
        if(tmp != NULL){
            tmp->message = message;
            return tmp;
        }
    }
    return NULL;
}
Artical* UserClass::NewUserArtical(string IP, int port, string artical){
    time_t ticks;
    char buf[MAXLINE];
    ticks = time(NULL);
    sprintf(buf, "%ld", (long int)ticks);
    User* user = this->FindUserFromIPAndPort(IP, port);
    if(user != NULL){
        cout << "Find user\n";
        return this->CreateArtical(user, artical, IP, port, string(buf));
    }
    return NULL;
}
Message* UserClass::NewUserMessage(string IP, int port, string author_account, int artical_index, string message){
    User* user = this->FindUserFromIPAndPort(IP, port);
    User* user_author = this->FindUser(string("account"), author_account);
    Artical* artical = this->FindArticalFromIndex(user_author->account, artical_index);
    if(artical != NULL && user != NULL && user_author != NULL){
        return this->CreateMessage(artical, user->account, message);
    }
    return NULL;
}
string UserClass::ShowUserArtical(string IP, int port){
    string output = "";
    char line[MAXLINE];
    char buf[100];
    vector<Artical*> user_art;
    User* user = this->FindUserFromIPAndPort(IP, port);
    Artical* artical_tmp;
    Message* message_tmp;
    for(Artical* artical_tmp=user->first_artical; artical_tmp!=NULL; artical_tmp=artical_tmp->next)
        user_art.push_back(artical_tmp);
    for(int i=0; i<user_art.size(); i++){
        for(int j=i+1; j<user_art.size(); j++){
            if(user_art[i]->time_tar > user_art[j]->time_tar){
                Artical* t = user_art[i];
                user_art[i] = user_art[j];
                user_art[j] = t;
            }
        }
    }
    for(int i=0; i<user_art.size(); i++){
        Artical* artical_tmp = user_art[i];
        User* usr = this->FindUser(string("account"), artical_tmp->author);
        sprintf(buf, "%s", ctime(&artical_tmp->time_tar));
        sprintf(line, "Artical ID: %d\tAuthor: %s(%s)\tTime: %s\tLikes: %d\tIP: %s\tport: %d\n", artical_tmp->index, artical_tmp->author.c_str(), usr->nickname.c_str(), buf, (int)artical_tmp->like_lst.size(), artical_tmp->IP.c_str(), artical_tmp->port);
        output += string(line);
        sprintf(line, "\tBegin\n\t\t%s\n\tEnd\n", artical_tmp->artical.c_str());
        output += string(line);
        for(Message* message_tmp = artical_tmp->first_message; message_tmp != NULL; message_tmp = message_tmp->next){
            sprintf(buf, "%s", ctime(&message_tmp->time_tar));
            sprintf(line, "\tMessage ID: %d\twho: %s\tTime: %s\n", message_tmp->index, message_tmp->who.c_str(), buf);
            output += string(line);
            sprintf(line, "\t\tBegin\n\t\t\t%s\n\t\tEnd\n", message_tmp->message.c_str());
            output += string(line);
        }
    }
    return output;
}
Artical* UserClass::DeleteArticalMessage(Artical* artical){
    Message* tmp;
    Message* tmp2;
    for(tmp = artical->first_message; tmp != NULL; tmp = tmp2){
        tmp2 = tmp->next;
        delete tmp;
    }
    return artical;
}
bool UserClass::DeleteUserArtical(string IP, int port, int artical_index){
    User* user = this->FindUserFromIPAndPort(IP, port);
    if(user != NULL){
        Artical* tmp = this->FindArticalFromIndex(user->account, artical_index);
        if(tmp != NULL){
            Message* message;
            if(tmp->previous != NULL && tmp->next != NULL){
                tmp->previous->next = tmp->next;
                tmp->next->previous = tmp->previous;
            }
            else if(tmp->previous == NULL && tmp->next != NULL){
                user->first_artical = tmp->next;
                tmp->next->previous = NULL;
            }
            else if(tmp->previous != NULL && tmp->next == NULL){
                user->last_artical = tmp->previous;
                tmp->previous->next = NULL;
            }
            else{
                user->first_artical = NULL;
                user->last_artical = NULL;
            }
            this->DeleteArticalMessage(tmp);
            delete tmp;
            return true;
        }
    }
    return false;
}
bool UserClass::DeleteUserMessage(string IP, int port, string author_account, int artical_index, int message_index){
    User* user = this->FindUserFromIPAndPort(IP, port);
    Artical* artical = this->FindArticalFromIndex(author_account, artical_index);
    Message* message = this->FindMessageFromIndex(author_account, artical_index, message_index);
    if(user != NULL && message != NULL && artical != NULL && user->account == message->who){
        if(message->previous != NULL && message->next != NULL){
            message->previous->next = message->next;
            message->next->previous = message->previous;
        }
        else if(message->previous == NULL && message->next != NULL){
            artical->first_message = message->next;
            message->next->previous = NULL;
        }
        else if(message->previous != NULL && message->next == NULL){
            artical->last_message = message->previous;
            message->previous->next = NULL;
        }
        else{
            artical->first_message = NULL;
            artical->last_message = NULL;
        }
        delete message;
        return true;
    }
    return false;
}
bool UserClass::GiveLike(string IP, int port, string account, int artical_index){
    User* user = this->FindUserFromIPAndPort(IP, port);
    Artical* art = this->FindArticalFromIndex(account, artical_index);
    //cout << account << endl;
    //cout << artical_index << endl;
    if(user != NULL && art != NULL){
        int check = 0;
        for(int i=0; i<art->like_lst.size(); i++){
            if(art->like_lst[i] == user->account){
                check = 1;
                break;
            }
        }
        if(check == 0){
            art->like_lst.push_back(user->account);
            return true;
        }
        else
            return false;
    }
    return false;
}
bool UserClass::UnLike(string IP, int port, string account, int artical_index){
    User* user = this->FindUserFromIPAndPort(IP, port);
    Artical* art = this->FindArticalFromIndex(account, artical_index);
    if(art != NULL && user != NULL){
        vector<string>::iterator i;
        for(i=art->like_lst.begin(); i!=art->like_lst.end(); i++){
            if(*i == user->account){
                art->like_lst.erase(i);
                return true;
            }
        }
    }
    return false;
}




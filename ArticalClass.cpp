#include "DataBaseHeader.h"

void ArticalClass::Init(){
    DIR *dp;
    dirent *d;

    system("mkdir -p Artical/");
    artical_index = 0;
    first_artical = NULL;
    last_artical = NULL;

    dp = opendir("Artical/");
    while((d = readdir(dp)) != NULL){
        if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, "..")){
            continue;
        }
        ifstream file;
        file.open((string("Artical/") + string(d->d_name)).c_str());
        if(file.is_open()){
            int art_index;
            string line;
            string account;
            string artical;
            string message;
            string who;
            while(getline(file, line)){
                if(line == "New Artical:"){
                    getline(file, line);
                    artical = line;
                }
                else if(line == "Author:"){
                    getline(file, line);
                    account = line;
                    art_index = CreateArtical(account, artical);
                }   
                else if(line == "A message:"){
                    getline(file, line);
                    message = line;
                }
                else if(line == "Who:"){
                    getline(file, line);
                    who = line;
                    CreateMessage(art_index, who, message);
                }
            }
        }
    }
    closedir(dp);
}
int ArticalClass::CreateArtical(string account, string artical){
    Artical *tmp = new Artical; 
    tmp->index = artical_index;
    tmp->message_count = 0;
    tmp->author = account;
    tmp->artical = artical;
    tmp->first_message = NULL;
    tmp->last_message = NULL;
    tmp->previous = NULL;
    tmp->next = NULL;
    artical_index++;
    if(last_artical == NULL){
        first_artical = tmp;
        last_artical = tmp;
    }
    else{
        tmp->previous = last_artical;
        last_artical->next = tmp;
        last_artical = tmp;
    }
    return tmp->index;
}
Artical* ArticalClass::FindArticalFromIndex(int index){
    for(Artical* tmp=first_artical; tmp!=NULL; tmp=tmp->next){
        if(tmp->index == index)
            return tmp;
    }
    return NULL;
}
Message* ArticalClass::FindMessageFromIndex(int art_index, int mess_index){
    Artical* artical = FindArticalFromIndex(art_index);
    if (artical == NULL)
        return NULL;
    else{
        for(Message* tmp = artical->first_message; tmp != NULL; tmp = tmp->next)
            if(tmp->index == mess_index)
                return tmp;
    }
    return NULL;
}
Message* ArticalClass::CreateMessage(int parent_artical_index, string account, string message){
    Artical* parent_artical = FindArticalFromIndex(parent_artical_index);
    if(parent_artical == NULL)
        return NULL;
    Message* tmp = new Message;
    tmp->who = account;
    tmp->message = message;
    tmp->index = parent_artical->message_count;
    tmp->previous = NULL;
    tmp->next = NULL;
    parent_artical->message_count++;
    if(parent_artical->last_message == NULL){
        parent_artical->first_message = tmp;
        parent_artical->last_message = tmp;
    }
    else{
        tmp->previous = parent_artical->last_message;
        parent_artical->last_message->next = tmp;
        parent_artical->last_message = tmp;
    }
    return tmp;
}
Message* ArticalClass::ModifyMessage(int art_index, int mess_index, string message){
    Message* tmp = this->FindMessageFromIndex(art_index, mess_index);
    tmp->message = message;
    return tmp;
}
bool ArticalClass::DeleteMessage(int art_index, int mess_index){
    Artical* art_tmp = FindArticalFromIndex(art_index);
    Message* tmp = FindMessageFromIndex(art_index, mess_index);
    if(tmp->previous != NULL && tmp->next != NULL){
        tmp->previous->next = tmp->next;
        tmp->next->previous = tmp->previous;
        delete tmp;
        return true;
    }
    else if(tmp->previous != NULL && tmp->next == NULL){
        art_tmp->last_message = tmp->previous;
        tmp->previous->next = NULL;
        delete tmp;
        return true;
    }
    else if(tmp->previous == NULL && tmp->next != NULL){
        art_tmp->first_message = tmp->next;
        tmp->next->previous = NULL;
        delete tmp;
        return true;
    }
    else{
        art_tmp->first_message = NULL;
        art_tmp->last_message = NULL;
        delete tmp;
        return true;
    }
    return false;
}
bool ArticalClass::DeleteArtical(int art_index){
    Artical* tmp = FindArticalFromIndex(art_index);
    Message* buf_tmp;
    for(Message* buf=tmp->first_message; buf!=NULL; buf=buf_tmp){
        buf_tmp = buf->next;
        delete buf;
    }
    if(tmp->previous != NULL && tmp->next != NULL){
        tmp->previous->next = tmp->next;
        tmp->next->previous = tmp->previous;
    }
    else if(tmp->previous == NULL && tmp->next != NULL){
        this->first_artical = tmp->next;
        tmp->next->previous = NULL;
    }
    else if(tmp->previous != NULL && tmp->next == NULL){
        this->last_artical = tmp->previous;
        tmp->previous->next = NULL;
    }
    else{
        this->first_artical = NULL;
        this->last_artical = NULL;
    }
    delete tmp;
    return true;
}
void ArticalClass::ShowArtical(){
    for(Artical* artical_tmp=first_artical; artical_tmp!=NULL; artical_tmp=artical_tmp->next){
        cout << "Artical ID: " << artical_tmp->index;
        cout << "\tAuthor: " << artical_tmp->author << endl;
        cout << "\tBegin:\n" << "\t\t"  << artical_tmp->artical << endl;
        cout <<  "\tEnd:\n";
        for(Message* message_tmp = artical_tmp->first_message; message_tmp != NULL; message_tmp = message_tmp->next){
            cout << "\tMessage ID: " << message_tmp->index << "   author: " << message_tmp->who << endl;
            cout << "\t\t" <<message_tmp->message << endl;
        }
    }
}
void ArticalClass::SaveArtical(){
    int count;
    Artical* art_tmp;
    Message* mess_tmp;
    for(art_tmp = first_artical, count = 0; art_tmp != NULL; art_tmp = art_tmp->next, count++){
        char buf[256];
        sprintf(buf, "%d", count);
        string file_name = string("Artical/artical_") + string(buf) + string(".txt");
        ofstream file;
        file.open(file_name.c_str(), fstream::out);
        file << "New Artical:\n";
        file << (art_tmp->artical + "\n");
        file << "Author:\n";
        file << (art_tmp->author + "\n");
        for(mess_tmp = art_tmp->first_message; mess_tmp != NULL; mess_tmp = mess_tmp->next){
             file << "A message:\n";
             file << (mess_tmp->message + "\n");
             file << "Who:\n";
             file << (mess_tmp->who + "\n");
        }
        file.close();
    }

}











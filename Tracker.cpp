#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <pthread.h>
#define ERROR -1
#define MAX_CLIENTS 3
#define BUFF_SIZE 512
using namespace std;

struct user
{
    string user_id;
    string password;
};

struct group
{
    string group_id;
    string owner_id;
};

struct membership
{
    string group_id;
    string user_id;
};

struct request
{
    string group_id;
    string user_id;
};

struct sharedfile
{
    string group_id;
    string file_name;
    string file_path;
    string sha;
    string user_id;
    string ip;
    string port;
    bool active;
};


vector<user> users;
vector<group> groups;
vector<membership> memberships;
vector<request> requests;
vector<sharedfile> sharedfiles;

string create_user(string data)
{
    char *dup = strdup(data.c_str());
    char *tok = strtok(dup, "::::");
    struct user user;
    user.user_id = strtok(NULL, "::::");
    user.password = strtok(NULL, "::::");
    users.push_back(user);
    cout <<"user created: \n"<< users[0].user_id << endl
         << users[0].password<<endl;
    return "User Created";
}

string login(string data)
{
    char *dup = strdup(data.c_str());
    char *tok = strtok(dup, "::::");
    struct user user;
    user.user_id = strtok(NULL, "::::");
    user.password = strtok(NULL, "::::");
    for (int i = 0; i < users.size(); i++)
    {
        if ((users[i].user_id == user.user_id) && (users[i].password == user.password))
        {
            for(int j=0; j<sharedfiles.size(); j++)
            {
                if (sharedfiles[j].user_id == user.user_id)
                    sharedfiles[j].active = true;
            }
            return "Valid User";
        }
    }
    return "Invalid User";
}

string create_group(string data)
{
    char *dup = strdup(data.c_str());
    char *tok = strtok(dup,"::::");
    struct group group;
    group.group_id = strtok(NULL,"::::");
    group.owner_id = strtok(NULL,"::::");
    for (int i=0;i<groups.size();i++)
    {
        if ((groups[i].group_id)==(group.group_id)) {
            return "Group already exists";
        }
    }
    groups.push_back(group);
    return "Group created";
}

string accept_request(string data)
{   
    char* data_copy=strdup(data.c_str());
    char *dup = strdup(data.c_str());
    char *tok = strtok(dup, "::::");
    string ownerid;
    struct request request1;
    request1.group_id = strtok(NULL, "::::");
    request1.user_id = strtok(NULL, "::::");
    vector<request>::iterator it;
    for(auto it=requests.begin();it!=requests.end();++it){
        if(((*it).group_id == request1.group_id)&&((*it).user_id == request1.user_id)){
              requests.erase(it);
              break;
        }
    }
    for(int i=0;i<groups.size();i++){
        if((groups[i].group_id)==(request1.group_id)){
            ownerid = groups[i].owner_id;
            break;
        }
    }
    if(ownerid.empty()){
        return "Request cannot be accepted";
    }
    struct membership membership;
    membership.group_id=request1.group_id;
    membership.user_id=request1.user_id;
    memberships.push_back(membership);
    return "Request for group joining is accepted";
}



string join_group(string data)
{
    char *dup = strdup(data.c_str());
    char *tok = strtok(dup, "::::");
    struct request request;
    request.group_id = strtok(NULL, "::::");
    request.user_id = strtok(NULL, "::::");
    requests.push_back(request);
   //will join_group first go to pending request or should call accept request?
    return "xyz";
}

string leave_group(string data)
{
    char *dup = strdup(data.c_str());
    char *tok = strtok(dup, "::::");
    string group_id = strtok(NULL, "::::");
    string user_id = strtok(NULL, "::::");
    vector<membership>::iterator it;
    for(auto it=memberships.begin();it!=memberships.end();++it){
        if((*it).group_id==group_id && (*it).user_id==user_id){
              memberships.erase(it);
              break;
        }
    }
    return "Left group successfully";
}

string list_requests(string data)
{   //in this after last group_id, user_id pair "::::" will be present
    char *dup = strdup(data.c_str());
    char *tok = strtok(dup, "::::");
    string group_id = strtok(NULL, "::::");
    string ans="";
    for (int i=0;i<requests.size();i++){
        if(requests[i].group_id==group_id){
            ans=ans+requests[i].group_id+"|"+requests[i].user_id+"::::";
        }
    }
    return ans;
}

string list_groups(string data)
{   
    string ans="";
    for(int i=0;i<groups.size();i++){
        ans=ans+groups[i].group_id+"::::";
    }
    return ans;

}

string list_files(string data)
{   
    char *dup = strdup(data.c_str());
    char *tok = strtok(dup, "::::");
    string group_id = strtok(NULL, "::::");
    string ans="";
    for(int i=0;i<sharedfiles.size();i++){
        if(sharedfiles[i].group_id==group_id){
            ans=ans+sharedfiles[i].file_name+"::::";
        }
    }
    return ans;
}

string upload_file(string data,string ip){
    char* dup=strdup(data.c_str());
    char* tok=strtok(dup,"::::");
    cout<<"tok inside upload:: "<<tok<<endl;
    string file_path=strtok(NULL,"::::");
    cout<<"file_path:: "<<file_path<<endl;
    string file_path_copy = file_path;
    string file_name = file_path_copy.substr(file_path_copy.find_last_of("/\\") + 1);
    cout<<"file_name:: "<<file_name<<endl;
    string group_id=strtok(NULL,"::::");
    cout<<"group_id:: "<<group_id<<endl;
    string sha="something";//not yet calculated
    string port_no = strtok(NULL,"::::");
    cout<<"port_no:: "<<port_no<<endl;
    string user_id = strtok(NULL,"::::");
    cout<<"user_id:: "<<user_id<<endl;
    string ip_port=ip;
    cout<<"ip:: "<<ip<<endl;
    bool active=1;
    struct sharedfile shared_file;
    shared_file.ip=ip_port;
    shared_file.active=active;
    shared_file.file_path=file_path;
    shared_file.file_name=file_name;
    shared_file.group_id=group_id;
    shared_file.port=port_no;
    shared_file.sha=sha;
    shared_file.user_id=user_id;
    cout<<"before pushing file"<<endl;
    sharedfiles.push_back(shared_file);
    cout<<"after pushing file"<<endl;
    return "File Uploaded Successfully";
}

string download_file(string data){
    char* dup=strdup(data.c_str());
    char* tok=strtok(dup,"::::");
    char* group_id = strtok(NULL,"::::");
    char* file_name = strtok(NULL,"::::");
    char* destination_path = strtok(NULL,"::::");
    string ans="";
    for(int i=0;i<sharedfiles.size();i++){
        if(sharedfiles[i].group_id==group_id && sharedfiles[i].file_name==file_name){
            ans=ans+sharedfiles[i].port+"|"+sharedfiles[i].file_path+"|"+sharedfiles[i].user_id+"::::";
        }
    }
    return ans;
}


string logout(string data){
    char* dup=strdup(data.c_str());
    char* tok=strtok(dup,"::::");
    string user_id = strtok(NULL,"::::");
    for(int i=0;i<sharedfiles.size();i++){
        if(sharedfiles[i].user_id==user_id){
            sharedfiles[i].active=0;
        }
    }
    return "Logged out successfully";
}

string stop_share(string data){
    char* dup=strdup(data.c_str());
    char* tok = strtok(dup,"::::");
    string group_id = strtok(NULL,"::::");
    string file_name = strtok(NULL,"::::");
    vector<sharedfile>::iterator it;
    for(auto it=sharedfiles.begin();it!=sharedfiles.end();it++){
        if((*it).group_id==group_id && (*it).file_name==file_name){
            sharedfiles.erase(it);
            break;
        }
    }
    return "File is no longer shared";
}


string check_action(string action, string data,string ip)
{
    int act = stoi(action);
    switch (act)
    {
    case 1:
        return create_user(data);
        break;
    case 2:
        return login(data);
        break;
    case 3:
        return create_group(data);
        break;
    case 4:
        return join_group(data);
        break;
    case 5:
        return leave_group(data);
        break;
    case 6:
        return list_requests(data);
        break;
    case 7:
        return accept_request(data);
        break;
    case 8:
        return list_groups(data);
        break;
    case 9:
        return list_files(data);
        break;
    case 10:
        return upload_file(data,ip);
        break;
    case 11:
        return download_file(data);
        break;
    case 12:
        return logout(data);
        break;
    // case 13:
    //     return Show_downloads(data);
    //     break;
    case 14:
        return stop_share(data);
        break;
    }
}

void run_server(char **argv)
{
    struct sockaddr_in server, client;
    socklen_t sin_size;
    int status, n;
    sin_size = sizeof(sockaddr);
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0)
    {
        cout << "Error in connection" << endl;
        exit(0);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    int ret = bind(serv_sock, (struct sockaddr *)&server, sizeof(server));
    cout << "listening at IP:Port - " << argv[1] << ":" << argv[2] << endl;
    if (ret < 0)
    {
        cout << "Error in Binding" << endl;
        exit(1);
    }
    int lis = listen(serv_sock, MAX_CLIENTS);
    if (lis < 0)
    {
        cout << "Error in Listening" << endl;
        exit(1);
    }
    while (1)
    {
        cout<<"Waiting for clients to send a msg..\n";
        status = accept(serv_sock, (struct sockaddr *)&client, &sin_size);
        cout<<"after status\n";
        if (status == ERROR)
        {
            cout<<"inside if\n";
            perror("accept");
            exit(1);
        }
        cout<<"outside if\n";
        cout << "New client from port number " << ntohs(client.sin_port) << " and IP is " << inet_ntoa(client.sin_addr);
        int data_len = 1;
        string data = "";
        char temp[BUFF_SIZE];
        data_len = recv(status, temp, BUFF_SIZE, 0);
        data = data + temp;
        // while (data_len)
        // {
        //     cout<<"data_len: "<<data_len<<endl;
        //     data_len = recv(status, temp, BUFF_SIZE, 0);
        //     data = data + temp;
        //     cout<<"data: "<<data<<endl;
        // }
        cout<<"outside while data: "<<data<<endl;
        data = data + "\0";
        char *dup = strdup(data.c_str());
        string identifier = strtok(dup, "::::");
        cout<<"identifier: "<<identifier<<endl;
        string result = check_action(identifier, data,inet_ntoa(client.sin_addr));
        cout<<"result: "<<result<<endl;
        memset(temp, '\0', BUFF_SIZE);
        char *dup1 = strdup(result.c_str());
        int sent = send(status, dup1, result.size(), 0);
        cout<<"sent response to client - length: "<<sent<<endl;
    }
}

int main(int argc, char **argv)
{
    //call for synchronization
     run_server(argv);
   // check_action("1", "1::::richaa1995::::ajrocks");
}

#include <bits/stdc++.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define ERROR -1
#define MAX_CLIENTS 3
#define BUFF_SIZE 512
using namespace std;

char response[BUFF_SIZE];
string login_id;
int isloggedin=0;
char** ipportinfo;
char* global_port;
char* global_portcon;

void* peer_server(void* argv)
{  
    struct sockaddr_in server, client;
    socklen_t sin_size;
    int status, n;
    sin_size = sizeof(sockaddr);
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0)
    {
        cout << "Error in connection\n" << endl;
        exit(0);
    }
    char* dupcopy=ipportinfo[1];
    char* ipcon = strtok(dupcopy,":");
    global_portcon=strtok(NULL,":");
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(global_portcon));
    server.sin_addr.s_addr = inet_addr(ipcon);
    cout<<"ipcon:: "<<ipcon<<endl;
    cout<<"global_portcon:: "<<global_portcon<<endl;
    int ret = bind(serv_sock, (struct sockaddr *)&server, sizeof(server));
    cout << "coming here"<< endl;
    if (ret < 0)
    {
        cout << "Error in Binding" << endl;
        exit(1);
    }
    int lis = listen(serv_sock, MAX_CLIENTS);
    if (ret < 0)
    {
        cout << "Error in Listening" << endl;
        exit(1);
    }
    if ((status = accept(serv_sock, (struct sockaddr *)&client, &sin_size)) == ERROR)
    {
        perror("accept");
        exit(1);
    }
    cout << "New client from port number " << ntohs(client.sin_port) << " and IP is " << inet_ntoa(client.sin_addr);
    // char* file_name;
    // int stat = recv(status, file_name, sizeof(file_name), 0);
    // cout<<"stat:: "<<stat<<endl;
    // cout<<"after sending data::"<<file_name<<endl;
     int data_len = 1;
    // string data = "";
    char file_path[BUFF_SIZE];
    data_len = recv(status, file_path, BUFF_SIZE, 0);
    cout<<"data_len:: "<<data_len<<endl;
    cout<<"file_path:: "<<file_path<<endl;
    FILE *fp = fopen(file_path, "rb");
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    cout<<"size:: "<<size<<endl;
    rewind(fp);
    int sent2=send(serv_sock, &size,sizeof(int), 0);
    cout<<"sizeofsent 2:: "<<sent2<<endl;

    char Buffer[BUFF_SIZE];
    while ((n = fread(Buffer, sizeof(char), BUFF_SIZE, fp)) > 0 && size > 0)
    {
        send(serv_sock, Buffer, n, 0);
        memset(Buffer, '\0', BUFF_SIZE);
        size = size - n;
    }
    close(status);
    close(serv_sock);
    fclose(fp);
    cout<<"exiting server\n"<<endl;
    pthread_exit(0);
}

void file_client(char** argv,string ans,string portcon,string file_path){
    int len, b;
    char* copy_dup=strdup(ans.c_str());
    char* copy_tok=strtok(copy_dup,"::::");
    char* copy_group_id = strtok(NULL,"::::");
    char* copy_file_name = strtok(NULL,"::::");
    char* destination_path = strtok(NULL,"::::");
    struct sockaddr_in remote_server, client;
    socklen_t sin_size;
    int status,n;
    sin_size = sizeof(sockaddr);
    int cli_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_sock < 0)
    {
        cout << "Error in connection" << endl;
        exit(0);
    }
    char* dup_response = response;
    global_port = strtok(dup_response,"|");
    remote_server.sin_family = AF_INET;
    remote_server.sin_port = htons(atoi(global_port));
    remote_server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if ((b = connect(cli_sock, (struct sockaddr *)&remote_server, sizeof(remote_server))) == ERROR)
    {
        cout << "Error in connect\n";
        exit(0);
    }
    string filename = copy_file_name;
    char* filepath = strdup(file_path.c_str());
    cout<<"file_path:: "<<file_path<<endl;
    int sent = send(cli_sock, filepath,BUFF_SIZE, 0);
    cout<<"\nmsg sent size: "<<sent<<endl;

    FILE *fp = fopen(destination_path, "wb");
    char Buffer[BUFF_SIZE];
   int file_size;

   int recv2=recv(status, &file_size, sizeof(file_size), 0);
   cout<<"recv2:: "<<recv2<<endl;
    while ((n = recv(status, Buffer, BUFF_SIZE, 0)) > 0 && file_size > 0)
    {
        fwrite(Buffer, sizeof(char), n, fp);
        cout<<"Recieved msg:: "<<Buffer<<endl;
        memset(Buffer, '\0', BUFF_SIZE);
        file_size = file_size - n;
    }
    close(status);
    close(cli_sock);
    fclose(fp);
}


void file_download(char** argv,string ans){
    string port[4];
    char* dup = response;
    port[0]=strtok(dup,"|");
    string file_path = strtok(NULL,"|");
    cout<<"file_path_retrieved:: "<<file_path<<endl;
    // string temp;
    // temp=strtok(NULL,"::::");
    // int i=1;
    // while (!temp.empty())
    // {  
    //   cout<<"port[i]:: "<<port[i]<<endl;
    //   port[i]=strtok(NULL,"|");
    //   temp=strtok(NULL,"::::");
    // }
    file_client(argv,ans,port[0],file_path);
}

void connect_server(char** argv,string ans){
    cout<<"coming in server"<<endl;
    struct sockaddr_in remote_server;
    int len, b,n;
    int cli_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_sock < 0)
    {
        cout << "Error in connection" << endl;
        exit(0);
    }
    cout<<"connecting to server at- "<<argv[1]<<endl;
    cout<<"argv[1]:: "<<argv[1]<<endl;
    // char* p =argv[1];
    // char* ip = strtok(p,":");
    // char* port = strtok(NULL,":");
    string line;
    string port[2];
    int i=0;
    ifstream myfile (argv[2]);
    if (myfile.is_open())
    {
      while (getline(myfile,line))
     {
       //cout << line << '\n';
       port[i++]=line;
     }
     myfile.close();
     }
    char* portcon=strdup(port[0].c_str());
    remote_server.sin_family = AF_INET;
    remote_server.sin_port = htons(atoi(portcon));
    remote_server.sin_addr.s_addr = inet_addr("127.0.0.1");
    cout<<"PORT:: "<<remote_server.sin_port<<endl;
    cout<<"IP:: "<<remote_server.sin_addr.s_addr<<endl;

    if ((b = connect(cli_sock, (struct sockaddr *)&remote_server, sizeof(remote_server))) == ERROR)
    {
        cout << "Error in connect\n";
        exit(0);
    }
    char *dup = strdup(ans.c_str());
    int sent = send(cli_sock, dup,sizeof(char)*ans.size(), 0);
    cout<<"\nmsg sent size: "<<sent<<endl;
    len=recv(cli_sock,response,BUFF_SIZE,0);
    cout<<"len: "<<len<<endl;
    response[len]='\0';
    // printf("out: %s\n",out);
    if(strcmp(response,"Valid User")==0){
        char* tok=strtok(dup,"::::");
        login_id = strtok(NULL, "::::");
        isloggedin=1;
    }
     if(strcmp(response,"Logged out successfully")==0){
        login_id = "";
        isloggedin=0;
    }
    cout<<"response: "<<response<<endl;
    close(cli_sock);
}

void* client_choice(void* param){
     string s;
     string ans;
     while(1){
      char** argv = *(char***)param;
      cout<<"Enter the command:: "<<endl;
      ans="";
      getline(cin,s);
      char* dup = strdup(s.c_str());
      char *tok = strtok(dup, " ");
      if(strcmp(tok,"create_user")==0){
           ans=ans+"1"+"::::";
           string user_id = strtok(NULL, " ");
           ans=ans+user_id+"::::";
           string password = strtok(NULL, "::::");
           ans=ans+password;
           connect_server(argv,ans);
      }
      if(strcmp(tok,"login")==0){
           ans="";
           ans=ans+"2"+"::::";
           string user_id = strtok(NULL, " ");
           ans=ans+user_id+"::::";
           string password = strtok(NULL, "::::");
           ans=ans+password;
           connect_server(argv,ans);
      }
      if(strcmp(tok,"create_group")==0){
           ans="";
           ans=ans+"3"+"::::";
           string group_id = strtok(NULL, " ");
           ans=ans+group_id+"::::";
           if(isloggedin && !login_id.empty()){
             ans=ans+login_id;
             connect_server(argv,ans);
           }
           else{
               cout<<"User is not logged in, hence group cannot be created"<<endl;
           }
      }
     if(strcmp(tok,"join_group")==0){
           ans="";
           ans=ans+"4"+"::::";
           string group_id = strtok(NULL, " ");
           ans=ans+group_id;
           connect_server(argv,ans);
      }
     if(strcmp(tok,"leave_group")==0){
           ans="";
           ans=ans+"5"+"::::";
           string group_id = strtok(NULL, " ");
           ans=ans+group_id;
           connect_server(argv,ans);
      } 
      if(strcmp(tok,"list_requests")==0){
           ans="";
           ans=ans+"6"+"::::";
           string group_id = strtok(NULL, " ");
           ans=ans+group_id;
           connect_server(argv,ans);
      }
      if(strcmp(tok,"accept_request")==0){
           ans="";
           ans=ans+"7"+"::::";
           string group_id = strtok(NULL, " ");
           ans=ans+group_id+"::::";
           string user_id = strtok(NULL, "::::");
           ans=ans+user_id;
           connect_server(argv,ans);
      }
      if(strcmp(tok,"list_groups")==0){
           ans="";
           ans=ans+"8";
           connect_server(argv,ans);
      }
       if(strcmp(tok,"list_files")==0){
           ans="";
           ans=ans+"9"+"::::";
           string group_id = strtok(NULL, " ");
           ans=ans+group_id;
           connect_server(argv,ans);
      }
       if(strcmp(tok,"upload_file")==0){
           ans="";
           ans=ans+"10"+"::::";
           string file_path = strtok(NULL, " ");
           ans=ans+file_path+"::::";
           string group_id = strtok(NULL, " ");
           ans=ans+group_id+"::::";
           cout<<"global_portcon:: "<<global_portcon<<endl;
           string port = global_portcon;
           ans=ans+port+"::::";
           if(isloggedin && !login_id.empty()){
             ans=ans+login_id;
             connect_server(argv,ans);
           }
      }
       if(strcmp(tok,"download_file")==0){
           ans="";
           ans=ans+"11"+"::::";
           string group_id = strtok(NULL, " ");
           ans=ans+group_id+"::::";
           string file_name = strtok(NULL, " ");
           ans=ans+file_name+"::::";
           string destination_path = strtok(NULL, " ");
           ans=ans+destination_path;
           connect_server(argv,ans);
           file_download(argv,ans);
      }
      if(strcmp(tok,"logout")==0){
           ans="";
           ans=ans+"12"+"::::";
           if(isloggedin && !login_id.empty()){
             ans=ans+login_id;
             connect_server(argv,ans);
           }
      }
      if(strcmp(tok,"Show_downloads")==0){
           ans="";
           ans=ans+"13";
           connect_server(argv,ans);
      }
       if(strcmp(tok,"stop_share")==0){
           ans="";
           ans=ans+"14"+"::::";
           string group_id = strtok(NULL, " ");
           ans=ans+group_id+"::::";
           string file_name = strtok(NULL, "::::");
           ans=ans+file_name;
           connect_server(argv,ans);
      }
      memset(response, '\0', BUFF_SIZE);
     }
    pthread_exit(0);
}


int main(int argc, char **argv)
{   
      pthread_t tid[2];
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_attr_t attr1;
      pthread_attr_init(&attr1);
      ipportinfo = argv;
      pthread_create(&tid[0],&attr,client_choice,&argv);
      pthread_create(&tid[1],&attr1,peer_server,&argv);
      pthread_join(tid[0], NULL);
      pthread_join(tid[1], NULL);
    return 0;
}

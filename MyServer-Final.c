
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <dirent.h>
#include "MyJSON.c"
//defining
#define MAX 80
#define PORT 12345
#define SA struct sockaddr
//global variable
int server_socket, client_socket;
struct sockaddr_in server, client;
char buffer[80];
char longbuffer[100000];
int n = 0;
//Online Users struct
typedef struct {
    char username[20];
    char usertoken[35];
    char channel[20];
    int line;
} OnlineUsers;

OnlineUsers OU[100];


void Start()
{
    // Create and verify socket
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    else
        printf("Socket successfully created..\n");

    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully bound..\n");

    // Now server is ready to listen and verify
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
}

void chat()
{
    char order[20];

    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        printf("Server made its socket and is listening to accept a client :\n");
        Start();
        //accept client
        int len = sizeof(client);
        client_socket = accept(server_socket, (SA *)&client, &len);
        if (client_socket < 0)
        {
            printf("Server acceptance failed...\n");
            exit(0);
        }
        else
            printf("Server accepted the client..\n");

        //receive data from client
        recv(client_socket, buffer, sizeof(buffer), 0);

        //check the order
        sscanf(buffer, "%s", order);

        //registration
        if(strcmp(order, "register") == 0 ){
            char username[40], pass[40];
            sscanf(buffer, "%s%s%s\n", order, username, pass);
            RemoveCama(username);

            if(IsThisValid("Users", username)){
                MyJson("Successful", "Registration successful");
                SetPass(username, pass);
            }
            send(client_socket, buffer, sizeof(buffer), 0);
            closesocket(server_socket);
            printf("-------------------------------------------------------\n");
        }

        //login
        else if(strcmp(order, "login") == 0){
            char username[40], pass[40];
            sscanf(buffer, "%s%s%s", order, username, pass);
            RemoveCama(username);

            if(IsUserAndPassValid(username, pass)){
                char token[31];
                MakeToken(token);
                printf("user token is : %s\n", token);
                MyJson("AuthToken", token);
                SetUser(username, token);
                n++;
            }

            send(client_socket, buffer, sizeof(buffer), 0);
            closesocket(server_socket);
            //Start();
            printf("-------------------------------------------------------\n");
        }

        //create channel
        else if(strcmp(order, "create") == 0)
        {
            char chname[20], token[35];
            sscanf(buffer, "%s%s%s%s\n", order, order, chname, token);
            RemoveCama(chname);

            if(CheckToken(token)){
                if(IsThisValid("Channels", chname)){
                    MyJson("Successful", "channel created successfully");
                    SetUserChannel(chname, token, "created");
                }
            }
            else {
                MyJson("Error", "Your token is not valid");
            }

            send(client_socket, buffer, sizeof(buffer), 0);
            closesocket(server_socket);
            //Start();
            printf("-------------------------------------------------------\n");
        }

        //join channel
        else if(strcmp(order, "join") == 0)
        {
            char chname[20], token[35];
            sscanf(buffer, "%s%s%s%s\n", order, order, chname, token);
            RemoveCama(chname);

            if(CheckToken(token)){
                if(IsChannelExist(chname)){
                    MyJson("Successful", "you joined channel");
                    SetUserChannel(chname, token, "joined");
                }
            }
            else {
                MyJson("Error", "Your token is not valid");
            }

            send(client_socket, buffer, sizeof(buffer), 0);
            closesocket(server_socket);
            //Start();
            printf("-------------------------------------------------------\n");
        }

        //log out
        else if(strcmp(order, "logout") == 0)
        {
            char token[35];
            sscanf(buffer, "%s%s\n", order, token);

            if(CheckToken(token)){
                SetUserOffline(token);
                MyJson("Successful", "You loged out successfully");
            }
            else {
                MyJson("Error", "Your token is not valid");
            }

            send(client_socket, buffer, sizeof(buffer), 0);
            closesocket(server_socket);
            //Start();
            printf("-------------------------------------------------------\n");
        }

        //send message
        else if(strcmp(order, "send") == 0)
        {
            char message[1000], token[35];
            GetMyMessage(message, token);

            if(CheckToken(token) && CheckChannel(token)){
                SendMessageToChannel(message ,token);
                MyJson("Successful", "your message sent successfully");
            }
            else {
                MyJson("Error", "Your token is not valid");
            }

            send(client_socket, buffer, sizeof(buffer), 0);
            closesocket(server_socket);
            //Start();
            printf("-------------------------------------------------------\n");
        }

        //refresh
        else if(strcmp(order, "refresh") == 0)
        {
            char token[35];
            sscanf(buffer, "%s%s\n", order, token);
            RemoveCama(token);

            if(CheckToken(token) && CheckChannel(token)){
                printf("messages are : \n");
                ShowMessages(token);
                send(client_socket, longbuffer, sizeof(longbuffer), 0);
            }
            else {
                MyJson("Error", "Your token is not valid");
                send(client_socket, buffer, sizeof(buffer), 0);
            }

            //send(client_socket, buffer, sizeof(buffer), 0);
            closesocket(server_socket);
            //Start();
            printf("-------------------------------------------------------\n");
        }

        //channel members
        else if(strcmp(order, "channel") == 0)
        {
            char token[35];
            sscanf(buffer, "%s%s%s\n", order, order, token);

            if(CheckToken(token) && CheckChannel(token)){
                ShowMembers(token);
            }
            else {
                MyJson("Error", "Your token is not valid");
            }

            send(client_socket, buffer, sizeof(buffer), 0);
            closesocket(server_socket);
            //Start();
            printf("-------------------------------------------------------\n");
        }

        //leave channel
        else if(strcmp(order, "leave") == 0)
        {
            char token[35];
            sscanf(buffer, "%s%s\n", order, token);

            if(CheckToken(token) && CheckChannel(token)){
                LeaveChannel(token);
                MyJson("Successful", "you got out of channel");
            }
            else {
                MyJson("Error", "Your token is not valid");
            }

            send(client_socket, buffer, sizeof(buffer), 0);
            closesocket(server_socket);
            //Start();
            printf("-------------------------------------------------------\n");
        }
    }
}

void RemoveCama(char* a)
{
    for(int i=0 ; ; i++){
        if(a[i] == 44 || a[i] == 10){
            a[i] = '\0';
            break;
        }
    }
}

int IsThisValid(char* root ,char* item)
{
    FILE* f;
    char addr[40];
    memset(addr, 0, sizeof(addr));
    strcat(addr, "Resources/");
    strcat(addr, root);
    strcat(addr, "/");
    strcat(addr, item);
    strcat(addr, ".javad");
    f = fopen(addr, "r");
    if (f != NULL){
        MyJson("Error", "The item(channel or user) is already exist");
        close(f);
        return 0;
    }
    fclose(f);
    f = fopen(addr, "w");
    fclose(f);
    return 1;
}

int IsUserAndPassValid(char* username, char* pass)
{
    FILE* f;
    char addr[40] = "Resources/Users/";
    strcat(addr, username);
    strcat(addr, ".javad");
    f = fopen(addr, "r");
    if(f == NULL){
        MyJson("Error", "User does not exist");
        fclose(f);
        return 0;
    }
    else{
        char p[40];
        fgets(p, 100, f);
        if(strcmp(p , pass) != 0){
            MyJson("Error", "Your Pass is not right");
            fclose(f);
            return 0;
        }
        if(CheckIsOnline(username)){
            MyJson("Error", "You are in");
            fclose(f);
            return 0;
        }
        return 1;
    }
}

int IsChannelExist(char* channel)
{
    FILE* f;
    char addr[40];
    memset(addr, 0, sizeof(addr));
    strcat(addr, "Resources/Channels/");
    strcat(addr, channel);
    strcat(addr, ".javad");
    f = fopen(addr, "r");
    if(f == NULL){
        MyJson("Error", "Channel does not exist");
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

void SetPass(char* user, char* pass)
{
    FILE* f;
    char addr[35] = "Resources/Users/";
    strcat(addr, user);
    strcat(addr, ".javad");
    f = fopen(addr, "w");
    fprintf(f, "%s", pass);
    fclose(f);
}

void SetUser (char* u, char* t)
{
    strcpy(OU[n].username, u);
    strcpy(OU[n].usertoken, t);
    OU[n].line = 0;
}

void SetUserChannel (char* ch, char* t, char* s)
{
    int i=0;
    char user[40];
    for( ; i<n ; i++)
    {
        if(strcmp(OU[i].usertoken, t) == 0) break;
    }
    strcpy(OU[i].channel, ch);
    strcpy(user, OU[i].username);
    OU[i].line = 0;

    FILE* f;
    char addr[35] = "Resources/Channels/";
    strcat(addr, ch);
    strcat(addr, ".javad");
    f = fopen(addr, "a");
    fprintf(f, "Server : %s %s %s\n", user, s, ch);
    fclose(f);
}

void Shift(int i)
{
    for(int j=i ; j<n-1 ; j++)
    {
        OU[j] = OU[j+1];
    }
    n--;
}

void SetUserOffline(char* t)
{
    int i=0;
    for( ; i<n ; i++)
    {
        if(strcmp(OU[i].usertoken ,t) == 0)
        {
            Shift(i);
            break;
        }
    }

}

int CheckIsOnline(char* u)
{
    for(int i=0 ; i<n ; i++)
    {
        if(strcmp(OU[i].username, u) == 0)
        {
            printf("user is Online\n");
            return 1;
        }
    }
    return 0;
}

int CheckToken(char* token)
{
    for(int i=0; i<n ; i++){
        if(strcmp(token, OU[i].usertoken) == 0) return 1;
    }
    return 0;
}

int CheckChannel(char* t)
{
    char ch[20];
    int i=0;
    for(int i=0; i<n ; i++){
        if(strcmp(t, OU[i].usertoken) == 0)
        {
            strcpy(ch, OU[i].channel);
            break;
        }
    }
    FILE* f;
    char addr[35] = "Resources/Channels/";
    strcat(addr, ch);
    strcat(addr, ".javad");
    f = fopen(addr, "r");
    if(f == NULL)
    {
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

void LeaveChannel(char* t)
{
    char user[40], ch[40];
    for(int i=0 ; i<n ; i++)
    {
        if(strcmp(OU[i].usertoken, t) == 0)
        {
            strcpy(ch, OU[i].channel);
            char nochannel[15] = "NoChannel";
            strcpy(OU[i].channel, nochannel);
            strcpy(user, OU[i].username);
            break;
        }
    }

    FILE* f;
    char addr[35] = "Resources/Channels/";
    strcat(addr, ch);
    strcat(addr, ".javad");
    f = fopen(addr, "a");
    fprintf(f, "Server : %s left the channel\n", user);
    fclose(f);

}

void GetMyMessage(char* m, char* t)
{
    int j = sizeof(buffer)-1;
    for( ; ; j--)
    {
        if(buffer[j] == ',') break;
    }
    int k=0;
    for(int i=5; i<j ; i++, k++)
    {
        m[k] = buffer[i];
    }
    m[k] = '\0';

    for(int i=j+2, k=0 ; k<30 ; i++, k++)
    {
        t[k] = buffer[i];
    }
}

void SendMessageToChannel(char* m, char* t)
{
    int i=0;
    char user[20], ch[20];
    for(; i<n ; i++)
    {
        if(strcmp(OU[i].usertoken , t) == 0)
        {
            strcpy(user, OU[i].username);
            strcpy(ch, OU[i].channel);
            break;
        }
    }
    FILE* f;
    char addr[35] = "Resources/Channels/";
    strcat(addr, ch);
    strcat(addr, ".javad");
    f = fopen(addr, "a");
    fprintf(f, "%s : %s\n", user, m);
    fclose(f);
}

void MyJson(char* type, char* content)
{
    char* a;
    //make JSONs
    cJSON* ans = cJSON_CreateObject();
    cJSON* tpe = cJSON_CreateString(type);
    cJSON* con = cJSON_CreateString(content);
    //add JSONs to object
    cJSON_AddItemToObject(ans, "type", tpe);
    cJSON_AddItemToObject(ans, "content", con);
    //finish object
    EndObject(ans);
    //copy JSON to buffer
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, cJSON_Print(ans));

    printf("this is buffer\n%s\n", buffer);

    //free memory
    Delete_MyJSON(ans);
    Delete_MyJSON(tpe);
    Delete_MyJSON(con);
}

void ShowMessages(char* t)
{
    int line;
    char ch[20];
    int u=0;
    for( ; u<n ; u++)
    {
        if(strcmp(OU[u].usertoken, t) == 0)
        {
            strcpy(ch, OU[u].channel);
            line = OU[u].line;
            break;
        }
    }
    FILE* f;
    char addr[35] = "Resources/Channels/";
    strcat(addr, ch);
    strcat(addr, ".javad");
    f = fopen(addr, "r");

    int j=line;
    char TotalLine[1500];

    //get pointer where user has not read
    for(int k=1 ; k<=line ; k++)
    {
        fgets(TotalLine, 2000, f);
    }

    //make JSONs
    cJSON* ans = cJSON_CreateObject();
    cJSON* arr = cJSON_CreateArray();
    cJSON* str = cJSON_CreateString("List");

    cJSON_AddItemToObject(ans, "type", str);
    Delete_MyJSON(str);

    //read messages and make JSON
    char c;
    int went_into_if = 0;
    for( ; ( c = fgetc(f)) != EOF ; )
    {
        went_into_if = 1;
        ungetc(c, f);
        char sender[20], message[1000];
        fgets(TotalLine, 2000, f);
        int i=0;
        for( ; TotalLine[i] != ' ' ; i++)
        {
            sender[i] = TotalLine[i];
        }
        sender[i] = '\0';

        i += 3;
        int k=0;
        for( ; TotalLine[i] != '\n' ; i++, k++)
        {
            message[k] = TotalLine[i];
        }
        message[k] = '\0';
        //create JSONs
        cJSON* m = cJSON_CreateObject();
        cJSON* send = cJSON_CreateString(sender);
        cJSON* mess = cJSON_CreateString(message);
        //add
        cJSON_AddItemToObject(m, "sender", send);
        cJSON_AddItemToObject(m, "content", mess);
        EndObject(m);
        cJSON_AddItemToArray(arr, m);
        //delete JSONs
        Delete_MyJSON(m);
        Delete_MyJSON(send);
        Delete_MyJSON(mess);

        j++;

    }
    //finish array
    if(went_into_if) EndArray(arr);
    else strcat(arr->text, " ]");
    //add array to object
    cJSON_AddItemToObject(ans, "content", arr);

    //finish Object which have array
    EndObjectwithArray(ans);
    //copy Object to buffer
    strcpy(longbuffer, cJSON_Print(ans));

    printf("this is longbuffer\n%s\n", longbuffer);

    //free memory
    Delete_MyJSON(arr);
    Delete_MyJSON(ans);
    OU[u].line = j;
    fclose(f);
}

void ShowMembers(char* t)
{
    char ch[20];
    for(int i=0 ; i<n ; i++)
    {
        if(strcmp(OU[i].usertoken, t) == 0)
        {
            strcpy(ch, OU[i].channel);
            break;
        }
    }
    //create JSONs
    cJSON* ans = cJSON_CreateObject();
    cJSON* mylist = cJSON_CreateArray();
    cJSON* str = cJSON_CreateString("List");

    cJSON_AddItemToObject(ans, "type", str);
    Delete_MyJSON(str);

    for(int i=0 ; i<n ; i++)
    {
        if(strcmp(OU[i].channel, ch) == 0)
        {
            cJSON* name = cJSON_CreateString(OU[i].username);
            cJSON_AddItemToArray(mylist, name);
            Delete_MyJSON(name);
        }
    }
    //finish array
    EndArray(mylist);
    //add array to object
    cJSON_AddItemToObject(ans, "content", mylist);
    //finish Object
    EndObject(ans);
    //copy JSON to buffer
    strcpy(buffer, cJSON_Print(ans));

    printf("this is buffer\n%s\n", buffer);
    //free memory
    Delete_MyJSON(mylist);
    Delete_MyJSON(ans);
}

void MakeToken(char* token)
{
    memset(token, 0, sizeof(token));
    srand(time(NULL));
    for(int i=0 ; i<30 ; i++){
        int j = rand();
        if(j%3 == 0){
            token[i] = (j%26) + 65;
        }
        else if(j%3 == 1){
            token[i] = (j%26) + 97;
        }
        else{
            token[i] = (j%10) + 48;
        }
    }
}

// Driver function
int main()
{
    mkdir("Resources");
    mkdir("Resources\\Users");
    mkdir("Resources\\Channels");

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Function for chatting between client and server
    chat();

    // Close the socket
    closesocket(server_socket);
}


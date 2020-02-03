#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "cJSON.h"
#include "cJSON.c"

struct sockaddr_in servaddr;
int client_socket;
#define MAX 80
#define PORT 12345
#define SA struct sockaddr
#define goitv(x , y) cJSON_GetObjectItem(ans , "type")->valuestring
#define goicv(x , y) cJSON_GetObjectItem(ans , "content")->valuestring


void chat()
{
    char buffer[MAX];
    char Token[1000];
    char longbuffer[100000];
    int n;
    printf("Welcome to this ChatApplication :)\n");
    int state = 1;
    while(1){
        if(state == 1 ){
            //start of Account Menu :)
            printf("----------------------------------\nAccount Menu :\n");
            printf("1. Register\n2. Login\n3. Exit\n");
            scanf("%d", &n);
            if(n == 1){
                //register - get registration info
                char user[40], pass[40];
                printf("UserName :\n");
                scanf("%s", user);
                printf("Password :\n");
                scanf("%s", pass);

                //send info to server and rec
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "register %s, %s\n", user , pass);
                CreatSocket();
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                closesocket(client_socket);
                cJSON* ans = cJSON_Parse(buffer);
                system("cls");
                //check if the request was successful or not
                if(strcmp("Successful" , goitv(ans , "type")) == 0){
                    printf("Your registration was successful :) \n");
                }
                else{
                    printf("\aRegistration failed =_=\n%s\n", goicv(ans, "content"));
                }
                //end of registration
            }
            else if(n == 2){
                //login - get login info
                char user[40], pass[40];
                printf("UserName :\n");
                scanf("%s", user);
                printf("Password :\n");
                scanf("%s", pass);

                //send info to server and rec
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "login %s, %s\n", user , pass);
                CreatSocket();
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                closesocket(client_socket);
                cJSON* ans = cJSON_Parse(buffer);

                system("cls");
                //check if the request was successful or not
                if(strcmp("AuthToken" , goitv(ans , "type")) == 0){
                    printf("Welcome, you're in :) \n");
                    strcpy(Token , goicv(ans , "content"));
                    state = 2;
                }
                else{
                    printf("Logging in failed =_=\n\a%s\n", goicv(ans, "content"));
                }
                //end of logging in
            }
            else if(n == 3){
                //exit
                closesocket(client_socket);
                printf("Thanks for using this application :) \n");
                char g = getch();
                return 0;
            }
            //end of Account Menu
        }
        else if(state == 2){
            //start of Menu
            printf("----------------------------------\nMenu:\n");
            printf("1. Create Channel\n2. Join Channel\n3. Logout\n");
            scanf("%d", &n);
            if(n == 1){
                //create channel - get channel info
                printf("Enter your Channel name: \n");
                char CHname[20];
                scanf("%s", CHname);

                //send info to server and rec
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "create channel %s, %s\n", CHname , Token);
                CreatSocket();
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                closesocket(client_socket);
                cJSON* ans = cJSON_Parse(buffer);

                system("cls");
                //check if the request was successful or not
                if(strcmp("Successful" , goitv(ans , "type")) == 0){
                    printf("Your channel created successfully :) \n");
                    state = 3;
                }
                else{
                    printf("I'm sorry, I could not create your channel =_=\n\a%s\n", goicv(ans, "content"));
                }
                //end of creating channel

            }
            else if(n == 2){
                //join channel - get that channel info
                printf("Enter the name of channel you want to join: \n");
                char CHname[20];
                scanf("%s", CHname);

                //send info to server and rec
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "join channel %s, %s\n", CHname , Token);
                CreatSocket();
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                closesocket(client_socket);
                cJSON* ans = cJSON_Parse(buffer);

                system("cls");
                //check if the request was successful or not
                if(strcmp("Successful" , goitv(ans , "type")) == 0){
                    printf("Now you are in ( %s ) channel :) \n", CHname);
                    state = 3;
                }
                else{
                    printf("I'm sorry, I could not join you in this channel =_=\n\a%s\n", goicv(ans, "content"));
                }
                //end of joining channel
            }
            else if(n == 3){
                //logout

                //send info to server and rec
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "logout %s\n", Token);
                CreatSocket();
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                closesocket(client_socket);
                cJSON* ans = cJSON_Parse(buffer);

                system("cls");
                //check if the request was successful or not
                if(strcmp("Successful" , goitv(ans , "type")) == 0){
                    printf("Goodbye, you got out of your account :( \n");
                    state = 1;
                }
                else{
                    printf(":) you are still in, please try again\n\a");
                }
                //end of logging out
            }
            //end of Menu
        }
        else if(state == 3){
            //start of Chat Menu
            printf("----------------------------------\nChat Menu:\n");
            printf("1. Send Message\n2. Refresh\n3. Channel Members\n4. Leave Channel\n");
            scanf("%d", &n);
            if(n == 1){
                //send message - get message
                char x;
                scanf("%c", &x);
                printf("Type your message here:\n");
                char message[1000];
                int i=0;
                while ((message[i++] = getchar()) != '\n');
                message[i-1] = '\0';

                //send info to server and rec
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "send %s, %s\n", message, Token);
                CreatSocket();
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                closesocket(client_socket);
                cJSON* ans = cJSON_Parse(buffer);

                system("cls");
                //check if the request was successful or not
                if(strcmp("Successful" , goitv(ans , "type")) == 0){
                    printf("Your message successfully sent :) \n");
                }
                else{
                    printf("Sorry, there was a problem, your message didn't sent =_=\n\a");
                }
                //end of sending message
            }
            else if(n == 2){
                //refresh

                //send info to server and rec
                memset(longbuffer, 0, sizeof(longbuffer));
                sprintf(longbuffer, "refresh %s\n", Token);
                CreatSocket();
                send(client_socket, longbuffer, sizeof(longbuffer), 0);
                memset(longbuffer, 0, sizeof(longbuffer));
                recv(client_socket, longbuffer, sizeof(longbuffer), 0);
                closesocket(client_socket);
                cJSON* ans = cJSON_Parse(longbuffer);

                system("cls");
                //update messages
                printf("----------------------------------\nHere are the new Messages :\n");
                int size = cJSON_GetArraySize(cJSON_GetObjectItem(ans , "content"));
                for(int i=0 ; i<size ; i++){
                    printf("%s : ", cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(ans , "content"),i) , "sender")->valuestring);
                    printf("%s\n", cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(ans , "content"),i) , "content")->valuestring);
                }
                //end of refreshing
            }
            else if(n == 3){
                //members


                //send info to server and rec
                memset(longbuffer, 0, sizeof(longbuffer));
                sprintf(longbuffer, "channel members %s\n", Token);
                CreatSocket();
                send(client_socket, longbuffer, sizeof(longbuffer), 0);
                memset(longbuffer, 0, sizeof(longbuffer));
                recv(client_socket, longbuffer, sizeof(longbuffer), 0);
                closesocket(client_socket);
                cJSON* ans = cJSON_Parse(longbuffer);

                system("cls");
                //listing members
                printf("----------------------------------\nList of Members:\n");
                int size = cJSON_GetArraySize(cJSON_GetObjectItem(ans , "content"));

                for(int i=0 ; i<size ; i++){
                    printf("%d. %s\n", i+1, cJSON_GetArrayItem(cJSON_GetObjectItem(ans , "content"),i)->valuestring);
                }
                //end of list members
            }
            else if(n == 4){
                //leave channel

                //send info to server and rec
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "leave %s\n", Token);
                CreatSocket();
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                closesocket(client_socket);
                cJSON* ans = cJSON_Parse(buffer);

                system("cls");
                //check if request was successful or not
                if(strcmp("Successful" , goitv(ans , "type")) == 0){
                    printf("Goodbye, you got out of this channel :( \n");
                    state = 2;
                }
                else{
                    printf(":) you are still in this channel, please try again\n\a");
                }
                //end of leaving the channel
            }
            //end of Chat Menu
        }
    }//end of while
}

void CreatSocket()
{
    system("cls");
    // Create and verify socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n\a");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");


	// Connect the client socket to server socket
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed... =_=\n\a");
		printf("Please check your server and try again\n");
		printf("(click to continue)\n");
		getch();
		exit(0);
	}
	else {
		printf("Successfully connected to the server..\n");
		system("cls");
	}
}

int main()
{
	struct sockaddr_in cli;

	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n\a", err);
        return 1;
    }

	// Assign IP and port
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.1.1");
	servaddr.sin_port = htons(PORT);
    /*
    // Create and verify socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n\a");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");


	// Connect the client socket to server socket
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed... =_=\n\a");
		printf("Please check your server and try again\n");
		printf("(click to continue)\n");
		getch();
		exit(0);
	}
	else {
		printf("Successfully connected to the server..\n");
		system("cls");
	}

    closesocket(client_socket);
    */
	// Function for chat
	chat(client_socket);

	// Close the socket
	closesocket(client_socket);
}

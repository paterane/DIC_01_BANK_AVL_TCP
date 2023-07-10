//
// Created by Peter Oo on 07/01/2023
//
#ifndef __linux__
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define ISINVALIDSOCKET(s) ((s) == INVALID_SOCKET)
    #define CLOSESOCKET(s) closesocket(s)
#else
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE 1
    #endif
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
    #define ISINVALIDSOCKET(s) ((s) < 0)
    #define CLOSESOCKET(s) close(s)
    #define SOCKET int
#endif

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "server.h"

SOCKET sockListen;
SOCKET sockClient;
/**
 * calling error message and shutdown the program
 * mode 1, call error message, close socket
*/
void err_call(char *errMsg, SOCKET s){
    printf(RED"%s\n"G_RESET, errMsg);
    CLOSESOCKET(s);
    #ifdef _WIN32
        WSACleanup();
    #endif
    // exit(-1);
}

void initServer(char *ip, unsigned short port){
    struct sockaddr_in sAddress = {0};
    sAddress.sin_family = AF_INET;
    sAddress.sin_port = htons(port);
    #ifdef _WIN32
        char option = 1;
        sAddress.sin_addr.S_un.S_addr = inet_addr(ip);
        WSADATA wsd = {0};
        if(WSAStartup(MAKEWORD(2,2), &wsd)){
            printf(RED"Startup() Failed\n"RESET);
            // exit(-1);
        }
    #else
        int option = 1;
        sAddress.sin_addr.s_addr = inet_addr(ip);
    #endif
    //Creating socket
    sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ISINVALIDSOCKET(sockListen)){
        err_call("socket() Failed", 0);
    }
    //Set socket option to reuse an address
    if(setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))){
        err_call("setsockopt() Failed", sockListen);
    }
    //bind socket to an address
    if(bind(sockListen, (struct sockaddr*)&sAddress, sizeof(sAddress)) < 0){
        err_call("bind() Failed", sockListen);
    }
    //Listening on a port
    if(listen(sockListen, SOMAXCONN) < 0){
        err_call("listen() Failed", sockListen);
    }
    printf("Server Listening on address: "YELLOW"%s:%d\n"G_RESET, ip, port);
}

void waitConnection(){
    char clientIP[NI_MAXHOST] = {0};
    char clientport[NI_MAXSERV] = {0};
    struct sockaddr_storage cAddress = {0};
    int cAddress_len = sizeof(cAddress);
    //wait client connection
    sockClient = accept(sockListen, (struct sockaddr*)&cAddress, &cAddress_len);
    if(ISINVALIDSOCKET(sockClient)){
        err_call("accept() Failed", sockListen); 
    }
    //Get client's address
    if(getnameinfo((struct sockaddr*)&cAddress, cAddress_len, clientIP, sizeof(clientIP), clientport, sizeof(clientport), NI_NUMERICHOST)){
        printf(RED"getnameinfo() Failed\n"G_RESET);
    }
    printf("Client Connected: "YELLOW"%s:%s\n"G_RESET, clientIP, clientport);
}

char *handleClient(){
    //Receiving client message
    char temp[1024] = {0};
    recv(sockClient, temp, sizeof(temp), 0);
    int len = stringLen(temp);
    char *buffer = malloc((len+1) * sizeof(char));
    for(int i=0; i<len; i++)
        buffer[i] = temp[i];
    buffer[len] = '\0';
    return buffer;
}

void respond(char *msg){
    int msg_size = stringLen(msg);
    send(sockClient, msg, msg_size, 0);
}

void waitResponse(){
    char temp[1024] = {0};
    recv(sockClient, temp, sizeof(temp), 0);
}

void termServer(){
    //Terminating server program
    CLOSESOCKET(sockListen);
    #ifdef _WIN32
        WSACleanup();
    #endif
}
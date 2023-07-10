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
#include "client.h"

SOCKET handshake;
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
    exit(-1);
}
/**
 * connect to the server 
*/
void sockConnect(char *ip, unsigned short port){
    struct sockaddr_in sAddress = {0};
    #ifdef _WIN32
        WSADATA wsd = {0};
        if(WSAStartup(MAKEWORD(2,2), &wsd)){
            printf(RED"Startup() Failed\n"G_RESET);
            exit(-1);
        }
    #endif
    sAddress.sin_family = AF_INET;
    #ifdef _WIN32
    sAddress.sin_addr.S_un.S_addr = inet_addr(ip);
    #else
    sAddress.sin_addr.s_addr = inet_addr(ip);
    #endif
    sAddress.sin_port = htons(port);
    handshake = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ISINVALIDSOCKET(handshake)){
        err_call("socket() Failed", 0);
    }
    if(connect(handshake, (struct sockaddr*)&sAddress, sizeof(sAddress)) < 0){
        err_call("connect() Failed", handshake);
    }
}

void sockSend(char *msg){
    int msg_len = stringLen(msg);
    send(handshake, msg, msg_len, 0);
}

char *sockRecv(){
    char temp[1024] = {0};
    recv(handshake, temp, sizeof(temp), 0);
    int len = stringLen(temp);
    char *buffer = malloc((len+1) * sizeof(char));
    for(int i=0; i<len; i++)
        buffer[i] = temp[i];
    buffer[len] = '\0';
    return buffer;
}

void waitResponse(){
    char temp[1024] = {0};
    recv(handshake, temp, sizeof(temp), 0);
}

void sockClose(){
    CLOSESOCKET(handshake);
    #ifdef _WIN32
        WSACleanup();
    #endif
}
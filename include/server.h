//
// Created by Peter Oo on 07/01/2023
//
#ifndef _SERVER_H
#define _SERVER_H

void initServer(char *ip, unsigned short port);
void waitConnection();
char *handleClient();
void respond(char *msg);
void termServer();
void waitResponse();

#endif

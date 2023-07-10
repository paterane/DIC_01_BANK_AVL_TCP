//
// Created by Peter Oo on 07/01/2023
//
#ifndef _CLIENT_H
#define _CLIENT_H

void sockConnect(char *ip, unsigned short port);
void sockSend(char *msg);
char *sockRecv();
void sockClose();
void waitResponse();

#endif

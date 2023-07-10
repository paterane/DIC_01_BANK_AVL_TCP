//
// Created by Peter Oo on 07/01/2023
//
#ifndef _NODE_H
#define _NODE_H

/**
 * FUNCTION DECLARATIONS
*/
void insertByEmail(char *email, int idx);
void insertByPhone(char *phone, int idx);
void insertByNRC(char *nrc, int idx);
void deleteByEmail(char *email);
void deleteByPhone(char *phone);
void deleteByNRC(char *nrc);
int searchByEmail(char *email);
int searchByPhone(char *phone);
int searchByNRC(char *nrc);
void traverseTree();
int getIndexes();

#endif
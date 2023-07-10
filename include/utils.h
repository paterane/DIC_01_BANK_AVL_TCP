//
// Created by Peter Oo on 07/01/2023
//
#ifndef _UTILS_H
#define _UTILS_H

/* dd-mm-yy info structure*/
struct Date{
    int dd;
    int mm;
    int yy;
    int H;
    int M;
};

#define G_RESET "\x1b[0m"
#define RESET "\x1b[37m"
#define BOLD "\x1b[1m"
#define PALE "\x1b[2m"
#define ITALIC "\x1b[3m"
#define UNDERLINE "\x1b[4m"
#define D_UNDERLINE "\x1b[1m"
#define INVERTED "\x1b[7m"
//#define CLEAR_TEXT "\x1b[8m"
#define STRIKETHROUGH "\x1b[9m"
#define GREY "\x1b[30m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define PURPLE "\x1b[35m"
#define L_BLUE "\x1b[36m"
#define B_GROUND "\x1b[40m"

char *wordLower(char *word);
int stringLen(const char *str);
void clear_mem(void *buff, size_t size);
int stringCmp(char *str1, char *str2);
char *stringCopy(char *src);
int indexOf(char *str, char *idxStr);
char *subString(char *str, int idxA, int idxB);
int stringCount(char *str, char *idxStr);
void stringConcat(char **main, char *sub);
char * freadline(FILE *fp);
char *readLine_csv(char *str, char dl);
float toFloat(char *str);
char *ftoChar(float a);
long toInt(char *str);
char *itoChar(int a);
int isNum(char *str);
int regExpress(char *str, char *pattern);
void current_time(char *buff, size_t b_size);
struct Date *str_To_StructDate(char *strDate);
int month_number(char *abbreviated_month);
long current_time_L();
double diff_time_L(long end, long begin);
long timeStruct_to_L(struct Date *timeInfo);
char *getpassword(const char *prompt);
void digit_input(int *buffer);

#endif // INCLUDE_UTILS_H
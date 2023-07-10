//
// Created by Peter Oo on 07/01/2023
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#else
#include <conio.h>
#endif
#define true 1
#define false 0

int tkIndex = 0; //global index for string to token func
char *tkString = NULL; // global string buffer for token

/***************************************************************************************/
//////////////////////////////////Utility Functions//////////////////////////////////////
/***************************************************************************************/
/*
  make the given string lower
  and, Return char pointer
*/
char *wordLower(char *word){
    int i = 0;
    int len = stringLen(word)+1;
    char *buff = (char*)malloc(len*sizeof(char));
    while(word[i] != '\0'){
        if(word[i] >= 'A' && word[i] <= 'Z') buff[i] = word[i] + 32;
        else buff[i] = word[i];
        i++;
    }
    buff[i] = '\0';
    return buff;
}
/* Note: This func is only valid for char array, not for int array */
int stringLen(const char *str){
    int n=0;
    while(str[n++] != '\0');
    return n-1;
}
/* RETURN: 0 if not match , 1 if match*/
int stringCmp(char *str1, char *str2){
    if(stringLen(str1) == stringLen(str2)){ //if length same, pass OR terminate func and return -1
        while(*str1 != '\0'){
            if(*str1++ != *str2++)
                return 0;
        }
        return 1;  //if every character is matched, return cnt
    }
    else
        return 0;
}
/*
 * Copy the content of src to dst
 * */
char *stringCopy(char *src){
    int len = stringLen(src);
    char *buff = malloc((len+1)*sizeof(char));
    for(int i=0; i<len; i++)
        buff[i] = src[i];
    buff[len] = '\0';
    return buff;
}
/**
 * used to clear memory contents of a pointer
 * buff = pointer to the given memory block
 * size = total size of buff
*/
void clear_mem(void *buff, size_t size){
    unsigned char *ptr = (char*)buff;
    for(int j=0; j<size; j++)
        *(ptr+j) = 0x00;
}


// RETURN: -1 if indexString longer than the string or not found,and index position if found
// NOTE: this function will only work on first occurrence of the index.
int indexOf(char *str, char *idxStr){
    int cnt=0; int found=0;
    int str_len = stringLen(str);
    int idxStr_len = stringLen(idxStr);
    if(idxStr_len <= str_len){
        for(;cnt < (str_len+idxStr_len); cnt++){
            if(idxStr[found] == str[cnt]){
                found++;
                if(found == idxStr_len)
                    break;
            }
            else if(found){
                cnt--;
                found=0;
            }
        }
        if(found == idxStr_len)
            return cnt-(found-1);
        else
            return -1; // if not found
    }
    else{
        printf(RED"Index length longer than the string!\n"RESET);
        return -1;
    }
}
/* continuous characters trimming in the string
 * continuous characters started from idxA upto idxB excluding character at idxB
 * RETURN: NULL if error indexing
 * */
char *subString(char *str, int idxA, int idxB){
    int length = stringLen(str); int x = 0;
    char *result = (char*)(malloc((length+1)*sizeof(char)));
    if(idxA >= length || idxB > (length+1) || idxA >= idxB){
        printf(RED"[ERROR!] indexing invalid!!!\n"RESET);
        return NULL;
    }
    for(int i=idxA; i<idxB; i++){
        result[x++] = str[i];
    }
    result[x] = '\0';
    return result;
}
/* number of occurrence of a substring "x" or "x.."
 * RETURN:-1 if error, 0 if not found, count of the occurrence if found
 * */
int stringCount(char *str, char *idxStr){
    int cnt=0; int found=0; int n_occur = 0;
    int str_len = stringLen(str);
    int idxStr_len = stringLen(idxStr);
    if(idxStr_len > str_len){
        printf(RED"[Error!] index > string\n"RESET);
        return -1; // if error
    }
    for(;cnt < (str_len+idxStr_len); cnt++){
        if(idxStr[found] == str[cnt]){
            found++;
            if(found == idxStr_len){ //not to exceed idxStr len
                n_occur++;
                found=0;
            }
        }
        else if(found){
            cnt--;
            found=0;
        }
    }
    return n_occur;
}
/*
 * combination of main string and sub string in such a way that sub join at the end of main*/
void stringConcat(char **main, char *sub){
    int main_len = stringLen(*main);
    int sub_len = stringLen(sub);
    int total_len = main_len+sub_len;
    char *temp = (char*)(malloc((total_len+1)*sizeof(char)));
    for(int i=0; i < main_len;i++){
        temp[i] = *(*main+i); //or main[0][i],but, can't do like this *main[i], haha, WTF "C"
    }
    for(int i=0; i<sub_len; i++){
        temp[main_len+i] = sub[i];
    }
    temp[total_len] = '\0';
    *main = temp;
}
/**
 * reading line by line from the given file pointer
*/
char * freadline(FILE *fp){
    char * sline = malloc(sizeof(char));
    char ch = 0;
    int idx = 0;
    ch = fgetc(fp);
    while(ch != '\r' && ch != '\n' && ch != EOF){
        sline[idx++] = ch;
        sline = realloc(sline, (idx+1)*sizeof(char));
        ch = fgetc(fp);
    }
    sline[idx] = '\0';
    if(sline[0] == '\0')
        return NULL;
    else
        return sline;
}
/*
    Taken the next row as string, iterating through the
    given string, whenever the next delimiter is met, a token started from previous index until delimiter index
    is returned.
    If iteration is finished, and end of string, NULL is returned.
*/
char *readLine_csv(char *str, char dl){
    char qt = '"';
    int len = stringLen(str);
    char *buff = NULL;
    if(tkString == NULL){
        tkString = stringCopy(str);
    }
    if(!stringCmp(str, tkString)){
        tkString = stringCopy(str);
        tkIndex = 0;
    }
    else{
        if(tkIndex >= len){
            tkIndex = 0;
            return NULL;
        }
    }
    for(int i=tkIndex; i<=len; i++){
        if((str[i] == dl) || (str[i] == '\0')){
            if(i > tkIndex) 
                buff=subString(str,tkIndex,i);
            else 
                buff="nan";
            tkIndex = i+1;
            break;
        }
        else if(str[i] == qt){
            for(int q=i+1; q<len; q++){
                if(str[q] == qt){
                    buff = subString(str, i+1,q);
                    tkIndex = q+2;
                    break;
                }
            }
            break;
        }
    }
    return buff;
}
/*
    The equivalent float number is returned if the given string is representing digits or started with digits.
    If not, 0.0 is returned.
*/
float toFloat(char *str){
    int i = 0; float value = 0.0; int c = 0;
    int len = stringLen(str);
    float digit[len];
    int sign = (str[0]=='-')? 1:0;
    if(sign){
        i = 1;
        c = 1;
    }
    while(str[i] >= '0' && str[i] <= '9'){
        digit[i-c] = str[i] - '0';
        for(int j=0; j<len; j++){
            int times = 1;
            if((i-c-j)>0) times = 10;
            if((i-c-j)>=0) 
                digit[j] = digit[j] * times;
            else    
                break;
        }
        i++;
    }
    if(str[i] == '.'){
        int times = 10;
        while(str[i+1] >= '0' && str[i+1] <= '9'){
            digit[i-c] = str[i+1] - '0';
            digit[i-c] /= times;
            times *= 10;
            i++;
        }
    }
    for(int j=0; j<(i-c); j++){
        value += digit[j];
    }
    value = (sign)? -1*value : value;
    return value;
}
/*
    The equivalent char array is returned for given float number.
*/
char *ftoChar(float a){
    int i = 10;
    int count = 1;
    int j=0;
    char *digit = (char*)malloc(11*sizeof(char));
    int sign = (a<0.0)? 1 : 0;
    if(sign){
        digit[0] = '-';
        a *= -1;
        count = 2;
        j = 1;
    }
    int d = (int)a;
    while(1){
        if(i > d){
            i /= 10;
            break;
        }
        else{
            i *= 10;
            count++;
        }
    }
    for(; j<count; j++){
        digit[j] = '0' + d/i;
        d %= i;
        i /= 10;
    }
    digit[j++] = '.';
    float r = a-(int)a;
    int f = (int)(r*10000);
    if((f/10)%10 == 4){
        f = (f%10 >= 5)? (f/10)+1:f/10;
        f = (f%10 >= 5)? (f/10)+1:f/10;
    }
    else{
        f = ((f/10)%10 >= 5)? (f/100)+1:f/100;
    }

    int lap = 10;
    for(; j<count+3; j++){
        digit[j] = '0' + f/lap;
        f %= lap;
        lap /= 10;
    }
    digit[j] = 0;
    return digit;
}
/*
    The equivalent int number is returned if the given string is representing digits or started with digits.
    If not , 0 is returned
*/  
long toInt(char *str){
    int i = 0; long value = 0; int c = 0;
    int len = stringLen(str);
    long digit[len];
    int sign = (str[0]=='-')? 1:0;
    if(sign){
        i = 1;
        c = 1;
    }
    while(str[i] >= '0' && str[i] <= '9'){ 
        digit[i-c] = str[i] - '0';
        for(int j=0; j<len; j++){
            int times = 1;
            if(((i-c)-j)>0) times = 10;
            if(((i-c)-j) >= 0)
                digit[j] = digit[j] * times;
            else
                break;
        }
        i++;
    }
    for(int k=0; k<(i-c); k++){
        value += digit[k];
    }
    value = (sign)? (~value)+1 : value;
    return value;
}
/*
    The equivalent char array is returned for given integer number.
*/
char *itoChar(int a){
    int i = 10;
    int count = 1;
    int j=0;
    char *digit = (char*)malloc(11*sizeof(char));
    int sign = (a&0x80000000)? 1 : 0;
    if(sign){
        a = (~a)+1;
        digit[0] = '-';
        count = 2;
        j = 1;
    }
    while(1){
        if(i > a){
            i /= 10;
            break;
        }
        else{
            i *= 10;
            count++;
        }
    }
    for(; j<count; j++){
        digit[j] = '0' + a/i;
        a %= i;
        i /= 10;
    }
    digit[count] = '\0';
    return digit;
}
/*
    Return 1 if given string have numerical form, else 0
*/
int isNum(char *str){
    while(*str != '\0'){
        if(*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}
/* "word Checker whether each character in the string existed in the patterns or not"
 * patterns => [start char] - [end char]
 * example  => a-z a,b,c,....,x,y,z
 * example  => 0-9 0,1,2,....,8,9
 * you can put any sequences from ascii table
 * RETURN: 1 if each character existed in patterns else 0
 * */
int regExpress(char *str, char *pattern){
    char pat_char[100] = {'\0'}; int pat_idx = 0;
    int pat_length = stringLen(pattern);
    int str_length = stringLen(str);
    for(int index=1; index < pat_length;){
        if(pattern[index] == '-'){
            if(pattern[index-1] <= pattern[index+1]){
                for(char ch=pattern[index-1]; ch<=pattern[index+1]; ch++){
                    pat_char[pat_idx++] = ch;
                }
            }
            else
                printf(RED"[-] Found pattern index error!!![pattern excluded]\n"RESET);
        }
        else{
            printf(RED"[-] Found strange patten!!![pattern excluded]\n"RESET);
        }
        index += 3;
    }
    int patChar_length = stringLen(pat_char);
    for(int i=0; i<str_length; i++){
        int found = false;
        for(int j=0; j<patChar_length; j++){
            if(str[i] == pat_char[j]){
                found = true;
                break;
            }
        }
        if(!found)
            return 0;
    }
    return 1;
}
/*
    user input function for password,
    input: char *buffer with n size
    input characters are returned in buffer
*/
#ifdef __linux__
int getch(){
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif
char *getpassword(const char *prompt){
    char ch = 0;
    int pos = 0;
    char *buffer = (char*)malloc(sizeof(char));
    printf("%s",prompt);
    while((ch=getch()) != '\r' && ch != '\n'){
        if(ch >= '!' && ch <= '~'){
            printf("%c", '*');
            buffer[pos++] = ch;
        }
        else if((ch == 8 || ch == 127) && pos > 0){ // if ch == backspace
                printf("\b%c\b", ' ');
                pos--;
        }
        else if(ch == 3){
            exit(1);
        }
        if(pos > 0){
            buffer = (char*)realloc(buffer, (pos+1)*sizeof(char));
        }
    }
    buffer[pos] = 0;
    printf("%c", '\n');
    return buffer;
}
/*
    You are only allowed to type in numbers
*/
void digit_input(int *buffer){
    char ch = 0;
    int pos = 0;
    char temp[100];
    while((ch=getch()) != '\r' && ch != '\n'){
        if(ch >= '0' && ch <= '9'){
            printf("%c", ch);
            temp[pos++] = ch;
        }
        else if(ch == 8 && pos > 0){ // if ch == backspace
                printf("\b%c\b", ' ');
                pos--;
        }
        else if(ch == 3){
            exit(1);
        }
    }
    printf("%c", '\n');
    temp[pos] = 0;
    *buffer = toInt(temp);
}
//////////////////////////////////////////////////////////////////////////
/* ***********************Time Related Functions*********************** */
//////////////////////////////////////////////////////////////////////////

// Array of Months Abbreviation
char month_name[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
/*
    Call local time in customized format
    buff: pointer to character capable of holding formatted characters
    RETURN: char pointer

*/
void current_time(char *buff, size_t b_size){
    time_t timer;
    time(&timer);
    struct tm *timeInfo = localtime(&timer);
    strftime(buff, b_size, "[%b-%d-%Y-%I:%M%p]", timeInfo); //[MM-DD-YY-H:M]
}
/* Transform [mm-dd-yy-H:M] format to struct Date format to be arithmetically operated */
struct Date *str_To_StructDate(char *strDate){
    char *time;
    int start = indexOf(strDate, "[");
    int end = indexOf(strDate, "]");
    strDate = subString(strDate, start+1, end);
    struct Date *date = (struct Date*)malloc(sizeof(struct Date));
    int col = 0;
    char *temp = NULL;
    while((temp=readLine_csv(strDate, '-'))){
        switch(col){
            case 0: date->mm = month_number(temp); break;
            case 1: date->dd = toInt(temp); break;
            case 2: date->yy = toInt(temp); break;
            case 3: time = stringCopy(temp); break;
            default: break;
        }
        col++;
    } //12:20PM
    free(temp);
    date->H = (time[5] == 'P')? toInt(subString(time, 0, 2))+12 : toInt(subString(time, 0, 2));
    date->M = toInt(subString(time, 3, 5));
    return date;
}
/*
    Convert time structure to machine readable long int form
*/
long timeStruct_to_L(struct Date *timeInfo){
    struct tm *timeData;
    time_t rawtime;
    time(&rawtime);
    timeData = localtime(&rawtime);
    timeData->tm_year = timeInfo->yy - 1900;
    timeData->tm_mon = timeInfo->mm;
    timeData->tm_mday = timeInfo->dd;
    time_t timer = mktime(timeData);
    return timer;
}
/*
    RETURN : decimal number of a given Abbreviated Month, if not found, RETURN: -1
*/
int month_number(char *abbreviated_month){
    int found = -1;
    for(int m=0; m<12; m++){
        if(stringCmp(month_name[m], abbreviated_month)){
            found = m;
            break;
        }
    }
    return found;
}
/* RETURN: LONG INT of current time if function is called */
long current_time_L(){
    time_t timer;
    time(&timer);
    return timer;
}
/* RETURN: LONG INT of time difference */
double diff_time_L(long end, long begin){
    double diff = difftime(end, begin);
    return diff;
}
// Monitoring Moth Abbreviation
// time_t timer;
// struct tm *timeInfo = localtime(&timer);
// for(int i=0; i<12; i++){
//     timeInfo->tm_mon = i;
//     char buff[5] = {0};
//     strftime(buff, 5, "%b", timeInfo);
//     printf("%s\n", buff);
// }
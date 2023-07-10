//
// Created by Peter Oo on 07/01/2023
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "include/utils.h"
#include "include/node.h"
#include "include/server.h"

#define serverIP       "127.0.0.1"
#define serverPort     8080
#define TSIZE          100
#define TRANSFER_MONEY 0
#define CASH_IN        1
#define WITHDRAW       2
#define BURROW         3
#define REPAY          4
#define RECEIVE_MONEY  5
#define INFORMATION    6
#define CLEAN_TRAY "clear tkString"
/* User transaction */
struct Transaction{
    char *note;
};
/* User DataBase */
typedef struct{
    unsigned int id; // Serial number of user
    char *name; // user name
    char *nrc; // user national registration card
    char *email; // user email
    char *pass; // user password
    char *phone; // user phone number
    char *address; // user address
    unsigned int curr_amt; // current amount in the account
    unsigned int income;   // User's monthly income
    unsigned int loan_amt; // amount burrowed from bank
    unsigned int loan_rate; // interest
    unsigned int isAdmin: 1; // 1 for admin 0 for user only one admin occur at a time
    unsigned int isPer: 1; // Personal or business account
    unsigned int acc_status: 1; // 1 for active, 0 for suspended
    unsigned int loan_status: 1; // 1 for burrowed, 0 for loan clear
    unsigned int p_count: 2; // counter for wrong password or email per user
    long lock_time; // time of account lock
    struct Date start; // Date of account creating
    long active; // time of account login
    unsigned int transLimit; //transaction amount limit per day
    struct Transaction transfer[TSIZE];
    unsigned int tIndex:8; //transfer transaction index
    struct Transaction receive[TSIZE]; 
    unsigned int rIndex:8; //receive transaction index
    struct Transaction cashIn[TSIZE];
    unsigned int cIndex:8; //cash_in transaction index
    struct Transaction withdraw[TSIZE];
    unsigned int wIndex:8; //withdraw transaction index
    struct Transaction burrow[TSIZE];
    unsigned int bIndex:8; //burrow transaction index
    struct Transaction repay[TSIZE];
    unsigned int pIndex:8; //repay transaction index
    struct Transaction info[TSIZE];
    unsigned int iIndex:8; //information transaction index
}db;
// id, name, nrc, email, pass, phone, address, curr_amt, income, loan_amt, loan_rate, isPer, acc_lvl, acc_status, loan_status, transLimit, records
db *userdb = NULL; //DataBase declaration
/* Global variables*/
int dbIndex = 0; //database index
int uIndex = -1; //Special index to be used in searching user index
int userFound = -1; //General userIndex that can be used if the index is to be returned
int reFound = -1; //General userIndex that can be used if the index is to be returned
unsigned int choice = 0;  //General user selection

/////////////////////////////////////////////////////////////////////////////////////////
/********************************FUNCTIONS DECLARATIONS*********************************/
/////////////////////////////////////////////////////////////////////////////////////////
void isEmailExisted(char *uEmail);
void isNRCExisted(char *uNRC);
void isPhoneExisted(char *uPhone);
int isAdminExisted();
void shift_trans_left(int uIdx, int type);
void transaction_record(unsigned int amount, int process);
void info_record(char *key, char *previous, char *current);
int get_amount_from_trans(char *transact);
int isTrans_amt_limit_OK(int idx, unsigned int amount, int process);
void service_fee(int uIdx, unsigned int amount);
void transfer_money(int idxS, int idxR, unsigned int amount);
void cashIn_withdraw(int uIdx, unsigned int amount, int process);
int days_left(int uIdx);
void burrow_repay(int uIdx, unsigned int amount, int process);
void shift_user_data(int curIdx, int nextIdx);
void pop_out_user(int idx);
void loadingDataFromFile();
void saveAllData();
void interruptHandle(int sigCode);
void sendValue(long data);
/**
 * START OF MAIN PROGRAM
*/
int main(){
    signal(SIGINT, interruptHandle);
    loadingDataFromFile();
    initServer(serverIP, serverPort);
    while(1){
        waitConnection();
        char *cmdForm = handleClient();
        printf("Requested: "YELLOW"%s\n"RESET, cmdForm);
        char *cmd = readLine_csv(cmdForm, ',');
        // searching block for email, nrc, phone 
        if(stringCmp(cmd, "search")){
            char *key = readLine_csv(cmdForm, ',');
            char *value = readLine_csv(cmdForm, ',');
            // email search block
            if(stringCmp(key, "email"))
                isEmailExisted(value);
            // phone search block
            else if(stringCmp(key, "phone"))
                isPhoneExisted(value);
            // nrc search block
            else if(stringCmp(key, "nrc"))
                isNRCExisted(value);
            else
                printf(RED"Unknown command\n"RESET);
            sendValue(uIndex);
        }
// get one data from the given key
//id,name,nrc,email,pass,phone,address,curr_amt,income,loan_amt,loan_rate,isAdmin,iPer,acc_status,loan_status,p_count,lock_time,Date start,active,translimit
        else if(stringCmp(cmd, "get")){
            int rIdx = toInt(readLine_csv(cmdForm, ','));
            char *key = readLine_csv(cmdForm, ',');
            if(stringCmp(key, "id"))
                sendValue(userdb[rIdx].id);
            else if(stringCmp(key, "name"))
                respond(userdb[rIdx].name);
            else if(stringCmp(key, "nrc"))
                respond(userdb[rIdx].nrc);
            else if(stringCmp(key, "email"))
                respond(userdb[rIdx].email);
            else if(stringCmp(key, "pass"))
                respond(userdb[rIdx].pass);
            else if(stringCmp(key, "phone"))
                respond(userdb[rIdx].phone);
            else if(stringCmp(key, "address"))
                respond(userdb[rIdx].address);
            else if(stringCmp(key, "curr_amt"))
                sendValue(userdb[rIdx].curr_amt);
            else if(stringCmp(key, "income"))
                sendValue(userdb[rIdx].income);
            else if(stringCmp(key, "loan_amt"))
                sendValue(userdb[rIdx].loan_amt);
            else if(stringCmp(key, "loan_rate"))
                sendValue(userdb[rIdx].loan_rate);
            else if(stringCmp(key, "isAdmin"))
                sendValue(userdb[rIdx].isAdmin);
            else if(stringCmp(key, "isPer"))
                sendValue(userdb[rIdx].isPer);
            else if(stringCmp(key, "acc_status"))
                sendValue(userdb[rIdx].acc_status);
            else if(stringCmp(key, "loan_status"))
                sendValue(userdb[rIdx].loan_status);
            else if(stringCmp(key, "p_count"))
                sendValue(userdb[rIdx].p_count);
            else if(stringCmp(key, "lock_time"))
                sendValue(userdb[rIdx].lock_time);
            else if(stringCmp(key, "start.yy"))
                sendValue(userdb[rIdx].start.yy);
            else if(stringCmp(key, "start.mm"))
                sendValue(userdb[rIdx].start.mm);
            else if(stringCmp(key, "start.dd"))
                sendValue(userdb[rIdx].start.dd);
            else if(stringCmp(key, "active"))
                sendValue(userdb[rIdx].active);
            else if(stringCmp(key, "translimit"))
                sendValue(userdb[rIdx].transLimit);
            else if(stringCmp(key, "currentTime"))
                sendValue(current_time_L());
            else if(stringCmp(key, "currentTimeS")){
                char time_buff[30] = {'\0'};
                current_time(time_buff, 30);
                respond(time_buff);
            }
            else if(stringCmp(key, "bIndex"))
                sendValue(userdb[rIdx].bIndex);
            else if(stringCmp(key, "rIndex"))
                sendValue(userdb[rIdx].rIndex);
            else if(stringCmp(key, "cIndex"))
                sendValue(userdb[rIdx].cIndex);
            else if(stringCmp(key, "wIndex"))
                sendValue(userdb[rIdx].wIndex);
            else if(stringCmp(key, "pIndex"))
                sendValue(userdb[rIdx].pIndex);
            else if(stringCmp(key, "tIndex"))
                sendValue(userdb[rIdx].tIndex);
            else if(stringCmp(key, "iIndex"))
                sendValue(userdb[rIdx].iIndex);
            else if(stringCmp(key, "tNote")){
                int ti = toInt(readLine_csv(cmdForm, ','));
                respond(userdb[rIdx].transfer[ti].note);
            }
            else if(stringCmp(key, "rNote")){
                int ti = toInt(readLine_csv(cmdForm, ','));
                respond(userdb[rIdx].receive[ti].note);
            }
            else if(stringCmp(key, "cNote")){
                int ti = toInt(readLine_csv(cmdForm, ','));
                respond(userdb[rIdx].cashIn[ti].note);
            }
            else if(stringCmp(key, "bNote")){
                int ti = toInt(readLine_csv(cmdForm, ','));
                respond(userdb[rIdx].burrow[ti].note);
            }
            else if(stringCmp(key, "wNote")){
                int ti = toInt(readLine_csv(cmdForm, ','));
                respond(userdb[rIdx].withdraw[ti].note);
            }
            else if(stringCmp(key, "pNote")){
                int ti = toInt(readLine_csv(cmdForm, ','));
                respond(userdb[rIdx].repay[ti].note);
            }
            else if(stringCmp(key, "iNote")){
                int ti = toInt(readLine_csv(cmdForm, ','));
                respond(userdb[rIdx].info[ti].note);
            }
            else
                printf(RED"Unknown command\n"RESET);
        }
        else if(stringCmp(cmd, "update")){
            int rIdx = toInt(readLine_csv(cmdForm, ','));
            char *key = readLine_csv(cmdForm, ',');
            char *value = readLine_csv(cmdForm, ',');
            if(stringCmp(key, "id"))
                userdb[rIdx].id = toInt(value);
            else if(stringCmp(key, "name"))
                userdb[rIdx].name = value;
            else if(stringCmp(key, "nrc"))
                userdb[rIdx].nrc = value;
            else if(stringCmp(key, "email"))
                userdb[rIdx].email = value;
            else if(stringCmp(key, "pass"))
                userdb[rIdx].pass = value;
            else if(stringCmp(key, "phone"))
                userdb[rIdx].phone = value;
            else if(stringCmp(key, "address"))
                userdb[rIdx].address = value;
            else if(stringCmp(key, "curr_amt"))
                userdb[rIdx].curr_amt = toInt(value);
            else if(stringCmp(key, "income"))
                userdb[rIdx].income = toInt(value);
            else if(stringCmp(key, "loan_amt"))
                userdb[rIdx].loan_amt = toInt(value);
            else if(stringCmp(key, "loan_rate"))
                userdb[rIdx].loan_rate = toInt(value);
            else if(stringCmp(key, "isAdmin"))
                userdb[rIdx].isAdmin = toInt(value);
            else if(stringCmp(key, "isPer"))
                userdb[rIdx].isPer = toInt(value);
            else if(stringCmp(key, "acc_status"))
                userdb[rIdx].acc_status = toInt(value);
            else if(stringCmp(key, "loan_status"))
                userdb[rIdx].loan_status = toInt(value);
            else if(stringCmp(key, "p_count"))
                userdb[rIdx].p_count = toInt(value);
            else if(stringCmp(key, "lock_time"))
                userdb[rIdx].lock_time = toInt(value);
            else if(stringCmp(key, "start.yy"))
                userdb[rIdx].start.yy = toInt(value);
            else if(stringCmp(key, "start.mm"))
                userdb[rIdx].start.mm = toInt(value);
            else if(stringCmp(key, "start.dd"))
                userdb[rIdx].start.dd = toInt(value);
            else if(stringCmp(key, "active"))
                userdb[rIdx].active = toInt(value);
            else if(stringCmp(key, "translimit"))
                userdb[rIdx].transLimit = toInt(value);
            else
                printf(RED"Unknown command\n"RESET);
            respond("PD");
        }
//id,name,nrc,email,pass,phone,address,curr_amt,income,loan_amt,loan_rate,isAdmin,iPer,acc_status,loan_status,p_count,lock_time,Date start,active,translimit
        else if(stringCmp(cmd, "insert")){
            char *key = readLine_csv(cmdForm, ',');
            char *value = readLine_csv(cmdForm, ',');
            if(stringCmp(key, "name"))
                userdb[dbIndex].name = value;
            else if(stringCmp(key, "nrc"))
                userdb[dbIndex].nrc = value;
            else if(stringCmp(key, "email"))
                userdb[dbIndex].email = value;
            else if(stringCmp(key, "pass"))
                userdb[dbIndex].pass = value;
            else if(stringCmp(key, "phone"))
                userdb[dbIndex].phone = value;
            else if(stringCmp(key, "address"))
                userdb[dbIndex].address = value;
            else if(stringCmp(key, "income"))
                userdb[dbIndex].income = toInt(value);
            else if(stringCmp(key, "isPer"))
                userdb[dbIndex].isPer = toInt(value);
            else
                printf(RED"Unknown command\n"RESET);
            respond("PD");
        }
        else if(stringCmp(cmd, "transfer")){
            userFound = toInt(readLine_csv(cmdForm, ','));
            reFound = toInt(readLine_csv(cmdForm, ','));
            unsigned int amount = toInt(readLine_csv(cmdForm, ',')); 
            transfer_money(userFound, reFound, amount);
        }
        else if(stringCmp(cmd, "cashin")){
            userFound = toInt(readLine_csv(cmdForm, ','));
            unsigned int amount = toInt(readLine_csv(cmdForm, ','));
            cashIn_withdraw(userFound, amount, CASH_IN);
        }
        else if(stringCmp(cmd, "withdraw")){
            userFound = toInt(readLine_csv(cmdForm, ','));
            unsigned int amount = toInt(readLine_csv(cmdForm, ','));
            cashIn_withdraw(userFound, amount, WITHDRAW);
        }
        else if(stringCmp(cmd, "burrow")){
            userFound = toInt(readLine_csv(cmdForm, ','));
            unsigned int amount = toInt(readLine_csv(cmdForm, ','));
            burrow_repay(userFound, amount, BURROW);
        }
        else if(stringCmp(cmd, "repay")){
            userFound = toInt(readLine_csv(cmdForm, ','));
            unsigned int amount = toInt(readLine_csv(cmdForm, ','));
            burrow_repay(userFound, amount, REPAY);
        }   
        else if(stringCmp(cmd, "editInfo")){
            userFound = toInt(readLine_csv(cmdForm, ','));
            char *key = readLine_csv(cmdForm, ',');
            char *previous = readLine_csv(cmdForm, ',');
            char *current = readLine_csv(cmdForm, ',');
            info_record(key, previous, current);
        }
        //cur_amt,loan_amt,loan_rate,isAdmin,acc_status,p_count,start.yy,start.mm,start.dd,active,lock_time,translimit,id
        else if(stringCmp(cmd, "register")){
            char time_buff[30];
            current_time(time_buff, 30);
            struct Date *current_time = str_To_StructDate(time_buff);
            userdb[dbIndex].curr_amt = 1000;                                      // default money amount in account for initial state
            userdb[dbIndex].loan_amt = 0;                                         // default loan clear
            userdb[dbIndex].loan_rate = 0;                                        // default loan rate
            if(!isAdminExisted()) userdb[dbIndex].isAdmin = 1;                    // make you admin
            else userdb[dbIndex].isAdmin = 0;                                     // make you user. Ask admin if want to be captain
            userdb[dbIndex].acc_status = 1;                                       // default account active
            userdb[dbIndex].loan_status = 0;                                      // default loan status clear
            userdb[dbIndex].p_count = 0;                                          // wrong password count
            userdb[dbIndex].start.yy = current_time->yy;                          // Account created year
            userdb[dbIndex].start.mm = current_time->mm;                          // Account created month
            userdb[dbIndex].start.dd = current_time->dd;                          // Account created day
            userdb[dbIndex].active = current_time_L();                            // current login time                   
            userdb[dbIndex].lock_time = 0;                                        // password lock time
            if(userdb[dbIndex].isPer == 1)  userdb[dbIndex].transLimit = 1000000; // 1,000,000 MMK per day
            else    userdb[dbIndex].transLimit = 10000000;                        // 10,000,000 MMK per day
            userdb[dbIndex].id = dbIndex + 1;                                     // last index number is next id number
            // Inserting index to AVL tree with email, nrc, phone
            insertByEmail(userdb[dbIndex].email, dbIndex);                        // insert index by email
            insertByNRC(userdb[dbIndex].nrc, dbIndex);                            // insert index by nrc
            insertByPhone(userdb[dbIndex].phone, dbIndex);                        // insert index by phone
            dbIndex++;                                                            // Updating user count after successful registeration
            userdb = realloc(userdb, ((dbIndex+1) * sizeof(db)));                 // Extending a user space for new users
            respond("PD");
        }
        else if(stringCmp(cmd, "dbIndex"))
            sendValue(dbIndex);
        else if(stringCmp(cmd, "delete")){
            int idx = toInt(readLine_csv(cmdForm, ','));
            pop_out_user(idx);
            respond("PD");
        }
        else if(stringCmp(cmd, "daysLeft")){
            int idx = toInt(readLine_csv(cmdForm, ','));
            sendValue(days_left(idx));
        }
        else
            printf(RED"Unknown Command\n"RESET);
        readLine_csv(CLEAN_TRAY, ',');
    }
    return 0;
}

/***************************************************************************************/
//////////////////////////////////Bank Functions/////////////////////////////////////////
/***************************************************************************************/
/*
    if not Existed, global var 'uIndex' = -1, or if existed, uIndex holds index of that existing one.
*/
void isEmailExisted(char *uEmail){
    uIndex = -1;
    uIndex = searchByEmail(uEmail);     // searching in AVL tree
}
/*
    if not Existed, global var 'uIndex' = -1, or if existed, uIndex holds index of that existing one.
*/
void isNRCExisted(char *uNRC){
    uIndex = -1;
    uIndex = searchByNRC(uNRC);         // searching in AVL tree
}
/*
    if not Existed, global var 'uIndex' = -1, or if exited, uIndex holds index of that existing one.
*/
void isPhoneExisted(char *uPhone){
    uIndex = -1;
    uIndex = searchByPhone(uPhone);     // searching in AVL tree
}
/*
    function check whether there is an admin or not if exited, return 1 else 0
*/
int isAdminExisted(){
    if(dbIndex >= 1) return 1;
    else return 0;
}
/**
 * this function shall shift the oldest messages to left to allocate new ones if buffer overflow
*/
void shift_trans_left(int uIdx, int type){
    if(type == TRANSFER_MONEY){
        for(int i=0; i<TSIZE-1; i++)
            userdb[uIdx].transfer[i].note = stringCopy(userdb[uIdx].transfer[i+1].note);
    }
    else if(type == CASH_IN){
        for(int i=0; i<TSIZE-1; i++)
            userdb[uIdx].cashIn[i].note = stringCopy(userdb[uIdx].cashIn[i+1].note);
    }
    else if(type == WITHDRAW){
        for(int i=0; i<TSIZE-1; i++)
            userdb[uIdx].withdraw[i].note = stringCopy(userdb[uIdx].withdraw[i+1].note);
    }
    else if(type == BURROW){
        for(int i=0; i<TSIZE-1; i++)
            userdb[uIdx].burrow[i].note = stringCopy(userdb[uIdx].burrow[i+1].note);
    }
    else if(type == REPAY){
        for(int i=0; i<TSIZE-1; i++)
            userdb[uIdx].repay[i].note = stringCopy(userdb[uIdx].repay[i+1].note);
    }
    else if(type == RECEIVE_MONEY){
        for(int i=0; i<TSIZE-1; i++)
            userdb[uIdx].receive[i].note = stringCopy(userdb[uIdx].receive[i+1].note);
    }
    else if(type == INFORMATION){
        for(int i=0; i<TSIZE-1; i++)
            userdb[uIdx].info[i].note = stringCopy(userdb[uIdx].info[i+1].note);
    }
}
/*
    xxx USD your process string[MM-DD-YY H:M]
*/
void transaction_record(unsigned int amount, int process){
    char time_buff[30] = {'\0'};
    current_time(time_buff, 30);
    char *newTrans = malloc(20 * sizeof(char));
    sprintf(newTrans, "%u", amount);
    if(process == TRANSFER_MONEY){
        char *sender = newTrans;
        char *recipient = newTrans;
        stringConcat(&sender, "MMK transferred to ");
        stringConcat(&sender, userdb[reFound].name);
        stringConcat(&sender, time_buff);
        if(userdb[userFound].tIndex >= TSIZE){
            shift_trans_left(userFound, TRANSFER_MONEY);
            userdb[userFound].transfer[userdb[userFound].tIndex-1].note = stringCopy(sender);
        }
        else{
            userdb[userFound].transfer[userdb[userFound].tIndex].note = stringCopy(sender);
            userdb[userFound].tIndex++;
        }
        stringConcat(&recipient, "MMK recieved from ");
        stringConcat(&recipient, userdb[userFound].name);
        stringConcat(&recipient, time_buff);
        if(userdb[reFound].rIndex >= TSIZE){
            shift_trans_left(reFound, RECEIVE_MONEY);
            userdb[reFound].receive[userdb[reFound].rIndex-1].note = stringCopy(recipient);
        }
        else{
            userdb[reFound].receive[userdb[reFound].rIndex].note = stringCopy(recipient);
            userdb[reFound].rIndex++;
        }
    }
    else if(process == CASH_IN){
        stringConcat(&newTrans, "MMK saved in the account");
        stringConcat(&newTrans, time_buff);             
        if(userdb[userFound].cIndex >= TSIZE){
            shift_trans_left(userFound, process);
            userdb[userFound].cashIn[userdb[userFound].cIndex-1].note = stringCopy(newTrans);
        }
        else{
            userdb[userFound].cashIn[userdb[userFound].cIndex].note = stringCopy(newTrans);
            userdb[userFound].cIndex++;
        }    
    }
    else if(process == WITHDRAW){
        stringConcat(&newTrans, "MMK withdrawn from the account");
        stringConcat(&newTrans, time_buff);
        if(userdb[userFound].wIndex >= TSIZE){
            shift_trans_left(userFound, process);
            userdb[userFound].withdraw[userdb[userFound].wIndex-1].note = stringCopy(newTrans);
        }
        else{
            userdb[userFound].withdraw[userdb[userFound].wIndex].note = stringCopy(newTrans);
            userdb[userFound].wIndex++;
        }
    }
    else if(process == BURROW){
        stringConcat(&newTrans, "MMK burrowed from the bank");
        stringConcat(&newTrans, time_buff);
        if(userdb[userFound].bIndex >= TSIZE){
            shift_trans_left(userFound, process);
            userdb[userFound].burrow[userdb[userFound].bIndex-1].note = stringCopy(newTrans);
        }
        else{
            userdb[userFound].burrow[userdb[userFound].bIndex].note = stringCopy(newTrans);
            userdb[userFound].bIndex++;
        }
    }
    else if(process == REPAY){
        stringConcat(&newTrans, "MMK repaid to the bank");
        stringConcat(&newTrans, time_buff);
        if(userdb[userFound].pIndex >= TSIZE){
            shift_trans_left(userFound, process);
            userdb[userFound].repay[userdb[userFound].pIndex-1].note = stringCopy(newTrans);
        }
        else{
            userdb[userFound].repay[userdb[userFound].pIndex].note = stringCopy(newTrans);
            userdb[userFound].pIndex++;
        }
    }
}
/**
 * Transaction record for user's information
*/
void info_record(char *key, char *previous, char *current){
    char time_buff[30] = {'\0'};
    char trans_buff[2048] = {0};
    current_time(time_buff, 30);
    if(stringCmp(key, "name"))
        sprintf(trans_buff, "Name changed from %s to %s%s", previous, current, time_buff);
    else if(stringCmp(key, "pass"))
        sprintf(trans_buff, "Password changed%s", time_buff);
    else if(stringCmp(key, "address"))
        sprintf(trans_buff, "Address changed from %s to %s%s", previous, current, time_buff);
    if(userdb[userFound].iIndex >= TSIZE){
        shift_trans_left(userFound, INFORMATION);
        userdb[userFound].info[userdb[userFound].iIndex-1].note = stringCopy(trans_buff);
    }
    else{
        userdb[userFound].info[userdb[userFound].iIndex].note = stringCopy(trans_buff);
        userdb[userFound].iIndex++;
    }
    respond(userdb[userFound].info[userdb[userFound].iIndex-1].note);
}
/* Take amount from transaction record and RETRUN as INT */
int get_amount_from_trans(char *transact){
    char *temp = subString(transact, 0, indexOf(transact, "MMK"));
    int amount = toInt(temp);
    return amount;
}
/*
    Check total transaction amount per day,and RETURN: 1 if it is less than or equal to max transaction per day, else 0
*/
int isTrans_amt_limit_OK(int idx, unsigned int amount, int process){
    char time_buff[30] = {0};
    current_time(time_buff, 30); // format [Mar-15-2023-06:52PM]
    struct Date *curr_date = str_To_StructDate(time_buff);
    struct Date *hist_date = NULL;
    if(process == TRANSFER_MONEY){
        for(int i=userdb[idx].tIndex-1; i>=0; i--){
            hist_date = str_To_StructDate(userdb[idx].transfer[i].note);
            if(curr_date->dd == hist_date->dd && curr_date->mm == hist_date->mm && curr_date->yy == hist_date->yy)
                amount += get_amount_from_trans(userdb[idx].transfer[i].note);
            else
                break;
        }
    }
    else if(process == RECEIVE_MONEY){
        for(int i=userdb[idx].rIndex-1; i>=0; i--){
            hist_date = str_To_StructDate(userdb[idx].receive[i].note);
            if(curr_date->dd == hist_date->dd && curr_date->mm == hist_date->mm && curr_date->yy == hist_date->yy)  
                amount += get_amount_from_trans(userdb[idx].receive[i].note);
            else
                break;
        }
    }
    else if(process == WITHDRAW){
        for(int i=userdb[idx].wIndex-1; i>=0; i--){
            hist_date = str_To_StructDate(userdb[idx].withdraw[i].note);
            if(curr_date->dd == hist_date->dd && curr_date->mm == hist_date->mm && curr_date->yy == hist_date->yy)  
                amount += get_amount_from_trans(userdb[idx].withdraw[i].note);
            else
                break;
        }
    }
    if(amount <= userdb[idx].transLimit) return 1;
    else return 0;
}

void service_fee(int uIdx, unsigned int amount){
    userdb[uIdx].curr_amt -= amount*0.01;
}

void transfer_money(int idxS, int idxR, unsigned int amount){
    if(isTrans_amt_limit_OK(idxS, amount, TRANSFER_MONEY)){
        if(isTrans_amt_limit_OK(idxR, amount, RECEIVE_MONEY)){
            userdb[idxS].curr_amt -= amount;
            userdb[idxR].curr_amt += amount;
            service_fee(idxS, amount);
            transaction_record(amount, TRANSFER_MONEY);
            respond(userdb[idxS].transfer[userdb[idxS].tIndex-1].note);
        }
        else{
            respond("R_NG");
        }
    }
    else{
        respond("T_NG");
    }
}
void cashIn_withdraw(int uIdx, unsigned int amount, int process){
    if(process==CASH_IN){
        userdb[uIdx].curr_amt += amount;
        transaction_record(amount, CASH_IN);
        respond(userdb[uIdx].cashIn[userdb[uIdx].cIndex-1].note);
    }
    else if(process==WITHDRAW){
        if(isTrans_amt_limit_OK(uIdx, amount, WITHDRAW)){
            userdb[uIdx].curr_amt -= amount;
            service_fee(uIdx, amount);
            transaction_record(amount, WITHDRAW);
            respond(userdb[uIdx].withdraw[userdb[uIdx].wIndex-1].note);
        }
        else
            respond("NG");
    }
}
/*RETURN: days left for loan repayment, if exceed, negative numbers are returned.*/
int days_left(int uIdx){
    long curr_time = current_time_L();
    struct Date *time_info = str_To_StructDate(userdb[uIdx].burrow[userdb[uIdx].bIndex-1].note);
    long due_time = timeStruct_to_L(time_info) + 2592000; // 30 days (30*24*60*60)
    double time_left = diff_time_L(due_time, curr_time);
    return time_left / 86400.0;
}
void burrow_repay(int uIdx, unsigned int amount, int process){
    if(process == BURROW){
        int process_fee = 5000;
        userdb[uIdx].loan_amt = amount;
        userdb[uIdx].loan_rate = amount * 0.3;
        userdb[uIdx].curr_amt += userdb[uIdx].loan_amt - process_fee;
        userdb[uIdx].loan_amt += userdb[uIdx].loan_rate;
        userdb[uIdx].loan_status = 1;            // mark loan status as active
        transaction_record(amount, BURROW);
        respond(userdb[uIdx].burrow[userdb[uIdx].bIndex-1].note);
    }
    else if(process == REPAY){
        if(amount < userdb[uIdx].loan_amt){
            userdb[uIdx].loan_amt -= amount;
            userdb[uIdx].curr_amt -= amount;
            transaction_record(amount, REPAY);
            respond(userdb[uIdx].repay[userdb[uIdx].pIndex-1].note);
        }
        else{
            userdb[uIdx].curr_amt -= amount;
            userdb[uIdx].curr_amt +=  amount - userdb[uIdx].loan_amt;
            transaction_record(userdb[uIdx].loan_amt, REPAY);
            respond(userdb[uIdx].repay[userdb[uIdx].pIndex-1].note);
            userdb[uIdx].loan_status = 0;
            userdb[uIdx].loan_amt = 0;
            userdb[uIdx].loan_rate = 0;
        }
    }
}
/*
    copying user's data to current idx from next idx
*/
void shift_user_data(int curIdx, int nextIdx){
    userdb[curIdx].name = stringCopy(userdb[nextIdx].name);
    userdb[curIdx].nrc = stringCopy(userdb[nextIdx].nrc);
    userdb[curIdx].email = stringCopy(userdb[nextIdx].email);
    userdb[curIdx].pass = stringCopy(userdb[nextIdx].pass);
    userdb[curIdx].phone = stringCopy(userdb[nextIdx].phone);
    userdb[curIdx].address = stringCopy(userdb[nextIdx].address);
    userdb[curIdx].curr_amt = userdb[nextIdx].curr_amt;
    userdb[curIdx].income = userdb[nextIdx].income;
    userdb[curIdx].loan_amt = userdb[nextIdx].loan_amt;
    userdb[curIdx].loan_rate = userdb[nextIdx].loan_rate;
    userdb[curIdx].isAdmin = userdb[nextIdx].isAdmin;
    userdb[curIdx].isPer = userdb[nextIdx].isPer;
    userdb[curIdx].acc_status = userdb[nextIdx].acc_status;
    userdb[curIdx].loan_status = userdb[nextIdx].loan_status;
    userdb[curIdx].p_count = userdb[nextIdx].p_count;
    userdb[curIdx].start.yy = userdb[nextIdx].start.yy;
    userdb[curIdx].start.mm = userdb[nextIdx].start.mm;
    userdb[curIdx].start.dd = userdb[nextIdx].start.dd;
    userdb[curIdx].active = userdb[nextIdx].active;
    userdb[curIdx].lock_time = userdb[nextIdx].lock_time;
    userdb[curIdx].transLimit = userdb[nextIdx].transLimit;
    userdb[curIdx].tIndex = userdb[nextIdx].tIndex;
    for(int i=0; i<userdb[curIdx].tIndex; i++){
        userdb[curIdx].transfer[i].note = stringCopy(userdb[nextIdx].transfer[i].note);
    }
    userdb[curIdx].rIndex = userdb[nextIdx].rIndex;
    for(int i=0; i<userdb[curIdx].rIndex; i++){
        userdb[curIdx].receive[i].note = stringCopy(userdb[nextIdx].receive[i].note);
    }
    userdb[curIdx].cIndex = userdb[nextIdx].cIndex;
    for(int i=0; i<userdb[curIdx].cIndex; i++){
        userdb[curIdx].cashIn[i].note = stringCopy(userdb[nextIdx].cashIn[i].note);
    }
    userdb[curIdx].wIndex = userdb[nextIdx].wIndex;
    for(int i=0; i<userdb[curIdx].wIndex; i++){
       userdb[curIdx].withdraw[i].note = stringCopy(userdb[nextIdx].withdraw[i].note);
    }
    userdb[curIdx].bIndex = userdb[nextIdx].bIndex;
    for(int i=0; i<userdb[curIdx].bIndex; i++){
        userdb[curIdx].burrow[i].note = stringCopy(userdb[nextIdx].burrow[i].note);
    }
    userdb[curIdx].pIndex = userdb[nextIdx].pIndex;
    for(int i=0; i<userdb[curIdx].pIndex; i++){
       userdb[curIdx].repay[i].note = stringCopy(userdb[nextIdx].repay[i].note);
    }
    userdb[curIdx].iIndex = userdb[nextIdx].iIndex;
    for(int i=0; i<userdb[curIdx].iIndex; i++){
       userdb[curIdx].info[i].note = stringCopy(userdb[nextIdx].info[i].note);
    }
}
/*
    pop a user and re-arrange the db's content in order
*/
void pop_out_user(int idx){
    dbIndex--;
    deleteByEmail(userdb[idx].email);
    deleteByNRC(userdb[idx].nrc);
    deleteByPhone(userdb[idx].phone);
    for(int i=idx; i<dbIndex; i++){
        deleteByEmail(userdb[i+1].email);
        deleteByNRC(userdb[i+1].nrc);
        deleteByPhone(userdb[i+1].phone);
        shift_user_data(i, i+1);
        insertByEmail(userdb[i].email, i);
        insertByNRC(userdb[i].nrc, i);
        insertByPhone(userdb[i].phone, i);
    }
    userdb = realloc(userdb, (dbIndex+1) * sizeof(db));
}
// id,name,nrc,email,pass,phone,address,curr_amt,income,loan_amt,loan_rate,isPer,acc_status,loan_status,translimit,records,transactions
void loadingDataFromFile(){
    userdb = malloc(sizeof(db));
    char *temp = NULL;
    char *buffer = NULL; // buffer to store data line by line from file
    char *tBuffer = NULL;
    char *rBuffer = NULL;
    char *cBuffer = NULL;
    char *wBuffer = NULL;
    char *bBuffer = NULL;
    char *pBuffer = NULL;
    char *iBuffer = NULL;
    printf(B_GROUND PURPLE"\t\tLoading data from file...\n"RESET);
    FILE *fp = NULL;
    fp = fopen("data.csv", "r");
    if(fp != NULL){
        while((buffer = freadline(fp)) != NULL){
            int col = 0;
            while((temp=readLine_csv(buffer, ','))){
                switch(col){
                    case 0: userdb[dbIndex].id = toInt(temp); break;
                    case 1: userdb[dbIndex].name = stringCopy(temp); break;
                    case 2: userdb[dbIndex].nrc = stringCopy(temp); 
                            insertByNRC(temp, dbIndex);
                            break;
                    case 3: userdb[dbIndex].email = stringCopy(temp); 
                            insertByEmail(temp, dbIndex);
                            break;
                    case 4: userdb[dbIndex].pass = stringCopy(temp); break;
                    case 5: userdb[dbIndex].phone = stringCopy(temp); 
                            insertByPhone(temp, dbIndex);
                            break;
                    case 6: userdb[dbIndex].address = stringCopy(temp); break;
                    case 7: userdb[dbIndex].curr_amt = toInt(temp); break;
                    case 8: userdb[dbIndex].income = toInt(temp); break;
                    case 9: userdb[dbIndex].loan_amt = toInt(temp); break;
                    case 10: userdb[dbIndex].loan_rate = toInt(temp); break;
                    case 11: userdb[dbIndex].isAdmin = toInt(temp); break;
                    case 12: userdb[dbIndex].isPer = toInt(temp); break;
                    case 13: userdb[dbIndex].acc_status = toInt(temp); break;
                    case 14: userdb[dbIndex].loan_status = toInt(temp); break;
                    case 15: userdb[dbIndex].p_count = toInt(temp); break;
                    case 16: userdb[dbIndex].start.yy = toInt(temp); break;
                    case 17: userdb[dbIndex].start.mm = toInt(temp); break;
                    case 18: userdb[dbIndex].start.dd = toInt(temp); break;
                    case 19: userdb[dbIndex].active = toInt(temp); break;    
                    case 20: userdb[dbIndex].lock_time = toInt(temp); break;
                    case 21: userdb[dbIndex].transLimit = toInt(temp); break;
                    //transfer , receive, cash_in, withdraw, burrow, repay
                    case 22: tBuffer = stringCopy(temp); break;
                    case 23: rBuffer = stringCopy(temp); break;
                    case 24: cBuffer = stringCopy(temp); break;
                    case 25: wBuffer = stringCopy(temp); break;
                    case 26: bBuffer = stringCopy(temp); break;
                    case 27: pBuffer = stringCopy(temp); break;
                    case 28: iBuffer = stringCopy(temp); break;
                }
                if(col>=28) break;
                col++;
            }
            userdb[dbIndex].tIndex = 0; 
            if(!stringCmp(tBuffer, "nan")){
                while((temp=readLine_csv(tBuffer, '|'))){
                    userdb[dbIndex].transfer[userdb[dbIndex].tIndex].note = stringCopy(temp);
                    userdb[dbIndex].tIndex++;
                }
            }
            userdb[dbIndex].rIndex = 0;
            if(!stringCmp(rBuffer, "nan")){
                while((temp=readLine_csv(rBuffer, '|'))){
                    userdb[dbIndex].receive[userdb[dbIndex].rIndex].note = stringCopy(temp);
                    userdb[dbIndex].rIndex++;
                }
            }
            userdb[dbIndex].cIndex = 0;
            if(!stringCmp(cBuffer, "nan")){               
                while((temp=readLine_csv(cBuffer, '|'))){
                    userdb[dbIndex].cashIn[userdb[dbIndex].cIndex].note = stringCopy(temp);
                    userdb[dbIndex].cIndex++;
                }
            }       
            userdb[dbIndex].wIndex = 0;
            if(!stringCmp(wBuffer, "nan")){
                while((temp=readLine_csv(wBuffer, '|'))){
                    userdb[dbIndex].withdraw[userdb[dbIndex].wIndex].note  = stringCopy(temp);
                    userdb[dbIndex].wIndex++;
                }
            } 
            userdb[dbIndex].bIndex = 0;
            if(!stringCmp(bBuffer, "nan")){
                while((temp=readLine_csv(bBuffer, '|'))){
                    userdb[dbIndex].burrow[userdb[dbIndex].bIndex].note = stringCopy(temp);
                    userdb[dbIndex].bIndex++;
                }
            }
            userdb[dbIndex].pIndex = 0;
            if(!stringCmp(pBuffer, "nan")){
                while((temp=readLine_csv(pBuffer, '|'))){
                    userdb[dbIndex].repay[userdb[dbIndex].pIndex].note = stringCopy(temp);
                    userdb[dbIndex].pIndex++;
                }
            }
            userdb[dbIndex].iIndex = 0;
            if(!stringCmp(iBuffer, "nan")){
                while((temp=readLine_csv(iBuffer, '|'))){
                    userdb[dbIndex].info[userdb[dbIndex].iIndex].note = stringCopy(temp);
                    userdb[dbIndex].iIndex++;
                }
            }
            dbIndex++;
            userdb = realloc(userdb, ((dbIndex+1) * sizeof(db)));
        }
        fclose(fp);
        printf(B_GROUND PURPLE"\t\tData loaded successfully...\n"RESET);
    }
    else
        printf(RED"[!]Loading database Failed.\n"RESET);
}

void saveAllData(){
    int i = 0;
    unsigned int id = 1;
    printf(PURPLE"\t\tSaving Data...\n"RESET);
    FILE *fp = fopen("data.csv", "w");
    traverseTree();
    if(fp != NULL){
        while((i = getIndexes()) != -1){
            fprintf(fp, "%u,%s,%s,%s,\"%s\",%s,\"%s\",%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%ld,%ld,%u", id++, userdb[i].name, 
                                                                            userdb[i].nrc, userdb[i].email, userdb[i].pass, 
                                                                            userdb[i].phone, userdb[i].address, userdb[i].curr_amt,
                                                                            userdb[i].income, userdb[i].loan_amt, userdb[i].loan_rate, 
                                                                            userdb[i].isAdmin, userdb[i].isPer, userdb[i].acc_status, 
                                                                            userdb[i].loan_status, userdb[i].p_count, userdb[i].start.yy, 
                                                                            userdb[i].start.mm, userdb[i].start.dd, userdb[i].active, 
                                                                            userdb[i].lock_time, userdb[i].transLimit);
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].tIndex; j++){
                fprintf(fp, "%s", userdb[i].transfer[j].note);
                if(j+1 < userdb[i].tIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].rIndex; j++){
                fprintf(fp, "%s", userdb[i].receive[j].note);
                if(j+1 < userdb[i].rIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].cIndex; j++){
                fprintf(fp, "%s", userdb[i].cashIn[j].note);
                if(j+1 < userdb[i].cIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].wIndex; j++){
                fprintf(fp, "%s", userdb[i].withdraw[j].note);
                if(j+1 < userdb[i].wIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].bIndex; j++){
                fprintf(fp, "%s", userdb[i].burrow[j].note);
                if(j+1 < userdb[i].bIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].pIndex; j++){
                fprintf(fp, "%s", userdb[i].repay[j].note);
                if(j+1 < userdb[i].pIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].iIndex; j++){
                fprintf(fp, "%s", userdb[i].info[j].note);
                if(j+1 < userdb[i].iIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            fprintf(fp, "%c", '\n');
        }
        printf(PURPLE"\t\tData saved...\n"G_RESET);
        if(fp != NULL)
            fclose(fp);
    }
    else
        printf(RED"File opening Failed\n"RESET);
}

//when the program catch aborting interrupt
void interruptHandle(int sigCode){
    saveAllData();
    termServer();
    printf(GREEN"Sever terminated successfully\n"G_RESET);
    exit(0);
}
// function to send integer data to client back
void sendValue(long data){
    char buffer[1024] = {0};
    sprintf(buffer, "%ld", data);
    respond(buffer);
}
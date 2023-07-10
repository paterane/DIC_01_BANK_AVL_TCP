//
// Created by Peter Oo on 07/01/2023
//
#include <stdio.h>
#include <stdlib.h>
#include "include/utils.h"
#include "include/client.h"

#define ip             "127.0.0.1"
#define port           8080

/* Global variables*/
int uIndex = -1; //Special index to be used in searching user index
int userFound = -1; //General userIndex that can be used if the index is to be returned
int reFound = -1; //General userIndex that can be used if the index is to be returned
unsigned int choice = 0;  //General user selection

/********************************FUNCTIONS DECLARATIONS*********************************/
int isMailValid(char *email, char *r_pattern);
int isNRCValid(char *nrc);
int isPassValid(char *pass);
int isPhoneValid(char *ph);
void isExisted(char *key, char *value);
int get_amount_from_trans(char *transact);
void show_info(int idx);
void user_sector();
void login_section();
void mail_form();
void nrc_form();
void pass_form();
void phone_form();
void registration();
void welcome();
void exitProgram();
void funcCall(void(*f)(), char *fname);
char *getString(unsigned int idx, char *key);
long getValue(unsigned int idx, char *key);
void updateString(unsigned int idx, char *key, char *value);
void updateValue(unsigned int idx, char *key, long value);
void insertString(char *key, char *value);
void insertValue(char *key, long value);
char *getTrans(unsigned int idx, char *key,int ti);
char *transferMoney(int idxS, int idxR, unsigned int amount);
char *doTransaction(int idx, unsigned int amount, char *process);
char *infoTransaction(int idx, char *key, char *previous, char *current);
long getCurTime();
char *getCurTimeS();
int days_left(int idx);
void deleteUser(int idx);

/**
 * START OF MAIN PROGRAM
*/
int main(){

    welcome();

    return 0;
}

/***************************************************************************************/
//////////////////////////////////Bank Functions/////////////////////////////////////////
/***************************************************************************************/
/*
 * RETURN: 0 if not email format, 1 if it is an email.
 * Format: xxxx@[A-Za-z0-9].com
 * */
int isMailValid(char *email, char *r_pattern){
    int indexA = indexOf(email, "@");
    int indexB = indexOf(email, ".com");
    if(indexA == -1 || indexB == -1)
        return 0;
    char *emailName = subString(email, 0, indexA);
    if(emailName == NULL)
        return 0;
    if(!regExpress(emailName, "a-z0-9.-.")){
        printf(RED"An email shouldn't contain any special or upper case characters.\n"RESET);
        return 0;
    }
    if(stringCount(emailName, ".") >= 2){
        printf(RED"An email shouldn't contain \".\" more than once in user name!!\n"RESET);
        return 0;
    }
    if(email[indexB+4] != '\0')
        return 0;
    char *domain = subString(email, indexA+1,indexB);
    if(domain == NULL)
        return 0;
    if(regExpress(domain, r_pattern))
        return 1;
    else
        return 0;
}
/*
    Return 1 if NRC format is valid, otherwise 0.
*/
int isNRCValid(char *nrc){
    int indexA = indexOf(nrc, "/");
    int indexB = indexOf(wordLower(nrc), "<n>");
    int indexC = indexOf(nrc, "\0");
    char *code = subString(nrc, 0, indexA);
    char *region = subString(wordLower(nrc), indexA+1, indexB);
    char *digit = subString(nrc, indexB+3, indexC);
    if(indexA == -1 || indexB == -1)    return 0;
    if(!isNum(code))    return 0;
    if(!regExpress(region, "a-zA-Z"))   return 0;
    if(isNum(digit))    return 1;
    else    return 0;
}
/*
    Return 1 if password format is correct and strong, else 0
*/
int isPassValid(char *pass){
    int len = stringLen(pass);
    int count = 0;
    for(int i=0; i<len; i++){
        if(pass[i] == ' ')  return 0;
    }
    if(len < 12) return 0;
    if(!regExpress(pass, "!-!#-~")) return 0;
    for(char c='A'; c<='Z'; c++){
        char temp[2] = {c,'\0'};
        count += stringCount(pass, temp);
        if(count > 0) break;
    }
    if(count < 1) return 0;
    count = 0;
    for(char c='0'; c<='9'; c++){
        char temp[2] = {c,'\0'};
        count += stringCount(pass, temp);
        if(count > 0) break;
    }
    if(count < 1) return 0;
    count = 0;
    char spChar[] = "!#$%&'()*+,-./:;<=>?@[]\\^_`{}|~";
    for(int i=0; i<stringLen(spChar); i++){
        char temp[2] = {spChar[i], '\0'};
        count += stringCount(pass, temp);
        if(count > 0) break;
    }
    if (count < 1) return 0;
    else return 1;
}
/*
    RETURN: 1 if phone format valid, else 0
    //09xxxxxxxxx [9digits next to 09]
*/
int isPhoneValid(char *ph){
    int indexA = indexOf(ph, "09");
    int indexB = indexOf(ph, "\0");
    if(indexA == -1)    return 0;
    char *digits = subString(ph, indexA+2, indexB);
    if(isNum(digits) && (stringLen(digits)==9))   return 1;
    else    return 0;
}
/*
    input key value pair to be searched for , return account index if existed otherwise 0
*/
void isExisted(char *key, char *value){
    uIndex = -1;    
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "search,%s,%s", key, value);
    sockConnect(ip, port);
    sockSend(cmdForm);
    char *rmsg = sockRecv();
    sockClose();
    long rValue = toInt(rmsg);
    uIndex = (int)rValue;
}
/* Take amount from transaction record and RETRUN as INT */
int get_amount_from_trans(char *transact){
    char *temp = subString(transact, 0, indexOf(transact, "MMK"));
    int amount = toInt(temp);
    return amount;
}
/**
 * whenever called by index, user's information at that index is returned
*/
void show_info(int idx){
    printf("%-20s: "BLUE"%s\n"RESET"%-20s: "L_BLUE"%s\n"RESET"%-20s: "BLUE
            "%s\n"RESET"%-20s: "L_BLUE"%s\n"RESET"%-20s: "BLUE"%s\n"RESET
            "%-20s: "L_BLUE"%uMMK\n"RESET"%-20s:"BLUE" %d/%d/%d"RESET"\n", "Name", getString(idx, "name"), "NRC", 
                                            getString(idx, "nrc"), "Email", getString(idx, "email"), 
                                            "Phone", getString(idx, "phone"), "Address", getString(idx, "address"), 
                                            "Balance", getValue(idx, "curr_amt"), "Created on", getValue(idx, "start.dd"),
                                            getValue(idx, "start.mm")+1, getValue(idx, "start.yy"));
    unsigned int loan_amt = 0;
    int loan_status = getValue(idx, "loan_status");
    if(loan_status == 1){
        int b = getValue(idx, "bIndex");
        loan_amt = get_amount_from_trans(getTrans(idx, "bNote", b-1)); 
    }
    printf("%-20s: "BLUE"%uMMK\n"RESET"%-20s: "L_BLUE"%uMMK\n"RESET"%-20s: "BLUE"%uMMK\n"RESET \
            "[Acc_Type: "BLUE"%s"RESET"]\t[Loan_Status: "BLUE"%s"RESET"]\n", "Income", getValue(idx, "income"),
                                            "Burrowed amount", loan_amt, "loan interest",
                                            getValue(idx, "loan_rate"),(getValue(idx, "isPer"))? "Personal":"Business",
                                            (loan_status)? "Burrowed":"Clear");
    if(loan_status){
        int days = days_left(idx);
        if(days > -1)
            printf("Days left to repay: "GREEN"%d"RESET" day%c\n", days, (days == 1)? ' ':'s');
        else
            printf(RED"Overdue to repay!\n"RESET
                      "Days overdued: "RED"%d"RESET" day%c\n", (-1)*days, (days == -1)? ' ':'s');
    }
}

void user_sector(){
    char userIn[30];
    printf("User name >>"BLUE" %s "RESET"<<\n", getString(userFound, "name"));
    while(1){
        int isAdmin = getValue(userFound, "isAdmin");
        printf(GREEN"Transfer"RESET","GREEN" Withdraw"RESET","GREEN" Info" \
                RESET","GREEN" Cash In"RESET","GREEN" Loan"RESET","GREEN" Log" \
                RESET","YELLOW" %s"RESET"%s"GREEN"Exit"RESET" or"GREEN" 1"RESET" to main page: ",
                    (isAdmin)? "admin" : "", (isAdmin)? ", " : "");
        scanf(" %[^\n]%*c", userIn);
        /**
         * TRANSFER OPTION
        */
        if(stringCmp(wordLower(userIn), "transfer")){
            while(1){
                while(1){
                    printf("Enter recipent email, phone_no or NRC: ");
                    scanf(" %[^\n]%*c", userIn);
                    if(isMailValid(userIn, "a-z0-9")){
                        isExisted("email", userIn);
                        break;
                    }
                    else if(isPhoneValid(userIn)){
                        isExisted("phone", userIn);
                        break;
                    }  
                    else if(isNRCValid(userIn)){
                       isExisted("nrc", userIn);
                       break;
                    } 
                    else
                        printf(RED"Submittant should be email, phone_no or NRC\n"RESET);
                }
                if(uIndex != userFound){
                    if(uIndex == -1){
                        printf(RED"Recipent not existed\n"RESET);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }else break;                    
                }
                else{
                    printf(RED"Sorry you can't transfer to your same account\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            reFound = uIndex;
            printf("Recipient found:"BLUE" %s\n"RESET"Email:"BLUE" %s\n"RESET, getString(reFound, "name"), getString(reFound, "email"));
            while(1){
                printf(GREEN"Minimum amount allowable to be transferred >> 1000MMK\n"RESET);
                printf("Enter an amount to be transferred: ");
                digit_input(&choice);
                if(choice >= 1000){
                    if((getValue(userFound, "curr_amt") - 1000) >= choice) break;
                    else{
                        printf(RED"Insufficient balance\n"RESET);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }
                }
                else{
                    printf(RED"Transferred amount shouldn't be less than 1000 mmk\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            int amount_t = choice;
            while(1){
                char *passIn = getpassword("Enter password to send: ");
                if(stringCmp(passIn, getString(userFound, "pass"))) break;
                else{
                    printf(RED"Wrong credential\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            char *transNote = transferMoney(userFound, reFound, amount_t);
            if(stringCmp(transNote, "R_NG"))
                printf(RED"Sorry, Recipient's account have reached max transaction amount limit.\nTry it, tomorrow.\n"RESET);
            else if(stringCmp(transNote, "T_NG"))
                printf(RED"Sorry, You have reached your max transaction amount limit.\nTry it, tomorrow.\n"RESET);
            else{
                printf("%-20s:"BLUE" %s\n"RESET, "Transaction", transNote);
                printf("%-20s:"BLUE" %u mmk\n"RESET, "Balance", getValue(userFound, "curr_amt"));
            }
        }
        /**
         * WITHDRAW OPTION
        */
        else if(stringCmp(wordLower(userIn), "withdraw")){
            while(1){
                printf(GREEN"Minimum amount allowable to be withdrawn >> 5000MMK\n"RESET);
                printf("Submit amount to be withdrawn: ");
                digit_input(&choice);
                if(choice >= 5000){
                    if((getValue(userFound, "curr_amt") - 1000) >= choice) break;
                    else{
                        printf(RED"Insufficient amount in the account\n"RESET);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }
                }
                else{
                    printf(RED"Amount not in the allowable limits\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            char *transNote = doTransaction(userFound, choice, "withdraw");
            if(stringCmp(transNote, "NG"))
                printf(RED"Sorry, You have reached your max transaction amount limit.\nTry it, tomorrow.\n"RESET);
            else{
                printf("%-20s:"BLUE" %s\n"RESET, "Transaction", transNote);
                printf("%-20s:"BLUE" %u mmk\n"RESET, "Balance", getValue(userFound, "curr_amt"));
            }
        }
        /**
         * INFORMATION OPTION
        */
        else if(stringCmp(wordLower(userIn), "info")){
            char edited[100];
            while(1){
                show_info(userFound);
                printf("Edit "YELLOW"Name"RESET", "YELLOW"Password"RESET", "YELLOW"Address"RESET" or "YELLOW"Back"RESET": ");
                scanf(" %[^\n]%*c", userIn);
                if(stringCmp(wordLower(userIn), "name")){
                    while(1){
                        printf("Enter new name or "BLUE"b"RESET" to exit: ");
                        scanf(" %[^\n]%*c", edited);
                        if(stringCmp(wordLower(edited), "b")) break; 
                        char *previous = getString(userFound, "name");
                        if(!stringCmp(wordLower(previous), wordLower(edited))){
                            updateString(userFound, "name", edited);
                            char *infoNote = infoTransaction(userFound, "name", previous, edited);
                            printf("Edited Record: "BLUE"%s\n"RESET, infoNote);
                            break;
                        }
                        else
                            printf(YELLOW"Previous and current names are the same\n"RESET);
                    }
                }
                else if(stringCmp(wordLower(userIn), "password")){
                    while(1){
                        printf("Enter new password or "BLUE"b"RESET" to exit: ");
                        char *ePass = getpassword("");
                        if(stringCmp(wordLower(ePass), "b")) break;
                        if(isPassValid(ePass)){
                            char *previous = getString(userFound, "pass");
                            if(!stringCmp(ePass, previous)){
                                printf("%-31s: ", "Confirm new password");
                                char *confirm = getpassword("");
                                if(stringCmp(ePass, confirm)){
                                    updateString(userFound, "pass", ePass);
                                    char *infoNote = infoTransaction(userFound, "pass", previous, ePass);
                                    printf("Edited Record: "BLUE"%s\n"RESET, infoNote);
                                    break;
                                }
                                else
                                    printf(RED"Passwords not matched\n"RESET);
                            }
                            else
                                printf(YELLOW"Previous and current passwords are the same\n"RESET);
                        }
                        else
                            printf(RED"Invalid or weak password,\n Password should have at least one Capital, number, special characters\n"RESET);
                    }
                }
                else if(stringCmp(wordLower(userIn), "address")){
                    while(1){
                        printf("Enter new address or "BLUE"b"RESET" to exit: ");
                        scanf(" %[^\n]%*c", edited);
                        if(stringCmp(wordLower(edited), "b")) break; 
                        char *previous = getString(userFound, "address");
                        if(!stringCmp(wordLower(previous), wordLower(edited))){
                            updateString(userFound, "address", edited);
                            char *infoNote = infoTransaction(userFound, "address", previous, edited);
                            printf("Edited Record: "BLUE"%s\n"RESET, infoNote);
                            break;
                        }
                        else
                            printf(YELLOW"Previous and current addresses are the same\n"RESET);
                    }
                }
                else if(stringCmp(wordLower(userIn), "back")) break;
                else    printf(RED"Wrong Input\n"RESET);
            }
        }
        /**
         * CASH IN OPTION
        */
        else if(stringCmp(wordLower(userIn), "cash in")){
            while(1){
                printf(GREEN"Minimum amount allowable to be submitted >> 5000MMK\n"RESET);
                printf("Please insert CASH in the slot: ");
                digit_input(&choice);
                if(choice >= 5000) break;
                else{
                    printf(RED"Amount not in the allowable limits\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            char *transNote = doTransaction(userFound, choice, "cashin");
            printf("%-20s:"BLUE" %s\n"RESET, "Transaction", transNote);
            printf("%-20s:"BLUE" %u mmk\n"RESET, "Balance", getValue(userFound, "curr_amt"));
        }
        /**
         * LOGGING OPTION
        */
        else if(stringCmp(wordLower(userIn), "log")){ 
            int num = 1;         
            int tIndex = getValue(userFound, "tIndex");  
            for(int i=0; i<tIndex; i++) 
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, getTrans(userFound, "tNote", i));
            if(tIndex > 0) printf("\n");
            int rIndex = getValue(userFound, "rIndex");
            for(int i=0; i<rIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, getTrans(userFound, "rNote", i));
            if(rIndex > 0) printf("\n");
            int cIndex = getValue(userFound, "cIndex");
            for(int i=0; i<cIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, getTrans(userFound, "cNote", i));
            if(cIndex > 0) printf("\n");
            int wIndex = getValue(userFound, "wIndex");
            for(int i=0; i<wIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, getTrans(userFound, "wNote", i));
            if(wIndex > 0) printf("\n");
            int bIndex = getValue(userFound, "bIndex");
            for(int i=0; i<bIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, getTrans(userFound, "bNote", i));
            if(bIndex > 0) printf("\n");    
            int pIndex = getValue(userFound, "pIndex");        
            for(int i=0; i<pIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, getTrans(userFound, "pNote", i));
            if(pIndex > 0) printf("\n");
            int iIndex = getValue(userFound, "iIndex");        
            for(int i=0; i<iIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, getTrans(userFound, "iNote", i));
            if(iIndex > 0) printf("\n");
        }
        /**
         * LOAN OPTION
        */
        else if(stringCmp(wordLower(userIn), "loan")){
            if(getValue(userFound, "loan_status") == 1){
                int time_left = days_left(userFound);
                printf("%-30s: "BLUE"%u mmk"RESET"\n", "Loan amount left to be repaid", getValue(userFound, "loan_amt")); 
                if(time_left < 0) printf("Loan overdue\nContact to HO\n%-30s:"RED" %d day%c"RESET, "time overdue", (-1)*time_left,
                                                                                                ((-1)*time_left > 1)? 's':' ');
                else printf("%-30s:"BLUE" %d day%c"RESET"\n", "Time left to repay", time_left, (time_left > 1)? 's':' ');
            }
            else    printf(GREEN"Loan Status Clear\nWould you like to get some loan...?\n"RESET);
            while(1){
                printf("Press "BLUE"<enter>"RESET" to user_sector or"BLUE" 1"RESET" to continue: ");
                fgets(userIn, 30, stdin);
                if(stringCmp(userIn, "\n")) user_sector();
                else if(stringCmp(userIn, "1\n")) break;
                else printf(RED"Wrong Input\n"RESET);
            }
            struct Date start;
            start.yy = getValue(userFound, "start.yy");
            start.mm = getValue(userFound, "start.mm");
            start.dd = getValue(userFound, "start.dd");
            long start_time = timeStruct_to_L(&start);
            long curr_time = getCurTime();
            double acc_exist_time = diff_time_L(curr_time, start_time);                                        // 3 days
            if((getValue(userFound, "loan_status") == 0) && (getValue(userFound, "income") >= 400000) && (acc_exist_time >= 259200.0)){
                while(1){
                    printf("Enter an amount to be burrowed: ");
                    digit_input(&choice);
                    if((choice <= (2 * getValue(userFound, "income"))) && (choice >= 400000))
                        break;
                    else{
                        printf(RED"Your max loan amount is limited to double of income, and\nmini loan amount at least 400,000\n"RESET);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }
                }
                char *transNote = doTransaction(userFound, choice, "burrow");
                unsigned int loan_rate = getValue(userFound, "loan_rate");
                unsigned int loan_amt = getValue(userFound, "loan_amt");
                printf("%-20s:"BLUE" %s\n"RESET, "Transaction", transNote);
                printf("%-20s:"BLUE" %u mmk"RESET"\n%-20s: "L_BLUE"%u mmk"RESET \
                        "\n%-20s: "BLUE"%u mmk"RESET"\n", "Balance", getValue(userFound, "curr_amt"), 
                                                                "Loan amount", loan_amt - loan_rate, 
                                                                "Loan interest", loan_rate);
            }
            else{
                if(getValue(userFound, "income") < 400000){
                    printf(RED"Sorry, you can't get loan as your salary is lower than 400,000MMK"RESET"\n");
                    user_sector();
                }
                else if(acc_exist_time < 259200.0){
                    printf(RED"Sorry, you can get loan only when your account existance is more than 3 days"RESET"\n");
                    user_sector();
                }
                while(1){
                    printf("Enter an amount to be repaid for loan: ");
                    digit_input(&choice);
                    int a = getValue(userFound, "bIndex") - 1;
                    unsigned int monthly_repay = (get_amount_from_trans(getTrans(userFound, "bNote", a)) + getValue(userFound, "loan_rate")) / 30;
                    if(((getValue(userFound, "curr_amt") - 1000) >= choice) && choice >= monthly_repay){
                        char *transNote = doTransaction(userFound, choice, "repay");
                        printf("%-20s: "BLUE"%s"RESET"\n", "transaction", transNote);
                        break;
                    }
                    else{
                        printf(RED"Sorry, insufficient balance or repaid amount should not less than monthly repay amount: %u mmk\n"
                                RESET, monthly_repay);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }
                }
            }
        }
        /**
         * ADMIN OPTION
        */
        else if(stringCmp(wordLower(userIn), "admin") && getValue(userFound, "isAdmin")){
            while(1){
                printf(BLUE"users"RESET" ,"BLUE"change admin"RESET" ,"BLUE"manage"RESET" ,"BLUE"remove"RESET" or " \
                        BLUE"exit"RESET" to user_sector: ");
                scanf(" %[^\n]%*c", userIn);
                if(stringCmp(wordLower(userIn), "users")){
                    sockConnect(ip, port);
                    sockSend("dbIndex");
                    int dbIndex = toInt(sockRecv());
                    sockClose();
                    int quitShow = 0;
                    for(int i=0; i<dbIndex; i++){
                        show_info(i);
                        printf("\n");
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to continue or"BLUE" 1"RESET" to admin: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "1\n")){
                                quitShow = 1;
                                break;
                            }
                            else break;
                        }
                        if(quitShow)    break;
                    }
                }
                else if(stringCmp(wordLower(userIn), "change admin")){
                    printf(GREEN"Change the Admin account"RESET"\n");
                    while(1){
                        int loopBreak = 0;
                        printf("%-15s: ", "Enter Email");
                        scanf(" %[^\n]%*c", userIn);
                        isExisted("email", userIn);
                        if(uIndex != -1 && uIndex != userFound){
                            printf("Are you sure to pass to"BLUE" %s"RESET" [Yes|No][No]: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(wordLower(userIn), "yes\n")){
                                updateValue(userFound, "isAdmin", 0);
                                updateValue(uIndex, "isAdmin", 1);
                                printf(GREEN"Admin position has been successfully passed to %s\n"RESET, getString(uIndex, "name"));
                                user_sector();
                            }
                            else printf("You are still"BLUE" Admin"RESET"\n");
                            break;
                        }
                        else{
                            printf(RED"Email not found\n"RESET);
                            while(1){
                                printf("Press"BLUE" <enter>"RESET" to admin or"BLUE" 1"RESET" to re-submit: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(userIn, "\n")){
                                    loopBreak = 1;
                                    break;
                                }
                                else if(stringCmp(userIn, "1\n")) break;
                                else printf(RED"Wrong Input\n"RESET);
                            }
                            if(loopBreak) break;
                        }
                    }
                }
                else if(stringCmp(wordLower(userIn), "manage")){
                    printf(GREEN"Admin can suspend an account or unlock from suspension\n"RESET);
                    while(1){
                        int loopbreak = 0;
                        printf("%-15s: ", "Enter Email");
                        scanf(" %[^\n]%*c", userIn);
                        isExisted("email", userIn);
                        if(uIndex != -1 && uIndex != userFound){
                            int acc_status = getValue(uIndex, "acc_status");
                            printf("This account, %s, is "BLUE"%s\n"RESET, getString(uIndex, "email"), 
                                                                            (acc_status)? "active":"suspended");
                            if(acc_status){
                                printf("Would you like to lock the account?[Yes|No][No]: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(wordLower(userIn), "yes\n")){
                                    updateValue(uIndex, "acc_status", 0);
                                    printf("The account has been locked successfully\n");
                                }
                            }
                            else{
                                printf("Would you like to unlock the account?[Yes|No][No]: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(wordLower(userIn), "yes\n")){
                                    updateValue(uIndex, "acc_status", 1);
                                    printf("The account has been unlocked successfully\n");
                                }                         
                            }
                            break;
                        }
                        else{
                            printf(RED"Email not found\n"RESET);
                            while(1){
                                printf("Press"BLUE" <enter>"RESET" to admin or"BLUE" 1"RESET" to re-submit: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(userIn, "\n")){
                                    loopbreak = 1;
                                    break;
                                }
                                else if(stringCmp(userIn, "1\n")) break;
                                else printf(RED"Wrong Input\n"RESET);
                            }
                            if(loopbreak) break;
                        }
                    }
                }
                else if(stringCmp(wordLower(userIn), "remove")){
                    printf(GREEN"Permanently delete an account\n"RESET);
                    while(1){
                        int loopbreak = 0;
                        printf("%-15s: ", "Enter Email");
                        scanf(" %[^\n]%*c", userIn);
                        isExisted("email", userIn);
                        if(uIndex != -1 && uIndex != userFound){
                            char *name = getString(uIndex, "name");
                            printf("%-15s>>> "BLUE"%s, %s\n"RESET,"User found", name, getString(uIndex, "email"));
                            printf(YELLOW"Are you sure to remove %s?[yes/no][no]: "RESET, name);
                            fgets(userIn, 30, stdin);
                            if(stringCmp(wordLower(userIn), "yes\n")){
                                deleteUser(uIndex);
                                printf("The account has been successfully removed.\n");
                            }
                            break;
                        }
                        else{
                            if(uIndex == userFound)
                                printf(RED"Admin account can't be deleted\n"RESET);
                            else
                                printf(RED"Email not found\n"RESET);
                            while(1){
                                printf("Press"BLUE" <enter>"RESET" to admin or"BLUE" 1"RESET" to re-submit: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(userIn, "\n")){
                                    loopbreak = 1;
                                    break;
                                }
                                else if(stringCmp(userIn, "1\n")) break;
                                else printf(RED"Wrong Input\n"RESET);
                            }
                            if(loopbreak) break;
                        }
                    }
                }
                else if(stringCmp(wordLower(userIn), "exit")) user_sector();
                else printf(RED"Wrong Input\n"RESET);
            }

        }
        else if(stringCmp(wordLower(userIn), "exit")) exitProgram();
        else if(stringCmp(userIn, "1")) welcome();
        else printf(RED"[-]Wrong Input\n"RESET);
    }
}
/**
 * user login function
*/
void login_section(){
    char uEmail[100];
    char *uPass;                                      
    long curr_time = getCurTime();
    printf(YELLOW"\t\tLOGIN SECTOR\n"RESET);
    printf("%-10s: ", "Email");
    scanf(" %[^\n]%*c", uEmail);
    isExisted("email", uEmail);
    if(uIndex != -1){                                
        if(diff_time_L(curr_time, getValue(uIndex, "active")) >= 7776000.0) updateValue(uIndex, "acc_status", 0);// 3 months inactive allowed
        if(getValue(uIndex, "p_count") >= 3){
            updateValue(uIndex, "p_count", 0);
            updateValue(uIndex, "lock_time", curr_time);
        }
        if(diff_time_L(curr_time, getValue(uIndex, "lock_time")) <= 300.0 || getValue(uIndex, "acc_status") == 0){
            if(getValue(uIndex, "acc_status") == 0) printf(RED"Account suspended due to inactive status or admin locked, contact to HO"RESET"\n");
            else printf(RED"Account locked temporarily\nTry another"RESET"\n");
            funcCall(login_section, "login_section");
        }
        printf("%-10s: ", "Password");
        uPass = getpassword("");
        if(!stringCmp(uPass, getString(uIndex, "pass"))){
            int p_count = getValue(uIndex, "p_count");
            updateValue(uIndex, "p_count", ++p_count);
            printf(RED"Password incorrect!\n"RESET);
            funcCall(login_section, "login_section");
        }
    }
    else{
        printf(RED"Email doesn't exist!\n"RESET);
        funcCall(login_section, "login_section");
    }
    userFound = uIndex;
    updateValue(userFound, "p_count", 0);
    curr_time = getCurTime();
    updateValue(userFound, "active", curr_time);
    user_sector();
}
/**
 * registration email input form
*/
void mail_form(){
    char buffer[100];
    printf("%-20s: ", "Enter Email");
    scanf(" %[^\n]%*c", buffer);
    if(isMailValid(buffer, "a-z0-9")){
        isExisted("email", buffer);
        if(uIndex == -1){
            insertString("email", buffer);
        }
        else{
            printf(RED"Email already existed\n"RESET);
            funcCall(mail_form, "mail_form");
        }
    }
    else{
        printf(RED"Email format not valid, the format should be username@domainname.com\n"RESET);
        funcCall(mail_form, "mail_form");
    }
}
/**
 * registration nrc input form
*/
void nrc_form(){
    char buffer[100];
    printf("%-20s: ", "NRC number");
    scanf(" %[^\n]%*c", buffer);
    if(isNRCValid(buffer)){
        isExisted("nrc", buffer);
        if(uIndex == -1){
            insertString("nrc", buffer);
        }
        else{
            printf(RED"NRC already existed\n"RESET);
            funcCall(nrc_form, "nrc_form");
        }
    }
    else{
        printf(RED"NRC format not valid, the format should be code/region<n or N>xxxxxx\n"RESET);
        funcCall(nrc_form, "nrc_form");
    }
}
/**
 * registration password input form
*/
void pass_form(){
    char *confirmPass;
    printf("[Not less than 12 chars including at least one Capital and number and special]\n%-20s: ", "Password");
    char *password = getpassword("");
    if(isPassValid(password)){
        printf("%-20s: ", "Re-type password");
        confirmPass = getpassword("");
        if(!stringCmp(password, confirmPass)){
            printf(RED"Password not matched\n"RESET);
            funcCall(pass_form, "pass_form");
        }
        else{
            insertString("pass", password);
        }
    }
    else{
        printf(RED"Password format invalid or weak password\n"RESET);
        funcCall(pass_form, "pass_form");
    }
}
/**
 * registration phone no: input form
*/
void phone_form(){
    char buffer[100];
    printf("%-20s: ", "Enter phone number");
    scanf(" %[^\n]%*c", buffer);
    if(isPhoneValid(buffer)){
        isExisted("phone", buffer);
        if(uIndex == -1){
            insertString("phone", buffer);
        }
        else{
            printf(RED"Phone number already existed\n"RESET);
            funcCall(phone_form, "phone_form");
        }
    }
    else {
        printf(RED"Phone number should start with 09xxxxxxxxx\n"RESET);
        funcCall(phone_form, "phone_form");
    }
}
/**
 * New user registration function
*/
void registration(){
    char buffer[100];
    printf(YELLOW"\t\tUSER REGISTRATION\n"RESET);
    mail_form();
    nrc_form();
    pass_form();
    phone_form();
    printf("%-20s: ", "Name");
    scanf(" %[^\n]%*c", buffer);
    insertString("name", buffer);
    printf("%-20s: ", "Address");
    scanf(" %[^\n]%*c", buffer);
    insertString("address", buffer);
    printf("%-20s: ", "Monthly income");
    digit_input(&choice);
    insertValue("income", choice);
    printf("[1 for Personal or 0 for business]\n%-15s: ", "Account-type");
    digit_input(&choice);
    insertValue("isPer", choice);
    sockConnect(ip, port);
    sockSend("register");
    waitResponse();
    sockClose();
    printf("Registeration succeeded.\n");
    printf(BLUE"EXIT"RESET" or Press "BLUE" <enter>"RESET" to Main Section: ");
    fgets(buffer, 100, stdin);
    if(stringCmp(wordLower(buffer), "exit\n")) exitProgram();
    else          welcome();
}
/**
 * Main user page login, register or exit
*/
void welcome(){
    char buffer[100];
    printf(YELLOW"\t\tWelcome to ebanking service\n"RESET);
    while(1){
        printf("["GREEN"login"RESET","GREEN" register"RESET" or"GREEN" exit"RESET"]: ");
        scanf(" %[^\n]%*c", buffer);
        if(stringCmp(wordLower(buffer), "login")) login_section();
        else if(stringCmp(wordLower(buffer), "register")) registration();
        else if(stringCmp(wordLower(buffer), "exit")) exitProgram();
        else printf(RED"[!]Wrong Input\n"RESET);
    }
}
/*
    particular function call to shorten the repeated codes
*/
void funcCall(void(*f)(), char *fname){
    char buffer[100] = {0};
    while(1){
        printf("Press"BLUE" <enter>"RESET" to main page or"BLUE" 1"RESET" to %s: ", fname);
        fgets(buffer, 100, stdin);
        if(stringCmp(buffer, "\n")){
            welcome();
            break;
        }
        else if(stringCmp(buffer, "1\n")){
            f();
            break;
        }
        else printf(RED"Wrong Input\n"RESET);
    }
}
/**
 * exit the program reseting console
*/
void exitProgram(){
    printf(GREEN"SEE YOU SOON\n"G_RESET);
    exit(0);
}
/**
 * get string value from given index and key from database
*/
char *getString(unsigned int idx, char *key){
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "get,%u,%s", idx, key);
    sockConnect(ip, port);
    sockSend(cmdForm);
    char *rmsg = sockRecv();
    sockClose();
    return rmsg;
}
/**
 * get long value from given index and key form database
*/
long getValue(unsigned int idx, char *key){
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "get,%u,%s", idx, key);
    sockConnect(ip, port);
    sockSend(cmdForm);
    char *rValue = sockRecv();
    sockClose();
    return toInt(rValue);
}
/**
 * update string data defined by user index at the required key
*/
void updateString(unsigned int idx, char *key, char *value){
    char cmdForm[1024] = {0};
    if(stringCmp(key, "address") || stringCmp(key, "pass"))
        sprintf(cmdForm, "update,%u,%s,\"%s\"", idx, key, value);
    else
        sprintf(cmdForm, "update,%u,%s,%s", idx, key, value);
    sockConnect(ip, port);
    sockSend(cmdForm);
    waitResponse();
    sockClose();
}
/**
 * update integer data defined by user index at the required key
*/
void updateValue(unsigned int idx, char *key, long value){
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "update,%u,%s,%ld", idx, key, value);
    sockConnect(ip, port);
    sockSend(cmdForm);
    waitResponse();
    sockClose();
}
/**
 * insert a string for registration
*/
void insertString(char *key, char *value){
    char cmdForm[1024] = {0};
    if(stringCmp(key, "address") || stringCmp(key, "pass"))
        sprintf(cmdForm, "insert,%s,\"%s\"", key, value);
    else
        sprintf(cmdForm, "insert,%s,%s", key, value);
    sockConnect(ip, port);
    sockSend(cmdForm);
    waitResponse();
    sockClose();
}
/**
 * insert an integer value for registration
*/
void insertValue(char *key, long value){
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "insert,%s,%ld", key, value);
    sockConnect(ip, port);
    sockSend(cmdForm);
    waitResponse();
    sockClose();
}
/**
 * get perticular transaction note from the given user index and note index
*/
char *getTrans(unsigned int idx, char *key,int ti){
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "get,%u,%s,%d", idx, key, ti);
    sockConnect(ip, port);
    sockSend(cmdForm);
    char *rmsg = sockRecv();
    sockClose();
    return rmsg;
}
/**
 * Process money_transfer Transaction
*/
char *transferMoney(int idxS, int idxR, unsigned int amount){
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "transfer,%d,%d,%u", idxS, idxR, amount);
    sockConnect(ip, port);
    sockSend(cmdForm);
    char *rmsg = sockRecv();
    sockClose();
    return rmsg;
}
/**
 * Process other transactions
 * process: cashin, withdraw, burrow, repay
*/
char *doTransaction(int idx, unsigned int amount, char *process){
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "%s,%d,%u", process, idx, amount);
    sockConnect(ip, port);
    sockSend(cmdForm);
    char *rmsg = sockRecv();
    sockClose();
    return rmsg;
}
/**
 * Processing transaction for updating user information
*/
char *infoTransaction(int idx, char *key, char *previous, char *current){
    char cmdForm[1024] = {0};
    if(stringCmp(key, "address") || stringCmp(key, "pass"))
        sprintf(cmdForm, "editInfo,%d,%s,\"%s\",\"%s\"", idx, key, previous, current);
    else    
        sprintf(cmdForm, "editInfo,%d,%s,%s,%s", idx, key, previous, current);
    sockConnect(ip, port);
    sockSend(cmdForm);
    char *rmsg = sockRecv();
    sockClose();
    return rmsg;
}
/**
 * get current time from database in long form
*/
long getCurTime(){
    sockConnect(ip, port);
    sockSend("get,0,currentTime");
    char *rmsg = sockRecv();
    sockClose();
    return toInt(rmsg);
}
/**
 * get current time from database in string form
*/
char *getCurTimeS(){
    sockConnect(ip, port);
    sockSend("get,0,currentTimeS");
    char *rmsg = sockRecv();
    sockClose();
    return rmsg;
}
/**
 * get days_left to repay burrowed money
*/
int days_left(int idx){
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "daysLeft,%d", idx);
    sockConnect(ip, port);
    sockSend(cmdForm);
    char *rmsg = sockRecv();
    sockClose();
    long rValue = toInt(rmsg);
    return (int)rValue;
}
/**
 * Delete an user from database
*/
void deleteUser(int idx){
    char cmdForm[1024] = {0};
    sprintf(cmdForm, "delete,%d", idx);
    sockConnect(ip, port);
    sockSend(cmdForm);
    waitResponse();
    sockClose();
}

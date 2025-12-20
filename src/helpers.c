// helpers.c
#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int cmp_accounts(Account a, Account b, SortMethod method) {
    if (method == ID) {
        return strcmp(a.id, b.id);
    }
    if (method == NAME) {
        return strcmp(a.name, b.name);
    }
    if (method == BALANCE) {
        return (a.balance < b.balance ? 1 : (a.balance == b.balance ? 0 : -1));
    }
    if (method == STATUS) {
        return (a.status < b.status ? 1 : (a.status == b.status ? 0 : -1));
    }
    if (method == DATE) {
        if (a.date.year < b.date.year) {
            return -1;
        } else if (a.date.year > b.date.year) {
            return 1;
        } else {
            if (a.date.month < b.date.month) {
                return -1;
            } else if (a.date.month > b.date.month) {
                return 1;
            } else {
                return 0;
            }
        }
    }
}

int cmp_transactions(Transaction a, Transaction b) {
    if (a.date.year < b.date.year) {
        return -1;
    } else if (a.date.year > b.date.year) {
        return 1;
    } else {
        if (a.date.month < b.date.month) {
            return -1;
        } else if (a.date.month > b.date.month) {
            return 1;
        } else {
            if (a.date.day < b.date.day) {
                return -1;
            } else if (a.date.day > b.date.day) {
                return 1;
            } else {
                return 0;
            }
        }
    }
}

void account_merge_sort(Account accounts[], int l, int r, SortMethod method) {
    if (l < r) {
        int mid = l + (r - l) / 2, i;
        account_merge_sort(accounts, l, mid, method);
        account_merge_sort(accounts, mid + 1, r, method);
        Account la[mid - l + 1], ra[r - mid];
        for (i = 0; i < mid - l + 1; i++) {
            la[i] = accounts[l + i];
        }
        for (i = 0; i < r - mid; i++) {
            ra[i] = accounts[mid + 1 + i];
        }
        int p1 = 0, p2 = 0, idx = l;
        while (p1 < mid - l + 1 && p2 < r - mid) {
            if (cmp_accounts(la[p1], ra[p2], method) <= 0) {
                accounts[idx++] = la[p1++];
            } else {
                accounts[idx++] = ra[p2++];
            }
        }
        while (p1 < mid - l + 1) {
            accounts[idx++] = la[p1++];
        }
        while (p2 < r - mid) {
            accounts[idx++] = ra[p2++];
        }
    }
}

double day_withdrawals(DateDay day, char *id) {
    char fileName[N];
    strcpy(fileName, "files/accounts/");
    strcat(fileName,id);
    strcat(fileName,".txt");
    FILE *accountFile = fopen(fileName, "r");
    if (accountFile == NULL) {
        FILE *createAccountFile = fopen(fileName, "w");
        fclose(createAccountFile);
        accountFile = fopen(fileName, "r");
    }
    char line[4 * N], ufid[N], uftype[N], ufamount[N], ufdaydate[N];
    int i;
    double sm = 0;
    for (i = 0; fgets(line, sizeof(line), accountFile); i++) {
        strcpy(ufid, strtok(line, ","));
        strcpy(uftype, strtok(NULL, ","));
        strcpy(ufamount, strtok(NULL, ","));
        strcpy(ufdaydate, strtok(NULL, ","));
        DateDay cur;
        cur.day = atoi(strtok(ufdaydate, "-"));
        cur.month = atoi(strtok(NULL, "-"));
        cur.year = atoi(strtok(NULL, "-"));
        if (cur.day == day.day && cur.month == day.month && cur.year == day.year && !strcmp(uftype, "withdraw")) {
            sm += strtod(ufamount, NULL);
        }
    }
    fclose(accountFile);
    return sm;
}

DateDay get_today() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    DateDay ret;
    ret.day = tm_info->tm_mday;
    ret.month = tm_info->tm_mon + 1;
    ret.year = tm_info->tm_year + 1900;
    return ret;
}

Date get_month() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    Date ret;
    ret.month = tm_info->tm_mon + 1;
    ret.year = tm_info->tm_year + 1900;
    return ret;
}

void transaction_merge_sort(Transaction transactions[], int l, int r) {
    if (l < r) {
        int mid = l + (r - l) / 2, i;
        transaction_merge_sort(transactions, l, mid);
        transaction_merge_sort(transactions, mid + 1, r);
        Transaction la[mid - l + 1], ra[r - mid];
        for (i = 0; i < mid - l + 1; i++) {
            la[i] = transactions[l + i];
        }
        for (i = 0; i < r - mid; i++) {
            ra[i] = transactions[mid + 1 + i];
        }
        int p1 = 0, p2 = 0, idx = l;
        while (p1 < mid - l + 1 && p2 < r - mid) {
            if (cmp_transactions(la[p1], ra[p2]) >= 0) {
                transactions[idx++] = la[p1++];
            } else {
                transactions[idx++] = ra[p2++];
            }
        }
        while (p1 < mid - l + 1) {
            transactions[idx++] = la[p1++];
        }
        while (p2 < r - mid) {
            transactions[idx++] = ra[p2++];
        }
    }
}

int month_diff(Date a, Date b) {
    int sm = 0;
    if (a.month >= b.month) {
        sm += a.month - b.month;
        sm += 12 * (a.year - b.year);
    } else {
        sm += b.month - a.month;
        sm += 12 * (a.year - b.year - 1);
    }
    return sm;
}

void debug(char *str) {
    FILE *f = fopen("files/debug.txt", "a");
    fprintf(f, "%s\n", str);
    fclose(f);
    return;
}

int valid_email(char *str){
    char s[N];
    strcpy(s,str);
    int at=0,i;
    if(s[0]=='@'){
        return 0;
    }
    for(i=0;s[i];i++){
        at+=(s[i]=='@');
        if(i){
            if(s[i]==s[i-1]&&s[i]=='.'){
                return 0;
            }
        }
    }
    if(at!=1){
        return 0;
    }
    char *a=strtok(s,"@"),*b=strtok(NULL,"@");
    for(i=0;b[i];i++){
        if(i>2){
            if(b[i]=='.'&&(b[i-1]=='.'||b[i-2]=='.'||b[i-3]=='.')){
                return 0;
            }
        }
    }
    if(strlen(a)==0||strlen(b)==0){
        return 0;
    }
    if(a[0]=='.'||a[strlen(a)-1]=='.'||b[0]=='.'||b[strlen(b)-1]=='.'){
        return 0;
    }
    return 1;
}

void save_transaction(char *id,double amount,TransactionType type,char *to){
    char fileName[N];
    strcpy(fileName, "files/accounts/");
    strcat(fileName,id);
    strcat(fileName,".txt");
    FILE *accountFile = fopen(fileName, "r");
    if (accountFile == NULL) {
        FILE *createAccountFile = fopen(fileName, "w");
        fclose(createAccountFile);
        accountFile = fopen(fileName, "r");
    }
    fclose(accountFile);
    accountFile = fopen(fileName, "a");
    if(type==WITHDRAW||type==DEPOSIT){
        strcpy(to,"");
    }
    fprintf(accountFile, "%s,%s,%.2f,%d-%d-%d%s%s\n", id, (type==WITHDRAW?"withdraw":(type==DEPOSIT?"deposit":"send")), amount, get_today().day, get_today().month, get_today().year,(type==TRANSFER?",":""),to);
    fclose(accountFile);
    if(type==TRANSFER){
        strcpy(fileName, "files/accounts/");
        strcat(fileName,to);
        strcat(fileName,".txt");
        accountFile = fopen(fileName, "r");
        if (accountFile == NULL) {
            FILE *createAccountFile = fopen(fileName, "w");
            fclose(createAccountFile);
            accountFile = fopen(fileName, "r");
        }
        fclose(accountFile);
        accountFile = fopen(fileName, "a");
        fprintf(accountFile, "%s,receive,%.2f,%d-%d-%d,%s\n", to, amount, get_today().day, get_today().month, get_today().year,id);
        fclose(accountFile);
    }
}
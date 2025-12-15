// helper.h
#ifndef HELPER_H
#define HELPER_H
#define N 200

typedef enum{
    SUCCESS,
    ERROR
} Status_code;

typedef enum{
    ID,
    NAME,
    BALANCE,
    DATE
} Sort_method;

typedef struct{
    Status_code status;
    char message[N];
} Status;

typedef struct{
    int month,year;
} Date;

typedef struct{
    int status;//inactive 0, active 1
    char id[20],name[N],mobile[15],email[N];
    double balance;
    Date date;
    
} Account;

//result of query, advanced search, print (account packaging)
typedef struct{
    Status status;
    int n;//number of accounts in package
    Account accounts[N];
} Account_result;

int cmp_accounts(Account a,Account b,Sort_method method);
void account_merge_sort(Account accounts[],int l,int r,Sort_method method);
#endif
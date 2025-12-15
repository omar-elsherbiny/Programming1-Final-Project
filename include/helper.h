// helper.h
#ifndef HELPER_H
#define HELPER_H
#define N 200

typedef enum{
    SUCCESS,
    ERROR
} StatusCode;

typedef enum{
    ID,
    NAME,
    BALANCE,
    DATE,
    STATUS
} SortMethod;

typedef struct{
    StatusCode status;
    char message[N];
} Status;

typedef struct{
    int month,year;
} Date;

typedef struct{
    int day,month,year;
} DateDay;
typedef struct{
    int status;//inactive 0, active 1
    char id[20],name[N],mobile[15],email[N];
    double balance;
    Date date;
    Date inactivity;
} Account;

//result of query, advanced search, print (account packaging)
typedef struct{
    Status status;
    int n;//number of accounts in package
    Account accounts[N];
} AccountResult;

typedef struct{
    char accountId[20],partyId[20],type[N];
    double amount;
    DateDay date;
} Transaction;

typedef struct{
    Status status;
    int n;//number of transactions in package
    Transaction transactions[N];
} ReportResult;

int cmp_accounts(Account a,Account b,SortMethod method);
void account_merge_sort(Account accounts[],int l,int r,SortMethod method);
double day_withdrawals(DateDay day,char *id);
DateDay get_today();
Date get_month();
int cmp_transactions(Transaction a,Transaction b);
void transaction_merge_sort(Transaction transactions[],int l,int r);
#endif
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
    int id,status;
    char name[N],mobile[15],email[N];
    double balance;
    Date date;
    
} Account;

//result of query, advanced search, print (account packaging)
typedef struct{
    Status status;
    int n;
    Account accounts[N];
} Account_result;
#endif
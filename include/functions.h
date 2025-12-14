// functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#define N 200

typedef enum{
    SUCCESS,
    ERROR
} status_code;

typedef struct{
    status_code status;
    char message[N];
} status;

typedef struct{
    int month,year;
} date;

typedef struct{
    int id,status;
    char name[N],mobile[15],email[N];
    double balance;
    date date_opened;
    
} account;
#endif
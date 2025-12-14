#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#define N 200

typedef struct{
    int status;
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
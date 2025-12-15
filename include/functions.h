// functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "helper.h"
Status login(char *username,char* password);
Status load();
AccountResult query(char *id);
AccountResult advanced_search(char *id);
Status add(Account acc);
Status delete(char *id);
Status modify(char *id,char *name,char *mobile,char *email);
Status change_status(char *id);
void save();
Status withdraw(char *id,double amount);
Status deposit(char *id,double amount);
Status transfer(char *idFrom,char *idTo,double amount);
#endif
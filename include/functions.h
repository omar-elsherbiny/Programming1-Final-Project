// functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "helper.h"
Status login(char *username,char* password);
Status load();
Account_result query(char *id);
Account_result advanced_search(char *id);
Status add(Account acc);
#endif
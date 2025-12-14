#include "functions.h"
#include <stdio.h>
#include <string.h>

status login(char *username,char* password){
    FILE *f=fopen("files/users.txt","r");
    status ret;
    if(f == NULL){
        ret.status=-1;
        strcpy(ret.message,"File users.txt not found!");
        return ret;
    }
    char user[N],pass[N];
    while(fscanf(f,"%s%s",user,pass)){
        if(!strcmp(user,username) && !strcmp(pass,password)){
            fclose(f);
            ret.status=0;
            strcpy(ret.message,"Login successful!");
            return ret;
        }
    }
    ret.status=1;
    strcpy(ret.message,"Username or password are incorrect!");
    return ret;
}
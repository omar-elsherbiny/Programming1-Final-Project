// functions.c
#include "functions.h"
#include "helper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Status login(char *username,char* password){
    FILE *f=fopen("files/users.txt","r");
    Status ret;
    if(f == NULL){
        ret.status=ERROR;
        strcpy(ret.message,"File users.txt not found!");
        return ret;
    }
    char user[N],pass[N];
    while(fscanf(f,"%s%s",user,pass)){
        if(!strcmp(user,username) && !strcmp(pass,password)){
            fclose(f);
            ret.status=SUCCESS;
            strcpy(ret.message,"Login successful!");
            return ret;
        }
    }
    ret.status=ERROR;
    strcpy(ret.message,"Username or password are incorrect!");
    return ret;
}

Account accounts[N];

Status load(){
    FILE *f=fopen("files/accounts.txt","r");
    Status ret;
    if(f == NULL){
        ret.status=ERROR;
        strcpy(ret.message,"File accounts.txt not found!");
        return ret;
    }
    char line[7*N],ufid[N],ufname[N],ufemail[N],ufbalance[N],ufmobile[N],ufdate[N],ufstatus[N];
    int i;
    for(i=0;fgets(line,sizeof(line),f);i++){//copy file data into array
        strcpy(ufid,strtok(line,","));
        strcpy(ufname,strtok(NULL,","));
        strcpy(ufemail,strtok(NULL,","));
        strcpy(ufbalance,strtok(NULL,","));
        strcpy(ufmobile,strtok(NULL,","));
        strcpy(ufdate,strtok(NULL,","));
        strcpy(ufstatus,strtok(NULL,","));
        strcpy(accounts[i].name,ufid);
        strcpy(accounts[i].name,ufname);
        strcpy(accounts[i].mobile,ufmobile);
        strcpy(accounts[i].email,ufemail);
        accounts[i].balance=strtod(ufbalance,NULL);
        accounts[i].status=(strcmp(strtok(ufstatus," "),"inactive")==0?0:1);
        accounts[i].date.month=atoi(strtok(ufdate,"-"));
        accounts[i].date.year=atoi(strtok(NULL,"-"));
    }
    ret.status=SUCCESS;
    strcpy(ret.message,"Accounts loaded successfully!");
    fclose(f);
    return ret;
}
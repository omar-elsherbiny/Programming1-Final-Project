// functions.c
#include "functions.h"
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
    fclose(f);
    return ret;
}

Account accounts[N];
int account_cnt;

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
    account_cnt=0;
    for(i=0;fgets(line,sizeof(line),f);i++){//copy file data into array
        //splitting the line
        strcpy(ufid,strtok(line,","));
        strcpy(ufname,strtok(NULL,","));
        strcpy(ufemail,strtok(NULL,","));
        strcpy(ufbalance,strtok(NULL,","));
        strcpy(ufmobile,strtok(NULL,","));
        strcpy(ufdate,strtok(NULL,","));
        strcpy(ufstatus,strtok(NULL,","));
        //copying data into array
        strcpy(accounts[i].name,ufid);
        strcpy(accounts[i].name,ufname);
        strcpy(accounts[i].mobile,ufmobile);
        strcpy(accounts[i].email,ufemail);
        accounts[i].balance=strtod(ufbalance,NULL);
        accounts[i].status=(strcmp(strtok(ufstatus," "),"inactive")==0?0:1);
        accounts[i].date.month=atoi(strtok(ufdate,"-"));
        accounts[i].date.year=atoi(strtok(NULL,"-"));
        account_cnt++;
    }
    ret.status=SUCCESS;
    strcpy(ret.message,"Accounts loaded successfully!");
    fclose(f);
    return ret;
}

Account_result query(char *id){
    Account_result ret;
    account_merge_sort(accounts,0,account_cnt,ID);
    int s=0,e=account_cnt,mid;
    // binary search for account
    while(s<e){
        mid=(s+e)/2;
        if(strcmp(accounts[mid].id,id)==0){
            ret.status.status=SUCCESS;
            strcpy(ret.status.message,"Account found successfully!");
            ret.n=1;
            ret.accounts[0]=accounts[mid];
            return ret;
        }
        else if(strcmp(accounts[mid].id,id)<0){
            s=mid+1;
        }
        else{
            e=mid-1;
        }
    }
    ret.status.status=ERROR;
    strcpy(ret.status.message,"Account not found!");
    ret.n=0;
    return ret;
}

Account_result advanced_search(char *keyword){
    Account_result ret;
    ret.n=0;
    int i;
    for(i=0;i<account_cnt;i++){
        if(strstr(accounts[i].name,keyword)!=NULL){
            ret.status.status=SUCCESS;
            strcpy(ret.status.message,"Account(s) with keyword found successfully!");
            ret.accounts[ret.n]=accounts[i];
            ret.n++;
        }
    }
    ret.status.status=ERROR;
    strcpy(ret.status.message,"No account with keyword found!");
    return ret;
}
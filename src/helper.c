// helper.c
#include "helper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int cmp_accounts(Account a,Account b,SortMethod method){
    if(method == ID){
        return strcmp(a.id,b.id);
    }
    if(method == NAME){
        return strcmp(a.name,b.name);
    }
    if(method == BALANCE){
        return (a.balance<b.balance?-1:(a.balance==b.balance?0:1));
    }
    if(method == DATE){
        if(a.date.year<b.date.year){
            return -1;
        }
        else if(a.date.year>b.date.year){
            return 1;
        }
        else{
            if(a.date.month<b.date.month){
                return -1;
            }
            else if(a.date.month>b.date.month){
                return 1;
            }
            else{
                return 0;
            }
        }
    }
}

void account_merge_sort(Account accounts[],int l,int r,SortMethod method){
    if(l<r){
        int mid=l+(r-l)/2,i;
        account_merge_sort(accounts,l,mid,method);
        account_merge_sort(accounts,mid+1,r,method);
        Account la[mid-l+1],ra[r-mid];
        for(i=0;i<mid-l+1;i++){
            la[i]=accounts[l+i];
        }
        for(i=0;i<r-mid;i++){
            ra[i]=accounts[mid+1+i];
        }
        int p1=0,p2=0,idx=0;
        while(p1<mid-l+1&&p2<r-mid){
            if(cmp_accounts(la[p1],ra[p1],method)){
                accounts[idx++]=la[p1++];
            }
            else{
                accounts[idx++]=ra[p2++];
            }
        }
        while(p1<mid-l+1){
            accounts[idx++]=la[p1++];
        }
        while(p2<r-mid){
            accounts[idx++]=ra[p2++];
        }
    }
}

double day_withdrawals(DateDay day,char *id){
    FILE *accountFile=fopen(strcat(id,".txt"),"r");
    if(accountFile==NULL){
        FILE *createAccountFile=fopen(strcat(id,".txt"),"w");
        fclose(createAccountFile);
        accountFile=fopen(strcat(id,".txt"),"r");
    }
    char line[4*N],ufid[N],uftype[N],ufamount[N],ufdaydate[N];
    int i;
    double sm=0;
    for(i=0;fgets(line,sizeof(line),accountFile);i++){
        strcpy(ufid,strtok(line,","));
        strcpy(uftype,strtok(NULL,","));
        strcpy(ufamount,strtok(NULL,","));
        strcpy(ufdaydate,strtok(NULL,","));
        DateDay cur;
        cur.day=atoi(strtok(ufdaydate,"-"));
        cur.month=atoi(strtok(NULL,"-"));
        cur.year=atoi(strtok(NULL,"-"));
        if(cur.day==day.day&&cur.month==day.month&&cur.year==day.year&&!strcmp(uftype,"withdraw")){
            sm+=strtod(ufamount,NULL);
        }
    }
    fclose(accountFile);
    return sm;
}

DateDay get_today(){
    time_t t = time(NULL);
    struct tm *tm_info=localtime(&t);
    DateDay ret;
    ret.day=tm_info->tm_mday;
    ret.month=tm_info->tm_mon+1;
    ret.year=tm_info->tm_year+1900;
    return ret;
}
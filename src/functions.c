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
int accountCnt;

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
    accountCnt=0;
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
        accounts[i].inactivity.month=atoi(strtok(ufdate,"-"));
        accounts[i].inactivity.year=atoi(strtok(NULL,"-"));
        //check if account transaction file exists, if not creates it
        FILE *accountFile=fopen(strcat(accounts[i].id,".txt"),"r");
        if(accountFile==NULL){
            FILE *createAccountFile=fopen(strcat(accounts[i].id,".txt"),"w");
            fclose(createAccountFile);
        }
        fclose(accountFile);
        accountCnt++;
    }
    ret.status=SUCCESS;
    strcpy(ret.message,"Accounts loaded successfully!");
    fclose(f);
    return ret;
}

AccountResult query(char *id){
    AccountResult ret;
    account_merge_sort(accounts,0,accountCnt,ID);
    int s=0,e=accountCnt,mid;
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

AccountResult advanced_search(char *keyword){
    AccountResult ret;
    ret.n=0;
    int i;
    for(i=0;i<accountCnt;i++){
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

Status add(Account acc){
    int i;
    Status ret;
    for(i=0;i<accountCnt;i++){
        if(!strcmp(acc.id,accounts[i].id)){
            ret.status=ERROR;
            strcpy(ret.message,"Account number is not unique!");
            return ret;
        }
    }
    FILE *f=fopen("files/accounts.txt","r");
    if(f == NULL){
        ret.status=ERROR;
        strcpy(ret.message,"File accounts.txt not found!");
        return ret;
    }
    fclose(f);
    f=fopen("files/accounts.txt","a");
    accounts[accountCnt]=acc;
    fprintf(f,"%s,%s,%s,%.2f,%s,%d-%d, %s\n",acc.id,acc.name,acc.email,acc.balance,acc.mobile,acc.date.month,acc.date.year,(acc.status?"active":"inactive"));
    fclose(f);
    accountCnt++;
    ret.status=SUCCESS;
    strcpy(ret.message,"Account added successfully!");
    return ret;
}

Status delete(char *id){
    int i,found=0;
    Status ret;
    FILE *f=fopen("files/accounts.txt","r");
    if(f == NULL){
        ret.status=ERROR;
        strcpy(ret.message,"File accounts.txt not found!");
        return ret;
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            found=1;
        }
    }
    if(!found){
        ret.status=ERROR;
        strcpy(ret.message,"Account not found!");
        return ret;
    }
    //account found and file exists
    found=0;
    for(i=0;i<accountCnt;i++){
        if(!found){
            if(!strcmp(id,accounts[i].id)){
                found=1;
            }
        }
        else{
            accounts[i-1]=accounts[i];
        }
    }
    accountCnt--;
    save();
    ret.status=SUCCESS;
    strcpy(ret.message,"Account deleted successfully!");
    return ret;
}

Status modify(char *id,char *name,char *mobile,char *email){
    int i,found=0;
    Status ret;
    FILE *f=fopen("files/accounts.txt","r");
    if(f == NULL){
        ret.status=ERROR;
        strcpy(ret.message,"File accounts.txt not found!");
        return ret;
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            found=1;
        }
    }
    if(!found){
        ret.status=ERROR;
        strcpy(ret.message,"Account not found!");
        return ret;
    }
    //account found and file exists
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            strcpy(accounts[i].name,name);
            strcpy(accounts[i].mobile,mobile);
            strcpy(accounts[i].email,email);
        }
    }
    save();
    ret.status=SUCCESS;
    strcpy(ret.message,"Account modified successfully!");
    return ret;
}

Status change_status(char *id){
    int i,found=0;
    Status ret;
    FILE *f=fopen("files/accounts.txt","r");
    if(f == NULL){
        ret.status=ERROR;
        strcpy(ret.message,"File accounts.txt not found!");
        return ret;
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            found=1;
        }
    }
    if(!found){
        ret.status=ERROR;
        strcpy(ret.message,"Account not found!");
        return ret;
    }
    //account found and file exists
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            accounts[i].status^=1; // toggles status
            accounts[i].inactivity=get_month();
        }
    }
    save();
    ret.status=SUCCESS;
    strcpy(ret.message,"Account status changed successfully!");
    return ret;
}

Status withdraw(char *id,double amount){
    int i,found=0;
    Status ret;
    FILE *f=fopen("files/accounts.txt","r");
    if(f == NULL){
        ret.status=ERROR;
        strcpy(ret.message,"File accounts.txt not found!");
        return ret;
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            found=1;
        }
    }
    if(!found){
        ret.status=ERROR;
        strcpy(ret.message,"Account not found!");
        return ret;
    }
    //account found and file exists
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            if(!accounts[i].status){
                ret.status=ERROR;
                strcpy(ret.message,"Account is inactive!");
                return ret;
            }
            if(amount>10000){
                ret.status=ERROR;
                strcpy(ret.message,"Withdrawal amount limit is $10,000!");
                return ret;
            }
            if(amount<=0){
                ret.status=ERROR;
                strcpy(ret.message,"Withdrawal amount must be positive!");
                return ret;
            }
            if(amount+day_withdrawals(get_today(),id)>50000){
                ret.status=ERROR;
                strcpy(ret.message,"Daily withdrawal limit is $50,000!");
                return ret;
            }
            if(accounts[i].balance<amount){
                ret.status=ERROR;
                strcpy(ret.message,"Not enough balance for withdrawal!");
                return ret;
            }
            FILE *accountFile=fopen(strcat(id,".txt"),"r");
            if(accountFile==NULL){
                FILE *createAccountFile=fopen(strcat(id,".txt"),"w");
                fclose(createAccountFile);
                accountFile=fopen(strcat(id,".txt"),"r");
            }
            fclose(accountFile);
            accountFile=fopen(strcat(id,".txt"),"a");
            fprintf(accountFile,"%s,withdraw,%.2f,%d-%d-%d\n",id,amount,get_today().day,get_today().month,get_today().year);
            fclose(accountFile);
            accounts[i].balance-=amount;
            save();
        }
    }
    ret.status=SUCCESS;
    strcpy(ret.message,"Withdrawal completed successfully!");
    return ret;
}

void save(){
    FILE *f=fopen("files/accounts.txt","w");
    int i;
    for(i=0;i<accountCnt;i++){
        fprintf(f,"%s,%s,%s,%.2f,%s,%d-%d, %s\n",accounts[i].id,accounts[i].name,accounts[i].email,accounts[i].balance,accounts[i].mobile,accounts[i].date.month,accounts[i].date.year,(accounts[i].status?"active":"inactive"));
    }
    fclose(f);
}
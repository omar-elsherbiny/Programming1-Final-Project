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
    while(fscanf(f,"%s%s",user,pass)==2){
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
        f=fopen("files/accounts.txt","w");
        fclose(f);
        f=fopen("files/accounts.txt","r");
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
        strcpy(accounts[i].id,ufid);
        strcpy(accounts[i].name,ufname);
        strcpy(accounts[i].mobile,ufmobile);
        strcpy(accounts[i].email,ufemail);
        char fileName[N];
        strcpy(fileName,"files/accounts/");
        strcat(fileName,ufid);
        strcat(fileName,".txt");
        accounts[i].balance=strtod(ufbalance,NULL);
        accounts[i].status=(strcmp(strtok(ufstatus," "),"inactive")==0?0:1);
        accounts[i].date.month=atoi(strtok(ufdate,"-"));
        accounts[i].date.year=atoi(strtok(NULL,"-"));
        //check if account transaction file exists, if not creates it
        FILE *accountFile=fopen(fileName,"r");
        if(accountFile==NULL){
            FILE *createAccountFile=fopen(fileName,"w");
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
    account_merge_sort(ret.accounts,0,ret.n,ID);
    if(!ret.n){
        ret.status.status=ERROR;
        strcpy(ret.status.message,"No account with keyword found!");
    }
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
        f=fopen("files/accounts.txt","w");
        fclose(f);
        f=fopen("files/accounts.txt","r");
    }
    fclose(f);
    f=fopen("files/accounts.txt","a+");
    if(fseek(f,-1,SEEK_END)==0&&fgetc(f)!='\n'){
        fseek(f,0,SEEK_END);
        fprintf(f,"\n");
    }
    acc.status=1;
    acc.date=get_month();
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
        f=fopen("files/accounts.txt","w");
        fclose(f);
        f=fopen("files/accounts.txt","r");
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
        f=fopen("files/accounts.txt","w");
        fclose(f);
        f=fopen("files/accounts.txt","r");
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            found=1;
            break;
        }
    }
    if(!found){
        ret.status=ERROR;
        strcpy(ret.message,"Account not found!");
        return ret;
    }
    //account found and file exists
    strcpy(accounts[i].name,name);
    strcpy(accounts[i].mobile,mobile);
    strcpy(accounts[i].email,email);
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
        f=fopen("files/accounts.txt","w");
        fclose(f);
        f=fopen("files/accounts.txt","r");
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            found=1;
            break;
        }
    }
    if(!found){
        ret.status=ERROR;
        strcpy(ret.message,"Account not found!");
        return ret;
    }
    //account found and file exists
    accounts[i].status^=1; // toggles status
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
        f=fopen("files/accounts.txt","w");
        fclose(f);
        f=fopen("files/accounts.txt","r");
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            found=1;
            break;
        }
    }
    if(!found){
        ret.status=ERROR;
        strcpy(ret.message,"Account not found!");
        return ret;
    }
    //account found and file exists
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

Status deposit(char *id,double amount){
    int i,found=0;
    Status ret;
    FILE *f=fopen("files/accounts.txt","r");
    if(f == NULL){
        f=fopen("files/accounts.txt","w");
        fclose(f);
        f=fopen("files/accounts.txt","r");
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            found=1;
            break;
        }
    }
    if(!found){
        ret.status=ERROR;
        strcpy(ret.message,"Account not found!");
        return ret;
    }
    //account found and file exists
    if(!accounts[i].status){
        ret.status=ERROR;
        strcpy(ret.message,"Account is inactive!");
        return ret;
    }
    if(amount>10000){
        ret.status=ERROR;
        strcpy(ret.message,"Deposit amount limit is $10,000!");
        return ret;
    }
    if(amount<=0){
        ret.status=ERROR;
        strcpy(ret.message,"Deposit amount must be positive!");
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
    fprintf(accountFile,"%s,deposit,%.2f,%d-%d-%d\n",id,amount,get_today().day,get_today().month,get_today().year);
    fclose(accountFile);
    accounts[i].balance+=amount;
    save();
    ret.status=SUCCESS;
    strcpy(ret.message,"Deposit completed successfully!");
    return ret;
}

Status transfer(char *idFrom,char *idTo,double amount){
    int i,foundFrom=0,foundTo=0,idxFrom,idxTo;
    Status ret;
    FILE *f=fopen("files/accounts.txt","r");
    if(f == NULL){
        f=fopen("files/accounts.txt","w");
        fclose(f);
        f=fopen("files/accounts.txt","r");
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(idFrom,accounts[i].id)){
            foundFrom=1;
            idxFrom=i;
        }
        if(!strcmp(idTo,accounts[i].id)){
            foundTo=1;
            idxTo=i;
        }
    }
    if(!foundFrom&&!foundTo){
        ret.status=ERROR;
        strcpy(ret.message,"Both accounts not found!");
        return ret;
    }
    if(!foundFrom){
        ret.status=ERROR;
        strcpy(ret.message,"Sender account not found!");
        return ret;
    }
    if(!foundTo){
        ret.status=ERROR;
        strcpy(ret.message,"Receiver account not found!");
        return ret;
    }
    //account found and file exists
    if(!accounts[idxFrom].status&&!accounts[idxTo].status){
        ret.status=ERROR;
        strcpy(ret.message,"Both accounts are inactive!");
        return ret;
    }
    if(!accounts[idxTo].status){
        ret.status=ERROR;
        strcpy(ret.message,"Receiver account is inactive!");
        return ret;
    }
    if(!accounts[idxFrom].status){
        ret.status=ERROR;
        strcpy(ret.message,"Sender account is inactive!");
        return ret;
    }
    if(amount<=0){
        ret.status=ERROR;
        strcpy(ret.message,"Transfer amount must be positive!");
        return ret;
    }
    if(amount>accounts[idxFrom].balance){
        ret.status=ERROR;
        strcpy(ret.message,"Transfer amount is greater than sender balance!");
        return ret;
    }
    FILE *accountFromFile=fopen(strcat(idFrom,".txt"),"r");
    FILE *accountToFile=fopen(strcat(idTo,".txt"),"r");
    if(accountFromFile==NULL){
        FILE *createAccountFile=fopen(strcat(idFrom,".txt"),"w");
        fclose(createAccountFile);
        accountFromFile=fopen(strcat(idFrom,".txt"),"r");
    }
    fclose(accountFromFile);
    if(accountToFile==NULL){
        FILE *createAccountFile=fopen(strcat(idTo,".txt"),"w");
        fclose(createAccountFile);
        accountToFile=fopen(strcat(idTo,".txt"),"r");
    }
    fclose(accountToFile);
    accountFromFile=fopen(strcat(idFrom,".txt"),"a");
    fprintf(accountFromFile,"%s,transfer,-%.2f,%d-%d-%d,%s\n",idFrom,amount,get_today().day,get_today().month,get_today().year,idTo);
    fclose(accountFromFile);
    accountToFile=fopen(strcat(idTo,".txt"),"a");
    fprintf(accountToFile,"%s,transfer,%.2f,%d-%d-%d,%s\n",idTo,amount,get_today().day,get_today().month,get_today().year,idFrom);
    fclose(accountToFile);
    accounts[idxTo].balance+=amount;
    accounts[idxFrom].balance-=amount;
    save();
    ret.status=SUCCESS;
    strcpy(ret.message,"Transfer completed successfully!");
    return ret;
}

ReportResult report(char *id){
    ReportResult ret;
    ret.n=0;
    int i,found=0;
    FILE *f=fopen("files/accounts.txt","r");
    if(f == NULL){
        ret.status.status=ERROR;
        strcpy(ret.status.message,"File accounts.txt not found!");
        return ret;
    }
    fclose(f);
    for(i=0;i<accountCnt;i++){
        if(!strcmp(id,accounts[i].id)){
            found=1;
            break;
        }
    }
    if(!found){
        ret.status.status=ERROR;
        strcpy(ret.status.message,"Account not found!");
        return ret;
    }
    FILE *accountFile=fopen(strcat(id,".txt"),"r");
    if(accountFile==NULL){
        FILE *createAccountFile=fopen(strcat(id,".txt"),"w");
        fclose(createAccountFile);
        accountFile=fopen(strcat(id,".txt"),"r");
    }
    char line[5*N],ufaccountId[N],ufpartyId[N],uftype[N],ufamount[N],ufdate[N];
    for(i=0;fgets(line,sizeof(line),accountFile);i++){
        strcpy(ufaccountId,strtok(line,","));
        strcpy(ufpartyId,strtok(NULL,","));
        strcpy(uftype,strtok(NULL,","));
        strcpy(ufamount,strtok(NULL,","));
        strcpy(ufdate,strtok(NULL,","));
        Transaction trans;
        strcpy(trans.accountId,ufaccountId);
        strcpy(trans.partyId,ufpartyId);
        strcpy(trans.type,uftype);
        trans.amount=strtod(ufamount,NULL);
        trans.date.day=atoi(strtok(ufdate,"-"));
        trans.date.month=atoi(strtok(NULL,"-"));
        trans.date.year=atoi(strtok(NULL,"-"));
        ret.transactions[ret.n]=trans;
        ret.n++;
        if(ret.n==5){
            break;
        }
    }
    fclose(accountFile);
    if(!ret.n){
        ret.status.status=ERROR;
        strcpy(ret.status.message,"No transactions by account found!");
    }
    else{
        ret.status.status=SUCCESS;
        strcpy(ret.status.message,"All recent transactions (up to 5) obtained!");
    }
    return ret;
}

AccountResult print(SortMethod method){
    AccountResult ret;
    account_merge_sort(accounts,0,accountCnt,method);
    int i;
    ret.n=accountCnt;
    if(!accountCnt){
        ret.status.status=ERROR;
        strcpy(ret.status.message,"No accounts found!");
        return ret;
    }
    for(i=0;i<accountCnt;i++){
        ret.accounts[i]=accounts[i];
    }
    ret.status.status=SUCCESS;
    strcpy(ret.status.message,"Obtained all sorted accounts successfully!");
    return ret;
}

Status delete_multiple(DeleteMethod method,Date date){
    int i,found=0;
    Status ret;
    FILE *f=fopen("files/accounts.txt","r");
    if(f == NULL){
        f=fopen("files/accounts.txt","w");
        fclose(f);
        f=fopen("files/accounts.txt","r");
    }
    fclose(f);
    Account temp[accountCnt];
    int idx=0;
    if(method==MONTH){
        for(i=0;i<accountCnt;i++){
            if(accounts[i].date.month==date.month&&accounts[i].date.year==date.year){
                found++;
            }
            else{
                temp[idx]=accounts[i];
                idx++;
            }
        }
        if(!found){
            ret.status=ERROR;
            strcpy(ret.message,"No accounts created on that date!");
            return ret;
        }
        accountCnt=idx;
        for(i=0;i<accountCnt;i++){
            accounts[i]=temp[i];
        }
        save();
        ret.status=SUCCESS;
        char buf[20];
        snprintf(buf,sizeof(buf),"%d",found);
        strcpy(ret.message,strcat(strcat("Successfully deleted ",buf)," account(s)!"));
        return ret;
    }
    else{
        for(i=0;i<accountCnt;i++){
            if(month_diff(get_month(),accounts[i].date)>3&&accounts[i].balance==0){
                found++;
            }
            else{
                temp[idx]=accounts[i];
                idx++;
            }
        }
        if(!found){
            ret.status=ERROR;
            strcpy(ret.message,"No accounts with 0 balance were inactive for over 3 months!");
            return ret;
        }
        accountCnt=idx;
        for(i=0;i<accountCnt;i++){
            accounts[i]=temp[i];
        }
        save();
        ret.status=SUCCESS;
        char buf[20];
        snprintf(buf,sizeof(buf),"%d",found);
        strcpy(ret.message,strcat(strcat("Successfully deleted ",buf)," account(s)!"));
        return ret;
    }
}
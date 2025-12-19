// menus.c
#include "menus.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"
#include "functions.h"

typedef enum {
    RETURN = -1,
    ENTRY = 0,
    LOGIN,
    QUIT,
    COMMANDS,
    ACC_NEW,
    ACC_DELETE,
    ACC_MODIFY,
    ACC_SEARCH,
    ACC_ADVANCESEARCH,
    ACC_STATUS,
    TRANS_WITHDRAW,
    TRANS_DEPOSIT,
    TRANS_TRANSFER,
    OTHER_REPORT,
    OTHER_PRINT,
} MenuIndex;

static MenuIndex (*menuFunctions[100])();
static MenuIndex prevIndex;
static MenuIndex currentIndex = ENTRY;
static MenuIndex tempIndex;

// -

static void free_result(PromptInputs results) {
    if (results.textInputs==NULL) return;
    
    for (int i = 0; i < results.textInputCount; i++) {
        free(results.textInputs[i]);
        results.textInputs[i]=NULL;
    }

    free(results.textInputs);
    results.textInputs=NULL;
}

static void remove_all_chars(char *str, char c) {
    char *pRead = str, *pWrite = str;
    while (*pRead) {
        *pWrite = *pRead++;
        pWrite += (*pWrite != c);
    }
    *pWrite = '\0';
}

static void print_status(Status status) {
    int lineCount;
    Line *statusMsgLines = MULTI_LINE_DEFAULT(status.message, (status.status == ERROR ? FG_RED : (status.status == WARNING ? FG_YELLOW : FG_GREEN)), 30, &lineCount);

    BoxContent statusPage = {0};
    strcpy(statusPage.title, (status.status == ERROR ? FG_RED "ERROR" : (status.status == ERROR ? FG_YELLOW "WARNING" : FG_GREEN "SUCCESS")));

    for (int i = 0; i < lineCount; i++) {
        statusPage.content[i] = statusMsgLines[i];
    }
    statusPage.content[lineCount] = LINE_DEFAULT(" ");
    statusPage.content[lineCount + 1] = LINE_DIALOGUE("Okay", 0);
    free(statusMsgLines);

    display_box_prompt(&statusPage, 0);
}

static int print_confirm(char title[], char message[]) {
    int lineCount;
    Line *messageLines = MULTI_LINE_DEFAULT(message, "", 30, &lineCount);

    BoxContent confirmPage = {0};
    strcpy(confirmPage.title, title);

    for (int i = 0; i < lineCount; i++) {
        confirmPage.content[i] = messageLines[i];
    }

    confirmPage.content[lineCount] = LINE_DEFAULT(" ");
    confirmPage.content[lineCount + 1] = LINE_DIALOGUE("Yes", 1);
    confirmPage.content[lineCount + 2] = LINE_DIALOGUE("No", 0);
    free(messageLines);

    return display_box_prompt(&confirmPage, 1).dialogueValue;
}

// -

// Menu functions definitions
static MenuIndex entry_func() { return LOGIN; }

static MenuIndex login_func() {
    enum DialogOptions { DIALOG_LOGIN,
                         DIALOG_QUIT,
                         DIALOG_PROCEED };

    BoxContent loginPage = {
        .title = "Login",
        .content = {LINE_DEFAULT("┌ Username ──────────────────┐"),
                    LINE_TEXT("│ %s │", 25, 0, "", ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ Password ──────────────────┐"),
                    LINE_TEXT("│ %s │", 50, 1, "", ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "), LINE_DIALOGUE("Login", DIALOG_LOGIN),
                    LINE_DIALOGUE(FG_RED "Quit", DIALOG_QUIT)}};

    PromptInputs results = display_box_prompt(&loginPage, 0);

    if (results.dialogueValue == DIALOG_QUIT) {
        free_result(results);
        return QUIT;
    }

    Status status = login(results.textInputs[0], results.textInputs[1]);

    // Checking if login fails
    if (status.status == ERROR) {
        print_status(status);

        free_result(results);
        return LOGIN;
    }

    status = load();
    // Checking if Loading accounts fails
    if (status.status == ERROR) {
        print_status(status);

        free_result(results);
        return LOGIN;
    }

    free_result(results);
    return COMMANDS;
}

static MenuIndex quit_func() {
    enum DialogOptions { DIALOG_YES,
                         DIALOG_NO };

    BoxContent quitPage = {
        .title = "Quit",
        .content = {LINE_DEFAULT("Are you sure you want to quit?"),
                    LINE_DEFAULT(" "), LINE_DIALOGUE(FG_RED "Yes", DIALOG_YES),
                    LINE_DIALOGUE("No", DIALOG_NO)}};

    PromptInputs results = display_box_prompt(&quitPage, 1);

    if (results.dialogueValue == DIALOG_YES) {
        return RETURN;
    }

    return LOGIN;
}

static MenuIndex commands_func() {
    BoxContent commandsPage = {
        .title = "Commands",
        .content = {
            LINE_DEFAULT("Manage Accounts:              "),
            LINE_DIALOGUE("  %sAdd a new Account%s", ACC_NEW),
            LINE_DIALOGUE("  %sDelete an Existing Account%s",
                          ACC_DELETE),
            LINE_DIALOGUE("  %sModify an Existing Account%s",
                          ACC_MODIFY),
            LINE_DIALOGUE("  %sSearch an Account%s", ACC_SEARCH),
            LINE_DIALOGUE("  %sAdvanced Searching%s", ACC_ADVANCESEARCH),
            LINE_DIALOGUE("  %sChange an Account Status%s", ACC_STATUS),
            LINE_DEFAULT(" "), LINE_DEFAULT("Transactions:"),
            LINE_DIALOGUE("  %sWithdraw from an Account%s",
                          TRANS_WITHDRAW),
            LINE_DIALOGUE("  %sDeposit to an Account%s", TRANS_DEPOSIT),
            LINE_DIALOGUE("  %sTransfer to an Account%s",
                          TRANS_TRANSFER),
            LINE_DEFAULT(" "), LINE_DEFAULT("Others:"),
            LINE_DIALOGUE("  %sReport last transactions%s",
                          OTHER_REPORT),
            LINE_DIALOGUE("  %sPrint all Accounts%s", OTHER_PRINT),
            LINE_DEFAULT(" "), LINE_DIALOGUE(FG_RED "Logout", LOGIN)}};

    int dialogueIndex = (int)prevIndex - (int)(ACC_NEW);  // depends on menu order in MenuIndex enum;
    dialogueIndex = (dialogueIndex < 0 ? 0 : dialogueIndex);
    PromptInputs results = display_box_prompt(&commandsPage, dialogueIndex);

    return results.dialogueValue;
}

static MenuIndex acc_new_func() {
    enum DialogOptions {
        DIALOG_ADD,
        DIALOG_DISCARD,
        DIALOG_YES,
        DIALOG_NO,
        DIALOG_PROCEED
    };

    Account account = {0};

    while (1) {
        // make a temp string to fill the balance
        char temp[LINE_LENGTH];
        sprintf(temp, "%.2f", account.balance);

        BoxContent addAccountPage = {
            .title = "Add Account",
            .content = {LINE_DEFAULT("┌ Account Number ────────────┐"),
                        LINE_TEXT("│ %s │", 10, 0, "0123456789\b", account.id),
                        LINE_DEFAULT("└────────────────────────────┘"),
                        LINE_DEFAULT("┌ Name ──────────────────────┐"),
                        LINE_TEXT("│ %s │", 25, 0, "", account.name),
                        LINE_DEFAULT("└────────────────────────────┘"),
                        LINE_DEFAULT("┌ E-mail ────────────────────┐"),
                        LINE_TEXT("│ %s │", 25, 0, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!#$%&'*+-/=?^_{|}~`@.\b ", account.email),
                        LINE_DEFAULT("└────────────────────────────┘"),
                        LINE_DEFAULT("┌ Balance ───────────────────┐"),
                        LINE_TEXT("│ %s ($) │", 20, 0, ",.0123456789\b", temp),
                        LINE_DEFAULT("└────────────────────────────┘"),
                        LINE_DEFAULT("┌ Mobile ────────────────────┐"),
                        LINE_TEXT("│ + 20 %s │", 10, 0, "0123456789\b", account.mobile),
                        LINE_DEFAULT("└────────────────────────────┘"),
                        LINE_DEFAULT(" "), LINE_DIALOGUE("Add", DIALOG_ADD),
                        LINE_DIALOGUE("Discard", DIALOG_DISCARD)}};

        PromptInputs results = display_box_prompt(&addAccountPage, 0);
    
        if (results.dialogueValue == DIALOG_DISCARD) {
            free_result(results);
            return COMMANDS;
        }
    
        strcpy(account.id, results.textInputs[0]);
        strcpy(account.name, results.textInputs[1]);
        strcpy(account.email, results.textInputs[2]);
        account.balance = strtod(results.textInputs[3], NULL);
        sprintf(account.mobile, "%s", results.textInputs[4]);
    
        // removing all commas from balance input field
        remove_all_chars(results.textInputs[3], ',');

        // Check if any field is empty
        if ((strlen(results.textInputs[0]) == 0) ||
            (strlen(results.textInputs[1]) == 0) ||
            (strlen(results.textInputs[2]) == 0) ||
            (strlen(results.textInputs[3]) == 0) ||
            (strlen(results.textInputs[4]) == 0)) {
                Status status = {
                    .status = ERROR,
                    .message = "You should fill out all the input fields"
                };
                print_status(status);

                free_result(results);
                continue;
        }

        // Acc number number 10 characters
        if (strlen(results.textInputs[0]) != 10) {
            Status status = {
                .status = ERROR,
                .message = "Account Number is not 10 characters"
            };
            print_status(status);

            free_result(results);
            continue;
        }

        // email validation
        if (!valid_email(results.textInputs[2])) {
            Status status = {
                .status = ERROR,
                .message = "Email is not valid"
            };
            print_status(status);

            free_result(results);
            continue;
        }

        // Check if 1st char in balance is a number
        if (!(results.textInputs[3][0] >= '0' &&
                results.textInputs[3][0] <= '9')) {
                Status status = {
                    .status = ERROR,
                    .message = "Balance should start with a number"
                };
                print_status(status);

                free_result(results);
                continue;
        }

        // Check if last char in balance is a number
        if (results.textInputs[3][strlen(results.textInputs[3]) - 1] == '.') {
            Status status = {
                .status = ERROR,
                .message = "Balance should end with a number"
            };
            print_status(status);

            free_result(results);
            continue;
        }

        // Check if there are only 1 decimal point
        const char *token = results.textInputs[3];
        int decimalCount = 0;
        while ((token = strstr(token, ".")) != NULL)
            decimalCount++, token++;
        if (decimalCount > 1) {
            Status status = {
                .status = ERROR,
                .message = "Balance should only have 1 decimal point"
            };
            print_status(status);

            free_result(results);
            continue;
        }

        // Check if balance has only 2 decimal places
        strcpy(temp, results.textInputs[3]);
        token = strtok(temp, ".");
        token = strtok(NULL, ".");
        if (token != NULL) {
            if (strlen(token) > 2) {
                Status status = {
                    .status = ERROR,
                    .message = "The balance should only be max of 2 decimal places"
                };
                print_status(status);

                free_result(results);
                continue;
            }
        }

        // phone number must be 10 characters (excluding the "+ 20")
        if (strlen(results.textInputs[4]) != 10) {
            Status status = {
                .status = ERROR,
                .message = "Phone Number is not valid"
            };
            print_status(status);

            free_result(results);
            continue;
        }

        // filling up the a dummy account 
        strcpy(account.id, results.textInputs[0]);
        strcpy(account.name, results.textInputs[1]);
        strcpy(account.email, results.textInputs[2]);
        account.balance = strtod(results.textInputs[3], NULL);
        sprintf(account.mobile, "0%s", results.textInputs[4]);
        
        // Sending the data to add() to check if possible
        Status status = add(account);
        if (status.status == ERROR) {
            sprintf(account.mobile, "%s", results.textInputs[4]);
            print_status(status);
            load();
            
            free_result(results);
            continue;
        }

        // Data is finally valid here

        // Lastly checking for confirmation
        int confirmResults = print_confirm("Confirm Add", "Are you sure you want to add this account?");

        if (confirmResults == 0) {
            sprintf(account.mobile, "%s", results.textInputs[4]);
            free_result(results);
            load();
            
            continue;
        }

        save();
        
        print_status(status);

        free_result(results);
        return COMMANDS;
    }

}

static MenuIndex acc_delete_func() {
    enum DialogOptions {
        DIALOG_DEL_ONE,
        DIALOG_DEL_MULTI,
        DIALOG_DISCARD,
        DIALOG_YES,
        DIALOG_NO,
        DIALOG_LESSTHAN3MONTH,
        DIALOG_DELETEGIVENDATE
    };

    BoxContent deletePage = {
        .title = FG_RED "Delete Account",
        .content = {LINE_DEFAULT("┌ Delete Option ─────────────┐"),
                    LINE_DIALOGUE("│ %sOne with an Account number%s │", DIALOG_DEL_ONE),
                    LINE_DIALOGUE("│ %sMultiple with a criteria%s   │", DIALOG_DEL_MULTI),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Discard", DIALOG_DISCARD)}};

    PromptInputs results = display_box_prompt(&deletePage, 2);

    if (results.dialogueValue == DIALOG_DISCARD) {
        free_result(results);
        return COMMANDS;
    }

    if (results.dialogueValue == DIALOG_DEL_ONE) {
        BoxContent deleteOnePage = {
            .title = FG_RED "Delete Account",
            .content = {LINE_DEFAULT("┌ Account Number ────────────┐"),
                        LINE_TEXT("│ %s │", 10, 0, "1234567890\b", ""),
                        LINE_DEFAULT("└────────────────────────────┘"),
                        LINE_DEFAULT(" "),
                        LINE_DIALOGUE(FG_RED "Delete", DIALOG_YES),
                        LINE_DIALOGUE("Discard", DIALOG_NO)}};

        PromptInputs delOneResults = display_box_prompt(&deleteOnePage, 2);

        if (delOneResults.dialogueValue == DIALOG_NO) {
            free_result(results);
            return ACC_DELETE;
        } else if (delOneResults.dialogueValue == DIALOG_YES) {
            char *toDel = delOneResults.textInputs[0];
            Status deletionStatus = delete(toDel);
            print_status(deletionStatus);
            free_result(results);
            return ACC_DELETE;
        }
    } else if (results.dialogueValue == DIALOG_DEL_MULTI) {
        // Status delete_multiple(DeleteMethod method,Date date);
        BoxContent deleteMultiPage = {
            .title = FG_RED "Delete Account",
            .content = {
                LINE_DEFAULT("┌ Multiple criteria ─────────┐"),
                LINE_DIALOGUE("│ %sAccounts created on a Date%s │", DIALOG_DELETEGIVENDATE),
                LINE_DIALOGUE("│ %sAccounts inactive 90 days%s  │", DIALOG_LESSTHAN3MONTH),
                LINE_DEFAULT("└────────────────────────────┘"),
                LINE_DEFAULT(" "),
                LINE_DIALOGUE("Discard", DIALOG_DISCARD)
            }};

        PromptInputs delMultiChoice = display_box_prompt(&deleteMultiPage, 2);

        if (delMultiChoice.dialogueValue == DIALOG_DISCARD) {
            free_result(results);
            return ACC_DELETE;
        }

        if (delMultiChoice.dialogueValue == DIALOG_DELETEGIVENDATE) {
            while (1) {
                BoxContent deleteGivenDate = {
                    .title =FG_RED  "Delete Account",
                    .content = {LINE_DEFAULT(FG_RED "Delete all accounts created on"),
                                LINE_DEFAULT(FG_RED "the given date below"),
                                LINE_DEFAULT("          ┌ Month ─┐          "),
                                LINE_TEXT("          │ %s │          ", 2, 0, "1234567890\b", ""),
                                LINE_DEFAULT("          └────────┘          "),
                                LINE_DEFAULT("          ┌ Year ──┐          "),
                                LINE_TEXT("          │ %s │          ", 4, 0, "1234567890\b", ""),
                                LINE_DEFAULT("          └────────┘          "),
                                LINE_DEFAULT(" "),
                                LINE_DIALOGUE(FG_RED "Delete", DIALOG_YES),
                                LINE_DIALOGUE("Discard", DIALOG_DISCARD)}};
                            
                PromptInputs delDateResults = display_box_prompt(&deleteGivenDate, 3);
                
                int month = atoi(delDateResults.textInputs[0]);
                int year = atoi(delDateResults.textInputs[1]);
                free_result(delDateResults);
                
                if (delDateResults.dialogueValue == DIALOG_DISCARD) {
                    free_result(results);
                    return ACC_DELETE;
                }

                if (month > 12) {
                    Status status = {
                        .status = ERROR,
                        .message = "Invalid month inputted. make sure that month is less 12."
                    };
                    print_status(status);
                    continue;
                }
                Date date = {
                    date.month = month,
                    date.year = year};
                Status status = delete_multiple(MONTH, date);

                print_status(status);

                if (status.status == SUCCESS) break;
            }
        } else if (delMultiChoice.dialogueValue == DIALOG_LESSTHAN3MONTH) {
            Date date;
            Status deleteStatus = delete_multiple(INACTIVITY, date);
            print_status(deleteStatus);
            return COMMANDS;
        }
    }

    free_result(results);
    return COMMANDS;
}

static MenuIndex acc_change_status() {
    enum DialogOptions {
        DIALOG_FIND,
        DIALOG_BACK,
        DIALOG_CHANGE,
        DIALOG_DISCARD,
        DIALOG_ACTIVE = 1,
        DIALOG_INACTIVE = 0,
        DIALOG_OKAY,
    };
    BoxContent statusPage = {
        .title = "Account Status",
        .content = {
            LINE_DEFAULT("┌ Account Number ────────────┐"),
            LINE_TEXT("│ %s │", 10, 0, "1234567890\b", ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Find", DIALOG_FIND),
            LINE_DIALOGUE("Back", DIALOG_BACK),
        }};
    PromptInputs statusResult = display_box_prompt(&statusPage, 0);
    if (statusResult.dialogueValue == DIALOG_BACK) {
        free_result(statusResult);
        return COMMANDS;
    } else if (statusResult.dialogueValue == DIALOG_FIND) {
        char *id = statusResult.textInputs[0];

        AccountResult accountResult = query(id);
        if (accountResult.status.status == SUCCESS) {
            enum DialogOptions selectedOption = accountResult.accounts[0].status;
            BoxContent changeStatusPage = {
                .title = "Account Status",
                .content = {
                    LINE_DEFAULT("Account is currently active   "),
                    LINE_DEFAULT(" "),
                    LINE_DEFAULT("┌ Change account status ─────┐"),
                    LINE_DIALOGUE(selectedOption ? "│ %s(x) Active%s                 │" : "│ %s( ) Active%s                 │", DIALOG_ACTIVE),
                    LINE_DIALOGUE(selectedOption ? "│ %s( ) Inactive%s               │" : "│ %s(x) Inactive%s               │", DIALOG_INACTIVE),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Change", DIALOG_CHANGE),
                    LINE_DIALOGUE("Discard", DIALOG_DISCARD),
                }};
            // printf("%s %d",accountResult.accounts[0].name,selectedOption);
            // return RETURN; uncomment those and observe the status of the user to see the bug
            sprintf(changeStatusPage.content[0].text, "Account is currently %sactive", (selectedOption == 0 ? "in" : ""));
            PromptInputs results = display_box_prompt(&changeStatusPage, (int)!selectedOption);  // !not just adjusts index
            while (results.dialogueValue != DIALOG_CHANGE && results.dialogueValue != DIALOG_DISCARD) {
                selectedOption = results.dialogueValue;
                changeStatusPage.content[3] = LINE_DIALOGUE((selectedOption == DIALOG_ACTIVE ? "│ %s(x) Active%s                 │" : "│ %s( ) Active%s                 │"), DIALOG_ACTIVE);
                changeStatusPage.content[4] = LINE_DIALOGUE((selectedOption == DIALOG_INACTIVE ? "│ %s(x) Inactive%s               │" : "│ %s( ) Inactive%s               │"), DIALOG_INACTIVE);
                results = display_box_prompt(&changeStatusPage, (int)!selectedOption);
            }

            if ((int)selectedOption != accountResult.accounts[0].status) {
                Status changeStatus = change_status(id);
                print_status(changeStatus);
                free_result(statusResult);
                return COMMANDS;
            } else {
                Status status = {
                    .status = WARNING,
                    .message = "Account status is already set to that option."
                };
                print_status(status);

                free_result(statusResult);
                return COMMANDS;
            }

        } else if (accountResult.status.status == ERROR) {
            print_status(accountResult.status);

            free_result(statusResult);
            return ACC_STATUS;
        }
    }
    
    free_result(statusResult);
    return COMMANDS;
}

static MenuIndex acc_modify_func() {
    enum DialogOptions {
        DIALOG_PROCEED,
        DIALOG_DISCARD,
        DIALOG_YES,
        DIALOG_NO
    };

    BoxContent modifyPage = {
        .title = "Modify Account",
        .content = {LINE_DEFAULT("┌ Account Number ────────────┐"),
                    LINE_TEXT("│ %s │", 10, 0, "0123456789\b", ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Find", DIALOG_PROCEED),
                    LINE_DIALOGUE("Back", DIALOG_DISCARD)}};

    PromptInputs results = display_box_prompt(&modifyPage, 0);
    AccountResult accountResults = query(results.textInputs[0]);

    if (results.dialogueValue == DIALOG_DISCARD) {
        free_result(results);
        return COMMANDS;
    }

    BoxContent modifySubPage = {
        .title = "Modify Account",
        .content = {LINE_DEFAULT("┌ Name ──────────────────────┐"),
                    LINE_TEXT("│ %s │", 25, 0, "", accountResults.accounts[0].name),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ E-mail ────────────────────┐"),
                    LINE_TEXT("│ %s │", 25, 0, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!#$%&'*+-/=?^_{|}~`@.\b ", accountResults.accounts[0].email),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ Mobile ────────────────────┐"),
                    LINE_TEXT("│ +20 %s │", 10, 0, "0123456789\b", accountResults.accounts[0].mobile + 1),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Modify", DIALOG_YES),
                    LINE_DIALOGUE("Back", DIALOG_DISCARD)}};

    if (accountResults.status.status == ERROR) {
        print_status(accountResults.status);
        return ACC_MODIFY;
    }

    // Here we are looping over modify sub menu until it succeeded or user chose to go back
    while (1) {
        PromptInputs results = display_box_prompt(&modifySubPage, 0);

        if (results.dialogueValue == DIALOG_DISCARD) {
            free_result(results);
            return ACC_MODIFY;
        }

        // phone number must be 10 characters (excluding the "+ 20")
        if (strlen(results.textInputs[2]) != 10) {
            Status status = {
                .status = ERROR,
                .message = "Phone Number is not valid"
            };
            print_status(status);

            continue;
        }

        // Making a dummy Account variable
        Account account;
        strcpy(account.id, accountResults.accounts[0].id);
        strcpy(account.name, results.textInputs[0]);
        strcpy(account.email, results.textInputs[1]);
        account.balance = accountResults.accounts[0].balance;
        // Adding the 0 at the beginning of the phone number
        sprintf(account.mobile, "0%s", results.textInputs[2]);

        // Sending data to apply modification to modify()
        Status status = modify(account.id, account.name, account.mobile, account.email);
        print_status(status);

        if (status.status == SUCCESS) break;
    }

    free_result(results);
    return COMMANDS;
}

static MenuIndex other_print_func() {
    enum DialogOptions { DIALOG_NAME = NAME,
                         DIALOG_BALANCE = BALANCE,
                         DIALOG_DATE = DATE,
                         DIALOG_PRINT,
                         DIALOG_BACK,
                         DIALOG_UP,
                         DIALOG_DOWN,
    };

    BoxContent printPage = {
        .title = "Print Accounts",
        .content = {LINE_DEFAULT("Choose how you want the       "),
                    LINE_DEFAULT("printing be sorted by         "),
                    LINE_DEFAULT(" "),
                    LINE_DEFAULT("┌ Sort by ───────────────────┐"),
                    LINE_DIALOGUE("│ %s(x) Name%s                   │", DIALOG_NAME),
                    LINE_DIALOGUE("│ %s( ) Balance%s                │", DIALOG_BALANCE),
                    LINE_DIALOGUE("│ %s( ) Date Opened%s            │", DIALOG_DATE),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "), LINE_DIALOGUE("Print", DIALOG_PRINT),
                    LINE_DIALOGUE("Back", DIALOG_BACK)}};

    // selection loop
    enum DialogOptions selectedOption = DIALOG_NAME;
    PromptInputs results = display_box_prompt(&printPage, (int)selectedOption - 1);  //-1 adjust index based on position in yasseens enum
    while (results.dialogueValue != DIALOG_PRINT && results.dialogueValue != DIALOG_BACK) {
        selectedOption = results.dialogueValue;
        printPage.content[4] = LINE_DIALOGUE((selectedOption == DIALOG_NAME ? "│ %s(x) Name%s                   │" : "│ %s( ) Name%s                   │"), DIALOG_NAME);
        printPage.content[5] = LINE_DIALOGUE((selectedOption == DIALOG_BALANCE ? "│ %s(x) Balance%s                │" : "│ %s( ) Balance%s                │"), DIALOG_BALANCE);
        printPage.content[6] = LINE_DIALOGUE((selectedOption == DIALOG_DATE ? "│ %s(x) Date Opened%s            │" : "│ %s( ) Date Opened%s            │"), DIALOG_DATE);
        results = display_box_prompt(&printPage, (int)selectedOption - 1);
    }

    // load();  // TODO: DELETE
    AccountResult accountResult = print((SortMethod)selectedOption);

    if (accountResult.status.status == ERROR) {
        print_status(accountResult.status);
        return OTHER_PRINT;
    }

    if (results.dialogueValue == DIALOG_BACK) return COMMANDS;

    // report
    int currIndex = 0;
    int lastScroll = 0;
    PromptInputs reportResults = {.dialogueValue = -1};
    char account1[LINE_LENGTH],
        name1[LINE_LENGTH],
        email1[LINE_LENGTH],
        balance1[LINE_LENGTH],
        mobile1[LINE_LENGTH],
        date1[LINE_LENGTH],
        status1[LINE_LENGTH],

        account2[LINE_LENGTH],
        name2[LINE_LENGTH],
        email2[LINE_LENGTH],
        balance2[LINE_LENGTH],
        mobile2[LINE_LENGTH],
        date2[LINE_LENGTH],
        status2[LINE_LENGTH];

    while (reportResults.dialogueValue != DIALOG_BACK) {
        Account *acc1 = &accountResult.accounts[currIndex];
        sprintf(account1, "Account #: %s", acc1->id);
        sprintf(name1, "Name : %s", acc1->name);
        sprintf(email1, "E-mail: %s", acc1->email);
        sprintf(balance1, "Balance: %.2f", acc1->balance);
        sprintf(mobile1, "Mobile: %s", acc1->mobile);
        sprintf(date1, "Date Opened: %d-%d", acc1->date.month, acc1->date.year);
        sprintf(status1, "Status: %s", acc1->status ? "active" : "inactive");
        _Bool isSecond = currIndex + 1 < accountResult.n;
        if (isSecond) {
            Account *acc2 = &accountResult.accounts[currIndex + 1];
            sprintf(account2, "Account #: %s", acc2->id);
            sprintf(name2, "Name : %s", acc2->name);
            sprintf(email2, "E-mail: %s", acc2->email);
            sprintf(balance2, "Balance: %.2f", acc2->balance);
            sprintf(mobile2, "Mobile: %s", acc2->mobile);
            sprintf(date2, "Date Opened: %d-%d", acc2->date.month, acc2->date.year);
            sprintf(status2, "Status: %s", acc2->status ? "active" : "inactive");
        }
        BoxContent reportPage = {
            .title = "Report",
            .content = {
                currIndex - 2 >= 0 ? LINE_DIALOGUE("%s↑ ...%s                         ", DIALOG_UP) : LINE_DEFAULT(" "),
                LINE_DEFAULT(account1),
                LINE_DEFAULT(name1),
                LINE_DEFAULT(email1),
                LINE_DEFAULT(balance1),
                LINE_DEFAULT(mobile1),
                LINE_DEFAULT(date1),
                LINE_DEFAULT(status1),
                LINE_DEFAULT(" "),
                isSecond ? LINE_DEFAULT(account2) : LINE_DEFAULT(" "),
                isSecond ? LINE_DEFAULT(name2) : LINE_DEFAULT(" "),
                isSecond ? LINE_DEFAULT(email2) : LINE_DEFAULT(" "),
                isSecond ? LINE_DEFAULT(balance2) : LINE_DEFAULT(" "),
                isSecond ? LINE_DEFAULT(mobile2) : LINE_DEFAULT(" "),
                isSecond ? LINE_DEFAULT(date2) : LINE_DEFAULT(" "),
                isSecond ? LINE_DEFAULT(status2) : LINE_DEFAULT(" "),
                currIndex + 1 < accountResult.n ? LINE_DIALOGUE("%s↓ ...%s                         ", DIALOG_DOWN) : LINE_DEFAULT(" "),
                LINE_DEFAULT(" "),
                LINE_DIALOGUE("Back", DIALOG_BACK)}};

        reportResults = display_box_prompt(&reportPage, lastScroll);
        if (reportResults.dialogueValue == DIALOG_UP) {
            currIndex = (currIndex - 2 >= 0) ? currIndex - 2 : 0;
            lastScroll = 0;
        } else if (reportResults.dialogueValue == DIALOG_DOWN) {
            currIndex = (currIndex + 2 < accountResult.n) ? currIndex + 2 : accountResult.n - 1;
            lastScroll = 1;
        }
    }

    return COMMANDS;
}
static MenuIndex acc_search_status(){
    enum DialogOptions{
        DIALOG_FIND,
        DIALOG_DISCARD,
        DIALOG_BACK,
    };
    BoxContent searchPage = {
        .title = "Search Account",
        .content = {
            LINE_DEFAULT("┌ Account Number ────────────┐"),
            LINE_TEXT("│ %s │", 10, 0, "1234567890\b", ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Find", DIALOG_FIND),
            LINE_DIALOGUE("Discard", DIALOG_DISCARD),
        }};
    PromptInputs searchChoice = display_box_prompt(&searchPage,0);
    if(searchChoice.dialogueValue == DIALOG_DISCARD){
        free_result(searchChoice);
        return COMMANDS;
    }
    else if(searchChoice.dialogueValue == DIALOG_FIND){
        char *id= searchChoice.textInputs[0];
        AccountResult searchResult = query(id);
        if(searchResult.status.status == ERROR){
            Status error = searchResult.status;
            free_result(searchChoice);
            print_status(error);
            return ACC_SEARCH;
        }
        else if(searchResult.status.status == SUCCESS){
            Account foundAccount = searchResult.accounts[0];
            char account1[LINE_LENGTH],
                name1[LINE_LENGTH],
                email1[LINE_LENGTH],
                balance1[LINE_LENGTH],
                mobile1[LINE_LENGTH],
                date1[LINE_LENGTH],
                status1[LINE_LENGTH];
            sprintf(account1, "Account #: %s", foundAccount.id);
            sprintf(name1, "Name : %s", foundAccount.name);
            sprintf(email1, "E-mail: %s", foundAccount.email);
            sprintf(balance1, "Balance: %.2f", foundAccount.balance);
            sprintf(mobile1, "Mobile: %s", foundAccount.mobile);
            sprintf(date1, "Date Opened: %d-%d", foundAccount.date.month, foundAccount.date.year);
            sprintf(status1, "Status: %s", foundAccount.status ? "active" : "inactive");
            BoxContent accountDetails = {
                .title = "Search Account",
                .content = {
                    LINE_DEFAULT(account1),
                    LINE_DEFAULT(name1),
                    LINE_DEFAULT(email1),
                    LINE_DEFAULT(balance1),
                    LINE_DEFAULT(mobile1),
                    LINE_DEFAULT(date1),
                    LINE_DEFAULT(status1),
                    LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Back", DIALOG_BACK)}};
            PromptInputs foundPage = display_box_prompt(&accountDetails,0);
            if(foundPage.dialogueValue == DIALOG_BACK){
                return ACC_SEARCH;
            }

        }
    }
}
// -

void mainloop() {
    // Put functions in the menuFunctions Array
    menuFunctions[ENTRY] = entry_func;
    menuFunctions[LOGIN] = login_func;
    menuFunctions[QUIT] = quit_func;

    // Manage Accounts
    menuFunctions[COMMANDS] = commands_func;
    menuFunctions[ACC_NEW] = acc_new_func;
    menuFunctions[ACC_DELETE] = acc_delete_func;
    menuFunctions[ACC_MODIFY] = acc_modify_func;
    menuFunctions[ACC_SEARCH] = acc_search_status;
    // menuFunctions[ACC_ADVANCESEARCH] = acc_advancesearch_status;
    menuFunctions[ACC_STATUS] = acc_change_status;

    // Transactions
    // menuFunctions[TRANS_WITHDRAW] = trans_withdraw_func;
    // menuFunctions[TRANS_DEPOSIT] = trans_deposit_func;
    // menuFunctions[TRANS_TRANSFER] = trans_transfer_func;

    // Others
    // menuFunctions[OTHER_REPORT] = other_report_func;
    menuFunctions[OTHER_PRINT] = other_print_func;

    // Runs the main looping
    while (currentIndex != RETURN) {
        tempIndex = currentIndex;
        currentIndex = menuFunctions[currentIndex]();
        prevIndex = tempIndex;
    }
}
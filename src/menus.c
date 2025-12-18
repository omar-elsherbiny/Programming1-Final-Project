// menus.c
#include "menus.h"
#include "display.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void free_result(PromptInputs results) {
    for (int i = 0; i < results.textInputCount; i++) {
        free(results.textInputs[i]);
    }

    free(results.textInputs);
}

static void remove_all_chars(char *str, char c) {
    char *pRead = str, *pWrite = str;
    while (*pRead) {
        *pWrite = *pRead++;
        pWrite += (*pWrite != c);
    }
    *pWrite = '\0';
}

// Functions definitions
static MenuIndex entry_func() { return LOGIN; }

static MenuIndex login_func() {
    enum DialogOptions { DIALOG_LOGIN, DIALOG_QUIT, DIALOG_PROCEED };

    BoxContent loginPage = {
        .title = "Login",
        .content = {LINE_DEFAULT("┌ Username ──────────────────┐"),
                    LINE_TEXT("│ %s │", 25, 0, ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ Password ──────────────────┐"),
                    LINE_TEXT("│ %s │", 50, 1, ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "), LINE_DIALOGUE("Login", DIALOG_LOGIN),
                    LINE_DIALOGUE(FG_RED "Quit", DIALOG_QUIT)}};

    PromptInputs results = display_box_prompt(&loginPage);

    if (results.dialogueValue == DIALOG_QUIT) {
        free_result(results);
        return QUIT;
    }

    Status status = login(results.textInputs[0], results.textInputs[1]);

    if (status.status == ERROR) {
        char errorMsg[LINE_LENGTH];
        sprintf(errorMsg, "%s", status.message);
        int lineCount;
        Line *errorMsgLines = MULTI_LINE_DEFAULT(errorMsg, 30, &lineCount);

        BoxContent errorPage = {.title = FG_RED "ERROR"};
        for (int i = 0; i < lineCount; i++) {
            // this is to append the red color to the string
            char temp[LINE_LENGTH];
            strcpy(temp, errorMsgLines[i].text);
            sprintf(errorMsgLines[i].text, FG_RED "%s", temp);

            errorPage.content[i] = errorMsgLines[i];
        }
        errorPage.content[lineCount] = LINE_DEFAULT(" ");
        errorPage.content[lineCount + 1] =
            LINE_DIALOGUE("Okay", DIALOG_PROCEED);
        free(errorMsgLines);

        PromptInputs errorResult = display_box_prompt(&errorPage);

        if (errorResult.dialogueValue == DIALOG_PROCEED) {
            free_result(results);
            return LOGIN;
        }
    }

    status = load();
    if (status.status == ERROR) {
        char errorMsg[LINE_LENGTH];
        sprintf(errorMsg, "%s", status.message);
        int lineCount;
        Line *errorMsgLines = MULTI_LINE_DEFAULT(errorMsg, 30, &lineCount);

        BoxContent errorPage = {.title = FG_RED "ERROR"};
        for (int i = 0; i < lineCount; i++) {
            // this is to append the red color to the string
            char temp[LINE_LENGTH];
            strcpy(temp, errorMsgLines[i].text);
            sprintf(errorMsgLines[i].text, FG_RED "%s", temp);

            errorPage.content[i] = errorMsgLines[i];
        }
        errorPage.content[lineCount] = LINE_DEFAULT(" ");
        errorPage.content[lineCount + 1] =
            LINE_DIALOGUE("Okay", DIALOG_PROCEED);
        free(errorMsgLines);

        PromptInputs errorResult = display_box_prompt(&errorPage);

        if (errorResult.dialogueValue == DIALOG_PROCEED) {
            free_result(results);
            return LOGIN;
        }
    }

    free_result(results);
    return COMMANDS;
}

static MenuIndex quit_func() {
    enum DialogOptions { DIALOG_YES, DIALOG_NO };

    BoxContent quitPage = {
        .title = "Quit",
        .content = {LINE_DEFAULT("Are you sure you want to quit?"),
                    LINE_DEFAULT(" "), LINE_DIALOGUE(FG_RED "Yes", DIALOG_YES),
                    LINE_DIALOGUE("No", DIALOG_NO)}};

    PromptInputs results = display_box_prompt(&quitPage);

    if (results.dialogueValue == DIALOG_YES) {
        return RETURN;
    }

    return LOGIN;
}

static MenuIndex commands_func() {
    enum DialogOptions {
        DIALOG_ACC_NEW,
        DIALOG_ACC_DELETE,
        DIALOG_ACC_MODIFY,
        DIALOG_ACC_SEARCH,
        DIALOG_ACC_ADVANCESEARCH,
        DIALOG_ACC_STATUS,
        DIALOG_TRANS_WITHDRAW,
        DIALOG_TRANS_DEPOSIT,
        DIALOG_TRANS_TRANSFER,
        DIALOG_OTHER_REPORT,
        DIALOG_OTHER_PRINT,
        DIALOG_LOGOUT
    };

    BoxContent commandsPage = {
        .title = "Commands",
        .content = {
            LINE_DEFAULT("Manage Accounts:              "),
            LINE_DIALOGUE("  %sAdd a new Account%s", DIALOG_ACC_NEW),
            LINE_DIALOGUE("  %sDelete an Existing Account%s",
                          DIALOG_ACC_DELETE),
            LINE_DIALOGUE("  %sModify an Existing Account%s",
                          DIALOG_ACC_MODIFY),
            LINE_DIALOGUE("  %sSearch an Account%s", DIALOG_ACC_SEARCH),
            LINE_DIALOGUE("  %sAdvanced Searching%s", DIALOG_ACC_ADVANCESEARCH),
            LINE_DIALOGUE("  %sChange an Account Status%s", DIALOG_ACC_STATUS),
            LINE_DEFAULT(" "), LINE_DEFAULT("Transactions:"),
            LINE_DIALOGUE("  %sWithdraw from an Account%s",
                          DIALOG_TRANS_WITHDRAW),
            LINE_DIALOGUE("  %sDeposit to an Account%s", DIALOG_TRANS_DEPOSIT),
            LINE_DIALOGUE("  %sTransfer to an Account%s",
                          DIALOG_TRANS_TRANSFER),
            LINE_DEFAULT(" "), LINE_DEFAULT("Others:"),
            LINE_DIALOGUE("  %sReport last transactions%s",
                          DIALOG_OTHER_REPORT),
            LINE_DIALOGUE("  %sPrint all Accounts%s", DIALOG_OTHER_PRINT),
            LINE_DEFAULT(" "), LINE_DIALOGUE(FG_RED "Logout", DIALOG_LOGOUT)}};

    PromptInputs results = display_box_prompt(&commandsPage);

    if (results.dialogueValue == DIALOG_LOGOUT)
        return LOGIN;

    if (results.dialogueValue == DIALOG_ACC_NEW)
        return ACC_NEW;
    if (results.dialogueValue == DIALOG_ACC_DELETE)
        return ACC_DELETE;
    if (results.dialogueValue == DIALOG_ACC_MODIFY)
        return ACC_MODIFY;
    if (results.dialogueValue == DIALOG_ACC_SEARCH)
        return ACC_SEARCH;
    if (results.dialogueValue == DIALOG_ACC_ADVANCESEARCH)
        return ACC_ADVANCESEARCH;
    if (results.dialogueValue == DIALOG_ACC_STATUS)
        return ACC_STATUS;
    if (results.dialogueValue == DIALOG_TRANS_WITHDRAW)
        return TRANS_WITHDRAW;
    if (results.dialogueValue == DIALOG_TRANS_DEPOSIT)
        return TRANS_DEPOSIT;
    if (results.dialogueValue == DIALOG_TRANS_TRANSFER)
        return TRANS_TRANSFER;
    if (results.dialogueValue == DIALOG_OTHER_REPORT)
        return OTHER_REPORT;
    if (results.dialogueValue == DIALOG_OTHER_PRINT)
        return OTHER_PRINT;
}

static MenuIndex acc_new_func() {
    enum DialogOptions {
        DIALOG_ADD,
        DIALOG_DISCARD,
        DIALOG_YES,
        DIALOG_NO,
        DIALOG_PROCEED
    };

    BoxContent addAccountPage = {
        .title = "Add Account",
        .content = {LINE_DEFAULT("┌ Account Number ────────────┐"),
                    LINE_TEXT("│ %s │", 10, 0, "0123456789\b"),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ Name ──────────────────────┐"),
                    LINE_TEXT("│ %s │", 25, 0, ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ E-mail ────────────────────┐"),
                    LINE_TEXT("│ %s │", 25, 0, ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ Balance ───────────────────┐"),
                    LINE_TEXT("│ %s ($) │", 20, 0, ",.0123456789\b"),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ Mobile ────────────────────┐"),
                    LINE_TEXT("│ + 20 %s │", 10, 0, "0123456789\b"),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "), LINE_DIALOGUE("Add", DIALOG_ADD),
                    LINE_DIALOGUE("Discard", DIALOG_DISCARD)}};

    PromptInputs results = display_box_prompt(&addAccountPage);

    if (results.dialogueValue == DIALOG_DISCARD) {
        free_result(results);
        return COMMANDS;
    }

    BoxContent confirmAddAccountPage = {
        .title = "Confirm Add",
        .content = {LINE_DEFAULT("Are you sure you want to add  "),
                    LINE_DEFAULT("this account?"), LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Yes", DIALOG_YES),
                    LINE_DIALOGUE("No", DIALOG_NO)}};

    PromptInputs confirmResults = display_box_prompt(&confirmAddAccountPage);

    if (confirmResults.dialogueValue == DIALOG_NO) {
        free_result(results);
        printf("%d", confirmResults.dialogueValue);
        return ACC_NEW;
    }

    if (confirmResults.dialogueValue == DIALOG_YES) {
        // removing all commas from balance input field
        remove_all_chars(results.textInputs[3], ',');

        // Check if any field is empty
        if ((strlen(results.textInputs[0]) == 0) ||
            (strlen(results.textInputs[1]) == 0) ||
            (strlen(results.textInputs[2]) == 0) ||
            (strlen(results.textInputs[3]) == 0) ||
            (strlen(results.textInputs[4]) == 0)) {
            BoxContent errorPage = {
                .title = FG_RED "ERROR",
                .content = {
                    LINE_DEFAULT(FG_RED "You should fill out all the   "),
                    LINE_DEFAULT(FG_RED "input fields"), LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Okay", DIALOG_PROCEED)}};

            PromptInputs errorResult = display_box_prompt(&errorPage);

            if (errorResult.dialogueValue == DIALOG_PROCEED) {
                free_result(results);
                return ACC_NEW;
            }
        }

        // Acc number number 10 characters
        if (strlen(results.textInputs[0]) != 10) {
            BoxContent errorPage = {
                .title = FG_RED "ERROR",
                .content = {
                    LINE_DEFAULT(FG_RED "Account Number is not 10      "),
                    LINE_DEFAULT(FG_RED "characters"), LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Okay", DIALOG_PROCEED)}};

            PromptInputs errorResult = display_box_prompt(&errorPage);

            if (errorResult.dialogueValue == DIALOG_PROCEED) {
                free_result(results);
                return ACC_NEW;
            }
        }

        // TODO
        // email validation
        // if (!is_valid_email(results.textInputs[2])) {
        //     if (!(results.textInputs[3][0] >= '0' &&
        //           results.textInputs[3][0] <= '9')) {
        //         BoxContent errorPage = {
        //             .title = FG_RED "ERROR",
        //             .content = {
        //                 LINE_DEFAULT(FG_RED "Email is not valid            "),
        //                 LINE_DEFAULT(" "),
        //                 LINE_DIALOGUE("Okay", DIALOG_PROCEED)}};

        //         PromptInputs errorResult = display_box_prompt(&errorPage);

        //         if (errorResult.dialogueValue == DIALOG_PROCEED) {
        //             free_result(results);
        //             return ACC_NEW;
        //         }
        //     }
        // }

        // Check if 1st char in balance is a number
        if (!(results.textInputs[3][0] >= '0' &&
              results.textInputs[3][0] <= '9')) {
            BoxContent errorPage = {
                .title = FG_RED "ERROR",
                .content = {
                    LINE_DEFAULT(FG_RED "Balance should start with a   "),
                    LINE_DEFAULT(FG_RED "number"), LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Okay", DIALOG_PROCEED)}};

            PromptInputs errorResult = display_box_prompt(&errorPage);

            if (errorResult.dialogueValue == DIALOG_PROCEED) {
                free_result(results);
                return ACC_NEW;
            }
        }

        // Check if last char in balance is a number
        if (results.textInputs[3][strlen(results.textInputs[3]) - 1] == '.') {
            BoxContent errorPage = {
                .title = FG_RED "ERROR",
                .content = {
                    LINE_DEFAULT(FG_RED "Balance should end with a     "),
                    LINE_DEFAULT(FG_RED "number"), LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Okay", DIALOG_PROCEED)}};

            PromptInputs errorResult = display_box_prompt(&errorPage);

            if (errorResult.dialogueValue == DIALOG_PROCEED) {
                free_result(results);
                return ACC_NEW;
            }
        }

        // Check if there are only 1 decimal point
        const char *token = results.textInputs[3];
        int decimalCount = 0;
        while ((token = strstr(token, ".")) != NULL)
            decimalCount++, token++;
        if (decimalCount > 1) {
            BoxContent errorPage = {
                .title = FG_RED "ERROR",
                .content = {
                    LINE_DEFAULT(FG_RED "Balance should only have 1    "),
                    LINE_DEFAULT(FG_RED "decimal point"), LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Okay", DIALOG_PROCEED)}};

            PromptInputs errorResult = display_box_prompt(&errorPage);

            if (errorResult.dialogueValue == DIALOG_PROCEED) {
                free_result(results);
                return ACC_NEW;
            }
        }

        // Check if balance has only 2 decimal places
        char temp[LINE_LENGTH];
        strcpy(temp, results.textInputs[3]);
        token = strtok(temp, ".");
        token = strtok(NULL, ".");
        if (token != NULL) {
            if (strlen(token) > 2) {
                BoxContent errorPage = {
                    .title = FG_RED "ERROR",
                    .content = {
                        LINE_DEFAULT(FG_RED "The balance should only be max"),
                        LINE_DEFAULT(FG_RED "of 2 decimal places"),
                        LINE_DEFAULT(" "),
                        LINE_DIALOGUE("Okay", DIALOG_PROCEED)}};

                PromptInputs errorResult = display_box_prompt(&errorPage);

                if (errorResult.dialogueValue == DIALOG_PROCEED) {
                    free_result(results);
                    return ACC_NEW;
                }
            }
        }

        // phone number must be 10 characters (excluding the "+ 20")
        if (strlen(results.textInputs[4]) != 10) {
            BoxContent errorPage = {
                .title = FG_RED "ERROR",
                .content = {
                    LINE_DEFAULT(FG_RED "Phone Number is not valid    "),
                    LINE_DEFAULT(" "), LINE_DIALOGUE("Okay", DIALOG_PROCEED)}};

            PromptInputs errorResult = display_box_prompt(&errorPage);

            if (errorResult.dialogueValue == DIALOG_PROCEED) {
                free_result(results);
                return ACC_NEW;
            }
        }

        Account account;
        strcpy(account.id, results.textInputs[0]);
        strcpy(account.name, results.textInputs[1]);
        strcpy(account.email, results.textInputs[2]);
        account.balance = strtod(results.textInputs[3], NULL);
        sprintf(account.mobile, "0%s", results.textInputs[4]);

        Status status = add(account);
        char statusMsg[LINE_LENGTH];
        sprintf(statusMsg, "%s", status.message);
        int lineCount;
        Line *statusMsgLines = MULTI_LINE_DEFAULT(statusMsg, 30, &lineCount);

        BoxContent statusPage;
        if (status.status == ERROR) {
            strcpy(statusPage.title, FG_RED "ERROR");
        } else {
            strcpy(statusPage.title, FG_GREEN "SUCCESS");
        }

        for (int i = 0; i < lineCount; i++) {
            // this is to append the red color to the string
            char temp[LINE_LENGTH];
            strcpy(temp, statusMsgLines[i].text);
            sprintf(statusMsgLines[i].text,
                    (status.status == ERROR ? FG_RED "%s" : FG_GREEN "%s"),
                    temp);

            statusPage.content[i] = statusMsgLines[i];
        }
        statusPage.content[lineCount] = LINE_DEFAULT(" ");
        statusPage.content[lineCount + 1] =
            LINE_DIALOGUE("Okay", DIALOG_PROCEED);
        free(statusMsgLines);

        PromptInputs statusResult = display_box_prompt(&statusPage);

        if (statusResult.dialogueValue == DIALOG_PROCEED) {
            free_result(results);

            if (status.status == ERROR)
                return ACC_NEW;
            if (status.status == SUCCESS)
                return COMMANDS;
        }
    }

    free_result(results);
    return COMMANDS;
}

static MenuIndex acc_delete_func() {
    enum DialogOptions {
        DIALOG_DEL_ONE,
        DIALOG_DEL_MULTI,
        DIALOG_DISCARD,
        DIALOG_YES,
        DIALOG_NO
    };

    BoxContent deletePage = {
        .title = "Delete Account",
        .content = {LINE_DEFAULT("┌ Delete Option ─────────────┐"),
                    LINE_DIALOGUE("│ %sOne with an Account number%s │", DIALOG_DEL_ONE),
                    LINE_DIALOGUE("│ %sMultiple with a criteria%s   │", DIALOG_DEL_MULTI),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Discard", DIALOG_DISCARD)}};

    PromptInputs results = display_box_prompt(&deletePage);

    if (results.dialogueValue == DIALOG_DISCARD) {
        free_result(results);
        return LOGIN;
    }

    if (results.dialogueValue == DIALOG_DEL_ONE) {
        BoxContent deleteOnePage = {
        .title = "Delete Account",
        .content = {LINE_DEFAULT("┌ Account Number ────────────┐"),
                    LINE_TEXT("│ %s │", 10, 0, "1234567890\b"),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "),
                    LINE_DIALOGUE(FG_RED "Delete", DIALOG_YES),
                    LINE_DIALOGUE("Discard", DIALOG_NO)}};

        PromptInputs delOneResults = display_box_prompt(&deleteOnePage);
        
        if (delOneResults.dialogueValue == DIALOG_NO) {
            free_result(results);
            return ACC_DELETE;
        }
    }

    free_result(results);
    return COMMANDS;
}

void mainloop() {
    // Put functions in the menuFunctions Array
    menuFunctions[ENTRY] = entry_func;
    menuFunctions[LOGIN] = login_func;
    menuFunctions[QUIT] = quit_func;
    menuFunctions[COMMANDS] = commands_func;
    menuFunctions[ACC_NEW] = acc_new_func;
    menuFunctions[ACC_DELETE] = acc_delete_func;

    // Runs the main looping
    MenuIndex currentIndex = ENTRY;
    while (currentIndex != RETURN) {
        currentIndex = menuFunctions[currentIndex]();
    }
}

/* temp container
static void temp () {
    BoxContent quitPage = {
        .title = FG_RED "Quit",
        .content = {
            LINE_DEFAULT(FG_RED "Are you sure you want to quit?"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE(FG_RED "Yes", 0),
            LINE_DIALOGUE(FG_GREEN "No", 1)}};

    BoxContent errorLoginPage = {
        .title = FG_RED "Error",
        .content = {
            LINE_DEFAULT(FG_RED "Invalid Username or Password  "),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Okay", 0)}};

    BoxContent commandsPage = {
        .title = "Commands" ,
        .content = {
            LINE_DEFAULT("Manage Accounts:              "),
            LINE_DIALOGUE("  Add a new Account", 11),
            LINE_DIALOGUE("  Delete an Existing Account", 12),
            LINE_DIALOGUE("  Modify an Existing Account", 13),
            LINE_DIALOGUE("  Search an Account", 14),
            LINE_DIALOGUE("  Advanced Searching", 15),
            LINE_DIALOGUE("  Change an Account Status", 16),
            LINE_DEFAULT(" "),
            LINE_DEFAULT("Transactions:"),
            LINE_DIALOGUE("  Withdraw from an Account", 21),
            LINE_DIALOGUE("  Deposit to an Account", 22),
            LINE_DIALOGUE("  Transfer to an Account", 23),
            LINE_DEFAULT(" "),
            LINE_DEFAULT("Others:"),
            LINE_DIALOGUE("  Report last transactions", 31),
            LINE_DIALOGUE("  Print all Accounts", 32),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE(FG_RED "Logout", 0)}};

    BoxContent addAccountPage = {
        .title = "Add Account" ,
        .content = {
            LINE_DEFAULT("┌ Account Number ────────────┐"),
            LINE_TEXT("│ %s │", 10,    0, ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT("┌ Name ──────────────────────┐"),
            LINE_TEXT("│ %s │", 25,    0, ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT("┌ E-mail ────────────────────┐"),
            LINE_TEXT("│ %s │", 25,    0, ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT("┌ Balance ───────────────────┐"),
            LINE_TEXT("│ %s ($) │", 15,    0, ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT("┌ Mobile ────────────────────┐"),
            LINE_TEXT("│ + %s │", 15,    0, ""),
            LINE_DEFAULT("└────────────────────────────┘"),

            LINE_DIALOGUE("Add", 0),
            LINE_DIALOGUE("Back", 1)}};

    BoxContent ConfirmAddPage = {
        .title = "Confirm Add" ,
        .content = {
            LINE_DEFAULT("Are you sure you want to add  "),
            LINE_DEFAULT("this account?"),
            LINE_DEFAULT(" "),

            LINE_DIALOGUE(FG_GREEN "Yes", 0),
            LINE_DIALOGUE(FG_RED "No", 1)}};

    BoxContent errorAddPage= {
        .title = FG_RED "Error",
        .content = {
            LINE_DEFAULT(FG_RED "Account Number is not Unique  "),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Okay", 0)}};

    BoxContent deleteOptionPage = {
        .title = FG_RED "Delete Account",
        .content = {
            LINE_DEFAULT("┌ "FG_RED "Delete Option" FG_RESET " ─────────────┐"),
            LINE_DIALOGUE("│ " FG_RED "%sOne with an Account number%s" FG_RESET
" │", 10), LINE_DIALOGUE("│ " FG_RED "%sMultiple with a criteria%s" FG_RESET"
│", 11), LINE_DEFAULT("└────────────────────────────┘"), LINE_DEFAULT(" "),
            LINE_DIALOGUE("Discard", 0)}};

    BoxContent deleteSinglePage = {
        .title = FG_RED "Delete Account",
        .content = {
            LINE_DEFAULT(FG_RED "┌ Account Number ────────────┐"),
            LINE_TEXT(FG_RED "│ %s │ ", 10,    0, ""),
            LINE_DEFAULT(FG_RED  "└────────────────────────────┘"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE(FG_RED "DELETE", 0),
            LINE_DIALOGUE("Discard", 1)}};

    BoxContent deleteMultipleDatePage = {
        .title = FG_RED "Delete Account",
        .content = {
            LINE_DEFAULT(FG_RED "Delete all accounts created on"),
            LINE_DEFAULT(FG_RED "the given date below"),
            LINE_DEFAULT( "          ┌ Month ─┐          "),
            LINE_TEXT( "          │ %s │          ", 25,    0, ""),
            LINE_DEFAULT( "          └────────┘          "),
            LINE_DEFAULT( "          ┌ Year ──┐          "),
            LINE_TEXT( "          │ %s │          ", 25,    0, ""),
            LINE_DEFAULT( "          └────────┘          "),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE(FG_RED "DELETE", 0),
            LINE_DIALOGUE("Discard", 1)}};

    BoxContent ConfirmDeleteSinglePage = {
        .title = FG_RED "Confirm Delete",
        .content = {
            LINE_DEFAULT(FG_RED "Are you sure you want to      "),
            LINE_DEFAULT(FG_RED "delete this account?"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE(FG_RED "YES", 0),
            LINE_DIALOGUE("No", 1)}};

    BoxContent ConfirmDeleteMultipleDatePage = {
        .title = FG_RED "Confirm Delete",
        .content = {
            LINE_DEFAULT(FG_RED "Are you sure you want to      "),
            LINE_DEFAULT(FG_RED "delete all accounts from"),
            LINE_DEFAULT(FG_RED "December 2025?"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE(FG_RED "YES", 0),
            LINE_DIALOGUE("No", 1)}};

    BoxContent ConfirmDeleteMultipleInactivePage = {
        .title = FG_RED "Confirm Delete",
        .content = {
            LINE_DEFAULT(FG_RED "Are you sure you want to      "),
            LINE_DEFAULT(FG_RED "delete all  accounts that were"),
            LINE_DEFAULT(FG_RED "inactive for 90 days with a"),
            LINE_DEFAULT(FG_RED "balance of $0"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE(FG_RED "YES", 0),
            LINE_DIALOGUE("No", 1)}};

    BoxContent errorDeleteAccountNotZeroPage = {
        .title = FG_RED "Error",
        .content = {
            LINE_DEFAULT(FG_RED "Account is not Zeroed         "),
            LINE_DEFAULT(FG_RED "You may only delete Accounts"),
            LINE_DEFAULT(FG_RED "whose balance is $0"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Okay", 0)}};

    BoxContent errorDeleteNoAccountExistPage = {
        .title = FG_RED "Error",
        .content = {
            LINE_DEFAULT(FG_RED "Account does not Exist"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Okay", 0)}};

    BoxContent searchPage = {
        .title = "Search Account",
        .content = {
            LINE_DEFAULT("┌ Account Number ────────────┐"),
            LINE_TEXT("│ %s │ ", 10,    0, ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Find", 0),
            LINE_DIALOGUE("Discard", 1)}};

    BoxContent searchSingleResultPage = {
        .title = "Search Account",
        .content = {
            LINE_DEFAULT("Account #: 6767676767         "),
            LINE_DEFAULT("Name: Rick Astley"),
            LINE_DEFAULT("E-mail: rick.astley@email.com"),
            LINE_DEFAULT("Mobile: +676767676767"),
            LINE_DEFAULT("Date Opened: February 2007"),
            LINE_DEFAULT("Status: active"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Back", 0)}};

    BoxContent errorSearchSinglePage = {
        .title = "Error",
        .content = {
            LINE_DEFAULT("Account does not Exist        "),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Okay", 0)}};

    BoxContent searchMultiplePage = {
        .title = "Search Accounts",
        .content = {
            LINE_DEFAULT("┌ Keyword ───────────────────┐"),
            LINE_TEXT("│ %s │ ", 25,    0, ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Find", 0),
            LINE_DIALOGUE("Discard", 1)}};

    BoxContent searchMultipleResultPage = {
        .title = "Search Accounts",
        .content = {
            LINE_DIALOGUE("↑ ...", 10),
            LINE_DEFAULT("Account #: 6767676767         "),
            LINE_DEFAULT("Name: Rick Astley"),
            LINE_DEFAULT("E-mail: rick.astley@email.com"),
            LINE_DEFAULT("Mobile: +676767676767"),
            LINE_DEFAULT("Date Opened: February 2007"),
            LINE_DEFAULT("Status: active"),
            LINE_DEFAULT(" "),
            LINE_DEFAULT("Account #: 0101010101         "),
            LINE_DEFAULT("Name: Hatsune Miku"),
            LINE_DEFAULT("E-mail: miku.uwu@gmail.com"),
            LINE_DEFAULT("Mobile: +010100111001"),
            LINE_DEFAULT("Date Opened: August 2007"),
            LINE_DEFAULT("Status: active"),
            LINE_DIALOGUE("↓ ...", 11),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Back", 0)}};

    BoxContent errorSearchMultiplePage = {
        .title = "Error",
        .content = {
            LINE_DEFAULT("No account was found          "),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Back", 0)}};

    BoxContent accountStatusPage = {
        .title = "Account Status",
        .content = {
            LINE_DEFAULT("Account is currently active   "),
            LINE_DEFAULT(" "),
            LINE_DEFAULT("┌ Change account status ─────┐"),
            LINE_DIALOGUE("│ (x) Active                 │", 10),
            LINE_DIALOGUE("│ ( ) Inactive               │", 11),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Change", 0),
            LINE_DIALOGUE("Discard", 1)}};

    BoxContent confirmAccountStatusPage = {
        .title = "Confirm Status",
        .content = {
            LINE_DEFAULT("Are you sure you want to      "),
            LINE_DEFAULT("change this account status"),
            LINE_DEFAULT("from Active to Inactive?"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("YES", 0),
            LINE_DIALOGUE("No", 1)}};

    BoxContent errorAccountStatusPage = {
        .title = "Error",
        .content = {
            LINE_DEFAULT("Account does not Exist        "),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Okay", 0)}};
}
*/
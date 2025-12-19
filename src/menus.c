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
    ACC_MODIFY_SUB,
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

static void print_status(Status status) {
    int lineCount;
    Line *statusMsgLines = MULTI_LINE_DEFAULT(status.message, (status.status == ERROR ? FG_RED : FG_GREEN), 30, &lineCount);

    BoxContent statusPage = {0};
    strcpy(statusPage.title, (status.status == ERROR ? FG_RED "ERROR" : FG_GREEN "SUCCESS"));

    for (int i = 0; i < lineCount; i++) {
        statusPage.content[i] = statusMsgLines[i];
    }
    statusPage.content[lineCount] = LINE_DEFAULT(" ");
    statusPage.content[lineCount + 1] = LINE_DIALOGUE("Okay", 0);
    free(statusMsgLines);

    display_box_prompt(&statusPage);
}

// Functions definitions
static MenuIndex entry_func() { return LOGIN; }

static MenuIndex login_func() {
    enum DialogOptions { DIALOG_LOGIN,
                         DIALOG_QUIT,
                         DIALOG_PROCEED };

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

    PromptInputs results = display_box_prompt(&quitPage);

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

    PromptInputs results = display_box_prompt(&commandsPage);

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
        print_status(status);

        free_result(results);
        if (status.status == ERROR)
            return ACC_NEW;
        if (status.status == SUCCESS)
            return COMMANDS;
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
                    LINE_TEXT("│ %s │", 10, 0, "0123456789\b"),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Find", DIALOG_PROCEED),
                    LINE_DIALOGUE("Back", DIALOG_DISCARD)}};

    PromptInputs results = display_box_prompt(&modifyPage);

    if (results.dialogueValue == DIALOG_DISCARD) {
        free_result(results);
        return COMMANDS;
    }

    AccountResult accountResults = query(results.textInputs[0]);

    if (accountResults.status.status == ERROR) {
            print_status(accountResults.status);
            return ACC_MODIFY;
    }

    if (accountResults.status.status == SUCCESS) {
        return ACC_MODIFY_SUB;
    }

    free_result(results);
    return COMMANDS;
}

static MenuIndex acc_modify_sub_func() {
    enum DialogOptions {
        DIALOG_PROCEED,
        DIALOG_DISCARD,
        DIALOG_YES,
        DIALOG_NO
    };

    BoxContent modifySubPage = {
        .title = "Modify Account",
        .content = {LINE_DEFAULT("┌ Name ──────────────────────┐"),
                    LINE_TEXT("│ %s │", 10, 0, ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ E-mail ────────────────────┐"),
                    LINE_TEXT("│ %s │", 25, 0, ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT("┌ Mobile ────────────────────┐"),
                    LINE_TEXT("│ +20 %s │", 10, 0, ""),
                    LINE_DEFAULT("└────────────────────────────┘"),
                    LINE_DEFAULT(" "),
                    LINE_DIALOGUE("Modify", DIALOG_YES),
                    LINE_DIALOGUE("Back", DIALOG_DISCARD)}};

    PromptInputs results = display_box_prompt(&modifySubPage);

    if (results.dialogueValue == DIALOG_DISCARD) {
        free_result(results);
        return ACC_MODIFY;
    }

    // phone number must be 10 characters (excluding the "+ 20")
    if (strlen(results.textInputs[2]) != 10) {
        BoxContent errorPage = {
            .title = FG_RED "ERROR",
            .content = {
                LINE_DEFAULT(FG_RED "Phone Number is not valid"),
                LINE_DEFAULT(" "),
                LINE_DIALOGUE("Okay", DIALOG_PROCEED)}};

        PromptInputs errorResult = display_box_prompt(&errorPage);

        free_result(results);
        return ACC_MODIFY;
    }

    // TODO send data to functions

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
    menuFunctions[ACC_MODIFY] = acc_modify_func;
    menuFunctions[ACC_MODIFY_SUB] = acc_modify_sub_func;

    // Runs the main looping
    MenuIndex currentIndex = ENTRY;
    while (currentIndex != RETURN) {
        currentIndex = menuFunctions[currentIndex]();
    }
}
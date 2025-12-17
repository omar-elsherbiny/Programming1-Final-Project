// menus.c
#include <stdio.h>
#include <stdlib.h>
#include "menus.h"
#include "display.h"
#include "functions.h"

typedef enum {
    RETURN = -1,
    ENTRY = 0,
    LOGIN,
    QUIT,
} MenuIndex;

static MenuIndex (*menuFunctions[100])();

void free_result(PromptInputs results) {
    for (int i = 0; i < results.textInputCount; i++){
        free(results.textInputs[i]);
    }    

    free(results.textInputs);
}

// prototype example ↓
// MenuIndex entry_func(params)
// menuFunctions[ENTRY] = entry_func

static MenuIndex entry_func() {
    return LOGIN;
}

MenuIndex login_func() {
    enum DialogOptions {
        DIALOG_LOGIN,
        DIALOG_QUIT
    } dialogOptions;

    BoxContent loginPage = {
        .title = "Login",
        .content = {
            LINE_DEFAULT("┌ Username ──────────────────┐"),
            LINE_TEXT("│ %s │", 25, 0, ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT("┌ Password ──────────────────┐"),
            LINE_TEXT("│ %s │", 50, 1, ""),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Login", DIALOG_LOGIN),
            LINE_DIALOGUE(FG_RED "Quit", DIALOG_QUIT)}};
    
    PromptInputs results = display_box_prompt(&loginPage);
    
    printf("%s", results.textInputs[1]);
    free_result(results);
    if (results.dialogueValue == DIALOG_LOGIN) {
        return RETURN;
    } else if (results.dialogueValue == DIALOG_QUIT) {
        return QUIT;
    }
}

MenuIndex quit_func() {
    enum DialogOptions {
        DIALOG_YES,
        DIALOG_NO
    } dialogOptions;

    BoxContent quitPage = {
        .title = "Quit",
        .content = {
            LINE_DEFAULT("Are you sure you want to quit?"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE(FG_RED "Yes", DIALOG_YES),
            LINE_DIALOGUE("No", DIALOG_NO)}};
    
    PromptInputs results = display_box_prompt(&quitPage);
    
    if (results.dialogueValue == DIALOG_YES) {
        return RETURN;
    } else if (results.dialogueValue == DIALOG_NO) {
        return LOGIN;
    }
}

void mainloop() {
    // Put functions in the menuFunctions Array
    menuFunctions[ENTRY] = entry_func;
    menuFunctions[LOGIN] = login_func;
    menuFunctions[QUIT] = quit_func;

    // Runs the main looping
    MenuIndex currentIndex = ENTRY;
    while (currentIndex != RETURN) {
        currentIndex = menuFunctions[currentIndex]();
    }
}

// temp container
void temp () {
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
            LINE_DIALOGUE("│ " FG_RED "%sOne with an Account number%s" FG_RESET " │", 10),
            LINE_DIALOGUE("│ " FG_RED "%sMultiple with a criteria%s" FG_RESET"   │", 11),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT(" "),
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

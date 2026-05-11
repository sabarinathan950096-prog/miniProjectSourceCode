// Enhanced Bank Account Management System
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structure definition
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    char mobile[15];      // NEW FEATURE 1: Mobile Number
    char accountType[10]; // NEW FEATURE 2: Account Type
    int pin;              // NEW FEATURE 3: PIN Protection
    double balance;
};

// function prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

// NEW FEATURES
void displayAll(FILE *fPtr);      // FEATURE 4: Display all accounts
void searchAccount(FILE *fPtr);   // FEATURE 5: Search account

int main()
{
    FILE *cfPtr;
    unsigned int choice;

    // open file
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        // create file if not exists
        cfPtr = fopen("credit.dat", "wb+");

        struct clientData blankClient = {0, "", "", "", "", 0, 0.0};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
        }

        rewind(cfPtr);
    }

    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        case 5:
            displayAll(cfPtr);
            break;

        case 6:
            searchAccount(cfPtr);
            break;

        default:
            printf("Invalid choice.\n");
        }
    }

    fclose(cfPtr);

    return 0;
}

// menu
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n========== BANK MANAGEMENT SYSTEM ==========\n");
    printf("1 - Store formatted text file\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Display all accounts\n");
    printf("6 - Search account\n");
    printf("7 - Exit\n");
    printf("Enter choice: ");

    scanf("%u", &choice);

    return choice;
}

// create text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;

    struct clientData client = {0, "", "", "", "", 0, 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        printf("File could not be created.\n");
    }
    else
    {
        rewind(readPtr);

        fprintf(writePtr,
                "%-6s%-15s%-15s%-15s%-12s%-10s\n",
                "Acct",
                "Last Name",
                "First Name",
                "Mobile",
                "Type",
                "Balance");

        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            if (client.acctNum != 0)
            {
                fprintf(writePtr,
                        "%-6d%-15s%-15s%-15s%-12s%-10.2f\n",
                        client.acctNum,
                        client.lastName,
                        client.firstName,
                        client.mobile,
                        client.accountType,
                        client.balance);
            }
        }

        fclose(writePtr);

        printf("accounts.txt created successfully.\n");
    }
}

// add new record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", "", "", 0, 0.0};

    unsigned int accountNum;

    printf("Enter new account number (1-100): ");
    scanf("%u", &accountNum);

    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
    }
    else
    {
        printf("Enter lastname firstname mobile accountType balance pin\n");

        scanf("%14s%14s%14s%9s%lf%d",
              client.lastName,
              client.firstName,
              client.mobile,
              client.accountType,
              &client.balance,
              &client.pin);

        client.acctNum = accountNum;

        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        printf("Account added successfully.\n");
    }
}

// update record
void updateRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", "", "", 0, 0.0};

    unsigned int account;
    int enteredPin;
    double transaction;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
    }
    else
    {
        // PIN verification
        printf("Enter PIN: ");
        scanf("%d", &enteredPin);

        if (enteredPin != client.pin)
        {
            printf("Incorrect PIN.\n");
            return;
        }

        printf("Current Balance: %.2f\n", client.balance);

        printf("Enter amount (+deposit / -withdraw): ");
        scanf("%lf", &transaction);

        // prevent negative balance
        if (client.balance + transaction < 0)
        {
            printf("Insufficient balance.\n");
            return;
        }

        client.balance += transaction;

        fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);

        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        printf("Updated Balance: %.2f\n", client.balance);
    }
}

// delete record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", "", "", 0, 0.0};

    unsigned int accountNum;
    int enteredPin;

    printf("Enter account number to delete: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
    }
    else
    {
        printf("Enter PIN: ");
        scanf("%d", &enteredPin);

        if (enteredPin != client.pin)
        {
            printf("Incorrect PIN.\n");
            return;
        }

        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);

        printf("Account deleted successfully.\n");
    }
}

// FEATURE: Display all accounts
void displayAll(FILE *fPtr)
{
    struct clientData client = {0, "", "", "", "", 0, 0.0};

    rewind(fPtr);

    printf("\n%-6s%-15s%-15s%-15s%-12s%-10s\n",
           "Acct",
           "Last Name",
           "First Name",
           "Mobile",
           "Type",
           "Balance");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6d%-15s%-15s%-15s%-12s%-10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.mobile,
                   client.accountType,
                   client.balance);
        }
    }
}

// FEATURE: Search account
void searchAccount(FILE *fPtr)
{
    struct clientData client = {0, "", "", "", "", 0, 0.0};

    unsigned int account;

    printf("Enter account number to search: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\nAccount Found\n");
        printf("Account Number : %d\n", client.acctNum);
        printf("Name           : %s %s\n",
               client.firstName,
               client.lastName);
        printf("Mobile         : %s\n", client.mobile);
        printf("Account Type   : %s\n", client.accountType);
        printf("Balance        : %.2f\n", client.balance);
    }
}

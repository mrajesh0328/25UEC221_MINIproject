

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILE_NAME "bank.dat"
#define TRANS_FILE "transactions.dat"
#define MAX_DAILY_LIMIT 50000

struct Account
{
    int acctNum;
    char firstName[30];
    char lastName[30];
    char type[20];
    int pin;
    double balance;
    int status; // 0 active, 1 frozen
};

struct Transaction
{
    int acctNum;
    char type[30];
    double amount;
    char date[50];
};

int generateAccountNumber();
void createAccount();
void displayAccounts();
void searchAccount();
void depositMoney();
void withdrawMoney();
void transferMoney();
void deleteAccount();
void transactionHistory();
void calculateInterest();
void freezeAccount();
void generateReport();
void backupDatabase();
void miniStatement();
int adminLogin();
void saveTransaction(int acct, char type[], double amount);

int main()
{
    int choice;

    if (!adminLogin())
    {
        printf("Access Denied\n");
        return 0;
    }

    while (1)
    {
        printf("\n========== BANK MENU ==========\n");

        printf("1. Create Account\n");
        printf("2. Display All Accounts\n");
        printf("3. Search Account\n");
        printf("4. Deposit Money\n");
        printf("5. Withdraw Money\n");
        printf("6. Transfer Money\n");
        printf("7. Delete Account\n");
        printf("8. Transaction History\n");
        printf("9. Calculate Interest\n");
        printf("10. Freeze Account\n");
        printf("11. Generate Report\n");
        printf("12. Backup Database\n");
        printf("13. Mini Statement\n");
        printf("14. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createAccount();
            break;

        case 2:
            displayAccounts();
            break;

        case 3:
            searchAccount();
            break;

        case 4:
            depositMoney();
            break;

        case 5:
            withdrawMoney();
            break;

        case 6:
            transferMoney();
            break;

        case 7:
            deleteAccount();
            break;

        case 8:
            transactionHistory();
            break;

        case 9:
            calculateInterest();
            break;

        case 10:
            freezeAccount();
            break;

        case 11:
            generateReport();
            break;

        case 12:
            backupDatabase();
            break;

        case 13:
            miniStatement();
            break;

        case 14:
            exit(0);

        default:
            printf("Invalid choice\n");
        }
    }

    return 0;
}

int adminLogin()
{
    char user[20], pass[20];

    printf("Admin Username: ");
    scanf("%s", user);

    printf("Admin Password: ");
    scanf("%s", pass);

    if (strcmp(user, "rajesh") == 0 &&
        strcmp(pass, "rajesh03") == 0)
    {
        return 1;
    }

    return 0;
}

int generateAccountNumber()
{
    FILE *fp;
    struct Account acc;
    int last = 1000;

    fp = fopen(FILE_NAME, "rb");

    if (fp == NULL)
        return 1001;

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        last = acc.acctNum;
    }

    fclose(fp);

    return last + 1;
}

void createAccount()
{
    FILE *fp;

    struct Account acc;

    fp = fopen(FILE_NAME, "ab");

    acc.acctNum = generateAccountNumber();

    printf("First Name: ");
    scanf("%s", acc.firstName);

    printf("Last Name: ");
    scanf("%s", acc.lastName);

    printf("Account Type (Savings/Current): ");
    scanf("%s", acc.type);

    printf("PIN: ");
    scanf("%d", &acc.pin);

    printf("Initial Balance: ");
    scanf("%lf", &acc.balance);

    acc.status = 0;

    fwrite(&acc, sizeof(acc), 1, fp);

    fclose(fp);

    saveTransaction(acc.acctNum, "ACCOUNT CREATED", acc.balance);

    printf("Account Created Successfully\n");
    printf("Generated Account Number: %d\n", acc.acctNum);
}

void displayAccounts()
{
    FILE *fp;
    struct Account acc;

    fp = fopen(FILE_NAME, "rb");

    if (fp == NULL)
    {
        printf("No records found\n");
        return;
    }

    printf("\n======= ACCOUNT LIST =======\n");

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        printf("\nAccount No: %d\n", acc.acctNum);
        printf("Name: %s %s\n", acc.firstName, acc.lastName);
        printf("Type: %s\n", acc.type);
        printf("Balance: %.2lf\n", acc.balance);

        if (acc.status == 0)
            printf("Status: ACTIVE\n");
        else
            printf("Status: FROZEN\n");
    }

    fclose(fp);
}

void searchAccount()
{
    FILE *fp;
    struct Account acc;
    int num, found = 0;

    printf("Enter Account Number: ");
    scanf("%d", &num);

    fp = fopen(FILE_NAME, "rb");

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (acc.acctNum == num)
        {
            found = 1;

            printf("\nAccount Found\n");

            printf("Name: %s %s\n",
                   acc.firstName,
                   acc.lastName);

            printf("Balance: %.2lf\n",
                   acc.balance);

            break;
        }
    }

    fclose(fp);

    if (!found)
        printf("Account Not Found\n");
}

void depositMoney()
{
    FILE *fp;
    struct Account acc;
    int num, found = 0;
    double amount;

    printf("Account Number: ");
    scanf("%d", &num);

    printf("Amount to Deposit: ");
    scanf("%lf", &amount);

    fp = fopen(FILE_NAME, "rb+");

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (acc.acctNum == num)
        {
            found = 1;

            acc.balance += amount;

            fseek(fp, -sizeof(acc), SEEK_CUR);

            fwrite(&acc, sizeof(acc), 1, fp);

            saveTransaction(num, "DEPOSIT", amount);

            printf("Deposit Successful\n");

            break;
        }
    }

    fclose(fp);

    if (!found)
        printf("Account Not Found\n");
}

void withdrawMoney()
{
    FILE *fp;
    struct Account acc;
    int num, pin, found = 0;
    double amount;

    printf("Account Number: ");
    scanf("%d", &num);

    printf("PIN: ");
    scanf("%d", &pin);

    printf("Amount: ");
    scanf("%lf", &amount);

    if (amount > MAX_DAILY_LIMIT)
    {
        printf("Daily Limit Exceeded\n");
        return;
    }

    fp = fopen(FILE_NAME, "rb+");

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (acc.acctNum == num &&
            acc.pin == pin)
        {
            found = 1;

            if (acc.status == 1)
            {
                printf("Account Frozen\n");
                break;
            }

            if (acc.balance < amount)
            {
                printf("Insufficient Balance\n");
                break;
            }

            acc.balance -= amount;

            fseek(fp, -sizeof(acc), SEEK_CUR);

            fwrite(&acc, sizeof(acc), 1, fp);

            saveTransaction(num, "WITHDRAW", amount);

            printf("Withdrawal Successful\n");

            break;
        }
    }

    fclose(fp);

    if (!found)
        printf("Invalid Account/PIN\n");
}

void transferMoney()
{
    FILE *fp;
    struct Account sender, receiver;
    int from, to, found1 = 0, found2 = 0;
    double amount;

    printf("Sender Account: ");
    scanf("%d", &from);

    printf("Receiver Account: ");
    scanf("%d", &to);

    printf("Amount: ");
    scanf("%lf", &amount);

    fp = fopen(FILE_NAME, "rb+");

    while (fread(&sender, sizeof(sender), 1, fp))
    {
        if (sender.acctNum == from)
        {
            found1 = 1;

            if (sender.balance < amount)
            {
                printf("Insufficient Balance\n");
                fclose(fp);
                return;
            }

            sender.balance -= amount;

            fseek(fp, -sizeof(sender), SEEK_CUR);

            fwrite(&sender, sizeof(sender), 1, fp);

            break;
        }
    }

    rewind(fp);

    while (fread(&receiver, sizeof(receiver), 1, fp))
    {
        if (receiver.acctNum == to)
        {
            found2 = 1;

            receiver.balance += amount;

            fseek(fp, -sizeof(receiver), SEEK_CUR);

            fwrite(&receiver, sizeof(receiver), 1, fp);

            break;
        }
    }

    fclose(fp);

    if (found1 && found2)
    {
        saveTransaction(from, "TRANSFER SENT", amount);
        saveTransaction(to, "TRANSFER RECEIVED", amount);

        printf("Transfer Successful\n");
    }
    else
    {
        printf("Transfer Failed\n");
    }
}

void deleteAccount()
{
    FILE *fp, *temp;
    struct Account acc;
    int num;

    printf("Enter Account Number: ");
    scanf("%d", &num);

    fp = fopen(FILE_NAME, "rb");
    temp = fopen("temp.dat", "wb");

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (acc.acctNum != num)
        {
            fwrite(&acc, sizeof(acc), 1, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.dat", FILE_NAME);

    printf("Account Deleted\n");
}

void saveTransaction(int acct, char type[], double amount)
{
    FILE *fp;
    struct Transaction tr;
    time_t t;

    fp = fopen(TRANS_FILE, "ab");

    tr.acctNum = acct;

    strcpy(tr.type, type);

    tr.amount = amount;

    time(&t);

    strcpy(tr.date, ctime(&t));

    fwrite(&tr, sizeof(tr), 1, fp);

    fclose(fp);
}

void transactionHistory()
{
    FILE *fp;
    struct Transaction tr;
    int num;

    printf("Enter Account Number: ");
    scanf("%d", &num);

    fp = fopen(TRANS_FILE, "rb");

    while (fread(&tr, sizeof(tr), 1, fp))
    {
        if (tr.acctNum == num)
        {
            printf("\n%s", tr.date);

            printf("%s : %.2lf\n",
                   tr.type,
                   tr.amount);
        }
    }

    fclose(fp);
}

void calculateInterest()
{
    FILE *fp;
    struct Account acc;

    double rate = 0.05;

    fp = fopen(FILE_NAME, "rb+");

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (strcmp(acc.type, "Savings") == 0)
        {
            acc.balance += acc.balance * rate;

            fseek(fp, -sizeof(acc), SEEK_CUR);

            fwrite(&acc, sizeof(acc), 1, fp);
        }
    }

    fclose(fp);

    printf("Interest Added Successfully\n");
}

void freezeAccount()
{
    FILE *fp;
    struct Account acc;
    int num;

    printf("Enter Account Number: ");
    scanf("%d", &num);

    fp = fopen(FILE_NAME, "rb+");

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (acc.acctNum == num)
        {
            acc.status = 1;

            fseek(fp, -sizeof(acc), SEEK_CUR);

            fwrite(&acc, sizeof(acc), 1, fp);

            printf("Account Frozen\n");

            break;
        }
    }

    fclose(fp);
}

void generateReport()
{
    FILE *fp;
    struct Account acc;

    int total = 0;

    double bankBalance = 0;

    fp = fopen(FILE_NAME, "rb");

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        total++;

        bankBalance += acc.balance;
    }

    fclose(fp);

    printf("\n======= BANK REPORT =======\n");

    printf("Total Customers: %d\n", total);

    printf("Total Bank Balance: %.2lf\n",
           bankBalance);
}

void backupDatabase()
{
    FILE *src, *dest;
    char ch;

    src = fopen(FILE_NAME, "rb");

    dest = fopen("backup.dat", "wb");

    while ((ch = fgetc(src)) != EOF)
    {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);

    printf("Backup Created Successfully\n");
}

void miniStatement()
{
    transactionHistory();
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // For isupper, islower, etc.
#include <time.h>   // For random password generation

// Define the Account structure to hold website, username, and encrypted password
typedef struct Account {
    char website[100];   // Stores the website name
    char username[100];  // Stores the username for the account
    char password[100];  // Stores the encrypted password
} Account;

// Define a Node structure for the linked list to store multiple accounts
typedef struct Node {
    Account acc;         // The account data
    struct Node* next;   // Pointer to the next node in the list
} Node;

// Global variables
Node* head = NULL;       // Head of the linked list for accounts
char masterPassword[20] = "admin";  // Hardcoded master password for simplicity
const char* dataFile = "passwords.txt";  // File to store persistent data

// Function to encrypt a password using XOR cipher with key 0xAA
// XOR is a simple symmetric encryption where each character is XORed with a key
// This is reversible: encrypting twice with same key decrypts
void encryptPassword(char* password) {
    int len = strlen(password);
    for (int i = 0; i < len; i++) {
        password[i] ^= 0xAA;  // XOR each character with 0xAA
    }
}

// Function to decrypt a password (same as encrypt since XOR is symmetric)
void decryptPassword(char* password) {
    encryptPassword(password);  // Reuse encrypt function
}

// Function to add a new account to the linked list
void addAccount(char* website, char* username, char* password) {
    // Allocate memory for new node
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    // Copy data to the new node
    strcpy(newNode->acc.website, website);
    strcpy(newNode->acc.username, username);
    strcpy(newNode->acc.password, password);
    encryptPassword(newNode->acc.password);  // Encrypt before storing
    newNode->next = head;  // Insert at beginning
    head = newNode;
    printf("Account added successfully!\n");
}

// Function to check password strength
// Returns 1 if strong (length >=8, has upper, lower, digit, symbol), else 0
int checkPasswordStrength(char* password) {
    int len = strlen(password);
    if (len < 8) return 0;  // Minimum length 8
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSymbol = 0;
    for (int i = 0; i < len; i++) {
        if (isupper(password[i])) hasUpper = 1;
        else if (islower(password[i])) hasLower = 1;
        else if (isdigit(password[i])) hasDigit = 1;
        else hasSymbol = 1;  // Any non-alphanumeric is symbol
    }
    return hasUpper && hasLower && hasDigit && hasSymbol;
}

// Function to generate a random strong password
// Uses uppercase, lowercase, digits, symbols
void generatePassword(char* password) {
    const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";
    int len = 12;  // Fixed length for strong password
    srand(time(NULL));  // Seed random number generator
    for (int i = 0; i < len; i++) {
        password[i] = chars[rand() % strlen(chars)];
    }
    password[len] = '\0';
}

// Function to display all accounts (decrypt passwords)
void viewAccounts() {
    if (head == NULL) {
        printf("No accounts stored.\n");
        return;
    }
    Node* temp = head;
    printf("Stored Accounts:\n");
    while (temp != NULL) {
        char decrypted[100];
        strcpy(decrypted, temp->acc.password);
        decryptPassword(decrypted);  // Decrypt to show
        printf("Website: %s, Username: %s, Password: %s\n", temp->acc.website, temp->acc.username, decrypted);
        temp = temp->next;
    }
}

// Function to search for an account by website name
void searchAccount(char* website) {
    Node* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->acc.website, website) == 0) {
            char decrypted[100];
            strcpy(decrypted, temp->acc.password);
            decryptPassword(decrypted);
            printf("Found: Website: %s, Username: %s, Password: %s\n", temp->acc.website, temp->acc.username, decrypted);
            return;
        }
        temp = temp->next;
    }
    printf("Account not found.\n");
}

// Function to save accounts to file
void saveToFile() {
    FILE* file = fopen(dataFile, "w");
    if (file == NULL) {
        printf("Error opening file for saving.\n");
        return;
    }
    Node* temp = head;
    while (temp != NULL) {
        fprintf(file, "%s|%s|%s\n", temp->acc.website, temp->acc.username, temp->acc.password);
        temp = temp->next;
    }
    fclose(file);
    printf("Data saved to file.\n");
}

// Function to load accounts from file
void loadFromFile() {
    FILE* file = fopen(dataFile, "r");
    if (file == NULL) {
        return;  // File doesn't exist, no data to load
    }
    char line[300];
    while (fgets(line, sizeof(line), file)) {
        char website[100], username[100], password[100];
        if (sscanf(line, "%[^|]|%[^|]|%s", website, username, password) == 3) {
            // Add directly without encrypting again (already encrypted in file)
            Node* newNode = (Node*)malloc(sizeof(Node));
            strcpy(newNode->acc.website, website);
            strcpy(newNode->acc.username, username);
            strcpy(newNode->acc.password, password);
            newNode->next = head;
            head = newNode;
        }
    }
    fclose(file);
}

// Function to free the linked list memory
void freeList() {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Main function with menu-driven interface
int main() {
    loadFromFile();  // Load existing data on start
    int loggedIn = 0;
    char inputPassword[20];

    // Master password login
    printf("Welcome to SafePass Password Manager\n");
    printf("Enter master password: ");
    scanf("%s", inputPassword);
    if (strcmp(inputPassword, masterPassword) != 0) {
        printf("Incorrect password. Exiting.\n");
        return 1;
    }
    loggedIn = 1;
    printf("Login successful!\n");

    int choice;
    while (1) {
        printf("\nMenu:\n");
        printf("1. Add Account\n");
        printf("2. View All Accounts\n");
        printf("3. Search Account by Website\n");
        printf("4. Generate Strong Password\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            char website[100], username[100], password[100];
            printf("Enter website: ");
            scanf("%s", website);
            printf("Enter username: ");
            scanf("%s", username);
            printf("Enter password: ");
            scanf("%s", password);
            if (!checkPasswordStrength(password)) {
                printf("Password is weak. Use at least 8 characters with upper, lower, digit, and symbol.\n");
                printf("Generate a strong password? (y/n): ");
                char gen;
                scanf(" %c", &gen);
                if (gen == 'y' || gen == 'Y') {
                    generatePassword(password);
                    printf("Generated password: %s\n", password);
                } else {
                    continue;
                }
            }
            addAccount(website, username, password);
        } else if (choice == 2) {
            viewAccounts();
        } else if (choice == 3) {
            char website[100];
            printf("Enter website to search: ");
            scanf("%s", website);
            searchAccount(website);
        } else if (choice == 4) {
            char password[100];
            generatePassword(password);
            printf("Generated strong password: %s\n", password);
        } else if (choice == 5) {
            saveToFile();  // Save before exit
            freeList();    // Free memory
            printf("Exiting SafePass.\n");
            printf("Future Scope: This console project can be extended into a real website or mobile app with database and advanced encryption.\n");
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
    return 0;
}

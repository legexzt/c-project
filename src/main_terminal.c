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
    printf("\n✓ Account added successfully!\n");
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
        printf("\n⚠ No accounts stored.\n");
        return;
    }
    Node* temp = head;
    printf("\n╔════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                         STORED ACCOUNTS                                ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════╣\n");
    int count = 1;
    while (temp != NULL) {
        char decrypted[100];
        strcpy(decrypted, temp->acc.password);
        decryptPassword(decrypted);  // Decrypt to show
        printf("║ %d. Website:  %-56s ║\n", count, temp->acc.website);
        printf("║    Username: %-56s ║\n", temp->acc.username);
        printf("║    Password: %-56s ║\n", decrypted);
        printf("╠════════════════════════════════════════════════════════════════════════╣\n");
        temp = temp->next;
        count++;
    }
    printf("╚════════════════════════════════════════════════════════════════════════╝\n");
}

// Function to search for an account by website name
void searchAccount(char* website) {
    Node* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->acc.website, website) == 0) {
            char decrypted[100];
            strcpy(decrypted, temp->acc.password);
            decryptPassword(decrypted);
            printf("\n✓ Account Found!\n");
            printf("  Website:  %s\n", temp->acc.website);
            printf("  Username: %s\n", temp->acc.username);
            printf("  Password: %s\n", decrypted);
            return;
        }
        temp = temp->next;
    }
    printf("\n✗ Account not found.\n");
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
    printf("✓ Data saved to file.\n");
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

// Function to delete an account by website
void deleteAccount(char* website) {
    Node* temp = head;
    Node* prev = NULL;
    while (temp != NULL) {
        if (strcmp(temp->acc.website, website) == 0) {
            if (prev == NULL) {
                head = temp->next;
            } else {
                prev->next = temp->next;
            }
            free(temp);
            printf("\n✓ Account deleted successfully!\n");
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("\n✗ Account not found.\n");
}

// Function to edit an account (update password)
void editAccount(char* website, char* newPassword) {
    Node* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->acc.website, website) == 0) {
            strcpy(temp->acc.password, newPassword);
            encryptPassword(temp->acc.password);
            printf("\n✓ Account updated successfully!\n");
            return;
        }
        temp = temp->next;
    }
    printf("\n✗ Account not found.\n");
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

// Function to clear screen (cross-platform)
void clearScreen() {
    printf("\033[2J\033[H");  // ANSI escape codes
}

// Function to display menu
void displayMenu() {
    printf("\n╔════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                   SafePass Password Manager                            ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════╣\n");
    printf("║  1. Add Account                                                        ║\n");
    printf("║  2. View All Accounts                                                  ║\n");
    printf("║  3. Search Account                                                     ║\n");
    printf("║  4. Generate Strong Password                                           ║\n");
    printf("║  5. Edit Account Password                                              ║\n");
    printf("║  6. Delete Account                                                     ║\n");
    printf("║  7. Save & Exit                                                        ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════╝\n");
    printf("\nEnter your choice: ");
}

// Main function with menu-driven interface
int main() {
    char website[100], username[100], password[100], newPassword[100];
    int choice;
    char masterPass[20];
    
    // Authentication
    clearScreen();
    printf("\n╔════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                   SafePass Password Manager                            ║\n");
    printf("║                          Login Required                                ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════╝\n");
    printf("\nEnter master password: ");
    scanf("%s", masterPass);
    
    if (strcmp(masterPass, masterPassword) != 0) {
        printf("\n✗ Incorrect password! Access denied.\n");
        return 1;
    }
    
    printf("\n✓ Login successful!\n");
    loadFromFile();  // Load existing data
    
    while (1) {
        displayMenu();
        scanf("%d", &choice);
        getchar();  // Clear newline
        
        switch (choice) {
            case 1:  // Add Account
                clearScreen();
                printf("\n=== Add New Account ===\n");
                printf("Website: ");
                fgets(website, 100, stdin);
                website[strcspn(website, "\n")] = 0;  // Remove newline
                
                printf("Username: ");
                fgets(username, 100, stdin);
                username[strcspn(username, "\n")] = 0;
                
                printf("Password: ");
                fgets(password, 100, stdin);
                password[strcspn(password, "\n")] = 0;
                
                if (checkPasswordStrength(password)) {
                    printf("✓ Password Strength: STRONG\n");
                    addAccount(website, username, password);
                } else {
                    printf("⚠ Password Strength: WEAK\n");
                    printf("Continue anyway? (y/n): ");
                    char c;
                    scanf("%c", &c);
                    getchar();
                    if (c == 'y' || c == 'Y') {
                        addAccount(website, username, password);
                    } else {
                        printf("✗ Account not added.\n");
                    }
                }
                break;
                
            case 2:  // View Accounts
                clearScreen();
                viewAccounts();
                break;
                
            case 3:  // Search Account
                clearScreen();
                printf("\n=== Search Account ===\n");
                printf("Enter website name: ");
                fgets(website, 100, stdin);
                website[strcspn(website, "\n")] = 0;
                searchAccount(website);
                break;
                
            case 4:  // Generate Password
                clearScreen();
                generatePassword(password);
                printf("\n=== Generated Strong Password ===\n");
                printf("Password: %s\n", password);
                printf("✓ Password Strength: STRONG\n");
                printf("\n(Copy this password to use when adding an account)\n");
                break;
                
            case 5:  // Edit Account
                clearScreen();
                printf("\n=== Edit Account Password ===\n");
                printf("Website: ");
                fgets(website, 100, stdin);
                website[strcspn(website, "\n")] = 0;
                
                printf("New Password: ");
                fgets(newPassword, 100, stdin);
                newPassword[strcspn(newPassword, "\n")] = 0;
                
                if (checkPasswordStrength(newPassword)) {
                    printf("✓ Password Strength: STRONG\n");
                    editAccount(website, newPassword);
                } else {
                    printf("⚠ Password Strength: WEAK\n");
                    printf("Continue anyway? (y/n): ");
                    char c;
                    scanf("%c", &c);
                    getchar();
                    if (c == 'y' || c == 'Y') {
                        editAccount(website, newPassword);
                    } else {
                        printf("✗ Account not updated.\n");
                    }
                }
                break;
                
            case 6:  // Delete Account
                clearScreen();
                printf("\n=== Delete Account ===\n");
                printf("Enter website name: ");
                fgets(website, 100, stdin);
                website[strcspn(website, "\n")] = 0;
                
                printf("Are you sure you want to delete '%s'? (y/n): ", website);
                char c;
                scanf("%c", &c);
                getchar();
                if (c == 'y' || c == 'Y') {
                    deleteAccount(website);
                } else {
                    printf("✗ Deletion cancelled.\n");
                }
                break;
                
            case 7:  // Exit
                clearScreen();
                printf("\n=== Saving and Exiting ===\n");
                saveToFile();
                freeList();
                printf("\nThank you for using SafePass Password Manager!\n");
                printf("Goodbye! 👋\n\n");
                return 0;
                
            default:
                printf("\n✗ Invalid choice! Please try again.\n");
        }
        
        printf("\nPress Enter to continue...");
        getchar();
        clearScreen();
    }
    
    return 0;
}

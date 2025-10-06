#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // For isupper, islower, etc.
#include <time.h>   // For random password generation
#include <windows.h>
#include <commctrl.h>  // For common controls like ListView

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

// GUI handles
HWND hWebsite, hUsername, hPassword, hAdd, hView, hSearch, hGenerate, hLabelWebsite, hLabelUsername, hLabelPassword, hListView, hDelete, hEdit, hStatusBar, hTitle, hTooltip, hStrengthLabel;

// Control IDs
#define ID_ADD 1
#define ID_VIEW 2
#define ID_SEARCH 3
#define ID_GENERATE 4
#define ID_WEBSITE 5
#define ID_USERNAME 6
#define ID_PASSWORD 7
#define ID_DELETE 8
#define ID_EDIT 9
#define ID_LISTVIEW 10
#define ID_STATUSBAR 11

// Animation variables
int animationX = 0;
int animationY = 0;
int directionX = 1;
int directionY = 1;

// Font handle
HFONT hFont, hBoldFont;

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
            printf("Account deleted successfully!\n");
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Account not found.\n");
}

// Function to edit an account (update password)
void editAccount(char* website, char* newPassword) {
    Node* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->acc.website, website) == 0) {
            strcpy(temp->acc.password, newPassword);
            encryptPassword(temp->acc.password);
            printf("Account updated successfully!\n");
            return;
        }
        temp = temp->next;
    }
    printf("Account not found.\n");
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

// Function to handle WM_CREATE
void OnCreate(HWND hwnd) {
    // Create font
    hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    hBoldFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    // Create title
    hTitle = CreateWindow("STATIC", "SafePass Password Manager", WS_CHILD | WS_VISIBLE | SS_CENTER, 20, 10, 450, 30, hwnd, NULL, NULL, NULL);
    // Create controls
    hLabelWebsite = CreateWindow("STATIC", "Website:", WS_CHILD | WS_VISIBLE, 20, 50, 70, 20, hwnd, NULL, NULL, NULL);
    hWebsite = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 50, 200, 25, hwnd, (HMENU)ID_WEBSITE, NULL, NULL);
    hLabelUsername = CreateWindow("STATIC", "Username:", WS_CHILD | WS_VISIBLE, 20, 90, 80, 20, hwnd, NULL, NULL, NULL);
    hUsername = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 90, 200, 25, hwnd, (HMENU)ID_USERNAME, NULL, NULL);
    hLabelPassword = CreateWindow("STATIC", "Password:", WS_CHILD | WS_VISIBLE, 20, 130, 75, 20, hwnd, NULL, NULL, NULL);
    hPassword = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 130, 200, 25, hwnd, (HMENU)ID_PASSWORD, NULL, NULL);
    hStrengthLabel = CreateWindow("STATIC", "Strength: Weak", WS_CHILD | WS_VISIBLE, 20, 160, 200, 20, hwnd, NULL, NULL, NULL);
    hAdd = CreateWindow("BUTTON", "Add Account", WS_CHILD | WS_VISIBLE, 20, 170, 120, 35, hwnd, (HMENU)ID_ADD, NULL, NULL);
    hView = CreateWindow("BUTTON", "View Accounts", WS_CHILD | WS_VISIBLE, 160, 170, 120, 35, hwnd, (HMENU)ID_VIEW, NULL, NULL);
    hSearch = CreateWindow("BUTTON", "Search", WS_CHILD | WS_VISIBLE, 20, 220, 120, 35, hwnd, (HMENU)ID_SEARCH, NULL, NULL);
    hGenerate = CreateWindow("BUTTON", "Generate Password", WS_CHILD | WS_VISIBLE, 160, 220, 150, 35, hwnd, (HMENU)ID_GENERATE, NULL, NULL);
    hDelete = CreateWindow("BUTTON", "Delete Account", WS_CHILD | WS_VISIBLE, 20, 270, 120, 35, hwnd, (HMENU)ID_DELETE, NULL, NULL);
    hEdit = CreateWindow("BUTTON", "Edit Password", WS_CHILD | WS_VISIBLE, 160, 270, 120, 35, hwnd, (HMENU)ID_EDIT, NULL, NULL);
    // Create ListView for accounts
    hListView = CreateWindow(WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, 20, 320, 450, 200, hwnd, (HMENU)ID_LISTVIEW, NULL, NULL);
    // Add columns to ListView
    LVCOLUMN lvCol;
    lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvCol.cx = 120;
    lvCol.pszText = "Website";
    ListView_InsertColumn(hListView, 0, &lvCol);
    lvCol.cx = 120;
    lvCol.pszText = "Username";
    ListView_InsertColumn(hListView, 1, &lvCol);
    lvCol.cx = 120;
    lvCol.pszText = "Password";
    ListView_InsertColumn(hListView, 2, &lvCol);
    // Create tooltip
    hTooltip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, NULL, NULL);
    // Add tooltips
    TOOLINFO ti = {0};
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.hwnd = hwnd;
    ti.uId = (UINT_PTR)hAdd;
    ti.lpszText = "Add a new account";
    SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    ti.uId = (UINT_PTR)hView;
    ti.lpszText = "View all accounts in the list";
    SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    ti.uId = (UINT_PTR)hSearch;
    ti.lpszText = "Search for an account by website";
    SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    ti.uId = (UINT_PTR)hGenerate;
    ti.lpszText = "Generate a strong password";
    SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    ti.uId = (UINT_PTR)hDelete;
    ti.lpszText = "Delete the selected account";
    SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    ti.uId = (UINT_PTR)hEdit;
    ti.lpszText = "Edit password of selected account";
    SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    // Create status bar
    hStatusBar = CreateWindow(STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwnd, (HMENU)ID_STATUSBAR, NULL, NULL);
    SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)"Ready - Accounts loaded");
    // Set font for all controls
    SendMessage(hTitle, WM_SETFONT, (WPARAM)hBoldFont, TRUE);
    SendMessage(hLabelWebsite, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hWebsite, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hLabelUsername, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hUsername, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hLabelPassword, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hPassword, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hStrengthLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hAdd, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hView, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hSearch, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hGenerate, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hDelete, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    // Set icons for buttons
    SendMessage(hAdd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(NULL, IDI_INFORMATION));
    SendMessage(hView, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(NULL, IDI_WINLOGO));
    SendMessage(hSearch, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(NULL, IDI_QUESTION));
    SendMessage(hGenerate, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(NULL, IDI_ASTERISK));
    SendMessage(hDelete, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(NULL, IDI_ERROR));
    SendMessage(hEdit, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(NULL, IDI_WARNING));
    loadFromFile(); // Load data
    SetTimer(hwnd, 1, 100, NULL); // Start animation timer (reduced frequency to reduce CPU usage)
}

// Function to handle WM_COMMAND
void OnCommand(HWND hwnd, WPARAM wParam) {
    if (LOWORD(wParam) == ID_ADD) {
        char website[100], username[100], password[100];
        GetWindowText(hWebsite, website, 100);
        GetWindowText(hUsername, username, 100);
        GetWindowText(hPassword, password, 100);
        if (!checkPasswordStrength(password)) {
            MessageBox(hwnd, "Password is weak. Please generate a strong one.", "Warning", MB_OK);
        } else {
            addAccount(website, username, password);
            MessageBox(hwnd, "Account added!", "Success", MB_OK);
        }
    } else if (LOWORD(wParam) == ID_VIEW) {
        ListView_DeleteAllItems(hListView);
        Node* temp = head;
        int i = 0;
        while (temp) {
            char decrypted[100];
            strcpy(decrypted, temp->acc.password);
            decryptPassword(decrypted);
            LVITEM lvItem;
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = i;
            lvItem.iSubItem = 0;
            lvItem.pszText = temp->acc.website;
            ListView_InsertItem(hListView, &lvItem);
            ListView_SetItemText(hListView, i, 1, temp->acc.username);
            ListView_SetItemText(hListView, i, 2, decrypted);
            temp = temp->next;
            i++;
        }
    } else if (LOWORD(wParam) == ID_SEARCH) {
        char website[100];
        GetWindowText(hWebsite, website, 100);
        Node* temp = head;
        int found = 0;
        while (temp) {
            if (strcmp(temp->acc.website, website) == 0) {
                char decrypted[100];
                strcpy(decrypted, temp->acc.password);
                decryptPassword(decrypted);
                char msg[200];
                sprintf(msg, "Website: %s\nUsername: %s\nPassword: %s", temp->acc.website, temp->acc.username, decrypted);
                MessageBox(hwnd, msg, "Found", MB_OK);
                found = 1;
                break;
            }
            temp = temp->next;
        }
        if (!found) MessageBox(hwnd, "Not found", "Error", MB_OK);
    } else if (LOWORD(wParam) == ID_GENERATE) {
        char password[100];
        generatePassword(password);
        SetWindowText(hPassword, password);
        if (checkPasswordStrength(password)) {
            SetWindowText(hStrengthLabel, "Strength: Strong");
        } else {
            SetWindowText(hStrengthLabel, "Strength: Weak");
        }
    } else if (LOWORD(wParam) == ID_DELETE) {
        int selected = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
        if (selected != -1) {
            char website[100];
            ListView_GetItemText(hListView, selected, 0, website, 100);
            deleteAccount(website);
            ListView_DeleteItem(hListView, selected);
            MessageBox(hwnd, "Account deleted!", "Success", MB_OK);
        } else {
            MessageBox(hwnd, "Please select an account to delete.", "Error", MB_OK);
        }
    } else if (LOWORD(wParam) == ID_EDIT) {
        int selected = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
        if (selected != -1) {
            char website[100], newPassword[100];
            ListView_GetItemText(hListView, selected, 0, website, 100);
            if (GetWindowText(hPassword, newPassword, 100) > 0) {
                if (!checkPasswordStrength(newPassword)) {
                    MessageBox(hwnd, "Password is weak. Please generate a strong one.", "Warning", MB_OK);
                } else {
                    editAccount(website, newPassword);
                    ListView_SetItemText(hListView, selected, 2, newPassword);
                    MessageBox(hwnd, "Account updated!", "Success", MB_OK);
                }
            } else {
                MessageBox(hwnd, "Please enter a new password.", "Error", MB_OK);
            }
        } else {
            MessageBox(hwnd, "Please select an account to edit.", "Error", MB_OK);
        }
    }
}

// Function to handle WM_PAINT
void OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    // Create memory DC for double buffering to prevent flickering
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, 500, 600);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
    
    // Fill memory DC with background color first
    RECT rect = {0, 0, 500, 600};
    FillRect(hdcMem, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    
    // Gradient background
    TRIVERTEX vert[2] = {
        {0, 0, 0x0000, 0x8000, 0x8000, 0x8000}, // blue
        {500, 600, 0xffff, 0xffff, 0xffff, 0x0000}  // white
    };
    GRADIENT_RECT gRect = {0, 1};
    GradientFill(hdcMem, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
    
    // Animated ellipses
    HBRUSH hBrush1 = CreateSolidBrush(RGB(255, 255, 0)); // yellow
    HBRUSH hBrushOld1 = (HBRUSH)SelectObject(hdcMem, hBrush1);
    Ellipse(hdcMem, animationX, 50, animationX + 50, 100);
    SelectObject(hdcMem, hBrushOld1);
    DeleteObject(hBrush1);
    
    HBRUSH hBrush2 = CreateSolidBrush(RGB(255, 0, 255)); // magenta
    HBRUSH hBrushOld2 = (HBRUSH)SelectObject(hdcMem, hBrush2);
    Ellipse(hdcMem, 200, animationY, 250, animationY + 50);
    SelectObject(hdcMem, hBrushOld2);
    DeleteObject(hBrush2);
    
    // Copy memory DC to screen
    BitBlt(hdc, 0, 0, 500, 600, hdcMem, 0, 0, SRCCOPY);
    
    // Clean up
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    
    EndPaint(hwnd, &ps);
}

// Window procedure for GUI
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE:
            OnCreate(hwnd);
            break;
        case WM_COMMAND:
            OnCommand(hwnd, wParam);
            break;
        case WM_TIMER:
            animationX += directionX * 5;
            if (animationX > 450) directionX = -1;
            if (animationX < 0) directionX = 1;
            animationY += directionY * 3;
            if (animationY > 550) directionY = -1;
            if (animationY < 0) directionY = 1;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case WM_PAINT:
            OnPaint(hwnd);
            break;
        case WM_CLOSE:
            KillTimer(hwnd, 1);
            saveToFile();
            freeList();
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            DeleteObject(hFont);
            DeleteObject(hBoldFont);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Main function with menu-driven interface
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    InitCommonControls();  // Initialize common controls for ListView
    // Register window class
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "PasswordManager";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    // Create window
    HWND hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "PasswordManager",
        "SafePass Password Manager",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 600,
        NULL, NULL, hInstance, NULL);
    if(hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    // Message loop
    MSG Msg;
    while(GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

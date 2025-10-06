# Password Manager

A secure password manager application written in C with a GUI interface using Windows API.

## Features

- 🔐 **Secure Storage**: Passwords are encrypted using XOR cipher before storage
- 👤 **Account Management**: Add, view, edit, and delete account credentials
- 🔍 **Search Functionality**: Quickly find stored accounts
- 🎲 **Password Generator**: Generate strong random passwords
- 💪 **Password Strength Checker**: Validates password complexity
- 💾 **Persistent Storage**: Data saved to `passwords.txt` file
- 🖥️ **GUI Interface**: User-friendly Windows-based graphical interface

## Project Structure

```
c-project/
├── src/           # Source code files
│   └── main.c     # Main application code
├── build/         # Compiled binaries and build artifacts
├── docs/          # Documentation
├── passwords.txt  # Encrypted password storage (generated at runtime)
├── Makefile       # Build automation
├── .gitignore     # Git ignore rules
└── README.md      # This file
```

## Requirements

- Windows OS (uses Windows API for GUI)
- GCC compiler (MinGW for Windows)
- Make (optional, for using Makefile)

## Building

### Using Make:
```bash
make
```

### Manual Compilation:
```bash
gcc src/main.c -o build/password_manager.exe -lcomctl32 -lgdi32
```

## Usage

1. Run the compiled executable from the `build/` directory
2. Default master password: `admin`
3. Use the GUI to:
   - Add new accounts with website, username, and password
   - View all stored accounts
   - Search for specific accounts
   - Generate strong passwords
   - Edit or delete existing accounts

## Security Note

⚠️ **Important**: This is a demonstration project. The encryption method (simple XOR cipher) is not suitable for production use. For real-world applications, use industry-standard encryption libraries like OpenSSL or libsodium.

## License

This project is open source and available for educational purposes.
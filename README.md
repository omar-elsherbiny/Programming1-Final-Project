# Programming I Final Project
A simple TUI bank management system implemented in pure C, without using any external libraries

This project was developed as part of the Programming 1 course for the Computer & Communications Engineering program at Alexandria University (Fall 2025)

The project requirements can be found here:
[Project Instructions](https://github.com/omar-elsherbiny/Programming1-Final-Project/blob/main/Report/Project-Instructions.pdf)

Documentation for the project can be found here:
[Project Report](https://github.com/omar-elsherbiny/Programming1-Final-Project/blob/main/Report/Project-Report.pdf)

## Features
- Create, view, update, and delete bank accounts
- Deposit, withdraw and transfer money
- Persistent data storage with files
- Keyboard-driven terminal interface

## Project Structure
```
.
├─ files/
│  ├─ users.txt        # Admin login details
│  ├─ accounts.txt     # Customer account details
│  └─ accounts/*.txt   # Individual customer transaction history
├─ src/                # Source code files
├─ include/            # Header files
├─ Report/             # Project report and instructions
├─ Makefile            # Build configuration
└─ README.md
```

## Building
Make sure you have a C compiler (such as `gcc`) and `make` installed

To build the application and generate executable, run:
```bash
make
```

To remove all compiled object files and any bank account data files used by the program, run:
```bash
make clean
```

## Running the Program
To run the executable, use:
```bash
./app
```
you must include a `users.txt` in `/files/` in order to login

write plaintext username and password pairs, for example:
```
username1 password1
username2 password2
```

## Controls & Navigation
The program uses a keyboard-driven interface for the TUI:
- `↑ / ↓` or `TAB` to cycle through menu options
- `ENTER` to confirm the currently selected option
- `← / →` to navigate within truncated text input fields
- `ESC` to immediately terminate the app

## Contributors

<p>
  <img src="https://github.com/zieddev.png" width="30" style="vertical-align:middle;border-radius:50%;">
  <a href="https://github.com/zieddev" style="vertical-align:middle;padding-left:5px;">zieddev</a>
</p>
<p>
  <img src="https://github.com/realhesh.png" width="30" style="vertical-align:middle;border-radius:50%;">
  <a href="https://github.com/realhesh" style="vertical-align:middle;padding-left:5px;">realhesh</a>
</p>
<p>
  <img src="https://github.com/omar-elsherbiny.png" width="30" style="vertical-align:middle;border-radius:50%;">
  <a href="https://github.com/omar-elsherbiny" style="vertical-align:middle;padding-left:5px;">omar-elsherbiny</a>
</p>
<p>
  <img src="https://github.com/YasseenKamel.png" width="30" style="vertical-align:middle;border-radius:50%;">
  <a href="https://github.com/YasseenKamel" style="vertical-align:middle;padding-left:5px;">YasseenKamel</a>
</p>

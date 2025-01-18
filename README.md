# Bank System

This is a simple bank system implemented in C++ using SQLite for database management. The system allows you to create accounts, deposit and withdraw funds, transfer funds between accounts, and update account information.

## Features

- Create a new account
- Deposit funds into an account
- Withdraw funds from an account
- Display account information
- Delete an account
- View all accounts
- Search for an account by account number
- Transfer funds between accounts
- Update account name

## Requirements

- C++11 or later
- SQLite3

## Setup

1. **Clone the repository**:
   ```sh
   git clone https://github.com/yourusername/your-repo-name.git
   cd your-repo-name

2. Install SQLite3: On macOS, you can use Homebrew to install SQLite3:
    `brew install sqlite`

3. Build the project: Create a CMakeLists.txt file if you don't have one, and use CMake to build the project:

```
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(bank_system)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SQLite3 package
find_package(SQLite3 REQUIRED)

# Add the executable
add_executable(bank_system main.cpp)

# Link SQLite3 library
target_link_libraries(bank_system SQLite::SQLite3)```

4. Then, run the following commands to build the project:

```
mkdir build
cd build
cmake ..
make```

5. Run the project:
```
./bank_system```

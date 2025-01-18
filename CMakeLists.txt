cmake_minimum_required(VERSION 3.10)

# Set the project name
project(bank_system)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
# add_executable(bank_system main.cpp)
add_executable(bank_system main.cpp)


target_link_libraries(bank_system sqlite3)
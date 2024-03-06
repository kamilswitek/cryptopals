cmake_minimum_required(VERSION 3.12)

project(Set1 VERSION 1.0)

add_executable(task1 1.cpp)

target_compile_definitions(task1 PUBLIC TASK_4_INPUT_FILE_PATH="/home/swift/cryptopals/1/4.txt")
target_compile_definitions(task1 PUBLIC TASK_4_OUTPUT_FILE_PATH="/home/swift/cryptopals/1/4_output.txt")

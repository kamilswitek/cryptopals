cmake_minimum_required(VERSION 3.12)

project(CryptoPals VERSION 1.0)

add_executable(CryptoPals 
    CryptoPals.cpp
    CryptoMethods/CryptoMethods.cpp
    FormatConversions/FormatConversions.cpp
    Metrics/Metrics.cpp
    Printer/Printer.cpp
    Solutions/Solutions.cpp
)

target_include_directories(CryptoPals PUBLIC
    CryptoMethods
    FormatConversions
    Metrics
    Printer
    Solutions
    Misc
)
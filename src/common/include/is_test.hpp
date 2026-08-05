#pragma once
#include <string>
#include <iostream>

using namespace std;

// extracting variable to check if it's a test
inline string is_test()
{
    // getting table name
    const char* isTest = std::getenv("IS_TEST");
    if (!isTest) {
        cerr << "TABLE_NAME not defined" << std::endl;
        return "";
    }
    return isTest;
}
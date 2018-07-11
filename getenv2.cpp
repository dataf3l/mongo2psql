#include <iostream>
#include <cstdlib>
using namespace std;

std::string getenv2(string envkey, string envdefault){
    char const* the_value= getenv(envkey.c_str());
    if (the_value == NULL){
        cout << "ENV:"<<envkey<<" MISSING \n";
        return envdefault;
    }
    string s_the_value(the_value);
    cout << "DEBUG:ENV:"<<envkey<<" = "<< the_value <<"\n";
    return the_value;
}


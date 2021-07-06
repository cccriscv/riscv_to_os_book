#include<iostream>
#include<stdio.h>
#include<stdlib.h>
int main() {
    char c;
   // Set the terminal to raw mode
    system("stty raw");
    while(1) {
        c = getchar();
        // terminate when "." is pressed
        if(c == '.') {
            system("stty cooked");
            exit(0);
        }  
        std::cout << c << " was pressed."<< std::endl;
    }
}
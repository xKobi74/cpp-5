/*
g++ -std=c++23 -Wall -Wextra 02.03.cpp -o 02.03.out
./02.03.out
*/

#include <iostream>
#include <format>

int main()
{
    char c;
    std::cin >> c;
    
    switch (c) 
    {
        case '0' ... '9' : 
        {
            std::cout << "цифра" << std::endl;
            break;
        }
        case 'A' ... 'Z' : 
        {
            std::cout << "заглавная буква" << std::endl;
            break;
        }
        case 'a' ... 'z' : 
        {
            std::cout << "строчная буква" << std::endl;
            break;
        }
        case '.' :
        case ',' :
        case ':' :
        case ';' :
        case '!' :
        case '?' :
        {
            std::cout << "строчная буква" << std::endl;
            break;
        }
        default :
        {
            std::cout << "прочий символ" << std::endl;
            break;
        }   
    }
    return 0;
}

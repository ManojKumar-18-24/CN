// p5.cpp
#include <iostream>

int main() {
    std::string buf;
    while (std::getline(std::cin, buf)) {
        std::cout << "p5 printing"<<buf << std::endl;
    }
    return 0;
}


#include <iostream>

int main(){
    std::cout << "Welcome to Bootleg sqlite!" << std::endl;
    std::cout << "Enter \".help\" for usage hints." << std::endl;
    std::cout << "Connected to a transient in-memory database." << std::endl;
    std::cout << "Use \".open FILENAME\" to reopen on a persistent database." << std::endl;

    while(true) {
        std::cout << "sqlite> ";
        int x;
        std::cin >> x;

        std::cout << "You entered: " << x << std::endl;
    }

    return 0;
}

#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

int main() {
    std::string filename;
    std::cin >> filename;
    std::cout << boost::filesystem::exists(filename);

    return 0;
}

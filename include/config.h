#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>

namespace config {
    static const std::vector<std::pair<std::string,int>> hosts={{"127.0.0.1",8080}};
    static const std::string dotterService="dot.service";
}
#endif
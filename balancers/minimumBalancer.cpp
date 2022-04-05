#include "minimumBalancer.h"
std::string minimumBalancer::getIP(std::set<std::string>& ipports,std::string fromIpPort) {
    std::vector<std::pair<std::string,clock_t>> timecosts;
    for (auto ipport:ipports) {
        timecosts.push_back(std::make_pair(ipport,ipTimeCostCache.get(ipport)));
    }
    return std::max_element(timecosts.begin(),timecosts.end(),[](auto &compA,auto &compB){
        return compA.second<compB.second;
    })->first;
}
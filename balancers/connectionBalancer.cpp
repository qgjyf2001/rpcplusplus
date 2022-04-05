#include "connectionBalancer.h"
void connectionBalancer::recordTimeCost(std::string& ip,clock_t timecost) {
    baseBalancer::recordTimeCost(ip,timecost);
    ipConnectionCache.decrease(ip);
}
std::string connectionBalancer::getIP(std::set<std::string>& ipports,std::string fromIpPort) {
    std::vector<std::pair<std::string,int>> timecosts;
    for (auto ipport:ipports) {
        timecosts.push_back(std::make_pair(ipport,ipConnectionCache.get(ipport)));
    }
    auto result=std::min_element(timecosts.begin(),timecosts.end(),[](auto &compA,auto &compB){
        return compA.second<compB.second;
    })->first;
    ipConnectionCache.increase(result);
    return result;
}
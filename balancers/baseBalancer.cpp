#include "baseBalancer.h"

#include <boost/algorithm/string.hpp>
void baseBalancer::recordTimeCost(std::string& ip,clock_t timecost) {
    ipTimeCostCache.set(ip,timecost);
}
std::string baseBalancer::getIP(std::set<std::string>& ipports,std::string fromIpPort) {
    auto sz=ipports.size();
    std::string chosen;
    chosen=*(ipports.begin());
    for (auto ipport:ipports) {
        if (rand()%sz==0)
        {
            chosen=ipport;
            break;
        }
    }
    return chosen;
}
JsonParser baseBalancer::balanceCall(cache& serviceCache,std::string service,JsonParser params) {
    std::vector<std::string> ipAndPort;
    std::string chosen;
    clock_t t0;
    while (true) {
        try
        {
            auto ipports=serviceCache.get(service);
            auto sz=ipports.size();
            if (sz==0) {
                throw std::runtime_error("service not found");
            }
            chosen=getIP(ipports,params["ip"].toString());
            boost::split(ipAndPort,chosen,boost::is_any_of(":"));
            assert(ipAndPort.size()==2);
            rpcClient client(ipAndPort[0],std::atoi(ipAndPort[1].c_str()));
            t0=clock();
            auto result=client.remoteCall(params);
            recordTimeCost(chosen,clock()-t0);
            return result;
        }
        catch (const std::runtime_error &re){
            if (std::string(re.what())=="connect error") {
                for (int i=0;i<2;i++)
                    try
                    {
                        rpcClient client(ipAndPort[0],std::atoi(ipAndPort[1].c_str()));
                        auto result=client.remoteCall(params);
                        recordTimeCost(chosen,clock()-t0);
                        return result;
                    }
                    catch(const std::runtime_error &re1)
                    {
                        if (std::string(re1.what())!="connect error")
                            return {};
                    }
                    catch(...) {
                        return {};
                    }
                serviceCache.erase(service,chosen);
                
            } else {
                return {};
            }
        }
        catch(...)
        {
            return {};
        }
    }
}
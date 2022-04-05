#ifndef HASHBALANCER_H
#define HASHBALANCER_H
#include "baseBalancer.h"
class hashBalancer:public baseBalancer
{
protected:
    virtual std::string getIP(std::set<std::string>& ipports,std::string fromIpPort) override;
};
#endif
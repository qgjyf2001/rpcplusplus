#ifndef MINIMUMBALANCER_H
#define MINIMUMBALANCER_H
#include "baseBalancer.h"

class minimumBalancer:public baseBalancer
{
protected:
    virtual std::string getIP(std::set<std::string>& ipports,std::string fromIpPort) override;
};
#endif
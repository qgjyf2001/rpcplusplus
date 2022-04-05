#include "hashBalancer.h"

#include <openssl/sha.h>
#include <boost/algorithm/string.hpp>
std::string hashBalancer::getIP(std::set<std::string>& ipports,std::string fromIpPort) {
    auto sz=ipports.size();
    std::vector<std::string> ipAndPort;
    boost::split(ipAndPort,fromIpPort,boost::is_any_of(":"));
    unsigned char sHash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,ipAndPort[0].c_str(),ipAndPort[1].length());
    SHA256_Final(sHash,&ctx);
    int hash=(*(unsigned int*)sHash)%sz;
    int tot=0;
    for (auto ipport:ipports) {
        if (tot==hash)
            return ipport;
        tot++;
    }
    assert(true);
    return "";
}
#include "serialization.h"
SERIALIZECLASS(testClass)
{
public:
    SERIALIZEOBJECT(int,a);
    SERIALIZEOBJECT(std::string,b);
    SERIALIZEOBJECTWITHVALUE(int,c,1234);
    SERIALIZEOBJECTWITHVALUE(std::string,d,"abcd");
    testClass(int a,std::string b):a(a),b(b)
    {
        
    }
    testClass(int a,std::string b,int c,std::string d):a(a),b(b),c(c),d(d)
    {
        
    }
    testClass()
    {

    }
};
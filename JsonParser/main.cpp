#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <bits/stdc++.h>
#include <sys/time.h>

#include "BaseMsg.hpp"
#include "Msg1.hpp"

namespace pt = boost::property_tree;

std::map<std::string, std::function<std::string(ipc::BaseMsg &)>> obs;

std::string msg1Cb(ipc::Msg1 & msg1);


std::stringstream MSG1(R"({
    "special_key_id": "MSG1"
    "boolField": true,
    "intField": 1,
    "floatField": 1.23,
    "stringField": "string data",
    "msg2List": [
        {
            "boolField": true,
            "intField": 1
        },
        {
            "boolField": true,
            "intField": 1
        },
        {
            "boolField": true
            "intField": 1
        }
    ]
})");

template<typename T>
void registerCb(std::string id, std::function<std::string(T &)> func)
{
    obs[id] = [func](ipc::BaseMsg & req) -> std::string {
        if (auto castedReq = dynamic_cast<T*>(&req))
        {            
            return func(*castedReq);        
        }
        else
        {
            return "Unknown msg received";
        }
    };
}

std::string msg1Cb(ipc::Msg1 & msg1)
{
    std::cout << "Msg1 received" << std::endl;

    std::cout << "request: " << msg1.toString() << std::endl;

    msg1.boolField = false;
    
    return msg1.toString();
}

int main()
{
    registerCb<ipc::Msg1>("Msg1", msg1Cb);
    pt::ptree root;
    
    try
    {
        pt::read_json(MSG1, root);
    }
    catch(const std::exception& e)
    {
        std::cout << "ERROR: reqs not in JSON formate" << std::endl; 
        // TODO replace cout with logger
        std::cout << e.what() << std::endl;

        // TODO replace exit with raised exception
        exit(1);
    }

    ipc::Msg1 & msg1 = *new ipc::Msg1(root);

    auto response = obs["Msg1"](msg1);
    
    std::cout << "response: " << response << std::endl;

    delete &msg1;

    return 0;
}
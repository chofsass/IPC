#include <bits/stdc++.h>
#include <boost/property_tree/ptree.hpp>

#include "BaseMsg.hpp"
#include "Msg1.hpp"
#include "Msg2.hpp"

namespace pt = boost::property_tree;

namespace ipc
{
    std::string placeHolderCb(BaseMsg &);

    template<typename T>
    std::function<BaseMsg*(const pt::ptree &)> getFactory()
    {
        return [](const pt::ptree & pt) -> BaseMsg* {
            return new T(pt);
        };
    }

    template<typename T>
    std::function<void(BaseMsg *)> getDelete()
    {
        return [](BaseMsg * pBaseMsg) {
            if (T * pT = dynamic_cast<T*>(pBaseMsg))
            {            
                delete pT;        
            }
        };
    }

    std::map<std::string, std::function<BaseMsg*(const pt::ptree &)>> getMsgFactoryLookup()
    {
        return std::map<std::string, std::function<BaseMsg*(const pt::ptree &)>>({
            {"MSG1", getFactory<Msg1>()},
            {"MSG2", getFactory<Msg2>()},
        });
    }

    std::map<std::string, std::function<std::string(BaseMsg &)>> getMsgCbLookup()
    {
        return std::map<std::string, std::function<std::string(BaseMsg &)>>({
            {"MSG1", placeHolderCb},
            {"MSG2", placeHolderCb},
        });
    }

    std::map<std::string, std::function<void(BaseMsg *)>> getDeleteLookup()
    {

        return std::map<std::string, std::function<void(BaseMsg *)>>({
            {"MSG1", getDelete<Msg1>()},
            {"MSG2", getDelete<Msg2>()},
        });
    }

    std::string placeHolderCb(BaseMsg &)
    {
        return R"({
            "error": "No action associated with this message type" 
        })";
    }
}

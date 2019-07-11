#include <bits/stdc++.h>
#include <boost/property_tree/ptree.hpp>
#include "BaseMsg.hpp"

namespace pt = boost::property_tree;

namespace ipc
{
    std::map<std::string, std::function<BaseMsg*(const pt::ptree &)>> getMsgFactoryLookup();
    std::map<std::string, std::function<std::string(BaseMsg &)>> getMsgCbLookup();
    std::map<std::string, std::function<void(BaseMsg *)>> getDeleteLookup();
}

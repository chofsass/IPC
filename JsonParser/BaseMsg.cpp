#include <boost/property_tree/ptree.hpp>
#include <bits/stdc++.h>

namespace pt = boost::property_tree;

namespace ipc
{
    std::string vector_as_json_list(std::vector<std::string> v)
    {
        std::stringstream ss;

        for(auto & data : v)
        {
            ss << "\"" << data << "\"" << (&data == &v.back() ? "\n" : ",\n");
        }

        return ss.str();
    }

    std::string vector_as_json_list(std::vector<bool> v)
    {
        std::stringstream ss;

        auto i = 1;

        for(auto data : v)
        {
            ss << ( data ? "true" : "false" ) << (i++ == v.size() ? "\n" : ",\n");
        }

        return ss.str();
    }
}
#pragma once

#include <boost/property_tree/ptree.hpp>
#include <bits/stdc++.h>

namespace pt = boost::property_tree;

namespace ipc
{
    class BaseMsg
    {
        public:
            ~BaseMsg() { std::cout << "BaseMsg Destroyed" << std::endl; }
            virtual std::string toString() = 0;

        private:
    };

    
    template <typename T>
    std::vector<T> json_list_as_vector(pt::ptree const& pt, pt::ptree::key_type const& key)
    {
        std::vector<T> r;
        for (auto& item : pt.get_child(key))
            r.push_back(item.second.get_value<T>());
        return r;
    }

    template<typename T>
    std::vector<T*> json_list_as_vector_cpx(pt::ptree const& pt, pt::ptree::key_type const& key)
    {
        std::vector<T*> r;

        for (auto& item : pt.get_child(key))
            r.push_back(new T(item.second));

        return r;
    }

    template<typename T>
    void delete_cpx_vector(std::vector<T*> v)
    {
        for (auto item : v)
            delete item;
    }

    template <class T>
    std::string vector_as_json_list_cpx(std::vector<T*> v)
    {
        std::string rval = "";

        for(auto msg : v)
        {
            rval += msg->toString();
            rval += msg == v.back() ? "\n" : ",\n";
        }

        return rval;
    }

    template <class T>
    std::string vector_as_json_list(std::vector<T> v)
    {
        std::stringstream ss;

        for(auto & data : v)
        {
            ss << data << (&data == &v.back() ? "\n" : ",\n");
        }

        return ss.str();
    }

    template <>
    std::string vector_as_json_list(std::vector<std::string> v);

    template <>
    std::string vector_as_json_list(std::vector<bool> v);
}
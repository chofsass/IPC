#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <bits/stdc++.h>

#include "BaseMsg.hpp"


namespace pt = boost::property_tree;

const std::string MSG2_TEMPLATE = R"(
{
    "special_key_id": "MSG2",
    "boolField": ph_boolField,
    "intField": ph_intField
}
)";

namespace ipc
{
    class Msg2 : public BaseMsg
    {
        public:
            bool boolField;
            int intField;

            Msg2(){};
            Msg2(const pt::ptree & pt){
                try
                {
                    boolField = pt.get<bool>("boolField");
                    intField = pt.get<int>("intField");
                }
                catch(const std::exception& e)
                {
                    //TODO Log Error
                    std::cout << "ERROR: exception while unpacking JSON" << std::endl; 
                    std::cout << e.what() << std::endl;

                    throw;
                }
            };

            ~Msg2(){
                std::cout << "Msg2 Destoried" << std::endl;
            }

            std::string toString()
            {
                std::string rString(MSG2_TEMPLATE);

                boost::replace_all(rString, "ph_boolField", boolField ? "true" : "false");
                boost::replace_all(rString, "ph_intField", boost::lexical_cast<std::string>(intField));

                return rString;
            }
        private:

    };
}
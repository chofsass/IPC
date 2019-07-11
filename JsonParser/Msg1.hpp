#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <bits/stdc++.h>

#include "BaseMsg.hpp"
#include "Msg2.hpp" 

namespace pt = boost::property_tree;

const std::string MSG1_TEMPLATE = R"(
{
    "special_key_id": "MSG1",
    "boolField": ph_boolField,
    "intField": ph_intField,
    "floatField": ph_floatField,
    "stringField": "ph_stringField",
    "msg2List": [ph_msg2List]
}
)";

namespace ipc
{
    class Msg1 : public BaseMsg
    {
        public:
            bool boolField;
            int intField;
            float floatField;
            std::string stringField;
            std::vector<Msg2*> msg2List;

            Msg1(){};
            Msg1(const pt::ptree & pt){
                try
                {
                    boolField = pt.get<bool>("boolField");
                    intField = pt.get<int>("intField");
                    floatField = pt.get<float>("floatField");
                    stringField = pt.get<std::string>("stringField");
                    msg2List = json_list_as_vector_cpx<Msg2>(pt, "msg2List");
                }
                catch(const std::exception& e)
                {
                    //TODO Log Error
                    std::cout << "ERROR: exception while unpacking JSON" << std::endl; 
                    std::cout << e.what() << std::endl;

                    throw;
                }
            };

            ~Msg1(){
                //TODO delete for cpx list
                std::cout << "Msg1 Destoried" << std::endl;
                delete_cpx_vector<Msg2>(msg2List);
            }

            std::string toString()
            {
                std::string rString(MSG1_TEMPLATE);

                boost::replace_all(rString, "ph_boolField", boolField ? "true" : "false");
                boost::replace_all(rString, "ph_intField", boost::lexical_cast<std::string>(intField));
                boost::replace_all(rString, "ph_floatField", boost::lexical_cast<std::string>(floatField));
                boost::replace_all(rString, "ph_stringField", boost::lexical_cast<std::string>(stringField));
                boost::replace_all(rString, "ph_msg2List", vector_as_json_list_cpx<Msg2>(msg2List));

                return rString;
            }
        private:

    };
}
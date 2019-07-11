import json
import os
import shutil

HEADER_FILE = """
#pragma once

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

#include "BaseMsg.hpp"
{0}

namespace pt = boost::property_tree;

const std::string {1}_TEMPLATE = R"(
{{
    "special_key_id": "{1}",
    {2}
}}
)";

namespace ipc
{{
    class {3} : public BaseMsg
    {{
        public:
            {4}

            {3}(){{}};
            {3}(const pt::ptree & pt){{
                try
                {{
                    {5}
                }}
                catch(const std::exception& e)
                {{
                    std::cout << "ERROR: exception while unpacking JSON" << std::endl; 
                    // TODO replace cout with logger
                    std::cout << e.what() << std::endl;

                    // TODO replace exit with raised exception
                    exit(1);
                }}
            }};

            ~{3}(){{
                {6}
            }}

            std::string toString()
            {{
                std::string rString({1}_TEMPLATE);

                {7}

                return rString;
            }}
        private:

    }};
}}
"""[1:-1]

STANDARD_FIELD = "{} {};"
COMPLEX_FIELD = "{0} & {1} = *new {0}();;"
LIST_FIELD = "std::vector<{}> {};"
COMPLEX_LIST_FIELD = "std::vector<{} *> {};"

STANDARD_RSTRING_FIELD = """"{0}": ph_{0}"""
STRING_RSTRING_FIELD = """"{0}": "ph_{0}" """[:-1] 
LIST_RSTRING_FIELD = """"{0}": [ph_{0}]"""

STANDARD_UNPACK = """{1} = pt.get<{0}>("{1}");"""
COMPLEX_UNPACK = """{1} = {0}(pt.get_child("{1}"));"""
LIST_UNPACK = """{1} = json_list_as_vector<{0}>(pt, "{1}");"""
COMPLEX_LIST_UNPACK = """{1} = json_list_as_vector_cpx<{0}>(pt, "{1}");"""

STANDARD_REPLACE = """boost::replace_all(rString, "ph_{0}", boost::lexical_cast<std::string>({0}));"""
BOOL_REPLACE = """boost::replace_all(rString, "ph_{0}", {0} ? "true" : "false");"""
COMPLEX_REPLACE = """boost::replace_all(rString, "ph_{0}", {0}.toString());"""
LIST_REPLACE = """boost::replace_all(rString, "ph_{0}", vector_as_json_list<{1}>({0}));"""
CPX_LIST_REPLACE = """boost::replace_all(rString, "ph_{0}", vector_as_json_list_cpx<{1}>({0}));"""

COMPLEX_DELETE = """delete &{};"""
COMPLEX_LIST_DELETE = """delete_cpx_vector<{}>({});"""

BOOL_TYPES = ["bool", "boolean"]
STANDARD_TYPES = ["int", "float", "double"]
STRING_TYPES = ["string", "std::string"]

BUILD_DIR = os.path.dirname(os.path.realpath(__file__))
MESSAGES_JSON = os.path.join(BUILD_DIR, "messages.json")
MESSAGES_DIR = os.path.join(BUILD_DIR, "Msgs")


class Orchestrator:
    def __init__(self, msgs_dict):
        self.msgs = []

        for msg in msgs_dict["msgs"]:
            self.msgs.append(Message(msg))

        self.validateAllFieldTypes()

    def __str__(self):
        return "".join(str(msg) for msg in self.msgs)

    def execute(self):
        if os.path.isdir(MESSAGES_DIR):
            shutil.rmtree(MESSAGES_DIR)

        os.mkdir(MESSAGES_DIR)

        for msg in self.msgs:
            msgHeader = os.path.join(MESSAGES_DIR, "{}.hpp".format(msg.uname))

            with open(msgHeader, "w") as fp:
                fp.write(HEADER_FILE.format(
                    msg.getIncludes(),
                    msg.uuname,
                    msg.getFormatFields(),
                    msg.uname,
                    msg.getMemberDeclaration(),
                    msg.getUnpackCommands(),
                    msg.getDeleteCommands(),
                    msg.getPackCommands()
                ))

    def validateAllFieldTypes(self):
        for _type in Field.fieldTypes:
            if not (    _type in BOOL_TYPES or
                        _type in STRING_TYPES or 
                        _type in STANDARD_TYPES or
                        _type in Message.msgTypes):
                print("Unknown type: {}".format(_type))
                exit(1)

class Message:
    msgTypes = {} # used to lookup msg as a type

    def __init__(self, msg_dict):
        self.lname = msg_dict["name"][0].lower() + msg_dict["name"][1:]
        self.uname = msg_dict["name"][0].upper() + msg_dict["name"][1:]
        self.uuname = msg_dict["name"].upper()
        self.fields = []

        for field in msg_dict["fields"]:
            self.fields.append(Field(field))

        Message.msgTypes[self.uname] = ""

    def __str__(self):
        return """
            lname: {},
            uname: {},
            uuname: {},
            {}
        """.format(self.lname, self.uname, self.uuname, "".join(str(field) for field in self.fields))

    def getIncludes(self):
        return "".join(
            """#include "{}.hpp" """.format(field.type) if field.isCpx else ""
            for field in self.fields
        )

    def getMemberDeclaration(self):
        return "\n            ".join(
            field.getDeclaration()
            for field in self.fields    
        )

    def getFormatFields(self):
        return ",\n    ".join(
            field.getFormatField()
            for field in self.fields    
        )

    def getUnpackCommands(self):
        return "\n                    ".join(
            field.getUnpackCommand()
            for field in self.fields    
        )

    def getPackCommands(self):
        return "\n                ".join(
            field.getPackCommand()
            for field in self.fields    
        )

    def getDeleteCommands(self):
        cpxFields = list(filter(lambda x: x.isCpx, self.fields))

        return "\n                ".join(
            field.getDeleteCommand()
            for field in cpxFields    
        )

class Field:
    fieldTypes = {}

    def __init__(self, field_dict):
        self.name = field_dict["name"][0].lower() + field_dict["name"][1:]
        self.type = field_dict["type"]
        self.listType = None if "listType" not in field_dict else field_dict["listType"]
        self.isCpx = (  self.type not in BOOL_TYPES and
                        self.type not in STRING_TYPES and 
                        self.type not in STANDARD_TYPES )

        Field.fieldTypes[self.type] = ""

    def __str__(self):
        return """
                name: {},
                type: {}
        """.format(
            self.name,
            self.type
        )

    def getDeclaration(self):
        if self.isCpx:
            if self.listType:
                return COMPLEX_LIST_FIELD.format(self.type, self.name)
            else:
                return COMPLEX_FIELD.format(self.type, self.name)

        elif self.type in STRING_TYPES:
            if self.listType:
                return LIST_FIELD.format("std::string", self.name)
            else:
                return STANDARD_FIELD.format("std::string", self.name)

        elif self.type in BOOL_TYPES:
            if self.listType:
                return LIST_FIELD.format("bool", self.name)
            else:
                return STANDARD_FIELD.format("bool", self.name)
                
        elif self.type in STANDARD_TYPES:
            if self.listType:
                return LIST_FIELD.format(self.type, self.name)
            else:
                return STANDARD_FIELD.format(self.type, self.name)

    def getFormatField(self):
        if self.listType:
            return LIST_RSTRING_FIELD.format(self.name)
        elif self.type in STRING_TYPES:
            return STRING_RSTRING_FIELD.format(self.name)
        else:
            return STANDARD_RSTRING_FIELD.format(self.name)

    def getUnpackCommand(self):
        if self.isCpx:
            if self.listType:
                return COMPLEX_LIST_UNPACK.format(self.type, self.name)
            else:
                return COMPLEX_UNPACK.format(self.type, self.name)

        elif self.type in STRING_TYPES:
            if self.listType:
                return LIST_UNPACK.format("std::string", self.name)
            else:
                return STANDARD_UNPACK.format("std::string", self.name)

        elif self.type in BOOL_TYPES:
            if self.listType:
                return LIST_UNPACK.format("bool", self.name)
            else:
                return STANDARD_UNPACK.format("bool", self.name)
                
        elif self.type in STANDARD_TYPES:
            if self.listType:
                return LIST_UNPACK.format(self.type, self.name)
            else:
                return STANDARD_UNPACK.format(self.type, self.name)

    def getPackCommand(self):
        if self.isCpx:
            if self.listType:
                return CPX_LIST_REPLACE.format(self.name, self.type)
            else:
                return COMPLEX_REPLACE.format(self.name)

        elif self.type in STRING_TYPES:
            if self.listType:
                return LIST_REPLACE.format(self.name, "std::string")
            else:
                return STANDARD_REPLACE.format(self.name, "std::string")

        elif self.type in BOOL_TYPES:
            if self.listType:
                return LIST_REPLACE.format(self.name, "bool")
            else:
                return BOOL_REPLACE.format(self.name)
                
        elif self.type in STANDARD_TYPES:
            if self.listType:
                return LIST_REPLACE.format(self.name, self.type)
            else:
                return STANDARD_REPLACE.format(self.name, self.type)

    def getDeleteCommand(self):
        if self.isCpx:
            if self.listType:
                return COMPLEX_LIST_DELETE.format(self.type, self.name)
            else:
                return COMPLEX_DELETE.format(self.name)
        else:
            return ""

if __name__ == "__main__":
    msgs_dict = {}

    with open(MESSAGES_JSON, "r") as fp:
        msgs_dict = json.load(fp)

    orch = Orchestrator(msgs_dict)

    orch.execute()
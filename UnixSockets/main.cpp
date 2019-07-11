#include <bits/stdc++.h>
#include "Server.hpp"
#include "Msg1.hpp"

#define BUF_SIZE 512

const char *server_path = "server.sock";
const char *client_path = "client.sock";

std::string msg1Cb(ipc::Msg1 & msg1);

std::string msg1Cb(ipc::Msg1 & msg1)
{
    std::cout << "Msg1 received" << std::endl;

    std::cout << msg1.toString() << std::endl;

    return R"({
        "msg": "Msg1 received"
    })";
}

int main() 
{
    
    ipc::Server server(server_path);

    server.registerCb<ipc::Msg1>("MSG1", msg1Cb);

    server.run();

    return 0;
}
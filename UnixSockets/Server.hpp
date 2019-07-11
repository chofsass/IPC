#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <bits/stdc++.h>

#include "BaseMsg.hpp"

#define BUF_SIZE 512

namespace ipc
{
    class Server
    {
        public:
            Server(const char *server_path);
            ~Server();
            void run();

            template<typename T>
            void registerCb(std::string id, std::function<std::string(T &)> func)
            {
                _observerCb[id] = [func](ipc::BaseMsg & req) -> std::string {
                    if (auto castedReq = dynamic_cast<T*>(&req))
                    {            
                        return func(*castedReq);        
                    }
                    else
                    {
                        return R"({
                            "error": "internal error"
                        })";
                    }
                };
            }

        private:
            struct sockaddr_un _servaddr, _cliaddr;
            char _buf[BUF_SIZE];
            int _fd;
            std::map<std::string, std::function<std::string(ipc::BaseMsg &)>> _observerCb;
            std::map<std::string, std::function<BaseMsg*(const pt::ptree &)>> _observerMsgFactory;
            std::map<std::string, std::function<void(ipc::BaseMsg *)>> _observerDelete;

            std::string processReqest(std::string & req);
    };
}
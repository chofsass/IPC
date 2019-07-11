#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sys/resource.h>
#include "Server.hpp"
#include "Observer.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>
#include <errno.h>

namespace pt = boost::property_tree;

void process_mem_usage();

namespace ipc
{
    Server::Server(const char *server_path)
    {
        _observerCb = getMsgCbLookup();
        _observerMsgFactory = getMsgFactoryLookup();
        _observerDelete = getDeleteLookup();

        if ( (_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
            perror("socket error");
            exit(-1);
        }
        
        memset(&_servaddr, 0, sizeof(_servaddr)); 
        memset(&_cliaddr, 0, sizeof(_cliaddr)); 

        _servaddr.sun_family = AF_UNIX;
        strncpy(_servaddr.sun_path, server_path, sizeof(_servaddr.sun_path)-1);
        unlink(server_path);

        if (bind(_fd, (struct sockaddr*)&_servaddr, sizeof(_servaddr)) == -1) {
            perror("bind error");
            exit(-1);
        }
    }

    Server::~Server()
    {

    }

    void Server::run()
    {
        int n; 
        socklen_t len = sizeof(struct sockaddr_un);
        std::string req;

        while (true)
        {
            memset(_buf, 0, sizeof(_buf)); 

            n = recvfrom(_fd, _buf, BUF_SIZE, MSG_WAITALL, ( struct sockaddr *)&_cliaddr, &len);

            process_mem_usage();

            req = std::string(_buf);
            memset(_buf, 0, sizeof(_buf));

            strcpy(_buf, processReqest(req).c_str());\

            process_mem_usage();

            n = sendto(_fd, _buf, strlen(_buf), MSG_CONFIRM, (const struct sockaddr *) &_cliaddr, len); 
            if(n == -1)
            {
                perror("bind error");
                break;
            }
        }
    }

    std::string Server::processReqest(std::string & req)
    {
        pt::ptree root;
        std::string id;
        BaseMsg * msg;
        std::string rval;
        std::stringstream ss(req);
        
        try
        {
            pt::read_json(ss, root);
            id = root.get<std::string>("special_key_id");
            
            if ( _observerMsgFactory.find(id) == _observerMsgFactory.end() ) 
                return R"({
                    "error": "unknown msg"
                })";
            else
                msg = _observerMsgFactory[id](root);
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << std::endl;
            return R"({
                "error": "bad data"
            })";
        }
        
        rval = _observerCb[id](*msg);
        _observerDelete[id](msg);

        return rval;
    }
}

void process_mem_usage()
{
    std::cout << "\n========================\n";
    malloc_stats();
}

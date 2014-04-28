#ifndef REDIS_SERVER_H
#define REDIS_SERVER_H

#include <string>

class redis_server
{
    public:
        std::string host;
        std::string port;

        redis_server();
        virtual ~redis_server();
    protected:
    private:
};

#endif // REDIS_SERVER_H

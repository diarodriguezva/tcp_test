#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <string>
#include <netinet/in.h>

#define MTU_SIZE 1200

class tcp_server {

    public:
        tcp_server(int port);
       ~tcp_server();
        
        void accept_connection();

    private:
        void error_handling(const std::string &message);

        int socket_handle_{0};
        struct sockaddr_in server_address_;

        int client_handle_{0};
        struct sockaddr_in client_address_;
        std::string client_ip_;
        char data_[MTU_SIZE];

};

#endif // TCP_SERVER_HPP
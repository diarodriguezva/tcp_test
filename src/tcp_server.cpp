#include <tcp_server.hpp>

#include <iostream>
#include <vector>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>

//---------------------------------------------------------------------------------------------------------------------
tcp_server::tcp_server(int port) {

    std::cout << "[info] server init" << std::endl;

    socket_handle_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_handle_ < 0)
    {
        int errsv = errno;
        error_handling("tcp_server: opening stream socket failed with errno: " + std::to_string(errsv));
    }

    // todo: check for errors, report them
    int reuse = 1;
    if (setsockopt(socket_handle_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
    {
        int errsv = errno;
        error_handling("tcp_server: setsockopt(SO_REUSEADDR) failed with errno: " + std::to_string(errsv));
    }
    if (setsockopt(socket_handle_, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
    {
        int errsv = errno;
        error_handling("tcp_server: setsockopt(SO_REUSEPORT) failed with errno: " + std::to_string(errsv));
    }

    memset(&server_address_, 0, sizeof(server_address_));

    server_address_.sin_family = AF_INET;
    server_address_.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address_.sin_port = htons(port);

    // todo: check for errors
    if (bind(socket_handle_, (struct sockaddr *)&server_address_, sizeof(server_address_)) < 0)
    {
        int errsv = errno;
        error_handling("tcp_server: bind() failed with errno: " + std::to_string(errsv));
    }

    // todo: check for errors
    if (listen(socket_handle_, 1) < 0)
    {
        int errsv = errno;
        error_handling("tcp_server: listen() failed with errno: " + std::to_string(errsv));
    }

    // todo: handle errors of this constructor
}

//---------------------------------------------------------------------------------------------------------------------
void tcp_server::accept_connection() {
    
    socklen_t client_socket_size = sizeof(client_address);
    client_handle = accept(socket_handle, (struct sockaddr*)&client_address, &client_socket_size); 
    client_ip = inet_ntoa(client_address.sin_addr);

    std::cout << "[info] accepted connection from: " << client_ip << std::endl;

    // todo: check for dead connection and go back to listening for new ones
    while (true){
        int status = recv(client_handle, data, MTU_SIZE, 0);
        if (status != -1 && status != 0){
            std::cout << std::string(data) << std::endl;
        }
    }

}


//---------------------------------------------------------------------------------------------------------------------
tcp_server::~tcp_server(){

    // todo: clean up
}


//---------------------------------------------------------------------------------------------------------------------
void tcp_server::error_handling(const std::string &message) {

    std::cerr << message << std::endl;
    exit(1);
}


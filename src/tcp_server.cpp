#include <tcp_server.hpp>

#include <iostream>
#include <vector>
#include <cstring>

#include <signal.h>

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>


// Declare static variables
bool tcp_server::running_ = true;

//---------------------------------------------------------------------------------------------------------------------
tcp_server::tcp_server(int port) {

    std::cout << "[info] server init" << std::endl;

    socket_handle_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_handle_ < 0)
    {
        int errsv = errno;
        error_handling("tcp_server: opening stream socket failed ()with errno: " + std::to_string(errsv));
    }

    // TODO: check for errors, report them
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

    // Make socket handle non-blocking
    if (fcntl(socket_handle_, F_SETFL, O_NONBLOCK) < 0)
        error_handling("tcp_server: fcnl() failed");

    memset(&server_address_, 0, sizeof(server_address_));
    server_address_.sin_family = AF_INET;
    server_address_.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address_.sin_port = htons(port);

    // TODO: check for errors
    if (bind(socket_handle_, (struct sockaddr *)&server_address_, sizeof(server_address_)) < 0)
    {
        int errsv = errno;
        error_handling("tcp_server: bind() failed with errno: " + std::to_string(errsv));
    }

    // TODO: check for errors
    if (listen(socket_handle_, 1) < 0)
    {
        int errsv = errno;
        error_handling("tcp_server: listen() failed with errno: " + std::to_string(errsv));
    }

    // TODO: handle errors of this constructor
}

//---------------------------------------------------------------------------------------------------------------------
void tcp_server::accept_connection() {

    socklen_t client_socket_size = sizeof(client_address_);
    int msg_size;

    // TODO: check for dead connection and go back to listening for new ones
    while (running_)
    {
        client_handle_ = accept(socket_handle_, (struct sockaddr*)&client_address_, &client_socket_size);
        if (client_handle_ < 0)
            continue;

        client_ip_ = inet_ntoa(client_address_.sin_addr);
        std::cout << "[info] accepted connection from: " << client_ip_ << std::endl;

        do
        {
            msg_size = recv(client_handle_, data_, MTU_SIZE, 0);
            if (msg_size != -1 && msg_size != 0){
                std::cout << std::string(data_, msg_size) << std::endl;
            }
            else
            {
                if(msg_size == 0)
                {
                    std::cout << "Ending TCP connection ... " << std::endl;
                }
                close(client_handle_);
            }
        } while (msg_size > 0);

        sleep(0.2);
    }
}


//---------------------------------------------------------------------------------------------------------------------
tcp_server::~tcp_server(){
    stop();
}


void tcp_server::handle_sigint()
{
    signal(SIGINT, tcp_server::sigintHandler);
}


//---------------------------------------------------------------------------------------------------------------------
void tcp_server::stop(){

    // TODO: clean up
    shutdown(socket_handle_, SHUT_RDWR);
    close(socket_handle_);

    shutdown(client_handle_, SHUT_RDWR);
    close(client_handle_);
}


//---------------------------------------------------------------------------------------------------------------------
void tcp_server::error_handling(const std::string &message) {

    std::cerr << message << std::endl;
    exit(1);
}


// Sources:
// Socket Tutorial (Oracle): https://docs.oracle.com/cd/E19620-01/805-4041/6j3r8iu2l/index.html
// http://dwise1.net/pgm/sockets/tcp_ip.html#SHUTDOWN

// API doc
// https://pubs.opengroup.org/onlinepubs/000095399/functions/socket.html

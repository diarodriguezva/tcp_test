#include <tcp_server.hpp>


int main(int argc, char** argv){

    tcp_server* server = new tcp_server(6699);

    // TODO: catch SIGINT to stop the server gracefully
    server->handle_sigint();

    // TODO: fix this function to repeatedly accept new connections
    server->accept_connection();

    // did we forget something?
    server->stop();

    return 0;
}

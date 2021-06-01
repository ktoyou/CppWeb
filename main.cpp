#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace Poco::Net;

#define PORT 1600
#define IPADDR "192.168.10.100"
#define UPDATE_HTMLDOC_TIMEOUT 5

char buff[4096];

void updateHtmlDoc()
{
    std::fstream html;

    html.open("index.html");
    html.read(buff, sizeof(buff));
    html.close();
}

void* retakeHtmlDoc(void* thread_data)
{
    while (true)
    {
        updateHtmlDoc();
        sleep(UPDATE_HTMLDOC_TIMEOUT);
    }
    
}

int main(int argc, char const *argv[])
{
    SocketAddress serverAddr(IPADDR, PORT);
    ServerSocket server(serverAddr);

    pthread_t thread;
    pthread_create(&thread, NULL, retakeHtmlDoc, NULL);

    while (true)
    {
        SocketStream connectedSocket(server.acceptConnection());
        connectedSocket << "HTTP/1.0 200 OK\r\n"
                        << "Content-Type: text/html\r\n"
                        << "\r\n"
                        << buff << std::flush;
        connectedSocket.close();
    }
    
    return 0;
}

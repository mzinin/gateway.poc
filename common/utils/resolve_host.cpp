#include <common/utils/resolve_host.hpp>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <cstring>


std::string common::resolveHost(const std::string& hostname)
{
    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* results = nullptr;

    std::string ip;
    if (getaddrinfo(hostname.data(), "http", &hints, &results) != 0)
    {
        return ip;
    }

	for(auto* it = results; it; it = it->ai_next)
	{
        auto* sockAddr = reinterpret_cast<sockaddr_in*>(it->ai_addr);
        if (sockAddr)
        {
            ip = inet_ntoa(sockAddr->sin_addr);
            break;
        }
	}

    freeaddrinfo(results);
    return ip;
}

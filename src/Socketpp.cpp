#include "Socketpp.h"

using namespace socketpp;

void BaseSocket::close() {
    if (closed) {
        //TODO throw errors, learn how excception work in cpp
    }

    closed = true;
    fD = -1;
}

std::size_t RWSocket::read(char* buffer, std::size_t size) {
    readLock.lock();

    std::size_t read = recv(getSocketNumber(), buffer, size, 0);

    if (read == static_cast<std::size_t>(-1)) {
        close();
        //TODO error
    }
    
    readLock.unlock();
    return read;
}

std::size_t RWSocket::write(const char* buffer, std::size_t size) {
    writeLock.lock();

    std::size_t written = send(getSocketNumber(), buffer, size, 0);

    if (written == static_cast<std::size_t>(-1)) {
        close();
        //TODO error
    }

    writeLock.unlock();
    return written;
}

Socket::Socket(const std::string& host, int port) : RWSocket(::socket(PF_INET, SOCK_STREAM, 0)) {
    struct sockaddr_in hostAddress {};
    std::memset(&hostAddress, 0, sizeof hostAddress);

    hostAddress.sin_family = AF_INET;
    hostAddress.sin_port = htons(port);
    hostAddress.sin_addr.s_addr = inet_addr(host.c_str());

    if (connect(getSocketNumber(), (struct sockaddr*)&hostAddress, sizeof hostAddress) != 0) {
        close();
        // TODO error
    }
}

SocketInputStream Socket::getInputStream() {
    return SocketInputStream(this);
}

SocketOutputStream Socket::getOutputStream() {
    return SocketOutputStream(this);
}

ServerSocket::ServerSocket(int port) : BaseSocket(::socket(PF_INET, SOCK_STREAM, 0)) {
    struct sockaddr_in hostAddress {};
    std::memset(&hostAddress, 0, sizeof hostAddress);

    hostAddress.sin_family = AF_INET;
    hostAddress.sin_port = htons(port);
    hostAddress.sin_addr.s_addr = INADDR_ANY;

    if (::bind(getSocketNumber(), (struct sockaddr*)&hostAddress, sizeof hostAddress) != 0) {
        close();
        std::cout << "Error bindind" << std::endl;
        // TODO error
    }

    if (::listen(getSocketNumber(), 255) != 0) {
        close();
        std::cout << "Error listening" << std::endl;
        // TODO error
    }
}

Socket ServerSocket::accept() {
    if (!isOpen()) {
        // TODO error
    }

    struct sockaddr clientAddress;
    socklen_t addressSize = sizeof clientAddress;
    int socketNumber = ::accept(getSocketNumber(), (struct sockaddr*)&clientAddress, &addressSize);

    if (socketNumber == -1) {
        //TODO error
    }

    return Socket(socketNumber);
}

#include "CanBus.hpp"
#include <iostream>

/**
 * CanBus constructor
 *
 * @param speed
 */
CanBus::CanBus(uint32_t speed) {
    this->speed = speed;
    this->sockfd = -1;

    this->setInterfaceUp();

    if (!this->connect()) {
        throw std::invalid_argument("Unable to connect to CAN socket");
    }
}


/**
 * Initialize can0 interface in linux
 */
void CanBus::setInterfaceUp() const {
    std::string command = "sudo ip link set can0 up type can bitrate "
                          + std::to_string(this->speed)
                          + " 2>/dev/null";  // Suppress RTNETLINK errors

    system(command.c_str());
    usleep(100 * 1000); // 0.1s
}

/**
 *
 * @return
 */
bool CanBus::connect() {
    // Open network socket
    this->sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (this->sockfd == -1) {
        close(this->sockfd);
        return false;
    }

    // Get the index of the network interface
    ifreq ifr{"can0"};
    if (ioctl(this->sockfd, SIOCGIFINDEX, &ifr) == -1) {
        close(this->sockfd);
        return false;
    }

    // Bind the socket to the network interface
    sockaddr_can address = {
            AF_CAN,
            ifr.ifr_ifindex
    };
    int rc = bind(this->sockfd, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (rc == -1) {
        close(this->sockfd);
        return false;
    }

    // Read existing socket flags
    int flags = fcntl(this->sockfd, F_GETFL, 0);
    if (flags == -1) {
        close(this->sockfd);
        return false;
    }

    // Apply non-blocking mode
    if (fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        close(this->sockfd);
        return false;
    }

    return true;
}

/**
 *
 * @param frame
 * @return
 */
bool CanBus::read(can_frame &frame) const {
    auto size = ::read(this->sockfd, &frame, CAN_MTU);
    return (size == CAN_MTU);
}

/**
 *
 * @param frame
 */
bool CanBus::send(const can_frame &frame) const {
    auto size = write(this->sockfd, &frame, sizeof(frame));
    return (size == CAN_MTU);
}


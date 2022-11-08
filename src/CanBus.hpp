#pragma once

#include <linux/can/bcm.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <exception>

class CanBus {

    uint32_t speed;
    int sockfd = -1;

    void setInterfaceUp() const;

    bool connect();

public:

    explicit CanBus(uint32_t speed = 500000);

    bool read(can_frame &frame) const;

    bool send(const can_frame &frame) const;
};

#pragma once

#include <thread>
#include <iostream>

#include "CanBus.hpp"
#include "SteeringCommnad.hpp"
#include "SteeringMode.hpp"

#define HS_CAN 500 * 1000

class Controller {

    CanBus interface;
    SteeringMode mode = SteeringMode::STEERING_ANGLE;
    volatile bool enabled = false;
    volatile float target = 0.f;

    static const uint16_t canRequestId = 0x400;
    static const uint8_t magicValue[4];

    void sendAngleFrame();

    void sendTorqueFrame();

    bool sendValueFrame(SteeringCommand command, float floatValue);

    bool sendFrame(SteeringCommand command, const uint8_t data[4]);

    void initSender();

public:
    explicit Controller(uint32_t bitrate);

    bool isEnabled() const;

    void setState(bool state);

    void toggleMode();

    SteeringMode getMode() const;

    void setTarget(float targetValue);
};

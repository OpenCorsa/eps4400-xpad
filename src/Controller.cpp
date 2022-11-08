#include "Controller.hpp"

/**
 * Define controller secret (not so secret in real life)
 */
const uint8_t Controller::magicValue[] = {0x05, 0x31, 0x80, 0x08};

/**
 * Constructor
 *
 * @param interface
 */
Controller::Controller(uint32_t bitrate) {
    this->interface = CanBus(bitrate);
    this->initSender();
}

/**
 *
 * @return
 */
bool Controller::isEnabled() const {
    return this->enabled;
}

/**
 *
 * @param state
 */
void Controller::setState(bool state) {
    if (state == this->enabled) {
        return;
    }

    this->enabled = state;

    // Transmit to controller state change
    auto command = (state)
                   ? SteeringCommand::COMMAND_ENABLE
                   : SteeringCommand::COMMAND_DISABLE;
    this->sendFrame(command, Controller::magicValue);
}

/**
 *
 */
void Controller::toggleMode() {
    if (!this->enabled) {
        return;
    }

    this->mode = (this->mode == SteeringMode::STEERING_ANGLE)
                 ? SteeringMode::STEERING_TORQUE
                 : SteeringMode::STEERING_ANGLE;
}


/**
 * @return
 */
SteeringMode Controller::getMode() const {
    return this->mode;
}

/**
 * @param targetValue
 */
void Controller::setTarget(float targetValue) {
    if (!this->enabled) {
        return;
    }

    this->target = targetValue;
}


/**
 *
 */
void Controller::sendAngleFrame() {
    float value = this->target;

    // Transform value from <-100; 100> to <-30; 30>
    value *= 0.3f;

    this->sendValueFrame(SteeringCommand::COMMAND_CONTROL_ANGLE, value);
}

/**
 *
 */
void Controller::sendTorqueFrame() {
    float value = this->target;

    // Transform value from <-100; 100> to <-5; 5>
    value *= 0.05;

    this->sendValueFrame(SteeringCommand::COMMAND_CONTROL_TORQUE, value);
}


/**
 * @param command
 * @param floatValue
 */
bool Controller::sendValueFrame(SteeringCommand command, float floatValue) {
    // Change precision to 1/100;
    floatValue *= 100.f;

    // Transform float into int32_t
    auto intValue = static_cast<int32_t>(floatValue);

    // Manually cast int32_t to uint8_t array (msb/lsb independent)
    uint8_t data[4] = {
            static_cast<uint8_t>((intValue & 0xff000000) >> 24),
            static_cast<uint8_t>((intValue & 0x00ff0000) >> 16),
            static_cast<uint8_t>((intValue & 0x0000ff00) >> 8),
            static_cast<uint8_t>((intValue & 0x000000ff)),
    };

    return this->sendFrame(command, data);
}


/**
 * @param command
 * @param data frame bytes [..xxxx..]
 */
bool Controller::sendFrame(SteeringCommand command, const uint8_t *data) {
    // Construct base object
    can_frame frame = {
            Controller::canRequestId,
            8,
    };
    frame.data[0] = command;
    frame.data[1] = 0x00;

    // Copy command value
    std::memcpy(frame.data + 2, data, 4);

    // Calculate primitive checksums
    frame.data[6] = frame.data[0] xor frame.data[2] xor frame.data[4];
    frame.data[7] = frame.data[1] xor frame.data[3] xor frame.data[5];

    // Transmit frame
    return this->interface.send(frame);
}

/**
 *
 */
void Controller::initSender() {
    std::thread sender([&]() {
        // Sending loop
        while (true) {
            if (this->enabled) {
                switch (this->mode) {
                    case SteeringMode::STEERING_ANGLE:
                        this->sendAngleFrame();
                        break;

                    case STEERING_TORQUE:
                        this->sendTorqueFrame();
                        break;
                }
            }

            usleep(10 * 990); // ~10ms
        }
    });

    // Detach sending thread
    sender.detach();
}

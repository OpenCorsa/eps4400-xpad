#pragma once

enum SteeringCommand {
    COMMAND_ENABLE = 0x00,

    COMMAND_CONTROL_ANGLE = 0x20,
    COMMAND_CONTROL_TORQUE = 0x40,

    COMMAND_DISABLE = 0xFF
};
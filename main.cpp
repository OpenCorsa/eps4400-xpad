#include <iostream>
#include <thread>

#include "src/Controller.hpp"
#include "include/simple-xpad/src/XPad.hpp"

int main() {
    // Initialize objects
    Controller controller(HS_CAN);
    XPad pad;

    std::cout << "Entering event loop...\n";

    // Event loop
    while (true) {
        // Resolve joystick device if disconnected
        if (!pad.isActive()) {
            try {
                auto device = XPad::getAvailableControllerPath();
                pad.connect(std::get<0>(device), std::get<1>(device));

                std::cout << "Connected to device " << std::get<0>(device) << "\n";
            } catch (...) {
                // Disable controller
                controller.setState(false);

                std::cout << "Awaiting controller...\n";
                sleep(1);
                continue;
            }
        }

        // Skip invalid / irrelevant events
        if (!pad.readEvent() || !pad.getEvent().isInputType()) {
            continue;
        }

        // Handle axis movement
        auto event = pad.getEvent();
        if (event.getSource() == XEventSource::ANALOG_LEFT_JOYSTICK_X) {
            auto target = event.getJoystickValue();
            // Update controller position
            controller.setTarget(target);
            continue;
        }

        // Handle buttons
        if (event.isInputType() == XEventType::EVENT_BUTTON && event.isButtonPressed()) {
            auto source = event.getSource();

            // Trigger enabled state
            if (source == XEventSource::BUTTON_XBOX) {
                auto newState = !controller.isEnabled();
                controller.setState(newState);

                if (newState) {
                    pad.rumble(5);
                }
                continue;
            }

            // Switch steering mode
            if (source == XEventSource::BUTTON_MENU) {
                controller.toggleMode();

                int rumbleCount = (controller.getMode() == SteeringMode::STEERING_ANGLE)
                                  ? 3
                                  : 1;

                pad.rumble(rumbleCount);
                continue;
            }

            // Zero-out position
            if (source == XEventSource::BUTTON_RIGHT_JOYSTICK) {
                controller.setTarget(0.f);
                continue;
            }
        }
    }
}
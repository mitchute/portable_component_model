#include "model.h"

int LumpedMass::stepForward(float dt, float heatAddition) {
    temperature = temperature + heatAddition * dt / (m * cp);
    return 0;
}

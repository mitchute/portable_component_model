#ifndef MODEL_H
#define MODEL_H

struct LumpedMass {
    float temperature = 0.0;
    float m = 0.0;
    float cp = 0.0;
    LumpedMass(float temperatureInit, float mass, float _cp) {
        temperature = temperatureInit;
        m = mass;
        cp = _cp;
    };
    int stepForward(float dt, float heatAddition);
};

#endif // MODEL_H

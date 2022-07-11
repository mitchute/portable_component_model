#include "Model.hh"
#include "EnergyPlus.hh"
#include <iostream>

void MyPlantModel::simulate(const PlantLocation &pl) { m.stepForward(heatGain, timeStep); }

void MyPlantModel::printResult() const { std::cout << "Model temperature: " << m.temperature << std::endl; }

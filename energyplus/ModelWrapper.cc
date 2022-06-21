#include <iostream>

#include <energyplus/EnergyPlus.hh> // "EnergyPlus.hh"
#include <energyplus/ModelWrapper.hh> // "ModelWrapper.hh"
#include <model/model.h>

void MyPlantModel::simulate(const PlantLocation &pl) {
  m.stepForward(heatGain, timeStep);
}

void MyPlantModel::printResult() const {
  std::cout << "Model temperature: " << m.temperature << std::endl;
}

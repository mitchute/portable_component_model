#include "EnergyPlus.hh"
#include "Model.hh"

int main() {
  MyPlantModel modelInstance(0.0, 1000, 300);
  PlantLocation modelLocation(1, 1, 1, 1);
  for (int timeStepNum = 1; timeStepNum < 24; timeStepNum++) {
    modelInstance.simulate(modelLocation);
    modelInstance.printResult();
  }
}

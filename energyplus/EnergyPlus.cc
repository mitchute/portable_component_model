#include <energyplus/EnergyPlus.hh>
#include <energyplus/ModelWrapper.hh>
#include <model/model.h>

int main() {
  MyPlantModel modelInstance(0.0, 1100, 300);
  PlantLocation modelLocation(1, 1, 1, 1);
  for (int timeStepNum = 1; timeStepNum < 24; timeStepNum++) {
    modelInstance.simulate(modelLocation);
    modelInstance.printResult();
  }
}

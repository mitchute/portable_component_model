#include "../model/model.h"
#include "EnergyPlus.hh"

struct MyPlantModel : PlantComponent {
  MyPlantModel(Real64 temperatureInit, Real64 mass, Real64 cp)
      : m(float(temperatureInit), float(mass), float(cp)) {}
  void simulate(PlantLocation const &pl) override;
  void printResult() const;
  LumpedMass m;
};

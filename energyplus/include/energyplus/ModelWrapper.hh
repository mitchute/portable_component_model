#ifndef ENEGYPLUS_MODELWRAPPER_H
#define ENEGYPLUS_MODELWRAPPER_H

#include <energyplus/EnergyPlus.hh>
#include <model/model.h>

struct MyPlantModel : PlantComponent {
  MyPlantModel(Real64 temperatureInit, Real64 mass, Real64 cp)
      : m(float(temperatureInit), float(mass), float(cp)) {}
  void simulate(PlantLocation const &pl) override;
  void printResult() const;
  LumpedMass m;
};

#endif // ENEGYPLUS_MODELWRAPPER_H

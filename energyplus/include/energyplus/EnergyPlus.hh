#ifndef ENERGYPLUS_HH
#define ENERGYPLUS_HH

using Real64 = double;

struct PlantLocation {
  [[maybe_unused]] int loopNum = 0;
  [[maybe_unused]] int loopSideNum = 0;
  [[maybe_unused]] int branchNum = 0;
  [[maybe_unused]] int compNum = 0;
  PlantLocation(int l, int s, int b, int c)
      : loopNum(l), loopSideNum(s), branchNum(b), compNum(c) {}
};

struct PlantComponent {
  virtual void simulate(PlantLocation const &pl) = 0;
};

constexpr Real64 timeStep = 3600;
constexpr Real64 heatGain = 30;

#endif // ENERGYPLUS_HH

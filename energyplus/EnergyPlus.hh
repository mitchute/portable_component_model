#ifndef ENERGYPLUSMOCK_HH
#define ENERGYPLUSMOCK_HH

using Real64 = double;

struct PlantLocation {
  int loopNum = 0;
  int loopSideNum = 0;
  int branchNum = 0;
  int compNum = 0;
  PlantLocation(int l, int s, int b, int c) : loopNum(l), loopSideNum(s), branchNum(b), compNum(c) {}
};

struct PlantComponent {
  virtual void simulate(PlantLocation const &pl) = 0;
};

constexpr Real64 timeStep = 3600;
constexpr Real64 heatGain = 30;

#endif // ENERGYPLUSMOCK_HH

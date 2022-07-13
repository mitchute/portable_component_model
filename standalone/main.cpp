#include <model/model.h>

int main() {
  LumpedMass m{0.0, 10, 1000};
  m.stepForward(1, 1);
}

#ifndef ANYTHINGYOUWANT
#define ANYTHINGYOUWANT

#include <tuple>
#include <vector>

class ThisGHE {
    double Ts; // Soil temp
    double cp; // Specific heat (heat energy required to change temp of material)
    double H;  // Active borehole length (Active length of pipe)
    double Rb; // Borehole Resistance
    double ks; // Soil conductivity
    double rcp; //Rho cp
    double mdot; //mass flow rate
    double cop_c; //Coefficient of performance chiller
    double cop_h; //Coefficient of performance heater
    std::tuple<std::vector<double>, std::vector<double>> indexed_data;

    std::vector<int> q_time;

    void load_data();
    std::vector<double> g_expander(int n, double ts);
    double summation(int n, std::vector<double> q_load, std::vector<double> g_data);
    double HP(double ghe_out, double bldgload, double HP_config [] );
  public:
    ThisGHE(int finalSizeOfVectorsIAlreadyKnow);
    void main_model();
};

#endif

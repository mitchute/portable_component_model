#ifndef GHEh
#define GHEh

#include <fstream>
#include <tuple>
#include <vector>

class Pump {
    // TODO: This could be expanded to more logic, also should rename to specify mass or volume flow
    double fixed_flow_rate = 0.2;

  public:
    double flow_rate = 0.0;
    void set_flow_rate() { flow_rate = fixed_flow_rate; }
};

class HeatPump {
    double specific_heat = 4200;
    std::array<double, 3> heating;
    std::array<double, 3> cooling;

  public:
    double outlet_temperature = 0.0;
    HeatPump(std::array<double, 3> heating_coefficients, std::array<double, 3> cooling_coefficients)
        : heating(heating_coefficients), cooling(cooling_coefficients){};
    void operate(double inlet_temperature, double operating_flow_rate, double building_load);
};

class GHE {
    // TODO: I would rename all these to meaningful variable names using snake case, i.e. Ts -> soil_temperature
    double Ts;  // Soil temp
    double cp;  // Specific heat (heat energy required to change temp of material)
    double H;   // Active borehole length (Active length of pipe)
    double Rb;  // Borehole Resistance
    double ks;  // Soil conductivity
    double rcp; // Rho cp
    double ts;  // Characteristic time
    double c0;  // Simplification term

    std::vector<double> g_func;
    std::vector<double> lntts;
    void g_expander(int m);
    double summation(int n);

  public:
    double outlet_temperature = 0.0;
    double qn;
    double ghe_Tin;
    double c1;
    std::vector<int> q_time;
    std::vector<double> q_lntts;
    std::vector<double> ghe_load;
    std::vector<double> g_data;
    std::vector<double> ghe_Tout;
    std::vector<double> ghe_Tf;

    explicit GHE(int m);
    void simulate(int m, double ghe_inlet_temperature, double mass_flow_rate);
};
#endif

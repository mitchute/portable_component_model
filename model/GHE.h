#ifndef GHEh
#define GHEh

#include <array>
#include <fstream>
#include <tuple>
#include <vector>

class Pump {
    // TODO: This could be expanded to more logic, also should rename to specify mass or volume flow. Talk to Matt
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
    double soil_temp;
    double specific_heat;
    double bh_length;
    double bh_resistance;
    double soil_conduct;
    double rho_cp;
    double ts;
    double c0;  // Simplification term

    std::vector<double> g_func; //g function output values
    std::vector<double> lntts; //g function input values
    void g_expander(int num_hours);
    double summation(int hour);

  public:
    double outlet_temperature = 0.0;
    double Tf;
    double qn;  // TODO: How does qn relate to ghe_load
    double ghe_Tin;
    double c1;
    std::vector<int> hours_as_seconds; // hourly data as seconds
    std::vector<double> calc_lntts; // Calculated lntts values from num_time_steps
    std::vector<double> ghe_load;
    std::vector<double> interp_g_values;  // Interpolated g values from interp_lntts

    explicit GHE(int num_hours);
    void simulate(int hour, double ghe_inlet_temperature, double mass_flow_rate);
};
#endif

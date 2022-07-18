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

// TODO: potentially add a new struct here that can be passed to GHE constructor instead passing each var individually

class GHE {
    double ts;
    double c0;
    void g_expander(int num_hour3s);
    std::array < double, 2 >  summation(int hour);

  public:
    double soil_temp;
    double specific_heat;
    double bh_length;
    double bh_resistance;
    double soil_conduct;
    double rho_cp;
    double outlet_temperature = 0.0;
    double MFT;
    double internal_BH_temp;
    double current_GHEload;
    double ghe_Tin;
    std::array < double, 2 > c1; //0 index is self, 1 index is cross
    std::vector<double> g_func_self;        // g function output values
    std::vector<double> lntts_self;         // g function input values
    std::vector<double> g_func_cross;        // g function output values
    std::vector<double> lntts_cross;         // g function input values
    std::vector<int> hours_as_seconds; // hourly data as seconds
    std::vector<double> calc_lntts;    // Calculated lntts values from num_time_steps
    std::vector<double> ghe_load;
    std::vector<double> interp_g_self; // Interpolated g values from interp_lntts
    std::vector<double> interp_g_cross; // Interpolated g values from interp_lntts

    GHE(int num_hours, double soil_temp, double specific_heat, double bh_length, double bh_resistance, double soil_conduct, double rho_cp, std::vector<double> &g_func_self, std::vector<double> &lntts_self, std::vector<double> &g_func_cross, std::vector<double> &lntts_cross);
    double simulate(int hour, double ghe_inlet_temperature, double mass_flow_rate, double external_BH_temp);
};
#endif

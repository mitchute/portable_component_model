#ifndef mainh
#define mainh

#include <array>
#include <vector>

class main_vars {
  public:
    // Setting up Variables:
    double soil_temp;
    double specific_heat = 4200;
    double bh_length;
    double bh_resistance;
    double soil_conduct;
    double rho_cp;
    double load_periods;
    double hr_per_timestep;
    int num_bh;
    int timestep_start;
    int num_hours;
    std::vector<double> g_func;
    std::vector<double> lntts;
    std::vector<double> building_load;
    std::array<double, 3> heating_coefficients = {0.705459, 0.005447, -0.000077}; // HP heating coefficients hard coded from GLHEPro
    std::array<double, 3> cooling_coefficients = {1.092440, 0.000314, 0.000114};  // HP cooling coefficients hard coded from GLHEPro
};

#endif
#include <GHE.h>
#include <cmath>
#include <iostream>
#include <json.hpp>
#include <sstream>
using json = nlohmann::json;
int main() {
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
    std::vector<double> g_func;
    std::vector<double> lntts;
    std::vector<double> building_load;
    // opening file and reading data
    std::string path = "/Users/ryan/Research_local/NREL/portable_component_model/standalone/inputs/test.json";
    std::ifstream file(path);
    json inputs;
    if (!file.is_open()) {
        std::cout << "Error, file is open" << std::endl;
        file.close();
        return 0;
    }
    file >> inputs;
    file.close();

    inputs["loads"]["load_periods"].get_to(load_periods);
    inputs["loads"]["hr_per_timestep"].get_to(hr_per_timestep);
    inputs["loads"]["data"].get_to(building_load);
    inputs["soil"]["soil_temperature"].get_to(soil_temp);
    inputs["soil"]["soil_conductivity"].get_to(soil_conduct);
    inputs["soil"]["soil_rhoCp"].get_to(rho_cp);
    inputs["ghe"][0]["bh_length"].get_to(bh_length);
    inputs["ghe"][0]["bh_resistance"].get_to(bh_resistance);
    inputs["ghe"][0]["num_bh"].get_to(num_bh);
    inputs["ghe"][0]["timestep_start_operate"].get_to(timestep_start);
    inputs["ghe"][0]["self_lntts"].get_to(lntts);
    inputs["ghe"][0]["self_g_func"].get_to(g_func);

    // load data
    int num_hours = building_load.size();

    std::cout << num_hours << ", " << load_periods << ", " << hr_per_timestep << ", " << soil_temp << ", " << soil_conduct << ", " << rho_cp << ", "
              << bh_length << ", " << bh_resistance << ", " << num_bh << ", " << timestep_start << std::endl;

    return 0;
}
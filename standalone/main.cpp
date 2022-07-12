#include <GHE.h>
#include <json.hpp>
#include <cmath>
#include <iostream>
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

    // Get objects for all the components around the loop
    Pump pump;
    std::array<double, 3> heating_coefficients = {0.705459, 0.005447, -0.000077}; //HP heating coefficients hard coded from GLHEPro
    std::array<double, 3> cooling_coefficients = {1.092440, 0.000314, 0.000114}; //HP cooling coefficients hard coded from GLHEPro
    HeatPump hp(heating_coefficients, cooling_coefficients);
    GHE ghe(num_hours);

    //Some data is hard coded here, not found in JSON:
    ghe.soil_temp = soil_temp;
    ghe.specific_heat = specific_heat;
    ghe.bh_length = bh_length;
    ghe.bh_resistance = bh_resistance;
    ghe.soil_conduct = soil_conduct;
    ghe.rho_cp = rho_cp;
    ghe.g_func = g_func;
    ghe.lntts = lntts;




    // Setup output streams
    std::stringstream output_string;
    std::ofstream static outputs("./outputs/outputs.csv", std::ofstream::out);
    std::ofstream static debug("./outputs/debug.csv", std::ofstream::out);
    outputs << "n"
            << ","
            << "GHE Load"
            << ","
            << "ghe_Tin (HP_Tout)"
            << ","
            << "ghe_Tout (HP_Tin)"
            << ","
            << "MFT"
            << ","
            << "bldgload"
            << "\n";
    debug << "ghe.qn"
          << ","
          << "ghe.ghe_Tin"
          << ","
          << "ghe.q_time[n]"
          << ","
          << "ghe.ghe_load[n]"
          << ","
          << "ghe.q_lntts[n]"
          << ","
          << "ghe.g_data[n]"
          << ","
          << "ghe.ghe_Tout[n]"
          << ","
          << "ghe.ghe_Tf[n]"
          << ","
          << "ghe.c1"
          << "\n";

    // Run the model
    int hour = 0;
    for (double bldgload : building_load) {
        // Operate the pump to set the loop flow rate
        pump.set_flow_rate();
        // Operate the heat pump using the last ghe outlet temperature as the new hp inlet temperature
        if (hour == 0) {
            hp.outlet_temperature = 0;
        } else {
            hp.operate(ghe.outlet_temperature, pump.flow_rate, bldgload);
        }
        // Now run the GHE
        ghe.simulate(hour, hp.outlet_temperature, pump.flow_rate);

        // Finally, write data for each loop iteration
        outputs << hour << "," << ghe.ghe_load.back() << "," << hp.outlet_temperature << "," << ghe.outlet_temperature << "," << ghe.Tf << ","
                << bldgload << "\n";
        debug << ghe.current_GHEload << "," << ghe.ghe_Tin << "," << ghe.hours_as_seconds[hour] << "," << ghe.ghe_load[hour] << ","
              << ghe.calc_lntts[hour] << "," << ghe.interp_g_values[hour] << "," << ghe.outlet_temperature << "," << ghe.Tf << "," << ghe.c1 << "\n";
    hour ++;
    }
    return 0;
}

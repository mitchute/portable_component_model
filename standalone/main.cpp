//#include "main.h"
#include <GHE.h>
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

struct main_vars {
    double soil_temp;
    double specific_heat;
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
    std::array<double, 3> heating_coefficients;
    std::array<double, 3> cooling_coefficients;
};

main_vars load_data() {
    main_vars load_vars;
    // opening file and reading data
    std::string path = "/home/edwin/Projects/portable_component_model/standalone/inputs/test.json";
    std::ifstream file(path);
    json inputs;
    if (!file.is_open()) {
        std::cout << "Error, file is open" << std::endl;
        file.close();
    }
    file >> inputs;
    file.close();

    inputs["loads"]["load_periods"].get_to(load_vars.load_periods);
    inputs["loads"]["hr_per_timestep"].get_to(load_vars.hr_per_timestep);
    inputs["loads"]["data"].get_to(load_vars.building_load);
    inputs["soil"]["soil_temperature"].get_to(load_vars.soil_temp);
    inputs["soil"]["soil_conductivity"].get_to(load_vars.soil_conduct);
    inputs["soil"]["soil_rhoCp"].get_to(load_vars.rho_cp);
    inputs["ghe"][0]["bh_length"].get_to(load_vars.bh_length);
    inputs["ghe"][0]["bh_resistance"].get_to(load_vars.bh_resistance);
    inputs["ghe"][0]["num_bh"].get_to(load_vars.num_bh);
    inputs["ghe"][0]["timestep_start_operate"].get_to(load_vars.timestep_start);
    inputs["ghe"][0]["self_lntts"].get_to(load_vars.lntts);
    inputs["ghe"][0]["self_g_func"].get_to(load_vars.g_func);

    load_vars.specific_heat = 4200;
    load_vars.heating_coefficients = {0.705459, 0.005447, -0.000077}; // HP heating coefficients hard coded from GLHEPro
    load_vars.cooling_coefficients = {1.092440, 0.000314, 0.000114};  // HP cooling coefficients hard coded from GLHEPro

    load_vars.num_hours = load_vars.building_load.size();
    return load_vars;
}

int main() {
    // set up classes
    main_vars inputs = load_data();
    Pump pump;
    HeatPump hp(inputs.heating_coefficients, inputs.cooling_coefficients);
    GHE ghe(inputs.num_hours, inputs.soil_temp, inputs.specific_heat, inputs.bh_length, inputs.bh_resistance, inputs.soil_conduct, inputs.rho_cp,
            inputs.g_func, inputs.lntts);

    // Setup output streams
    std::stringstream output_string;
    std::ofstream static outputs("/tmp/outputs.csv", std::ofstream::out);
    std::ofstream static debug("/tmp/debug.csv", std::ofstream::out);
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
    for (double bldgload : inputs.building_load) {
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
        hour++;
    }
    return 0;
}

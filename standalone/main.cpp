#include "main.h"
#include <GHE.h>
#include <iostream>
#include <json.hpp>
#include <sstream>

using json = nlohmann::json;

void load_data() {
    main_vars main_vars;
    // opening file and reading data
    std::string path = "/Users/ryan/Research_local/NREL/portable_component_model/standalone/inputs/test.json";
    std::ifstream file(path);
    json inputs;
    if (!file.is_open()) {
        std::cout << "Error, file is open" << std::endl;
        file.close();
    }
    file >> inputs;
    file.close();

    inputs["loads"]["load_periods"].get_to(main_vars.load_periods);
    inputs["loads"]["hr_per_timestep"].get_to(main_vars.hr_per_timestep);
    inputs["loads"]["data"].get_to(main_vars.building_load);
    inputs["soil"]["soil_temperature"].get_to(main_vars.soil_temp);
    inputs["soil"]["soil_conductivity"].get_to(main_vars.soil_conduct);
    inputs["soil"]["soil_rhoCp"].get_to(main_vars.rho_cp);
    inputs["ghe"][0]["bh_length"].get_to(main_vars.bh_length);
    inputs["ghe"][0]["bh_resistance"].get_to(main_vars.bh_resistance);
    inputs["ghe"][0]["num_bh"].get_to(main_vars.num_bh);
    inputs["ghe"][0]["timestep_start_operate"].get_to(main_vars.timestep_start);
    inputs["ghe"][0]["self_lntts"].get_to(main_vars.lntts);
    inputs["ghe"][0]["self_g_func"].get_to(main_vars.g_func);

    main_vars.num_hours = main_vars.building_load.size();
}

int main() {
    // set up classes
    main_vars main_vars;
    Pump pump;
    HeatPump hp(main_vars.heating_coefficients, main_vars.cooling_coefficients);
    GHE ghe(main_vars.num_hours);
    // load data
    load_data();
    ghe.soil_temp = main_vars.soil_temp;
    ghe.specific_heat = main_vars.specific_heat;
    ghe.bh_length = main_vars.bh_length;
    ghe.bh_resistance = main_vars.bh_resistance;
    ghe.soil_conduct = main_vars.soil_conduct;
    ghe.rho_cp = main_vars.rho_cp;
    ghe.g_func = main_vars.g_func;
    ghe.lntts = main_vars.lntts;

    // Setup output streams
    std::stringstream output_string;
    std::ofstream static outputs("/Users/ryan/Research_local/NREL/portable_component_model/standalone/outputs/outputs.csv", std::ofstream::out);
    std::ofstream static debug("/Users/ryan/Research_local/NREL/portable_component_model/standalone/outputs/debug.csv", std::ofstream::out);
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
    for (double bldgload : main_vars.building_load) {
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

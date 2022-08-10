// ensure proper libraries are linked in the CMakeLists.txt file for building the executable
//#include "main.h"
#include <GHE.h>
#include <iostream>
#include <json.hpp>
#include <filesystem>

using json = nlohmann::json;

struct main_vars {
    double soil_temp;
    double specific_heat;
    double bh_length_a;
    double bh_resistance_a;
    double soil_conduct;
    double rho_cp;
    double load_periods;
    double hr_per_timestep;
    int num_bh_a;
    int timestep_start_a;
    int num_time_steps;
    bool file_open;
    std::vector<double> g_self_a;
    std::vector<double> lntts_self_a;
    std::vector<double> g_cross_a;
    std::vector<double> lntts_cross_a;
    std::vector<double> building_load;
    std::array<double, 3> heating_coefficients;
    std::array<double, 3> cooling_coefficients;
};

struct input_vars {
    std::string json_path_in;
    double specific_heat_in;
    std::array<double, 3> heating_coefficients_in;
    std::array<double, 3> cooling_coefficients_in;
};

main_vars load_data() {
    main_vars load_vars;
    std::string input_path;

    // hardcoded data not found in json
    load_vars.specific_heat = 4200;
    load_vars.heating_coefficients = {0.705459, 0.005447, -0.000077}; // HP heating coefficients hard coded from GLHEPro
    load_vars.cooling_coefficients = {1.092440, 0.000314, 0.000114};  // HP cooling coefficients hard coded from GLHEPro
    input_path = "../standalone/inputs/C.json";                     // default path

    // opening file and reading data
    std::cout << "Path to ghe config data is: " << input_path << std::endl;

    std::ifstream file(input_path);
    json inputs;
    if (!file.is_open()) {
        std::cout << "Error, file is open" << std::endl;
        file.close();
        load_vars.file_open = true;
        return load_vars;
    }
    load_vars.file_open = false;
    file >> inputs;
    file.close();

    inputs["loads"]["load_periods"].get_to(load_vars.load_periods);
    inputs["loads"]["hr_per_timestep"].get_to(load_vars.hr_per_timestep);
    inputs["loads"]["data"].get_to(load_vars.building_load);
    inputs["soil"]["soil_temperature"].get_to(load_vars.soil_temp);
    inputs["soil"]["soil_conductivity"].get_to(load_vars.soil_conduct);
    inputs["soil"]["soil_rhoCp"].get_to(load_vars.rho_cp);

    inputs["ghe"][0]["bh_length"].get_to(load_vars.bh_length_a);
    inputs["ghe"][0]["bh_resistance"].get_to(load_vars.bh_resistance_a);
    inputs["ghe"][0]["num_bh"].get_to(load_vars.num_bh_a);
    inputs["ghe"][0]["timestep_start_operate"].get_to(load_vars.timestep_start_a);
    inputs["ghe"][0]["self_lntts"].get_to(load_vars.lntts_self_a);
    inputs["ghe"][0]["self_g_func"].get_to(load_vars.g_self_a);

    // load_vars.num_time_steps = load_vars.building_load.size() * load_vars.hr_per_timestep * load_vars.load_periods;
    load_vars.num_time_steps = load_vars.building_load.size() * load_vars.load_periods; // monthly for 20 years
    return load_vars;
}

int main() {
    std::vector <double> stand_in;
    // Setup output streams
    // Note: data will be cleared for each run. Make sure to save data in a separate directory before running again.
    if (!std::filesystem::is_directory("../standalone/outputs/")) {
        std::filesystem::create_directory("../standalone/outputs/");
    }
    std::stringstream output_string;
    std::string output_file_path = "../standalone/outputs/single_outputs.csv";
    std::ofstream outputs(output_file_path);
    std::ofstream debug("../standalone/outputs/single_debug.csv");
    outputs << "n"
            << ","
            << "C: GHE Load"
            << ","
            << "C: ghe_Tin (HP_Tout)"
            << ","
            << "C: ghe_Tout (HP_Tin)"
            << ","
            << "C: MFT"
            << ","
            << "bldgload"
            << "\n";
    debug << "time_step" << ","
          << "ghe_a.calc_lntts[time_step]" << ","
          << "ghe_a.interp_g_self[time_step]" << ","
          << "ghe_a.interp_g_cross[time_step]" << ","
          << "ghe_a.current_GHEload" << ","
          << "ghe_a.c1[0]" << ","
          << "ghe_a.c1[1]" << ","
          << "ghe_a.internal_Tr" << ","
          << "ghe_a.BH_temp" << ","
          << "ghe_a.outlet_temperature" << ","
          << "ghe_a.MFT" << ","
          << "bldgload[time_step]" << "\n";

    // Create instances of classes
    main_vars inputs = load_data();
    if (inputs.file_open){
        return 1;
    }
    Pump pump;
    HeatPump hp_a(inputs.heating_coefficients, inputs.cooling_coefficients);

    GHE ghe_a(inputs.num_time_steps, inputs.hr_per_timestep, inputs.soil_temp, inputs.specific_heat, inputs.bh_length_a, inputs.bh_resistance_a,
              inputs.soil_conduct, inputs.rho_cp, inputs.g_self_a, inputs.lntts_self_a, stand_in, stand_in, false);

    // reading and creating load vector
    std::vector<double> bldgload;
    bldgload.reserve(inputs.num_time_steps);
    int month = 0;
    for (int time_step = 0; time_step < inputs.num_time_steps; time_step++) {
        if (std::remainder(time_step, inputs.building_load.size()) == 0) {
            month = 0;
        }
        bldgload.push_back(0.5*inputs.building_load[month]);
        month++;
    }

    // Run the model
    double Tr_a = 0;
    for (int time_step = 0; time_step < inputs.num_time_steps; time_step++) {

        // Operate the pump to set the loop flow rate
        pump.set_flow_rate();

        // Operate the heat pump using the last ghe outlet temperature as the new hp inlet temperature
        if (time_step == 0) {
            hp_a.outlet_temperature = 0;
        } else {
            hp_a.operate(ghe_a.outlet_temperature, pump.flow_rate, bldgload[time_step]);
        }
        double scaled_load = -1*bldgload[time_step]/ghe_a.bh_length;
        // Now run the GHE        
        Tr_a = ghe_a.simulate(time_step, hp_a.outlet_temperature, pump.flow_rate, scaled_load);

        // Finally, write data for each loop iteration
       outputs << "n"
            << ","
            << ghe_a.ghe_load[time_step]
            << ","
            << hp_a.outlet_temperature
            << ","
            << ghe_a.outlet_temperature
            << ","
            << ghe_a.MFT
            << ","
            << bldgload[time_step]
            << "\n";
        debug << time_step << ","
            << ghe_a.calc_lntts[time_step] << ","
            << ghe_a.interp_g_self[time_step] << ","
            << ghe_a.interp_g_cross[time_step] << ","
            << ghe_a.current_GHEload << ","
            << ghe_a.c1[0] << ","
            << ghe_a.c1[1] << ","
            << ghe_a.internal_Tr << ","
            << ghe_a.BH_temp << ","
            << ghe_a.outlet_temperature << ","
            << ghe_a.MFT << ","
            << bldgload[time_step] << "\n";
    }
    std::cout << "Executed for " << inputs.num_time_steps << " iterations"
              << "\n";
    std::cout << "csv outputs found at " << output_file_path << std::endl;
    return 0;
}

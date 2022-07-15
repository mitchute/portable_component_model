//#include "main.h"
#include <GHE.h>
#include <iostream>
#include <json.hpp>

// ToDo: Add comments explaining parts of this main function

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

struct input_vars {
    std::string json_path_in;
    double specific_heat_in;
    std::array<double, 3> heating_coefficients_in;
    std::array<double, 3> cooling_coefficients_in;
};

input_vars usr_inputs () {
    input_vars inputs;
    //User inputs
    std::cout << "Please specify the path of the ghe configuration data (type .json): "
              << "\n";
    std::cin >> inputs.json_path_in;

    //User inputs for non json data
    std::string choice;
    std::cout << "Would you like to enter a value for specific heat or use the default (4200)? (input yes or default): "
              << "\n";
    std::cin >> choice;
    if (choice == "yes") {
        std::cout << "Please enter the value of the specific heat: "
                  << "\n";
        std::cin >> inputs.specific_heat_in;
    }
    std::cout
        << "Would you like to enter a value for HP heating coefficients or use the default (0.705459, 0.005447, -0.000077)? (input yes or default): "
        << "\n";
    std::cin >> choice;
    if (choice == "yes") {
        std::cout << "Please enter the first heating coefficient: "
                  << "\n";
        std::cin >> inputs.heating_coefficients_in[0];
        std::cout << "Please enter the second heating coefficient: "
                  << "\n";
        std::cin >> inputs.heating_coefficients_in[1];
        std::cout << "Please enter the third heating coefficient: "
                  << "\n";
        std::cin >> inputs.heating_coefficients_in[2];
    }
    std::cout
        << "Would you like to enter a value for HP cooling coefficients or use the default (1.092440, 0.000314, 0.000114)? (input yes or default): "
        << "\n";
    std::cin >> choice;
    if (choice == "yes") {
        std::cout << "Please enter the first cooling coefficient: "
                  << "\n";
        std::cin >> inputs.cooling_coefficients_in[0];
        std::cout << "Please enter the second cooling coefficient: "
                  << "\n";
        std::cin >> inputs.cooling_coefficients_in[1];
        std::cout << "Please enter the third cooling coefficient: "
                  << "\n";
        std::cin >> inputs.cooling_coefficients_in[2];
    }
    return inputs;
};

main_vars load_data() {
    main_vars load_vars;
    std::string input_path;
    input_vars user_inputs = usr_inputs();
    load_vars.specific_heat = user_inputs.specific_heat_in;
    load_vars.heating_coefficients = user_inputs.heating_coefficients_in;
    load_vars.cooling_coefficients = user_inputs.cooling_coefficients_in;

    std::cout << "Specific heat = " << load_vars.specific_heat << "\n";
    std::cout << "Heating coefficients = {" << load_vars.heating_coefficients[0] << ", " << load_vars.heating_coefficients[1] << ", "
              << load_vars.heating_coefficients[2] << "}"
              << "\n";
    std::cout << "Cooling coefficients = {" << load_vars.cooling_coefficients[0] << ", " << load_vars.cooling_coefficients[1] << ", "
              << load_vars.cooling_coefficients[2] << "}"
              << "\n";

    input_path = user_inputs.json_path_in;
    input_path = "/Users/ryan/research_local/NREL/portable_component_model/standalone/inputs/test.json"; // temporary hard coded path for dev

    // opening file and reading data
    std::cout << "Path to ghe config data is: " << input_path << std::endl;

    std::ifstream file(input_path);
    json inputs;
    if (!file.is_open()) {
        std::cout << "Error, file is open" << std::endl;
        file.close();
        exit (EXIT_FAILURE);
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

    //hardcoded data not found in json
    load_vars.specific_heat = 4200;
    load_vars.heating_coefficients = {0.705459, 0.005447, -0.000077}; // HP heating coefficients hard coded from GLHEPro
    load_vars.cooling_coefficients = {1.092440, 0.000314, 0.000114}; // HP cooling coefficients hard coded from GLHEPro

    load_vars.num_hours = load_vars.building_load.size();
    return load_vars;
}

int main() {

    // Setup output streams
    // Note: data will be cleared for each run. Make sure to save data in a separate directory before running again.
    std::stringstream output_string;
    std::string output_file_path = "../standalone/outputs/outputs.csv";
    std::ofstream outputs(output_file_path);
    std::ofstream debug("../standalone/outputs/debug.csv");
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

    // Create instances of classes
    main_vars inputs = load_data();
    Pump pump;
    HeatPump hp(inputs.heating_coefficients, inputs.cooling_coefficients);
    GHE ghe(inputs.num_hours, inputs.soil_temp, inputs.specific_heat, inputs.bh_length, inputs.bh_resistance, inputs.soil_conduct, inputs.rho_cp,
            inputs.g_func, inputs.lntts);

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
    std::cout << "Executed successfully for " << inputs.num_hours << " iterations" << "\n";
    std::cout << "csv outputs found at " << output_file_path << std::endl;
    return 0;
}

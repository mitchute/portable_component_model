// ensure proper libraries are linked in the CMakeLists.txt file for building the executable
//#include "main.h"
#include <GHE.h>
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

struct main_vars {
    double soil_temp;
    double specific_heat;
    double bh_length_a;
    double bh_resistance_a;
    double bh_length_b;
    double bh_resistance_b;
    double soil_conduct;
    double rho_cp;
    double load_periods;
    double hr_per_timestep;
    int num_bh_a;
    int timestep_start_a;
    int num_bh_b;
    int timestep_start_b;
    int num_time_steps;
    std::vector<double> g_self_a;
    std::vector<double> lntts_self_a;
    std::vector<double> g_cross_a;
    std::vector<double> lntts_cross_a;
    std::vector<double> g_self_b;
    std::vector<double> lntts_self_b;
    std::vector<double> g_cross_b;
    std::vector<double> lntts_cross_b;
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

// input_vars usr_inputs() {
//     input_vars inputs;
//     std::string choice;
//
//     // User inputs
//     std::cout << "Is ghe configuration data located in standalone/inputs/ ? (input yes or no)"
//               << "\n";
//     std::cin >> choice;
//     if (choice == "no") {
//         std::cout << "Please specify the path of the ghe configuration data (type .json): "
//                   << "\n";
//         std::cin >> inputs.json_path_in;
//     }
//
//     // User inputs for non json data
//     std::cout << "Would you like to enter a value for specific heat or use the default (4200)? (input yes or default): "
//               << "\n";
//     std::cin >> choice;
//     if (choice == "yes") {
//         std::cout << "Please enter the value of the specific heat: "
//                   << "\n";
//         std::cin >> inputs.specific_heat_in;
//     }
//     std::cout
//         << "Would you like to enter a value for HP heating coefficients or use the default (0.705459, 0.005447, -0.000077)? (input yes or default):
//         "
//         << "\n";
//     std::cin >> choice;
//     if (choice == "yes") {
//         std::cout << "Please enter the first heating coefficient: "
//                   << "\n";
//         std::cin >> inputs.heating_coefficients_in[0];
//         std::cout << "Please enter the second heating coefficient: "
//                   << "\n";
//         std::cin >> inputs.heating_coefficients_in[1];
//         std::cout << "Please enter the third heating coefficient: "
//                   << "\n";
//         std::cin >> inputs.heating_coefficients_in[2];
//     }
//     std::cout
//         << "Would you like to enter a value for HP cooling coefficients or use the default (1.092440, 0.000314, 0.000114)? (input yes or default):
//         "
//         << "\n";
//     std::cin >> choice;
//     if (choice == "yes") {
//         std::cout << "Please enter the first cooling coefficient: "
//                   << "\n";
//         std::cin >> inputs.cooling_coefficients_in[0];
//         std::cout << "Please enter the second cooling coefficient: "
//                   << "\n";
//         std::cin >> inputs.cooling_coefficients_in[1];
//         std::cout << "Please enter the third cooling coefficient: "
//                   << "\n";
//         std::cin >> inputs.cooling_coefficients_in[2];
//     }
//     return inputs;
// };

main_vars load_data() {
    main_vars load_vars;
    std::string input_path;

    // hardcoded data not found in json
    load_vars.specific_heat = 4200;
    load_vars.heating_coefficients = {0.705459, 0.005447, -0.000077}; // HP heating coefficients hard coded from GLHEPro
    load_vars.cooling_coefficients = {1.092440, 0.000314, 0.000114};  // HP cooling coefficients hard coded from GLHEPro
    input_path = "../standalone/inputs/A-B.json";                     // default path

    // User inputs to change the above hard coded data. Can be commented out.
    //    input_vars user_inputs = usr_inputs();
    //    load_vars.specific_heat = user_inputs.specific_heat_in;
    //    load_vars.heating_coefficients = user_inputs.heating_coefficients_in;
    //    load_vars.cooling_coefficients = user_inputs.cooling_coefficients_in;
    //    if (user_inputs.json_path_in != "") {
    //        input_path = user_inputs.json_path_in;
    //    }

    std::cout << "Specific heat = " << load_vars.specific_heat << "\n";
    std::cout << "Heating coefficients = {" << load_vars.heating_coefficients[0] << ", " << load_vars.heating_coefficients[1] << ", "
              << load_vars.heating_coefficients[2] << "}"
              << "\n";
    std::cout << "Cooling coefficients = {" << load_vars.cooling_coefficients[0] << ", " << load_vars.cooling_coefficients[1] << ", "
              << load_vars.cooling_coefficients[2] << "}"
              << "\n";

    // opening file and reading data
    std::cout << "Path to ghe config data is: " << input_path << std::endl;

    std::ifstream file(input_path);
    json inputs;
    if (!file.is_open()) {
        std::cout << "Error, file is open" << std::endl;
        file.close();
        exit(EXIT_FAILURE);
    }
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
    inputs["ghe"][0]["cross_lntts"].get_to(load_vars.lntts_cross_a);
    inputs["ghe"][0]["cross_g_func"].get_to(load_vars.g_cross_a);

    inputs["ghe"][1]["bh_length"].get_to(load_vars.bh_length_b);
    inputs["ghe"][1]["bh_resistance"].get_to(load_vars.bh_resistance_b);
    inputs["ghe"][1]["num_bh"].get_to(load_vars.num_bh_b);
    inputs["ghe"][1]["timestep_start_operate"].get_to(load_vars.timestep_start_b);
    inputs["ghe"][1]["self_lntts"].get_to(load_vars.lntts_self_b);
    inputs["ghe"][1]["self_g_func"].get_to(load_vars.g_self_b);
    inputs["ghe"][1]["cross_lntts"].get_to(load_vars.lntts_cross_b);
    inputs["ghe"][1]["cross_g_func"].get_to(load_vars.g_cross_b);

    // load_vars.num_time_steps = load_vars.building_load.size() * load_vars.hr_per_timestep * load_vars.load_periods;
    load_vars.num_time_steps = load_vars.building_load.size() * load_vars.load_periods; // monthly for 20 years
    return load_vars;
}

int main() {
    std::vector<double> stand_in_cross;

    // Setup output streams
    // Note: data will be cleared for each run. Make sure to save data in a separate directory before running again.
    std::stringstream output_string;
    std::string output_file_path = "../standalone/outputs/outputs.csv";
    std::ofstream outputs(output_file_path);
    std::ofstream debug("../standalone/outputs/debug.csv");
    outputs << "n"
            << ","
            << "A: GHE Load"
            << ","
            << "B: GHE Load"
            << ","
            << "A: ghe_Tin (HP_Tout)"
            << ","
            << "B: ghe_Tin (HP_Tout)"
            << ","
            << "A: ghe_Tout (HP_Tin)"
            << ","
            << "B: ghe_Tout (HP_Tin)"
            << ","
            << "A: MFT"
            << ","
            << "B: MFT"
            << ","
            << "bldgload"
            << "\n";
    debug << "time_step"
          << ","
          << "Tr_a"
          << ","
          << "Tr_b"
          << ","
          << "ghe_a.current_GHEload"
          << ","
          << "ghe_b.current_GHEload"
          << ","
          << "hp_a.outlet_temperature"
          << ","
          << "hp_b.outlet_temperature"
          << ","
          << "ghe_a.calc_lntts[time_step]"
          << ","
          << "ghe_b.calc_lntts[time_step]"
          << ","
          << "ghe_a.interp_g_self[time_step]"
          << ","
          << "ghe_b.interp_g_self[time_step]"
          << ","
          << "ghe_a.interp_g_cross[time_step]"
          << ","
          << "ghe_b.interp_g_cross[time_step]"
          << ","
          << "ghe_a.outlet_temperature"
          << ","
          << "ghe_b.outlet_temperature"
          << ","
          << "ghe_a.MFT"
          << ","
          << "ghe_b.MFT"
          << ","
          << "bldgload"
          << "\n";

    // Create instances of classes
    main_vars inputs = load_data();
    Pump pump;
    HeatPump hp_a(inputs.heating_coefficients, inputs.cooling_coefficients);
    HeatPump hp_b(inputs.heating_coefficients, inputs.cooling_coefficients);

    GHE ghe_a(inputs.num_time_steps, inputs.hr_per_timestep, inputs.soil_temp, inputs.specific_heat, inputs.bh_length_a, inputs.bh_resistance_a,
              inputs.soil_conduct, inputs.rho_cp, inputs.g_self_a, inputs.lntts_self_a, inputs.g_cross_a, inputs.lntts_cross_a);
    GHE ghe_b(inputs.num_time_steps, inputs.hr_per_timestep, inputs.soil_temp, inputs.specific_heat, inputs.bh_length_b, inputs.bh_resistance_b,
              inputs.soil_conduct, inputs.rho_cp, inputs.g_self_b, inputs.lntts_self_b, inputs.g_cross_b, inputs.lntts_cross_b);

    // reading and creating load vector
    std::vector<double> bldgload;
    bldgload.reserve(inputs.num_time_steps);
    int month = 0;
    for (int time_step = 0; time_step < inputs.num_time_steps; time_step++) {
        if (std::remainder(time_step, inputs.building_load.size()) == 0) {
            month = 0;
        }
        bldgload.push_back(inputs.building_load[month]);
        month++;
    }

    // Run the model
    double Tr_a = 0;
    double Tr_b = 0;
    for (int time_step = 0; time_step < inputs.num_time_steps; time_step++) {

        // Operate the pump to set the loop flow rate
        pump.set_flow_rate();

        // Operate the heat pump using the last ghe outlet temperature as the new hp inlet temperature
        if (time_step == 0) {
            hp_a.outlet_temperature = 0;
            // hp_b.outlet_temperature = 0;
        } else {
            hp_a.operate(ghe_a.outlet_temperature, pump.flow_rate, bldgload[time_step]);
            hp_b.operate(ghe_b.outlet_temperature, pump.flow_rate, bldgload[time_step]);
        }

        // Now run the GHE
        Tr_a = ghe_a.simulate(time_step, hp_a.outlet_temperature, pump.flow_rate, Tr_b);
        Tr_b = ghe_b.simulate(time_step, hp_b.outlet_temperature, pump.flow_rate, Tr_a);

        // Finally, write data for each loop iteration
        outputs << time_step << "," << ghe_a.ghe_load.back() << ","
                << ghe_b.ghe_load.back()
                << "," << hp_a.outlet_temperature << ","
                << hp_b.outlet_temperature
                << "," << ghe_a.outlet_temperature << ","
                << ghe_b.outlet_temperature
                << "," << ghe_a.MFT << ","
                << ghe_b.MFT
                << "," << bldgload[time_step] << "\n";
        debug << time_step << "," << Tr_a << ","
              << Tr_b
              << "," << ghe_a.current_GHEload << ","
              << ghe_b.current_GHEload
              << "," << hp_a.outlet_temperature << ","
              << hp_b.outlet_temperature
              << "," << ghe_a.calc_lntts[time_step] << ","
              << ghe_b.calc_lntts[time_step]
              << "," << ghe_a.interp_g_self[time_step] << ","
              << ghe_b.interp_g_self[time_step]
              << "," << ghe_a.interp_g_cross[time_step] << ","
              << ghe_b.interp_g_cross[time_step]
              << "," << ghe_a.outlet_temperature << ","
              << ghe_b.outlet_temperature
              << "," << ghe_a.MFT << ","
              << ghe_b.MFT
              << "," << bldgload[time_step] << "\n";
    }
    std::cout << "Executed for " << inputs.num_time_steps << " iterations"
              << "\n";
    std::cout << "csv outputs found at " << output_file_path << std::endl;
    return 0;
}

// Notes for hunting down exponential behavior:
// g function both self and cross matches the python code exactly after interpolation, so not source of error
//  Variables with exponential behavior:

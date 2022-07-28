#include <GHE.h>
#include <iostream>
#include <json.hpp>
#include <filesystem>
#include "doctest.h"

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
    bool file_open;
    std::vector<double> g_self_a;
    std::vector<double> lntts_self_a;
    std::vector<double> g_cross_a;
    std::vector<double> lntts_cross_a;
    std::vector<double> g_self_b;
    std::vector<double> lntts_self_b;
    std::vector<double> g_cross_b;
    std::vector<double> lntts_cross_b;
    std::vector<double> building_load;
  
};




main_vars load_data() {
    main_vars load_vars;

    // hardcoded data not found in json
    load_vars.specific_heat = 4200;
    std::string input_path = "../standalone/inputs/A-B.json";                     // default path

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

std::vector <double> load_test(){
    std::ifstream PY_MFT_in ("../test/inputs/PY_MFT.txt");
    std::vector<double> PY_MFT(std::istream_iterator<double>{PY_MFT_in}, std::istream_iterator<double>{});
    return PY_MFT;
}

TEST_CASE("Test GHE Multi Borehole") {

    std::vector<double> MFT = load_test(); 

    // Setup output streams
    // Note: data will be cleared for each run. Make sure to save data in a separate directory before running again.
    if (!std::filesystem::is_directory("../standalone/outputs/")) {
        std::filesystem::create_directory("../standalone/outputs/");
    }
    std::stringstream output_string;
    std::string output_file_path = "../standalone/outputs/multi_outputs.csv";
    std::ofstream outputs(output_file_path);
    std::ofstream debug("../standalone/outputs/multi_debug.csv");
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
    debug << "time_step" << ","
          << "ghe_a.calc_lntts[time_step]" << ","
          << "ghe_a.interp_g_self[time_step]" << ","
          << "ghe_a.interp_g_cross[time_step]" << ","
          << "ghe_a.current_GHEload" << ","
          << "ghe_a.c1[0]" << ","
          << "ghe_a.c1[1]" << ","
          << "ghe_a.internal_Tr" << ","
          << "ghe_b.cross_Tr" << ","
          << "ghe_a.BH_temp" << ","
          << "ghe_a.outlet_temperature" << ","
          << "ghe_a.MFT" << ","
          << "bldgload[time_step]" << "\n";

    // Create instances of classes
    main_vars inputs = load_data();
    Pump pump;

    GHE ghe_a(inputs.num_time_steps, inputs.hr_per_timestep, inputs.soil_temp, inputs.specific_heat, inputs.bh_length_a, inputs.bh_resistance_a,
              inputs.soil_conduct, inputs.rho_cp, inputs.g_self_a, inputs.lntts_self_a, inputs.g_cross_a, inputs.lntts_cross_a, false);
    GHE ghe_b(inputs.num_time_steps, inputs.hr_per_timestep, inputs.soil_temp, inputs.specific_heat, inputs.bh_length_b, inputs.bh_resistance_b,
              inputs.soil_conduct, inputs.rho_cp, inputs.g_self_b, inputs.lntts_self_b, inputs.g_cross_b, inputs.lntts_cross_b, false);

    // reading and creating load vector
    std::vector<double> bldgload;
    bldgload.reserve(inputs.num_time_steps);
    int month = 0;
    for (int time_step = 0; time_step < inputs.num_time_steps; time_step++) {
        if (std::remainder(time_step, inputs.building_load.size()) == 0) {
            month = 0;
        }
        bldgload.push_back(-1*inputs.building_load[month]);
        month++;
    }

    // Run the model
    double Tr_a = 0;
    double Tr_b = 0;
    for (int time_step = 0; time_step < inputs.num_time_steps; time_step++) {

        // Operate the pump to set the loop flow rate
        pump.set_flow_rate();


        // Now run the GHE
        double scaled_load_a = (bldgload[time_step]/(4*inputs.bh_length_a)); // in order to match the load given by python
        double scaled_load_b = (bldgload[time_step]/(4*inputs.bh_length_b)); // in order to match the load given by python

        Tr_a = ghe_a.simulate(time_step, 0, pump.flow_rate, scaled_load_a, Tr_b);
        Tr_b = ghe_b.simulate(time_step, 0, pump.flow_rate, scaled_load_b,Tr_a);
               // Finally, write data for each loop iteration
        outputs << time_step << "," << -1*ghe_a.ghe_load.back() << ","
                << -1*ghe_b.ghe_load.back()
                << "," << "hp_a.outlet_temperature" << ","
                << "hp_b.outlet_temperature"
                << "," << ghe_a.outlet_temperature << ","
                << ghe_b.outlet_temperature
                << "," << ghe_a.MFT << ","
                << ghe_b.MFT
                << "," << bldgload[time_step] << "\n";
        debug << time_step << ","
              << ghe_a.calc_lntts[time_step] << ","
              << ghe_a.interp_g_self[time_step] << ","
              << ghe_b.interp_g_cross[time_step] << ","
              << ghe_a.current_GHEload << ","
              << ghe_a.c1[0] << ","
              << ghe_a.c1[1] << ","
              << ghe_a.internal_Tr << ","
              << ghe_b.cross_Tr << ","
              << ghe_a.BH_temp << ","
              << ghe_a.outlet_temperature << ","
              << ghe_a.MFT << ","
              << bldgload[time_step] << "\n";

        CHECK(ghe_a.MFT == doctest::Approx(MFT[time_step]).epsilon(0.1));
    }
}

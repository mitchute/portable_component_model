#include "doctest.h"
#include <GHE.h>
#include <cmath>
#include <iostream>
#include <sstream>

struct main_vars {
    double soil_temp = 10;
    double specific_heat = 4200;
    double bh_length = 100;
    double bh_resistance = 0.2477;
    double soil_conduct = 2;
    double rho_cp = 2000000;
    double load_periods = 1;
    double hr_per_timestep = 1;
    int num_bh = 1;
    int timestep_start = 1;
    int num_hours = 8760;
    std::vector<double> g_func = {
        -2.556919564, -2.483889186, -2.408186285, -2.329364731, -2.247153415, -2.161382726, -2.071953832, -1.978823254, -1.881993912, -1.781509211,
        -1.677448741, -1.569925155, -1.45908234,  -1.34509531,  -1.228172249, -1.108558858, -0.986544532, -0.862469138, -0.736728573, -0.609776928,
        -0.482123413, -0.354322868, -0.226959789, -0.100626916, 0.024099618,  0.146685992,  0.266664496,  0.383652944,  0.497367798,  0.607629979,
        0.714363225,  0.817585782,  0.917396924,  1.013960203,  1.107485466,  1.198211462,  1.286390425,  1.372275509,  1.456111388,  1.538127898,
        1.618536337,  1.697527825,  1.775273191,  1.851923828,  1.927613133,  2.002458214,  2.076561654,  2.150013227,  2.222891481,  2.29526516,
        2.776706956,  3.073538762,  3.418085031,  3.711170756,  4.049191639,  4.381141145,  4.630139663,  4.941340709,  5.117006771,  5.237526289,
        5.401676261,  5.512698806,  5.759025737,  5.96747168,   6.024558855,  6.076104037,  6.126494025,  6.16581059,   6.199436891,  6.228510294,
        6.252934535,  6.271152953,  6.29201203,   6.306229149,  6.313183067,  6.324383675};
    std::vector<double> lntts = {
        -15.22015406, -15.08300806, -14.94586206, -14.80871605, -14.67157005, -14.53442405, -14.39727805, -14.26013205, -14.12298605, -13.98584005,
        -13.84869405, -13.71154804, -13.57440204, -13.43725604, -13.30011004, -13.16296404, -13.02581804, -12.88867204, -12.75152604, -12.61438004,
        -12.47723403, -12.34008803, -12.20294203, -12.06579603, -11.92865003, -11.79150403, -11.65435803, -11.51721203, -11.38006602, -11.24292002,
        -11.10577402, -10.96862802, -10.83148202, -10.69433602, -10.55719002, -10.42004402, -10.28289802, -10.14575201, -10.00860601, -9.871460012,
        -9.734314011, -9.597168009, -9.460022008, -9.322876007, -9.185730006, -9.048584005, -8.911438004, -8.774292002, -8.637146001, -8.5,
        -7.8,         -7.2,         -6.5,         -5.9,         -5.2,         -4.5,         -3.963,       -3.27,        -2.864,       -2.577,
        -2.171,       -1.884,       -1.191,       -0.497,       -0.274,       -0.051,       0.196,        0.419,        0.642,        0.873,
        1.112,        1.335,        1.679,        2.028,        2.275,        3.003};
    std::vector<double> building_load;
    std::array<double, 3> heating_coefficients = {0.705459, 0.005447, -0.000077};
    std::array<double, 3> cooling_coefficients = {1.092440, 0.000314, 0.000114};
};

TEST_CASE("Test the GHE Model") {

    // Setup output streams
    // Note: data will be cleared for each run. Make sure to save data in a separate directory before running again.
    std::stringstream output_string;
    std::ofstream outputs("../standalone/Outputs/outputs.csv");
    std::ofstream debug("../standalone/Outputs/debug.csv");

    main_vars inputs;
    double bldgn = -1000;
    inputs.building_load.reserve(inputs.num_hours);
    for (int j = 0; j <= inputs.num_hours; j++) {
        // Update the building load
        if (std::remainder(j, 730) == 0) {
            bldgn = bldgn * -1;
        }
        inputs.building_load.push_back(bldgn);
    }

    // Create the GHE model
    int num_hours = 8760; // num of iterations
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

    // Get objects for all the components around the loop
    Pump pump;
    HeatPump hp(inputs.heating_coefficients, inputs.cooling_coefficients);
    GHE ghe(inputs.num_hours, inputs.soil_temp, inputs.specific_heat, inputs.bh_length, inputs.bh_resistance, inputs.soil_conduct, inputs.rho_cp,
            inputs.g_func, inputs.lntts);

    // Run the model
    for (int hour = 0; hour < num_hours; hour++) {
        // Operate the pump to set the loop flow rate
        pump.set_flow_rate();
        // Operate the heat pump using the last ghe outlet temperature as the new hp inlet temperature
        if (hour == 0) {
            hp.outlet_temperature = 0;
        } else {
            hp.operate(ghe.outlet_temperature, pump.flow_rate, inputs.building_load[hour]);
        }
        // Now run the GHE
        ghe.simulate(hour, hp.outlet_temperature, pump.flow_rate);
        outputs << hour << "," << ghe.ghe_load.back() << "," << hp.outlet_temperature << "," << ghe.outlet_temperature << "," << ghe.Tf << ","
                << inputs.building_load[hour] << "\n";
        debug << ghe.current_GHEload << "," << ghe.ghe_Tin << "," << ghe.hours_as_seconds[hour] << "," << ghe.ghe_load[hour] << ","
              << ghe.calc_lntts[hour] << "," << ghe.interp_g_values[hour] << "," << ghe.outlet_temperature << "," << ghe.Tf << "," << ghe.c1 << "\n";
    }
    CHECK(pump.flow_rate == 0.2);
}

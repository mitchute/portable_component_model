#include "doctest.h"
#include <GHE.h>
#include <cmath>
#include <sstream>
#include <iostream>

TEST_CASE("Test the GHE Model") {
    // Setup output streams
    std::stringstream output_string;
    std::ofstream static outputs("../Outputs/outputs.csv", std::ofstream::out);
    std::ofstream static debug("../Outputs/debug.csv", std::ofstream::out);

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
    HeatPump hp({0.705459, 0.005447, -0.000077},{1.092440, 0.000314, 0.000114});
    GHE ghe(num_hours);

    // Run the model
    double building_load = -1000;
    for (int hour = 0; hour < num_hours; hour++) {
        // Update the building load
        if (std::remainder(hour, 730) == 0) {
            building_load = building_load * -1;
        }
        // Operate the pump to set the loop flow rate
        pump.set_flow_rate();
        // Operate the heat pump using the last ghe outlet temperature as the new hp inlet temperature
        if (hour == 0){
            hp.outlet_temperature = 0;
        }
        else {
            hp.operate(ghe.outlet_temperature, pump.flow_rate, building_load);
        }
        // Now run the GHE
        ghe.simulate(hour, hp.outlet_temperature, pump.flow_rate);
        outputs << hour << "," << ghe.ghe_load.back() << "," << hp.outlet_temperature << "," << ghe.outlet_temperature << "," << ghe.Tf << ","
                << building_load << "\n";
        debug << ghe.current_GHEload << "," << ghe.ghe_Tin << "," << ghe.hours_as_seconds[hour] << "," << ghe.ghe_load[hour] << "," << ghe.calc_lntts[hour] << "," << ghe.interp_g_values[hour]
              << "," << ghe.outlet_temperature << "," << ghe.Tf << "," << ghe.c1 << "\n";
    }
    CHECK(pump.flow_rate == 0.2);
}

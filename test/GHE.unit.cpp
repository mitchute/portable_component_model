#include "doctest.h"
#include <GHE.h>
#include <cmath>
#include <sstream>

TEST_CASE("Test the GHE Model") {
    // Setup output streams
    std::stringstream output_string;
    std::ofstream static outputs("../Outputs/outputs.csv", std::ofstream::out);
    std::ofstream static debug("../Outputs/debug.csv", std::ofstream::out);

    // Create the GHE model
    int num_time_steps = 8760; // num of iterations
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
    HeatPump hp({1.092440, 0.000314, 0.000114}, {0.705459, 0.005447, -0.000077});
    GHE ghe(num_time_steps);

    // Run the model
    double building_load = -1000;
    for (int n = 0; n < num_time_steps; n++) {
        // Update the building load
        if (std::remainder(n, 730) == 0) {
            building_load = building_load * -1;
        }
        // Operate the pump to set the loop flow rate
        pump.set_flow_rate();
        // Operate the heat pump using the last ghe outlet temperature as the new hp inlet temperature
        // TODO: Set initial value to something? It looks like it was using 0.0 on the first pass previously
        hp.operate(ghe.outlet_temperature, pump.flow_rate, building_load);
        // Now run the GHE
        ghe.simulate(n, hp.outlet_temperature, pump.flow_rate);
        // TODO: I have no idea if I got this list merged right, so you should verify the right outputs are coming out
        outputs << n << "," << ghe.ghe_load.back() << "," << hp.outlet_temperature << "," << ghe.outlet_temperature << "," << ghe.ghe_Tf.back() << ","
                << building_load << "\n";
        debug << ghe.qn << "," << ghe.ghe_Tin << "," << ghe.q_time[n] << "," << ghe.ghe_load[n] << "," << ghe.q_lntts[n] << "," << ghe.g_data[n]
              << "," << ghe.ghe_Tout[n] << "," << ghe.ghe_Tf[n] << "," << ghe.c1 << "\n";
    }
    CHECK(pump.flow_rate == 0.2);
}

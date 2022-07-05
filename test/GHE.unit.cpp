#include "doctest.h"
#include <GHE.h>
#include <sstream>
#include <iostream>

TEST_CASE("Test the GHE Model") {
    // Setup output streams
    std::stringstream output_string;
    std::stringstream debug_string;

    // Create the GHE model
    int num_time_steps = 8760;  //num of iterations
    GHE ghe(&output_string, &debug_string, num_time_steps);  // TODO: Consider pulling time step loop out to here
    output_string << "n" << "," << "GHE Load" << "," << "ghe_Tin (HP_Tout)" << "," << "ghe_Tout (HP_Tin)" <<  "," << "bldgload" << "\n";
    debug_string << "n" << "," << "g_data[n]" << "," << "q_lntts[n]" << "," << "q_time[n]" << "\n";

    // Run the model
    ghe.main_model(num_time_steps);
    CHECK(ghe.mdot == 0.2);

    //CHECK(ghe.temp)
}

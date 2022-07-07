#include "doctest.h"
#include <GHE.h>
#include <sstream>

TEST_CASE("Test the GHE Model") {
    // Setup output streams
    std::stringstream output_string;
    std::ofstream static outputs("../Outputs/outputs.csv", std::ofstream::out);

    // Create the GHE model
    int num_time_steps = 8760; // num of iterations
    GHE ghe(&outputs, num_time_steps);
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

    // Run the model
    int n = 0;
    while (n < num_time_steps) {
        ghe.main_model(n);
        n++;
    }
    CHECK(ghe.mdot == 0.2);
}

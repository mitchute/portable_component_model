#include <fstream>
#include <iostream>
#include <GHE.h>

int main() {
    // Setup output streams
    std::ofstream outputs("../ouputs.csv", std::ofstream::out);
    std::ofstream debug_outputs("../debugging.csv", std::ofstream::out);

    // Create the GHE model
    int num_time_steps = 8760;  //num of iterations
    GHE ghe(&outputs, num_time_steps);
    outputs << "n" << "," << "GHE Load" << "," << "ghe_Tin (HP_Tout)" << "," << "ghe_Tout (HP_Tin)" <<  "," << "bldgload" << "\n";
    debug_outputs << "n" << "," << "g_data[n]" << "," << "q_lntts[n]" << "," << "q_time[n]" << "\n";

    // Run the model
    ghe.main_model(num_time_steps);
    std::cout << "Executed successfully" << std::endl;
    return 0;
}

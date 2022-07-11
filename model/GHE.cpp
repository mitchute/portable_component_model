//-------------------SIMPLE MEAN GHE -------------------------------

// Main loop built upon Appendix A of https://shareok.org/handle/11244/323367

#include "GHE.h"
#include <algorithm>
#include <cmath>
#include <iostream>

// Variables (See nomenclature section):
// H - active borehole length (Active length of pipe)
// Rb - borehole Resistance
// Ts - soil temp
// ghe_Tout - GHE outlet temp (C)
// ghe_Tf - MFT of GHE
// mdot - mass flow rate
// cp - specific heat (heat energy required to change temp of material)
// ks - soil conductivity
// ghe_load - GHE load
// lntts - non-dimensionalized time input into g-function (x-axis on plots)
// g_func - g values for given lntts values (y-axis on plots)
// q_load - load data
// q_time - time data corresponding to each ghe_load value
// q_lntts - calculated non-dimensionalized time for q_time
// n - index of main loop
// m - number of iterations of main loop
// c1 - history terms
// c0 - simplification term
// qn - current GHE load
// qn1 - previous iteration GHE load
// q_delta - GHE load delta
// total - total of summation equation. Defined in function
// inputs - data structure with inputs read from the JSON file
// alphas - soil thermal diffusivity (see pg 12 of Mitchell) (also see line 30 of main.py from the repo shared by Matt)
// ts - characteristic time (way to non-dimensionalize the time) see page 12
// i/j - general purpose indexes
// gn - g value at index n

void HeatPump::operate(double inlet_temperature, double operating_flow_rate, double building_load) {
    double source_side_load;
    if (building_load > 0) {
        source_side_load = building_load * (heating[0] + heating[1] * (inlet_temperature) + heating[2] * (inlet_temperature * inlet_temperature));
    } else {
        source_side_load = building_load * (cooling[0] + cooling[1] * (inlet_temperature) + cooling[2] * (inlet_temperature * inlet_temperature));
    }
    outlet_temperature = inlet_temperature - (source_side_load / (operating_flow_rate * specific_heat));
}

GHE::GHE(int num_hours) {
    hours_as_seconds.reserve(num_hours);
    calc_lntts.reserve(num_hours);
    ghe_load.reserve(num_hours);
    interp_g_values.reserve(num_hours);

    // TODO: Make a worker function for this called calculate_g_functions() that automatically populates these variables. Discuss with Matt
    lntts = {-15.22015406, -15.08300806, -14.94586206, -14.80871605, -14.67157005, -14.53442405, -14.39727805, -14.26013205, -14.12298605,
             -13.98584005, -13.84869405, -13.71154804, -13.57440204, -13.43725604, -13.30011004, -13.16296404, -13.02581804, -12.88867204,
             -12.75152604, -12.61438004, -12.47723403, -12.34008803, -12.20294203, -12.06579603, -11.92865003, -11.79150403, -11.65435803,
             -11.51721203, -11.38006602, -11.24292002, -11.10577402, -10.96862802, -10.83148202, -10.69433602, -10.55719002, -10.42004402,
             -10.28289802, -10.14575201, -10.00860601, -9.871460012, -9.734314011, -9.597168009, -9.460022008, -9.322876007, -9.185730006,
             -9.048584005, -8.911438004, -8.774292002, -8.637146001, -8.5,         -7.8,         -7.2,         -6.5,         -5.9,
             -5.2,         -4.5,         -3.963,       -3.27,        -2.864,       -2.577,       -2.171,       -1.884,       -1.191,
             -0.497,       -0.274,       -0.051,       0.196,        0.419,        0.642,        0.873,        1.112,        1.335,
             1.679,        2.028,        2.275,        3.003};

    g_func = {-2.556919564, -2.483889186, -2.408186285, -2.329364731, -2.247153415, -2.161382726, -2.071953832, -1.978823254, -1.881993912,
              -1.781509211, -1.677448741, -1.569925155, -1.45908234,  -1.34509531,  -1.228172249, -1.108558858, -0.986544532, -0.862469138,
              -0.736728573, -0.609776928, -0.482123413, -0.354322868, -0.226959789, -0.100626916, 0.024099618,  0.146685992,  0.266664496,
              0.383652944,  0.497367798,  0.607629979,  0.714363225,  0.817585782,  0.917396924,  1.013960203,  1.107485466,  1.198211462,
              1.286390425,  1.372275509,  1.456111388,  1.538127898,  1.618536337,  1.697527825,  1.775273191,  1.851923828,  1.927613133,
              2.002458214,  2.076561654,  2.150013227,  2.222891481,  2.29526516,   2.776706956,  3.073538762,  3.418085031,  3.711170756,
              4.049191639,  4.381141145,  4.630139663,  4.941340709,  5.117006771,  5.237526289,  5.401676261,  5.512698806,  5.759025737,
              5.96747168,   6.024558855,  6.076104037,  6.126494025,  6.16581059,   6.199436891,  6.228510294,  6.252934535,  6.271152953,
              6.29201203,   6.306229149,  6.313183067,  6.324383675};

    soil_temp = 10;
    specific_heat = 4200;
    bh_length = 100;
    bh_resistance = 0.2477;
    soil_conduct = 2.0;
    rho_cp = 2000000.0;

    double constexpr pi = 3.14159265358979323846;
    c0 = 1 / (2 * pi * soil_conduct);

    // Deriving characteristic time
    ts = pow(bh_length, 2) / (9 * (soil_conduct / rho_cp));
    // Creating interpolated g dataset
    g_expander(num_hours);
}

// Expanding g data as step function so that it has same length as q_load
void GHE::g_expander(int num_hours) {
    int n = 0;
    auto lntts_begin = lntts.begin();
    auto lntts_end = lntts.end();
    while (n < num_hours) {
        // Building vector of lntts values
        hours_as_seconds.push_back(3600 * (n + 1));
        calc_lntts.push_back(log(hours_as_seconds[n] / ts));

        // Interpolator
        // Assuming x and y are equal length - this should be caught upon initialization
        //  Assuming x and y have at least 2 elements
        //  Assuming x is monotonic

        auto upper_it = std::upper_bound(lntts_begin, lntts_end, calc_lntts[n]);
        if (upper_it == lntts_begin) {
            // Extrapolating beyond the lower bound
            interp_g_values.push_back(g_func.front());
            std::cout << "Extrapolating beyond the lower bound index = " << n << "\n";
            std::cout << g_func.front() << "\n";
        } else if (upper_it == lntts_end) {
            // Extrapolating beyond the upper bound
            interp_g_values.push_back(g_func.back());
            std::cout << "Extrapolating beyond the upper bound index = " << n << "\n";
        } else {
            auto u_idx = std::distance(lntts.begin(), upper_it);
            auto l_idx = u_idx - 1;
            double lntts_low = lntts[l_idx];
            double lntts_high = lntts[u_idx];
            double g_func_low = g_func[l_idx];
            double g_func_high = g_func[u_idx];
            double g_temp = (calc_lntts[n] - lntts_low) / (lntts_high - lntts_low) * (g_func_high - g_func_low) + g_func_low;
            interp_g_values.push_back(g_temp);
        }
        ++n;
    }
}

// Summation eqn 1.11 from Mitchell Appendix A
double GHE::summation(int hour) {
    double q_delta;
    double total = 0;
    int i = 0;
    int j = hour;
    if (hour != 0) {
        while (i < hour) {
            if (i == 0) {
                q_delta = ghe_load[i] - 0;
            } else {
                q_delta = ghe_load[i] - ghe_load[i - 1];
            }
            // eqn 1.11
            total = total + (q_delta * interp_g_values[j]);
            j = j - 1;
            ++i;
        }
    }
    return total;
}

void GHE::simulate(int hour, double ghe_inlet_temperature, double mass_flow_rate) {
    // loading g_data
    double gn = interp_g_values[hour];

    // eqn 1.11
    c1 = summation(hour);

    // calculating current load and appending to data
    double previous_GHEload = 0.0;
    if (hour > 0) {
        previous_GHEload = ghe_load[hour - 1];
    }
    current_GHEload = 0.0;
    if (hour > 0) {
        current_GHEload = (ghe_inlet_temperature - soil_temp + ((previous_GHEload * gn) * c0) - (c1 * c0)) /
                          ((0.5 * (bh_length / (mass_flow_rate * specific_heat))) + (gn * c0) + bh_resistance);
    }
    ghe_load.push_back(current_GHEload);

    // 1.12
    Tf = soil_temp + c0 * (((current_GHEload - previous_GHEload) * gn) + c1) + current_GHEload * bh_resistance;
    // 1.14
    outlet_temperature = Tf - 0.5 * ((current_GHEload * bh_length) / (mass_flow_rate * specific_heat));
}

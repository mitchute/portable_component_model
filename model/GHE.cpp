//-------------------SIMPLE MEAN GHE -------------------------------

// Main loop built upon Appendix A of https://shareok.org/handle/11244/323367

#include "GHE.h"
#include <algorithm>
#include <cmath>
#include <iostream>

// TODO: move these descriptions to a README.txt file that explain model and vars.

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

GHE::GHE(int num_hours, double soil_temp, double specific_heat, double bh_length, double bh_resistance, double soil_conduct, double rho_cp,
         std::vector<double> &g_func, std::vector<double> &lntts) {

    // TODO: change var names so that 'this->' is not necessary

    this->soil_temp = soil_temp;
    this->specific_heat = specific_heat;
    this->bh_length = bh_length;
    this->bh_resistance = bh_resistance;
    this->soil_conduct = soil_conduct;
    this->rho_cp = rho_cp;
    this->g_func = g_func;
    this->lntts = lntts;

    hours_as_seconds.reserve(num_hours);
    calc_lntts.reserve(num_hours);
    ghe_load.reserve(num_hours);
    interp_g_values.reserve(num_hours);

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

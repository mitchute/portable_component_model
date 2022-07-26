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

GHE::GHE(int num_time_steps, int hours_per_timestep_in, double soil_temp_in, double specific_heat_in, double bh_length_in, double bh_resistance_in,
         double soil_conduct_in, double rho_cp_in, std::vector<double> &g_func_self_in, std::vector<double> &lntts_self_in,
         std::vector<double> &g_func_cross_in, std::vector<double> &lntts_cross_in, bool load_from_building_in) {

    soil_temp = soil_temp_in;
    specific_heat = specific_heat_in;
    bh_length = bh_length_in;
    bh_resistance = bh_resistance_in;
    soil_conduct = soil_conduct_in;
    rho_cp = rho_cp_in;
    g_func_self = g_func_self_in;
    lntts_self = lntts_self_in;
    g_func_cross = g_func_cross_in;
    lntts_cross = lntts_cross_in;
    hours_per_timestep = hours_per_timestep_in;
    load_from_building = load_from_building_in;
    calc_lntts.reserve(num_time_steps);
    ghe_load.reserve(num_time_steps);
    interp_g_self.reserve(num_time_steps);
    interp_g_cross.reserve(num_time_steps);

    double constexpr pi = 3.14159265358979323846;
    c0 = 1 / (2 * pi * soil_conduct);

    // Deriving characteristic time
    ts = pow(bh_length, 2) / (9 * (soil_conduct / rho_cp));
    // Creating interpolated g dataset
    g_expander(num_time_steps);
}

// Expanding g data as step function so that it has same length as q_load
void GHE::g_expander(int num_time_steps) {
    int n = 0;
    auto lntts_self_begin = lntts_self.begin();
    auto lntts_self_end = lntts_self.end();
    auto lntts_cross_begin = lntts_cross.begin();
    auto lntts_cross_end = lntts_cross.end();
    while (n < num_time_steps) {
        // Building vector of lntts values
        hours_as_seconds = 3600 * ((n + 1) * hours_per_timestep);
        calc_lntts.push_back(log(hours_as_seconds / ts));

        auto upper_it = std::upper_bound(lntts_self_begin, lntts_self_end, calc_lntts[n]);
        if (upper_it == lntts_self_begin) {
            // Extrapolating beyond the lower bound
            interp_g_self.push_back(g_func_self.front());
            std::cout << "SELF: Extrapolating beyond the lower bound index = " << n << "\n";
            std::cout << g_func_self.front() << "\n";
        } else if (upper_it == lntts_self_end) {
            // Extrapolating beyond the upper bound
            interp_g_self.push_back(g_func_self.back());
            std::cout << "SELF: Extrapolating beyond the upper bound index = " << n << "\n";
        } else {
            auto u_idx = std::distance(lntts_self.begin(), upper_it);
            auto l_idx = u_idx - 1;
            double lntts_low = lntts_self[l_idx];
            double lntts_high = lntts_self[u_idx];
            double g_func_low = g_func_self[l_idx];
            double g_func_high = g_func_self[u_idx];
            double g_temp = (calc_lntts[n] - lntts_low) / (lntts_high - lntts_low) * (g_func_high - g_func_low) + g_func_low;
            interp_g_self.push_back(g_temp);
        }
        if (!lntts_cross.empty()) {
            auto upper_it_cross = std::upper_bound(lntts_cross_begin, lntts_cross_end, calc_lntts[n]);
            if (upper_it_cross == lntts_cross_begin) {
                // Extrapolating beyond the lower bound
                interp_g_cross.push_back(g_func_cross.front());
                std::cout << "CROSS: Extrapolating beyond the lower bound index = " << n << "\n";
                std::cout << g_func_cross.front() << "\n";
            } else if (upper_it_cross == lntts_cross_end) {
                // Extrapolating beyond the upper bound
                interp_g_cross.push_back(g_func_cross.back());
                std::cout << "CROSS: Extrapolating beyond the upper bound index = " << n << "\n";
            } else {
                auto u_idx_cross = std::distance(lntts_self.begin(), upper_it);
                auto l_idx_cross = u_idx_cross - 1;
                double lntts_low_cross = lntts_cross[l_idx_cross];
                double lntts_high_cross = lntts_cross[u_idx_cross];
                double g_func_low_cross = g_func_cross[l_idx_cross];
                double g_func_high_cross = g_func_cross[u_idx_cross];
                double g_temp_cross =
                    (calc_lntts[n] - lntts_low_cross) / (lntts_high_cross - lntts_low_cross) * (g_func_high_cross - g_func_low_cross) +
                    g_func_low_cross;
                interp_g_cross.push_back(g_temp_cross);
            }
        }
        ++n;
    }
}

// Summation eqn 1.11 from Mitchell Appendix A
std::array<double, 2> GHE::summation(int time_step) {
    std::array<double, 2> total = {0, 0};
    int i = 0;
    int j = time_step;
    while (i <= time_step) {
        if (i == 0) {
            q_delta = ghe_load[i] - 0;
        } else {
            q_delta = ghe_load[i] - ghe_load[i - 1];
        }
        // eqn 1.11
        total[0] = total[0] + (q_delta * interp_g_self[j]);
        total[1] = total[1] + (q_delta * interp_g_cross[j]);
        j = j - 1;
        ++i;
    }
    return total; // 0 index is self, 1 index is cross
}

double GHE::simulate(int time_step, double ghe_inlet_temperature, double mass_flow_rate, double GHE_load, double external_Tr) {
    // loading g_data
    double gn_self = interp_g_self[time_step];
    double gn_cross = interp_g_cross[time_step];

    // calculating current load and appending to data
    if (load_from_building) {
        double previous_GHEload = 0.0;
        if (time_step > 0) {
            c1 = summation(time_step - 1); // 0 index is self, 1 index is cross
            previous_GHEload = ghe_load[time_step - 1];
            current_GHEload = (ghe_inlet_temperature - soil_temp + ((previous_GHEload * gn_self) * c0) - (c1[0] * c0)) /
                              ((0.5 * (bh_length / (mass_flow_rate * specific_heat))) + (gn_self * c0) + bh_resistance);
        }
        else {
            current_GHEload = (ghe_inlet_temperature - soil_temp) / ((0.5 * (bh_length / (mass_flow_rate * specific_heat))) + (gn_self * c0) + bh_resistance);
            c1 = {current_GHEload * gn_self, current_GHEload * gn_cross};

        }
        ghe_load.push_back(current_GHEload);
        internal_Tr = c0 * (((current_GHEload - previous_GHEload) * gn_self) + c1[0]);
        cross_Tr = c0 * (((current_GHEload - previous_GHEload) * gn_cross) + c1[1]);
    }
    else {
        current_GHEload = GHE_load;
        ghe_load.push_back(current_GHEload);
        c1 = summation(time_step); // 0 index is self, 1 index is cross
        // 1.12
        if (time_step > 0) {
            internal_Tr = c0 * c1[0];
            cross_Tr = c0 * c1[1];
        } else {
            internal_Tr = c0 * current_GHEload * gn_self;
            cross_Tr = c0 * current_GHEload * gn_cross;
        }
    }
    BH_temp = (soil_temp) + (internal_Tr) + (external_Tr);
    MFT = (current_GHEload * bh_resistance) + BH_temp;
    // 1.14
    outlet_temperature = MFT - 0.5 * ((current_GHEload * bh_length) / (mass_flow_rate * specific_heat));
    std::cout << c1[0] << "\n";
    return cross_Tr;
}

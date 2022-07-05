//-------------------SIMPLE MEAN GHE -------------------------------

//Main loop built upon Appendix A of https://shareok.org/handle/11244/323367

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>
#include "GHE.h"

// Variables (See nomenclature section):
//H - active borehole length (Active length of pipe)
//Rb - borehole Resistance
//Ts - soil temp
//ghe_Tout - GHE outlet temp (C)
//ghe_Tf - MFT of GHE
//mdot - mass flow rate
//cp - specific heat (heat energy required to change temp of material)
//ks - soil conductivity
//ghe_load - GHE load
//g_data - tuple with data set of g values from the JSON file and index n
//path - location of the JSON file
//lntts - non-dimensionalized time input into g-function (x-axis on plots) from JSON
//g_func - g values for given lntts values (y-axis on plots) from JSON
//q_load - load data
//q_time - time data corresponding to each ghe_load value
//q_lntts - calculated non-dimensionalized time for q_time
//n - index of main loop
//m - number of iterations of main loop
//c1 - history terms
//c0 - simplification term
//qn - current GHE load
//qn1 - previous iteratiuon GHE load
//q_delta - GHE load delta
//total - total of summation equation. Defined in function
//inputs - data structure with inputs read from the JSON file
//alphas - soil thermal diffusivity (see pg 12 of Mitchell) (also see line 30 of main.py from the repo shared by Matt)
//ts - characteristic time (way to non-dimensionalize the time) see page 12
//i/j - general purpose indexes
//gn - g value at index n

//TODO: Work to get all functions shorter than 20-25 lines. Anything bigger than that needs to be broken into smaller functions
//TODO: Make sure function names are super super clear, allows people to read the code and see exactly what is happening


GHE::GHE(std::ostream *main_output, std::ostream *debug_output, int m) {
    out = main_output;
    debug = debug_output;
    q_time.reserve(m);
    q_lntts.reserve(m);
    ghe_load.reserve(m);
    g_data.reserve(m);
    lntts = { -15.22015406,
                                 -15.08300806,
                                 -14.94586206,
                                 -14.80871605,
                                 -14.67157005,
                                 -14.53442405,
                                 -14.39727805,
                                 -14.26013205,
                                 -14.12298605,
                                 -13.98584005,
                                 -13.84869405,
                                 -13.71154804,
                                 -13.57440204,
                                 -13.43725604,
                                 -13.30011004,
                                 -13.16296404,
                                 -13.02581804,
                                 -12.88867204,
                                 -12.75152604,
                                 -12.61438004,
                                 -12.47723403,
                                 -12.34008803,
                                 -12.20294203,
                                 -12.06579603,
                                 -11.92865003,
                                 -11.79150403,
                                 -11.65435803,
                                 -11.51721203,
                                 -11.38006602,
                                 -11.24292002,
                                 -11.10577402,
                                 -10.96862802,
                                 -10.83148202,
                                 -10.69433602,
                                 -10.55719002,
                                 -10.42004402,
                                 -10.28289802,
                                 -10.14575201,
                                 -10.00860601,
                                 -9.871460012,
                                 -9.734314011,
                                 -9.597168009,
                                 -9.460022008,
                                 -9.322876007,
                                 -9.185730006,
                                 -9.048584005,
                                 -8.911438004,
                                 -8.774292002,
                                 -8.637146001,
                                 -8.5,
                                 -7.8,
                                 -7.2,
                                 -6.5,
                                 -5.9,
                                 -5.2,
                                 -4.5,
                                 -3.963,
                                 -3.27,
                                 -2.864,
                                 -2.577,
                                 -2.171,
                                 -1.884,
                                 -1.191,
                                 -0.497,
                                 -0.274,
                                 -0.051,
                                 0.196,
                                 0.419,
                                 0.642,
                                 0.873,
                                 1.112,
                                 1.335,
                                 1.679,
                                 2.028,
                                 2.275,
                                 3.003};

    g_func = {-2.556919564,
        -2.483889186,
        -2.408186285,
        -2.329364731,
        -2.247153415,
        -2.161382726,
        -2.071953832,
        -1.978823254,
        -1.881993912,
        -1.781509211,
        -1.677448741,
        -1.569925155,
        -1.45908234,
        -1.34509531,
        -1.228172249,
        -1.108558858,
        -0.986544532,
        -0.862469138,
        -0.736728573,
        -0.609776928,
        -0.482123413,
        -0.354322868,
        -0.226959789,
        -0.100626916,
        0.024099618,
        0.146685992,
        0.266664496,
        0.383652944,
        0.497367798,
        0.607629979,
        0.714363225,
        0.817585782,
        0.917396924,
        1.013960203,
        1.107485466,
        1.198211462,
        1.286390425,
        1.372275509,
        1.456111388,
        1.538127898,
        1.618536337,
        1.697527825,
        1.775273191,
        1.851923828,
        1.927613133,
        2.002458214,
        2.076561654,
        2.150013227,
        2.222891481,
        2.29526516,
        2.429306993,
        2.776706956,
        3.073538762,
        3.418085031,
        3.711170756,
        4.049191639,
        4.381141145,
        4.630139663,
        4.941340709,
        5.117006771,
        5.237526289,
        5.401676261,
        5.512698806,
        5.759025737,
        5.96747168,
        6.024558855,
        6.076104037,
        6.126494025,
        6.16581059,
        6.199436891,
        6.228510294,
        6.252934535,
        6.271152953,
        6.29201203,
        6.306229149,
        6.313183067,
        6.324383675};

    Ts = 10;
    cp = 4200;
    H = 100;
    Rb = 0.2477;
    ks = 2.0;
    rcp = 2000000.0;
    mdot = 0.2;
    bldgload = -1000;
    // Deriving characteristic time
    ts = pow(H, 2) / (9 * (ks/rcp));
}

// Expanding g data as step function so that it has same length as q_load
void GHE::g_expander(int m) {
    int n = 0;
    while (n < m) {
        // Building vector of lntts values
        q_time.push_back(3600 * n);
        q_lntts.push_back(log(q_time[n] / ts));
        // Interpolator
        // Assuming x and y are equal length - this should be caught upon initialization
        //  Assuming x and y have at least 2 elements
        //  Assuming x is monotonic

        auto lntts_begin = lntts.begin();
        auto lntts_end = lntts.end();
        auto upper_it = std::upper_bound(lntts_begin, lntts_end, q_lntts[n]);
        if (upper_it == lntts_begin) {
            // Extrapolating beyond the lower bound
            g_data.push_back(g_func.front());
        } else if (upper_it == lntts_end) {
            // Extrapolating beyond the upper bound
            g_data.push_back(g_func.back());
        } else {
            auto u_idx = std::distance(lntts.begin(), upper_it);
            auto l_idx = u_idx - 1;
            double lntts_low = lntts[l_idx];
            double lntts_high = lntts[u_idx];
            double g_func_low = g_func[l_idx];
            double g_func_high = g_func[u_idx];
            double g_temp = (q_lntts[n] - lntts_low) / (lntts_high - lntts_low) * (g_func_high - g_func_low) + g_func_low;
            g_data.push_back(g_temp);
            *debug << n << "," << g_data[n] << "," << q_lntts[n] << "," << q_time[n] << "\n";
        }
        ++n;
    }
}

// eqn 1.11 from Mitchell Appendix A
double GHE::summation(int n) {
    double q_delta;
    double total = 0;
    int i = 0;
    int j = n;
    if (n !=0) {
        while (i < n) {
            if (i == 0) {
                q_delta = ghe_load[i] - 0;
            } else {
                q_delta = ghe_load[i] - ghe_load[i - 1];
            }
            // eqn 1.11
            total = total + (q_delta * g_data[j]);
            j = j - 1;
            ++i;
        }
    }
    return total;
}

double GHE::HP(double ghe_Tout_n) const {
    double srcload;
    double HP_config [] = {1.092440,0.000314,0.000114,0.705459,0.005447,-0.000077};
    if (bldgload < 0) {srcload = bldgload*(HP_config [0] + HP_config [1]*(ghe_Tout_n) + HP_config [2]*(ghe_Tout_n*ghe_Tout_n));}
    else {srcload = bldgload*(HP_config [3] + HP_config [4]*(ghe_Tout_n) + HP_config [5]*(ghe_Tout_n*ghe_Tout_n));}
    double T_out_hp = ghe_Tout_n - (srcload / (mdot*cp));
    return T_out_hp;
}

void GHE::main_model(int m) {
    
    // eqn 1.3
    double c0 = 1 / (2 * M_PI * ks);

    //Interpolating g_data
    g_expander(m);
    
    // Main Loop
    int n = 0;
    double qn1, ghe_Tin;
    std::vector<double> ghe_Tout(m), ghe_Tf(m);
    while (n < m) {
        // Calculating Inlet temp
        if (std::remainder(n, 730) == 0) {
            bldgload = bldgload * -1;
        }
        if (n>0) {
            ghe_Tin = HP(ghe_Tout[n - 1]);
        }
        else {
            ghe_Tin = HP(0);
        }
        // loading g_data
        double gn = g_data[n];

        // eqn 1.11

        double c1 = (1)*summation(n);

        // calculating current load and appending to data

        if (n>0) {
            qn1 =  ghe_load[n-1];
        }
        else{
            qn1 =  0;
        }

        double qn = (ghe_Tin - Ts + ((qn1 * gn)*c0) - (c1*c0))/((0.5*(H/(mdot*cp)))+(gn*c0)+Rb);
        ghe_load.push_back (qn);

        // 1.12
        ghe_Tf[n] = Ts + c0*(((qn - qn1)*gn) + c1) + qn * Rb;


        // 1.14
        ghe_Tout[n] = ghe_Tf[n] - 0.5 * ((qn * H) / (mdot * cp));
        *out << n << "," << qn << "," << ghe_Tin << "," << ghe_Tout[n] << ","  << bldgload << "\n";
        n++;
    }

}

//// TODO: So the library really shouldn't have its own main function
//// To exercise the model, we should use the simple_GHE standalone executable, the mock energyplus, the fmu, and soon the unit tests
//// Should be able to achieve the same thing, and keep the underlying model as just a plain little library with an API.
//int main() {
//    // Setup output streams
//    std::ofstream outputs("../ouputs.csv", std::ofstream::out);
//    std::ofstream debug_outputs("../debugging.csv", std::ofstream::out);
//
//    // Create the GHE model
//    int num_time_steps = 8760;  //num of iterations
//    GHE ghe(&outputs, &debug_outputs, num_time_steps);
//    outputs << "n" << "," << "GHE Load" << "," << "ghe_Tin (HP_Tout)" << "," << "ghe_Tout (HP_Tin)" <<  "," << "bldgload" << "\n";
//    debug_outputs << "n" << "," << "g_data[n]" << "," << "q_lntts[n]" << "," << "q_time[n]" << "\n";
//
//    // Run the model
//    ghe.main_model(num_time_steps);
//    std::cout << "Executed successfully" << std::endl;
//    return 0;
//}

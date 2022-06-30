//-------------------SIMPLE MEAN GHE -------------------------------

//Main loop built upon Appendix A of https://shareok.org/handle/11244/323367

#include <cmath>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>
#include <fstream>

//Variables (See nomenclature section):
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

//CSV Set up for debugging
std::ofstream static outputs("../ouputs.csv", std::ofstream::out);

struct inital_data {
    double Ts; // Soil temp
    double cp; // Specific heat (heat energy required to change temp of material)
    double H;  // Active borehole length (Active length of pipe)
    double Rb; // Borehole Resistance
    double ks; // Soil conductivity
    double rcp; //Rho cp
    double mdot; //mass flow rate
    double cop_c; //Coefficient of performance chiller
    double cop_h; //Coefficient of performance heater
    std::tuple<std::vector<double>, std::vector<double>> indexed_data;
};

//---------------------------------------------------
//Function definitions
//---------------------------------------------------
inital_data load_data() {
    inital_data inputs;
    // HARD CODED TEST DATA

    std::vector<double> lntts = { -15.22015406,
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

    std::vector<double> g_func = {-2.556919564,
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
    std::tuple<std::vector<double>, std::vector<double>> indexed_data;
    std::get<0>(indexed_data) = lntts;
    std::get<1>(indexed_data) = g_func;

    inputs.Ts = 10;
    inputs.cp = 4200;
    inputs.H = 100;
    inputs.Rb = 0.2477;
    inputs.ks = 2.0;
    inputs.rcp = 2000000.0;
    inputs.mdot = 0.2;
    inputs.cop_c = 3;
    inputs.cop_h = 3;
    inputs.indexed_data = indexed_data;

    return inputs;
}

// Expanding g data as step function so that it has same length as q_load
std::vector<double>
g_expander(std::tuple<std::vector<double>, std::vector<double>> test_data, int n, double ts) {

    std::vector<double> lntts = std::get<0>(test_data);
    std::vector<double> g_func = std::get<1>(test_data);
    ++n;

    //Building vector of lntts values
    int q_time[n];
    int k = 0;
    while (k < n) {
        q_time[k] = 3600 * (k + 1);
        ++k;
    };

    int i = 0;
    double q_lntts[n];
    for (int t : q_time) {
        q_lntts[i] = log(t / ts);
        ++i;
    }
    //Interpolator
    int j = 0;
    std::vector<double> output_g_values(n);
    for (double lntts_val : q_lntts){
        // Assuming x and y are equal length - this should be caught upon initialization
        // Assuming x and y have at least 2 elements
        // Assuming x is monotonic

        auto lntts_begin = lntts.begin();
        auto lntts_end = lntts.end();
        auto upper_it = std::upper_bound(lntts_begin, lntts_end, lntts_val);
        if (upper_it == lntts_begin) {
            // Extrapolating beyond the lower bound
            output_g_values[j] = g_func.front();
            }
        else if (upper_it == lntts_end) {
            // Extrapolating beyond the upper bound
            output_g_values[j] = g_func.back();
            }
        else{
            int u_idx = std::distance(lntts.begin(), upper_it);
            int l_idx = u_idx - 1;
            double lntts_low = lntts[l_idx];
            double lntts_high = lntts[u_idx];
            double g_func_low = g_func[l_idx];
            double g_func_high = g_func[u_idx];
            output_g_values[j] = (lntts_val - lntts_low) / (lntts_high - lntts_low) * (g_func_high - g_func_low) + g_func_low;
            }
        ++j;
    }

    // Write data to g_data structure and return
    std::vector<double> g_data = output_g_values;
    return g_data;
}

// eqn 1.11 from Mitchell Appendix A
double summation(int n, std::vector<double> q_load, std::vector<double> g_data) {
    double q_delta;
    double total = 0;
    int i = 0;
    int j = n;
    if (n !=0) {
        while (i < n) {
            if (i == 0) {
                q_delta = q_load[i] - 0;
            } else {
                q_delta = q_load[i] - q_load[i - 1];
            }
            // eqn 1.11
            total = total + (q_delta * g_data[j]);
            j = j - 1;
            ++i;
        }
    }
    return total;
}

double HP(double ghe_out, double bldgload, inital_data inputs, double HP_config [] ){
    double srcload;
    if (bldgload < 0) {srcload = bldgload*(HP_config [0] + HP_config [1]*(ghe_out) + HP_config [2]*(ghe_out*ghe_out));}
    else {srcload = bldgload*(HP_config [3] + HP_config [4]*(ghe_out) + HP_config [5]*(ghe_out*ghe_out));}
    double T_out_hp = ghe_out - (srcload / (inputs.mdot*inputs.cp));
    return T_out_hp;
}

void main_model() {

    inital_data inputs = load_data();

    //Setting up HP configuration
    double HP_config [] = {1.092440,0.000314,0.000114,0.705459,0.005447,-0.000077};

    // Deriving characteristic time
    double alpha_s = inputs.ks/inputs.rcp;
    double ts = pow(inputs.H, 2) / (9 * alpha_s);

    // eqn 1.3
    double c0 = 1 / (2 * M_PI * inputs.ks);
    
    // Main Loop
    int n = 0;
    int m = 8760; //num of iterations
    double bldgload = -1000.00;
    double qn1, ghe_Tin;
    std::vector<double> ghe_load;
    std::vector<double> ghe_Tout(m), ghe_Tf(m);
    while (n < m) {

        // Calculating Inlet temp
        if (std::remainder(n, 730) == 0) {
            bldgload = bldgload * -1;
        }

        if (n>0) {
            ghe_Tin = HP(ghe_Tout[n - 1], bldgload, inputs, HP_config);
        }
        else {
            ghe_Tin = HP(0, bldgload, inputs, HP_config);
        }
        // loading g_data
        std::vector<double> g_data = g_expander(inputs.indexed_data, n, ts);
        double gn = g_data[n];

        // eqn 1.11

        double c1 = (1)*summation(n, ghe_load, g_data);

//        // calculating current load and appending to data
//
        if (n>0) {
            qn1 =  ghe_load[n-1];
        }
        else{
            qn1 =  0;
        }

        double qn = (ghe_Tin - inputs.Ts + ((qn1 * gn)*c0) - (c1*c0))/((0.5*(inputs.H/(inputs.mdot*inputs.cp)))+(gn*c0)+inputs.Rb);
        ghe_load.push_back (qn);

        // 1.12
        ghe_Tf[n] = inputs.Ts + c0*(((qn - qn1)*gn) + c1) + qn * inputs.Rb;


        // 1.14
        ghe_Tout[n] = ghe_Tf[n] - 0.5 * ((qn * inputs.H) / (inputs.mdot * inputs.cp));
        outputs << n << "," << qn << "," << ghe_Tin << "," << ghe_Tout[n] << ","  << bldgload << "\n";
        n++;
    };

}


int main() {

    outputs << "n" << "," << "GHE Load" << "," << "ghe_Tin/HP_Tout" << "," << "ghe_Tout/HP_Tin" <<  "," << "bldgload" << "\n";
    main_model();
    std::cout << "executed successfully" << std::endl;
    return 0;
}


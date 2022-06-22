//-------------------SIMPLE MEAN GHE -------------------------------
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

//Variables (See nomenclature section):
//H - active borehole length (Active length of pipe)
//Rb - borehole Resistance
//Ts - soil temp
//Tout - outlet temp (C)
//Tf - simple mean of inlet and outlet temps
//mdot - mass flow rate
//cp - specific heat (heat energy required to change temp of material)
//ks - soil conductivity
//q_data - heat load data
//q_out - updated q load data
//g_data - tuple with data set of g values from the JSON file and index n
//path - location of the JSON file
//lntts - non-dimensionalized time input into g-function (x-axis on plots) from JSON
//g_func - g values for given lntts values (y-axis on plots) from JSON
//q_load - load data
//q_time - time data corresponding to each q_load
//q_lntts - calcualted non-dimensionalized time for q_time
//n - index value equal to the last position in q_load
//c1 - history terms
//c0 - simplication term
//qn - current heat load
//q_delta - load delta
//total - total of summation equation. Defined in function
//inputs - data structure with inputs read from the JSON file
//alphas - soil thermal diffusivity (see pg 12 of Mitchell) (also see line 30 of main.py from the repo shared by Matt)
//ts - characteristic time (way to non-dimensionalize the time) see page 12
//i/j - genral purpose indexes
//gn - g value at index n
//outputs - funtion output

// Definition of structures
struct output_data {
    std::vector<double> Tout; // outlet temperature
    std::vector<double> Tf;
};

struct json_data {
    double Ts; // Soil temp
    double cp; // Specific heat (heat energy required to change temp of material)
    double H;  // Active borehole length (Active length of pipe)
    double Rb; // Borehole Resistance
    double ks; // Soil conductivity
    double rcp; //Rho cp
    std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> indexed_data;
};

//---------------------------------------------------
//Functions definitions: called inside of simple_GHE
//---------------------------------------------------
// reading data from JSON file
json_data read_json() {
    json_data input;
    //std::string path = "/Users/ryan/Downloads/simpleGHE/simpleGHE/test.json";
    // json_data.indexed_data needs to have vectors in the following order:
    // lntts, g_func, q_load

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
        15.96862802,
        15.83148202,
        15.69433602,
        15.55719002,
        15.42004402,
        15.28289802,
        15.14575201,
        15.00860601,
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
    //Debugging print function
    std::cout << "Size of lntts is " << lntts.size() << std::endl;
    std::cout << " " << std::endl;
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
    std::vector<double> q_load = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};  // length is 24
    std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> indexed_data;
    std::get<0>(indexed_data) = lntts;
    std::get<1>(indexed_data) = g_func;
    std::get<2>(indexed_data) = q_load;

    input.Ts = 10;
    input.cp = 4200;
    input.H = 100;
    input.Rb = 0.2477;
    input.ks = 2.0;
    input.rcp = 2000000.0;
    input.indexed_data = indexed_data;

    return input;
}

// Expanding g data as step function so that it has same length as q_load
std::tuple<std::vector<double>, std::vector<int>::size_type>
g_expander(std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> test_data, double ts) {

    std::vector<double> lntts = std::get<0>(test_data);
    std::vector<double> g_func = std::get<1>(test_data);
    std::vector<double> q_load = std::get<2>(test_data);

    // from length of q_load, build 0 - n integer vector called q_time.
    std::vector<int>::size_type n = q_load.size();
    std::vector<double> output_g_values(n);
    double q_time[n];
    int k = 0;
    while (k < n) {
        q_time[k] = 3600 * (k + 1);
        k++;
    }; // 3600 is used here to convert hourly timesteps to seconds

    // for each value of q_time calculate ln(t/ts)
    int i = 0;
    double q_lntts[n];
    for (int t : q_time) {
        q_lntts[i] = log(t / ts);
        ++i;
    }
    // PURPOSE OF THIS SUBROUTINE:
    //    To interpolate or extrapolate data in GFILE
    //    to find the correct g-function value for a
    //    known value of the natural log of (T/Ts)

    // The following IF loop determines the g-function for the case
    // when q_lntts_temp is less than the first element of the lntts vector.
    // In this case, the g-function must be found by extrapolation.
    int j = 0;
    int Npairs = lntts.size();
    for (double q_lntts_temp : q_lntts) {
        if (q_lntts_temp <= lntts[0]) {
            output_g_values[j] = ((q_lntts_temp - lntts[0]) / (lntts[1] - lntts[0])) * (g_func[1] - g_func[0]) + g_func[0];
            std::cout << "q_lntts_temp is less min lntts at index " << j << std::endl;
        }

        // The following IF loop determines the g-function for the case
        // when q_lntts_temp is greater than the last element of the lntts
        // array. In this case, the g-function must be found by extrapolation.

        else if (q_lntts_temp > lntts[Npairs]) {
            output_g_values[j] =
                ((q_lntts_temp - lntts[Npairs]) / (lntts[Npairs - 1] - lntts[Npairs])) * (g_func[Npairs - 1] - g_func[Npairs]) + g_func[Npairs];
            std::cout << "q_lntts_temp is greater than max lntts at index " << j << std::endl;
        }

        // The following DO loop is for the case when LnTTsVal falls within
        // the first and last elements of the LnTTs array, or is identically
        // equal to one of the LnTTs elements.  In this case the g-function
        // must be found by interpolation.
        // USING BINARY SEARCH TO FIND THE ELEMENT
        bool Found = false;
        int Low = 1;
        int High = Npairs;
        int Mid = (Low + High) / 2;
        while (Low <= High) {
            Mid = (Low + High) / 2;
            if (lntts[Mid] < q_lntts_temp) {
                Low = Mid + 1;
            } else {
                if (lntts[Mid] > q_lntts_temp) {
                    High = Mid - 1;
                } else {
                    Found = true;
                    break;
                }
            }
        }
        // q_lntts_temp is identical to one of the lntts array elements return
        // gFuncVal the gFuncVal after applying the correction
        if (Found) {
            output_g_values[j] = g_func[Mid];
        }

        // q_lntts_temp is in between any of the two lntts array elements find
        // the g-function value by interpolation and apply the correction and
        // return gFuncVal
        else {
            if (lntts[Mid] < q_lntts_temp)
                ++Mid;
            output_g_values[j] = ((q_lntts_temp - lntts[Mid]) / (lntts[Mid - 1] - lntts[Mid])) * (g_func[Mid - 1] - g_func[Mid]) + g_func[Mid];
            //Debugging print functions
            std::cout << "g value interpolated at index j = " << j << std::endl;
            std::cout << "Mid value is " << Mid << std::endl;
        };
        
        j++;
    }
//    //Debugging print functions
//    std::cout << " " << std::endl;
//    i = 0;
//    for (double g : output_g_values){
//        std::cout << "output_g_values at index j = " << i << " is " << g << std::endl;
//        i++;
//    }
//    std::cout << " " << std::endl;
//    i = 1;
//    for (double output : q_time) {
//        std::cout << "q_time at timestep " << i << " is " << output << std::endl;
//        ++i;
//    }
//    i = 0;
//    for (double output : lntts) {
//        std::cout << "lntts at index " << i << " is " << output << std::endl;
//        ++i;
//    };
//    i = 1;
//    for (double output : q_lntts) {
//        std::cout << "q_lntts at timestep " << i << " is " << output << std::endl;
//        ++i;
//    };
//    i = 0;
//    for (double output : g_func) {
//        std::cout << "g_func at index " << i << " is " << output << std::endl;
//        ++i;
//    };
//    i = 1;
//    for (double output : output_g_values) {
//        std::cout << "output_g_value at timestep " << i << " is " << output << std::endl;
//        ++i;
//    }

    // Write data to g_data structure and return
    std::tuple<std::vector<double>, std::vector<int>::size_type>(g_data);
    std::get<0>(g_data) = output_g_values;
    std::get<1>(g_data) = n;

    return g_data;
}

// eqn 1.11
double summation(int n, std::vector<double> q_load, std::vector<double> g_data) {
    double q_delta = 0;
    double total = 0;
    int i = 0;
    int j = n;
    while (i <= n) {
        if (i == 0) {
            q_delta = q_load[i] - 0;
        } else {
            q_delta = q_load[i] - q_load[i - 1];
        }
        // eqn 1.11
        total = total + (q_delta * g_data[j]);
//        //Debugging print functions
//        std::cout << "Summation term at " << i << " is " << total << std::endl;
//        std::cout << "q_delta for summation index " << i << " is " << q_delta << std::endl;
        j = j - 1;
        ++i;
    }
//    //Debugging Print Functions
//    std::cout << "g_data for index " << n << " n " << g_data[n] << std::endl;
//    std::cout << "summation total for timestep " << n << " is " << total << std::endl;
    return total;
}

/*---------------
Main function
----------------*/
output_data simple_GHE(double mdot) {
    // loading JSON data. In development
    json_data inputs = read_json(); // this will be a function that reads and
                                    // returns the data from the JSON file. See
                                    // struct json_data for output variable type
    std::vector<double> q_load = std::get<2>(inputs.indexed_data);

    // Deriving characteristic time
    double alpha_s = inputs.ks/inputs.rcp;
    double ts = pow(inputs.H, 2) / (9 * alpha_s);

    // loading g_data
    std::tuple<std::vector<double>, std::vector<int>::size_type> g_data = g_expander(inputs.indexed_data, ts);

    // eqn 1.3
    double c0 = 1 / (2 * M_PI * inputs.ks);
    
    // Main Loop
    int n = 0;
    auto m = std::get<1>(g_data);
    std::vector<double> Tout(m), Tf(m);
    double c1, qn;
    while (n < m) {

        // current load
        qn = -q_load[n];

        // eqn 1.11
        
        c1 = (-1)*summation(n, q_load, std::get<0>(g_data));
        
        // 1.12
        Tf[n] = inputs.Ts + (c0 * c1) + qn * inputs.Rb;
        // Debugging print functions
//        std::cout << "Ts for timestep " << n << " is " << inputs.Ts << std::endl;
//        std::cout << "c1 for timestep " << n << " is " << c1 << std::endl;
//        std::cout << "c0 for timestep " << n << " is " << c0 << std::endl;
//        std::cout << "qn for timestep " << n << " is " << qn << std::endl;
//        std::cout << "Rb for timestep" << n << " is " << inputs.Rb << std::endl;
//        std::cout << " " << std::endl;
        
        
        // 1.14
        Tout[n] = Tf[n] - 0.5 * ((qn * inputs.H) / (mdot * inputs.cp));
        n++;
    };
    
    // Writing to structure for export from function:
    output_data outputs;
    outputs.Tout = Tout;
    outputs.Tf = Tf;

    return outputs;
}

int main() {

    double mdot = 0.2;
    int i;
    output_data outputs = simple_GHE(mdot);

//    // Output print function
//    std::cout << " " << std::endl;
//    i = 1;
//    for (double output : outputs.Tout) {
//        std::cout << "Tout for timestep " << i << " is " << output << std::endl;
//        ++i;
//    }
    // Output print function
    std::cout << " " << std::endl;
    i = 1;
    for (double output : outputs.Tf) {
        std::cout << "Tf for timestep " << i << " is " << output << std::endl;
        ++i;
    }
    return 0;
}

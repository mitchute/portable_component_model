//-------------------SIMPLE MEAN GHE -------------------------------
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

/* Variables (See nomenclature section):
H - active borehole length (Active length of pipe)
Rb - borehole Resistance
Ts - soil temp
Tout - outlet temp (C)
Tf - simple mean of inlet and outlet temps
mdot - mass flow rate
cp - specific heat (heat energy required to change temp of material)
ks - soil conductivity
q_data - heat load data
q_out - updated q load data
g_data - tuple with data set of g values from the JSON file and index n
path - location of the JSON file
lntts - non-dimensionalized time input into g-function (x-axis on plots) from
JSON g_func - g values for given lntts values (y-axis on plots) from JSON q_load
- load data q_time - time data corresponding to each q_load q_lntts - calcualted
non-dimensionalized time for q_time n - index value equal to the last position
in q_load c1 - history terms c0 - simplication term qn - current heat load qn1 -
heat load from previous run qi - heat load at index i qi1 - heat load at index
i-1 total - total of summation equation. Defined in function inputs - data
structure with inputs read from the JSON file alphas - soil thermal diffusivity
(see pg 12 of Mitchell) (also see line 30 of main.py from the repo shared by
Matt) ts - characteristic time (way to non-dimensionalize the time) see page 12
i/j - genral purpose indexes
gn - g value at index n
outputs - funtion output
 */

// Definition of structures
struct output_data {
    std::vector<double> Tout; // outlet temperature
    std::vector<double> q_out;
};

struct json_data {
    double Ts; // Soil temp
    double cp; // Specific heat (heat energy required to change temp of material)
    double H;  // Active borehole length (Active length of pipe)
    double Rb; // Borehole Resistance
    double ks; // Soil conductivity
    std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> indexed_data;
};

/* --------------------------------------------------
Functions definitions: called inside of simple_GHE
---------------------------------------------------*/
// reading data from JSON file
json_data read_json() {
    json_data input;
    std::string path = "/Users/ryan/Downloads/simpleGHE/simpleGHE/test.json";
    // json_data.indexed_data needs to have vectors in the following order:
    // lntts, g_func, q_load

    // HARD CODED TEST DATA

    std::vector<double> lntts = {-15.22015406, -15.08300806, -14.94586206, -14.80871605, -14.67157005, -14.53442405, -14.39727805, -14.26013205,
                                 -14.12298605, -13.98584005, -13.84869405, -13.71154804, -13.57440204, -13.43725604, -13.30011004, -13.16296404,
                                 -13.02581804, -12.88867204, -12.75152604, -12.61438004, -12.47723403, -12.34008803, -12.20294203, -12.06579603,
                                 -11.92865003, -11.79150403, -11.65435803, -11.51721203, -11.38006602, -11.24292002, -11.10577402}; // length is 31
    std::vector<double> g_func = {-2.556919564, -2.483889186, -2.408186285, -2.329364731, -2.247153415, -2.161382726, -2.071953832, -1.978823254,
                                  -1.881993912, -1.781509211, -1.677448741, -1.569925155, -1.45908234,  -1.34509531,  -1.228172249, -1.108558858,
                                  -0.986544532, -0.862469138, -0.736728573, -0.609776928, -0.482123413, -0.354322868, -0.226959789, -0.100626916,
                                  0.024099618,  0.146685992,  0.266664496,  0.383652944,  0.497367798,  0.607629979,  0.714363225}; // length is 31
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
    double q_time[n];
    std::vector<double> output_g_values(n);
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
        }

        // The following IF loop determines the g-function for the case
        // when q_lntts_temp is greater than the last element of the lntts
        // array. In this case, the g-function must be found by extrapolation.

        else if (q_lntts_temp > lntts[Npairs]) {
            output_g_values[j] =
                ((q_lntts_temp - lntts[Npairs]) / (lntts[Npairs - 1] - lntts[Npairs])) * (g_func[Npairs - 1] - g_func[Npairs]) + g_func[Npairs];
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
        };
        j++;
    }

    // Debugging print function
    i = 1;
    for (double output : q_time) {
        std::cout << "q_time at timestep " << i << " is " << output << std::endl;
        ++i;
    }

    // Debugging print function
    i = 1;
    for (double output : lntts) {
        std::cout << "lntts at timestep " << i << " is " << output << std::endl;
        ++i;
    };
    // Debugging print function
    i = 1;
    for (double output : q_lntts) {
        std::cout << "q_lntts at timestep " << i << " is " << output << std::endl;
        ++i;
    };
    // Debugging print function
    i = 0;
    for (double output : g_func) {
        std::cout << "g_func at index " << i << " is " << output << std::endl;
        ++i;
    };
    // Debugging print function
    i = 1;
    for (double output : output_g_values) {
        std::cout << "output_g_value at timestep " << i << " is " << output << std::endl;
        ++i;
    }

    // Write data to g_data structure and return
    std::tuple<std::vector<double>, std::vector<int>::size_type>(g_data);
    std::get<0>(g_data) = output_g_values;
    std::get<1>(g_data) = n;

    return g_data;
}

// eqn 1.11
double summation(int n, std::vector<double> q_load, std::vector<double> g_data) {
    double q_delta;
    double total = 0;
    int i = 0;
    int j = n;
    while (i <= n) {
        if (n == 0) {
            q_delta = q_load[i] - 0;
        } else {
            q_delta = q_load[i] - q_load[i - 1];
        }
        // eqn 1.11
        total = total + (q_delta * g_data[j]);
        // Debugging print function
        std::cout << "Summation term at " << i << " is " << total << std::endl;
        j = j - 1;
        ++i;
    }
    // Debugging print function
    std::cout << "q_delta for timestep " << n << " is " << q_delta << std::endl;
    std::cout << "g_data for index " << n << " n " << g_data[n] << std::endl;
    std::cout << "summation total for timestep " << n << " is " << total << std::endl;
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
    double alpha_s = pow(10, -7); // TODO: not sure whether to use this estimate for alpha_s or
                                  // the below equation, which was copied from the python code.
    // double alpha_s = inputs.ks/inputs.cp;
    double ts = pow(inputs.H, 2) / (9 * alpha_s);

    // loading g_data
    std::tuple<std::vector<double>, std::vector<int>::size_type> g_data = g_expander(inputs.indexed_data, ts);

    // Main Loop
    int n = 0;
    auto m = std::get<1>(g_data);
    std::vector<double> Tout(m);
    double c0, c1, Tf, qn;
    // Debugging print function
    std::cout << " m is " << m << std::endl;
    while (n < m) {

        // current load
        qn = q_load[n];

        // eqn 1.3
        c0 = 1 / (2 * M_PI * inputs.ks);

        // eqn 1.11
        // Debugging print function
        std::cout << " " << std::endl;
        c1 = summation(n, q_load, std::get<0>(g_data));
        // Debugging print function
        std::cout << "c1 for timestep " << n << " is " << c1 << std::endl;

        // 1.12
        Tf = inputs.Ts + (c0 * c1) + qn * inputs.Rb;

        // 1.14
        Tout[n] = Tf - 0.5 * ((qn * inputs.H) / (mdot * inputs.cp));
        n++;
    };

    // Writing to structure for export from function:
    output_data outputs;
    outputs.Tout = Tout;
    outputs.q_out = q_load;

    return outputs;
}

int main() {

    double mdot = 0.2;
    output_data outputs = simple_GHE(mdot);

    // Output print function
    std::cout << " " << std::endl;
    int i = 1;
    for (double output : outputs.Tout) {
        std::cout << "Tout for timestep " << i << " is " << output << std::endl;
        ++i;
    }
    return 0;
}


//#include <cmath>
//#include <numeric>
//#include <stdio.h>
//#include <string>
//#include <tuple>
//#include <vector>
//using namespace std;
//
//struct json_data {
//    double Tin;
//    double Ts;
//    double mdot;
//    double cp;
//    double H;
//    double Rb;
//    double ks;
//    tuple<vector<double>, vector<double>, vector<double>> indexed_data;
//};
//
//json_data read_json() {
//    json_data output;
//    string path = "/Users/ryan/Downloads/simpleGHE/simpleGHE/test.json";
//
//    return output;
//}
//
//----------------------
// Expanding g data as step function so that it has same length as q_load
// ----------------------
//vector<double> g_expander(tuple<vector<double>, vector<double>, vector<double>> test_data) {
//    vector<double> g_data;
//    vector<double> lntts = get<0>(test_data);
//    vector<double> g_func = get<1>(test_data);
//    vector<double> q_load = get<2>(test_data);
//
//    // from length of q_load, build 0 - 8760 integer vector called q_time.
//    vector<int>::size_type length = q_load.size();
//    vector<int> q_time(length);
//    iota(begin(q_time), end(q_time), 1);
//
//    // for each value of q_time calculate ln(t/ts)
//    double ts = 60; // dummy number for now
//    int i = 0;
//    vector<double> q_lntts;
//    for (int t : q_time) {
//        q_lntts[i] = log(t / ts);
//        i++;
//    }
//
//    // Create a matching condition that replaces each lntts value from q_lnnts to
//    // the closest value from lnnts
//    int j = 0;
//    vector<double> q_lntts_new;
//    for (double q_t : q_lntts) {
//        for (double time : lntts) {
//            if (abs(q_t - time) < 0.2) {
//                q_lntts_new[j] = time;
//            }
//        }
//        j++;
//    }
//
//    // Match the values (which will now look like steps) of q_lnnts to the
//    // corresponding g values
//    i = 0;
//    j = 0;
//    for (double q_t_new : q_lntts_new) {
//        if (q_t_new == lntts[j]) {
//            g_data[i] = g_func[j];
//        } else {
//            j++;
//        }
//        i++;
//    }
//
//    // Create a vector, g_data, with these values that is the same length as
//    // q_load and return g_data
//    if (g_data.size() == q_load.size()) {
//        return g_data;
//    } else {
//        exit(EXIT_FAILURE);
//    }
//
//    //-------------------------------
//    // Need to load the g_data from the tuple. Interpolate linearly
//    // ignore below
//    //    double x,x1,x2,y,y1,y2;
//    //    int i = 0;
//    //    //TODO: determine i as index for referencing in vectors. For loop needed
//    //    here.
//    //    // start for loop
//    //    x1 = lntts[i];
//    //    x2 = lntts[i+1];
//    //    y1 = g_func[i];
//    //    y2 = g_func[i];
//    //    x = (x2-x1)/2;
//    //    y = y1 + ((x-x1)*(y2-y1))/(x2-x1);
//        //TODO: insert y at location index that yields x
//
//    //    // end for loop
//    //-------------------------------
//
//    // TODO: calculate lntts from the t in the q data. The match that to the lnnts
//    // in JSON to get the g function data for that q point. See teams message
//    // (DONE). Also ask Matt if short time periods will have errors, wierd numbers
//    // coming through with dummy data i.e. Tout = 8000 when Tin = 20. Possibly
//    // also error in dummy input data.
//
//    /* Notes from Teams Meeting:
//    g functions are a mathematical tool for deriving these models. It is the
//    non-dimensional temp rise of the borehole divided by the non-dimensional time.
//    close to zero time units are min, far from zero units are 100 of years. He
//    will send me some values for a single borehole for testing purposes.  */
//}
//
//int main2() {
//    // data needs to be sent in as lntts , g values, q_load
//    tuple<vector<double>, vector<double>, vector<double>> test_data;
//    get<0>(test_data) = {-15.22015406, -15.08300806, -14.94586206, -14.80871605, -14.67157005, -14.53442405, -14.39727805, -14.26013205, -14.12298605,
//                         -13.98584005, -13.84869405, -13.71154804, -13.57440204, -13.43725604, -13.30011004, -13.16296404, -13.02581804, -12.88867204,
//                         -12.75152604, -12.61438004, -12.47723403, -12.34008803, -12.20294203, -12.06579603, -11.92865003, -11.79150403, -11.65435803,
//                         -11.51721203, -11.38006602, -11.24292002, -11.10577402, 15.96862802,  15.83148202,  15.69433602,  15.55719002,  15.42004402,
//                         15.28289802,  15.14575201,  15.00860601,  -9.871460012, -9.734314011, -9.597168009, -9.460022008, -9.322876007, -9.185730006,
//                         -9.048584005, -8.911438004, -8.774292002, -8.637146001, -8.5,         -8.5,         -7.8,         -7.2,         -6.5,
//                         -5.9,         -5.2,         -4.5,         -3.963,       -3.27,        -2.864,       -2.577,       -2.171,       -1.884,
//                         -1.191,       -0.497,       -0.274,       -0.051,       0.196,        0.419,        0.642,        0.873,        1.112,
//                         1.335,        1.679,        2.028,        2.275,        3.003};
//    get<1>(test_data) = {-2.556919564, -2.483889186, -2.408186285, -2.329364731, -2.247153415, -2.161382726, -2.071953832, -1.978823254, -1.881993912,
//                         -1.781509211, -1.677448741, -1.569925155, -1.45908234,  -1.34509531,  -1.228172249, -1.108558858, -0.986544532, -0.862469138,
//                         -0.736728573, -0.609776928, -0.482123413, -0.354322868, -0.226959789, -0.100626916, 0.024099618,  0.146685992,  0.266664496,
//                         0.383652944,  0.497367798,  0.607629979,  0.714363225,  0.817585782,  0.917396924,  1.013960203,  1.107485466,  1.198211462,
//                         1.286390425,  1.372275509,  1.456111388,  1.538127898,  1.618536337,  1.697527825,  1.775273191,  1.851923828,  1.927613133,
//                         2.002458214,  2.076561654,  2.150013227,  2.222891481,  2.29526516,   2.429306993,  2.776706956,  3.073538762,  3.418085031,
//                         3.711170756,  4.049191639,  4.381141145,  4.630139663,  4.941340709,  5.117006771,  5.237526289,  5.401676261,  5.512698806,
//                         5.759025737,  5.96747168,   6.024558855,  6.076104037,  6.126494025,  6.16581059,   6.199436891,  6.228510294,  6.252934535,
//                         6.271152953,  6.29201203,   6.306229149,  6.313183067,  6.324383675};
//
//    return 0;
//}



#include <algorithm>
#include <iostream>
#include <vector>

double interp1d(const std::vector<double> &x, const std::vector<double> &y, double x_val){
    
    // Assuming x and y are equal length - this should be caught upon initialization
    // Assuming x and y have at least 2 elements
    // Assuming x is monotonic

    // Probably shouldn't pass x and y each time. Just need to create an interpolator object,
    // and pass in the data once upon construction.

    auto x_begin = x.begin();
    auto x_end = x.end();
    auto upper_it = std::upper_bound(x_begin, x_end, x_val);
    // std::cout << *upper_it << "\n";
    if (upper_it == x_begin) {
        // Extrapolating beyond the lower bound
        return y.front();
    } else if (upper_it == x_end) {
        // Extrapolating beyond the upper bound
        return y.back();
    }

    int u_idx = std::distance(x.begin(), upper_it);
    int l_idx = u_idx - 1;
    double x_low = x[l_idx];
    double x_high = x[u_idx];
    double y_low = y[l_idx];
    double y_high = y[u_idx];
    return (x_val - x_low) / (x_high - x_low) * (y_high - y_low) + y_low;
}

int main() {

    std::vector<double> lntts = {-15.22015406,
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
    std::vector<double> g = {-2.55692,
                                  -2.48389,
                                  -2.40819,
                                  -2.32936,
                                  -2.24715,
                                  -2.16138,
                                  -2.07195,
                                  -1.97882,
                                  -1.88199,
                                  -1.78151,
                                  -1.67745,
                                  -1.56993,
                                  -1.45908,
                                  -1.3451,
                                  -1.22817,
                                  -1.10856,
                                  -0.986545,
                                  -0.862469,
                                  -0.736729,
                                  -0.609777,
                                  -0.482123,
                                  -0.354323,
                                  -0.22696,
                                  -0.100627,
                                  0.0240996,
                                  0.146686,
                                  0.266664,
                                  0.383653,
                                  0.497368,
                                  0.60763,
                                  0.714363,
                                  0.817586,
                                  0.917397,
                                  1.01396,
                                  1.10749,
                                  1.19821,
                                  1.28639,
                                  1.37228,
                                  1.45611,
                                  1.53813,
                                  1.61854,
                                  1.69753,
                                  1.77527,
                                  1.85192,
                                  1.92761,
                                  2.00246,
                                  2.07656,
                                  2.15001,
                                  2.22289,
                                  2.29527,
                                  2.42931,
                                  2.77671,
                                  3.07354,
                                  3.41809,
                                  3.71117,
                                  4.04919,
                                  4.38114,
                                  4.63014,
                                  4.94134,
                                  5.11701,
                                  5.23753,
                                  5.40168,
                                  5.5127,
                                  5.75903,
                                  5.96747,
                                  6.02456,
                                  6.0761,
                                  6.12649,
                                  6.16581,
                                  6.19944,
                                  6.22851,
                                  6.25293,
                                  6.27115,
                                  6.29201,
                                  6.30623,
                                  6.31318,
        6.32438};
    std::vector<double> q_lntts = {-12.6399,
                                  -11.9468,
                                  -11.5413,
                                  -11.2536,
                                  -11.0305,
                                  -10.8482,
                                  -10.694,
                                  -10.5605,
                                  -10.4427,
                                  -10.3374,
                                  -10.242,
                                  -10.155,
                                  -10.075,
                                  -10.0009,
                                  -9.93189,
                                  -9.86735,
                                  -9.80672,
                                  -9.74957,
                                  -9.6955,
                                  -9.6442,
                                  -9.59541,
                                  -9.54889,
                                  -9.50444,
        -9.46188};
    
    std::vector<double> x1 = {1,2,3,4,5};
    std::vector<double> x2 = {-5,-4,-3,-2,-1};
    std::vector<double> y1 = {1,2,3,4,5};
    std::vector<double> y2 = {-5,-4,3,2,1};
    std::vector<double> to_find1 = {0, 1, 1.5, 2, 3, 3.666, 4, 5, 6.5};
    std::vector<double> to_find2 = {-6.5,-5,-4,-3.666,-3,-2,-1.5,-1,0};
    std::cout<< "\n find1" << std::endl;
//    for (double v : to_find1){
//        std::cout << "Value 11: " << v << " Returns: " << interp1d(x1, y1, v) << "\n";
//        }
//    for (double v : to_find1){
//        std::cout << "Value 12: " << v << " Returns: " << interp1d(x1, y2, v) << "\n";
//        }
//    for (double v : to_find1){
//        std::cout << "Value 21: " << v << " Returns: " << interp1d(x2, y1, v) << "\n";
//        }
//    for (double v : to_find1){
//        std::cout << "Value 22: " << v << " Returns: " << interp1d(x2, y2, v) << "\n";
//        }
//    std::cout<< "find2" << std::endl;
//    for (double v : to_find2){
//        std::cout << "Value 11: " << v << " Returns: " << interp1d(x1, y1, v) << "\n";
//        }
//    for (double v : to_find2){
//        std::cout << "Value 12: " << v << " Returns: " << interp1d(x1, y2, v) << "\n";
//        }
//    for (double v : to_find2){
//        std::cout << "Value 21: " << v << " Returns: " << interp1d(x2, y1, v) << "\n";
//        }
//    for (double v : to_find2){
//        std::cout << "Value 22: " << v << " Returns: " << interp1d(x2, y2, v) << "\n";
//        }
    for (double v : q_lntts){
        //std::cout << "Value: " << v << " Returns: " << interp1d(lntts, g, v) << "\n";
        std::cout << interp1d(lntts, g, v) << "," <<"\n";
    }

    return 0;
}

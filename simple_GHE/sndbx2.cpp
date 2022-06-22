//
//  sndbx.cpp
//  simpleGHE
//
//  Created by Ryan Davies on 6/15/22.
//

#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

int main() {

    std::vector<double> lntts = {-15.22015406, -15.08300806, -14.94586206, -14.80871605, -14.67157005, -14.53442405,
                                 -14.39727805, -14.26013205, -14.12298605, -13.98584005, -13.84869405, -13.71154804,
                                 -13.57440204, -13.43725604, -13.30011004, -13.16296404, -13.02581804, -12.88867204,
                                 -12.75152604, -12.61438004, -12.47723403, -12.34008803, -12.20294203, -12.06579603}; // length is 24
    std::vector<double> g_func = {-2.556919564, -2.483889186, -2.408186285, -2.329364731, -2.247153415, -2.161382726,
                                  -2.071953832, -1.978823254, -1.881993912, -1.781509211, -1.677448741, -1.569925155,
                                  -1.45908234,  -1.34509531,  -1.228172249, -1.108558858, -0.986544532, -0.862469138,
                                  -0.736728573, -0.609776928, -0.482123413, -0.354322868, -0.226959789, -0.100626916}; // length is 24
    std::vector<double> q_load = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
                                  1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000}; // length is 24
    int i = 0;
    for (double q : q_load) {
        q_load[i] = q / 100;
    }
    double cp = 4200;
    double H = 100;
    double ks = 2.0;
    double alpha_s = pow(10, -7);
    double ts = pow(H, 2) / (9 * alpha_s);

    // from length of q_load, build 0 - n integer vector called q_time.
    std::vector<int>::size_type n = q_load.size();
    double q_time[n];
    double output_g_values[n];
    int k = 0;
    while (k < n) {
        q_time[k] = 3600 * (k + 1);
        k++;
    };         // 3600 is used here to convert hourly timesteps to seconds
    n = n - 1; // converting n from size of vector to n as an index

    // for each value of q_time calculate ln(t/ts)
    i = 0;
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
        // when q_lntts_temp is greater than the last element of the lntts array.
        // In this case, the g-function must be found by extrapolation.

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
        // LnTTsVal is identical to one of the LnTTS array elements return gFuncVal
        // the gFuncVal after applying the correction
        if (Found) {
            output_g_values[j] = g_func[Mid];
        }

        // LnTTsVal is in between any of the two LnTTS array elements find the
        // g-function value by interpolation and apply the correction and return
        // gFuncVal
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

    return 0;
}

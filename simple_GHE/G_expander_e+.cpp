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
                std::cout << "q_lntts_temp is less than lntts[0]" << std::endl;
            }

            // The following IF loop determines the g-function for the case
            // when q_lntts_temp is greater than the last element of the lntts
            // array. In this case, the g-function must be found by extrapolation.

            else if (q_lntts_temp > lntts[Npairs]) {
                output_g_values[j] =
                    ((q_lntts_temp - lntts[Npairs]) / (lntts[Npairs - 1] - lntts[Npairs])) * (g_func[Npairs - 1] - g_func[Npairs]) + g_func[Npairs];
                std::cout << "q_lntts_temp is greater than lntts[0]" << std::endl;
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
            std::cout << "output_g_values at index j = " << j << " is " << output_g_values[j] << std::endl;
            j++;
        }

    //    // Debugging print function
    //    i = 1;
    //    for (double output : q_time) {
    //        std::cout << "q_time at timestep " << i << " is " << output << std::endl;
    //        ++i;
    //    }
    //    // Debugging print function
    //    i = 0;
    //    for (double output : lntts) {
    //        std::cout << "lntts at index " << i << " is " << output << std::endl;
    //        ++i;
    //    };
    //    // Debugging print function
    //    i = 1;
    //    for (double output : q_lntts) {
    //        std::cout << "q_lntts at timestep " << i << " is " << output << std::endl;
    //        ++i;
    //    };
    //    // Debugging print function
    //    i = 0;
    //    for (double output : g_func) {
    //        std::cout << "g_func at index " << i << " is " << output << std::endl;
    //        ++i;
    //    };
    //    // Debugging print function
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
}

//-------------------SIMPLE MEAN GHE -------------------------------
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <numeric>
#include <cmath>


/* Variables (See nomenclature section):
H - active borehole length (Active length of pipe)
Rb - borehole Resistance
Ts - soil temp
Tin - inlet temp
Tout - outlet temp
Tf - simple mean of inlet and outlet temps
mdot - mass flow rate
cp - specific heat (heat energy required to change temp of material)
ks - soil conductivity
q_data - heat load data
q_out - updated q load data
g_data - tuple with data set of g values from the JSON file and index n
path - location of the JSON file
lntts - non-dimensionalized time input into g-function (x-axis on plots) from JSON
g_func - g values for given lntts values (y-axis on plots) from JSON
q_load - load data
q_time - time data corresponding to each q_load
q_lntts - calcualted non-dimensionalized time for q_time
n - index value equal to the last position in q_load
c1 - history terms
c0 - simplication term
qn - current heat load
qn1 - heat load from previous run
qi - heat load at index i
qi1 - heat load at index i-1
total - total of summation equation. Defined in function
inputs - data structure with inputs read from the JSON file
alphas - soil thermal diffusivity (see pg 12 of Mitchell) (also see line 30 of main.py from the repo shared by Matt)
ts - characteristic time (way to non-dimensionalize the time) see page 12
i/j - genral purpose indexes
gn - g value at index n
outputs - funtion output
 */

//test change 

//TEST VALUES:

// Definition of structures
struct output_data{
    double Tout; // outlet temperature
    std::vector<double> q_out;};

struct json_data{
    double Ts; // Soil temp
    double cp; // Specific heat (heat energy required to change temp of material)
    double H; // Active borehole length (Active length of pipe)
    double Rb; // Borehole Resistance (undefined)
    double ks; // Soil conductivity
    std::tuple <std::vector<double> ,std::vector<double>, std::vector<double>> indexed_data;};

/* --------------------------------------------------
Functions definitions: called inside of simple_GHE
---------------------------------------------------*/
// reading data from JSON file
json_data read_json()
{
    json_data input;
    std::string path = "/Users/ryan/Downloads/simpleGHE/simpleGHE/test.json";
    // json_data.indexed_data needs to have vectors in the following order: lntts, g_func, q_load
    
    return input;
}

//Expanding g data as step function so that it has same length as q_load
std::tuple<std::vector<double>,std::vector<int>::size_type> g_expander(std::tuple <std::vector<double> ,std::vector<double>, std::vector<double>> test_data, double ts){
    std::tuple<std::vector<double>,std::vector<int>::size_type> (g_data);
    std::vector<double> lntts = std::get <0> (test_data);
    std::vector<double> g_func = std::get <1> (test_data);
    std::vector<double> q_load = std::get <2>(test_data);
    
    // from length of q_load, build 0 - 8760 integer vector called q_time.
    std::vector<int>::size_type n = q_load.size();
    std::vector<int> q_time(n);
    std::iota(begin(q_time),end(q_time),1);
    n = n-1; //converting n from size of vector to an index
    std::get<1>(g_data) = n ;
    // for each value of q_time calculate ln(t/ts)
    
    //TODO: ask Matt if q_time needs to be converted to SI units to use with the characteristic time (which is derived from SI values
    int i = 0;
    std::vector <double> q_lntts;
    for (int t : q_time){
        q_lntts[i] = log (t/ts);
        i++;}
    // Create a matching condition that replaces each lntts value from q_lnnts to the closest value from lnnts
    std::vector <double> q_lntts_new;
    int j = 0;
    for (double q_t : q_lntts){
        for ( double time : lntts){
            if (abs(q_t - time) < 0.2){q_lntts_new [j] = time ;}
        }
        j++;}
    // Match the values (which will now look like steps) of q_lnnts to the corresponding g values
    i = 0;
    j = 0;
    for(double q_t_new : q_lntts_new){
        if (q_t_new == lntts [j]){std::get<0>(g_data)[i] = g_func [j];}
        else {j++;}
        i++;}
    // Create a vector, g_data, with these values that is the same length as q_load and return g_data
    if (std::get<0>(g_data).size() == q_load.size()){return g_data;}
    else{puts("q_load size does not equal g_data size");
              exit (EXIT_FAILURE);}}

// eqn 1.11
double summation(double n, std::vector<double> q_load, std::vector<double> g_data)
{
    double qi, qi1;
    double total = 0;
    int i = 0;
    while ( i < n ){
        qi = q_load[i];
        qi1 = q_load[i-1];
        // eqn 1.1
        // need to ensure variables passed to g are cosistent with what is required
        total = total + (qi - qi1)* g_data[i];
        ++i;}
    return total;
}

/*---------------
Main function
----------------*/
output_data simple_GHE(double Tin, double mdot)
{
//    //Debugging print functions:
//    int j = 0;
//    while (j < 5){
//        printf("q at import for index %d is %f\n",j,q_array[j]);
//        j = j + 1;
//    }
//    j = 0;
//    while (j < 5){
//        printf("g at import for index %d is %f\n",j,g_data[j]);
//        j = j + 1;
//    }
    
    // loading JSON data. In development
    json_data inputs = read_json(); //this will be a function that reads and returns the data from the JSON file. See struct json_data for output variable type
    std::vector<double> q_load = std::get<2>(inputs.indexed_data);
    
    //deriving characteristic time
    double alpha_s = 1*pow(10,-6);
    double ts = pow(inputs.H,2)/(9*alpha_s);
    
    //loading g_data
    std::tuple<std::vector<double>, std::vector<int>::size_type> g_data = g_expander(inputs.indexed_data, ts);
    
    //Defining variables for use in fucntion
    auto n = std::get<1>(g_data);
    double qn1 = q_load[n];
   
    

    
    //Debugging print functions:
    std::cout << "n is " << n << std::endl;
    std::cout << "qn1 is " << qn1 << std::endl;

    //eqn 1.2
    // need to ensure variables passed to g are cosistent with what is required
    double gn = std::get<0>(g_data)[n];
    
    // eqn 1.3
    double c0 = 1/(2 * M_PI * inputs.ks);
    
    //eqn 1.11
    double c1 = summation(n, q_load, std::get<0>(g_data));

    /* Main functions to solve: 1.13, 1.12, 1.14 (In that order)*/
    // 1.13
    double qn = ( (Tin - inputs.Ts) + ( ((qn1*gn)/c0) - (c1/c0) ) ) / ( (0.5 * (inputs.H/(mdot * inputs.cp))) + (gn / c0) + inputs.Rb);
    // 1.12
    double Tf = inputs.Ts + c0*(((qn - qn1) * gn) + c1) + qn * inputs.Rb;
    //1.14
    double Tout = Tf - 0.5*( (qn * inputs.H)/(mdot * inputs.cp) );
    
    std::cout << "qn before export is " << qn << std::endl;
    std::cout << "Tf before export is " << Tf << std::endl;
    std::cout << "Tout before export is " << Tout << std::endl;
    
    //updating q data and writing to structure for export from function:
    q_load[n+1] = qn;
    output_data outputs;
    outputs.Tout = Tout;
    outputs.q_out = q_load;
    
//    Debugging print functions:
//    j = 0;
//    while (j < 6){
//        printf("q before export for index %d is %f\n",j,q_array[j]);
//        j = j + 1;
//    }
//    j = 0;
//    while (j < 5){
//        printf("g before export for index %d is %f\n",j,g_data[j]);
//        j = j + 1;
//    }
    return outputs ;
}


int main()
{
    
    double Tin = 20;
    double mdot = 0.0002;
    output_data outputs = simple_GHE(Tin, mdot);
   
    //reading output data
    int j = 0;
    while (j < 6){
        std::cout << "q after export for index " << j << "is "<< outputs.q_out[j] << std::endl;
        j = j + 1;
    }
    std::cout << "Tout after export is " << outputs.Tout << std::endl;
    return 0;
}

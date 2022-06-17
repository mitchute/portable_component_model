//
//  GHE_main.cpp
//  
//
//  Created by Ryan Davies on 6/14/22.
//

#include <stdio.h>
#include <string>
#include <tuple>
#include <vector>
#include <numeric>
#include <cmath>
using namespace std;


/* -------------------SIMPLE MEAN GHE -------------------------------
Variables (See nomenclature section):
H - Active borehole length (Active length of pipe)
B - Borehole to Borehole spacing (undefined)
D - Borehole depth below surface (undefined)
Rb - Borehole Resistance (undefined)
rb (undefined)
ts - Characteristic time (way to non-dimensionalize the time) see page 12
Ts - soil temp
Tin - inlet temp
mdot - mass flow rate
cp - Specific heat (heat energy required to change temp of material)
t_step - time step
ks - Soil conductivity
q_array - previous heat load data
t_array - previous time data
g_data - data set of g values from the JSON file
Tout - outlet temp
Tf - simple mean of inlet and outlet temps
c1 - history terms
c0 - simplication term
qn - current heat load
qn1 - heat load from previous run
qi - heat at index i
qi1 - heat at index i-1
tn - current time step
tn1 - previous time step
ti - time step at index i
ti1 - time step at index i-1
alphas - soil thermal diffusivity (see pg 12 of Mitchell) (also see line 30 of main.py from the repo shared by Matt)
n - current index
i - index for summations
total - total of summation equation. Defined in function
g_value - output of g function. Defined in function
 */

/*TEST VALUES:
Note: Some variables above not used because they are unique to the definition of the g function. These are marked as '(undefined)'. Becasue the g function is a provided data set they can be ignored. */

// Definition of derived values and structures
double Tout, c1, c0, Tf, qn, qi, qn1, qi1, gn, gi, ts, alphas;
struct output_data{
    double temp_out;
    vector<double> q_out;};

struct json_data{
    double Ts;
    double cp;
    double H;
    double Rb;
    double ks;
    tuple <vector<double> ,vector<double>, vector<double>> indexed_data;};

/* --------------------------------------------------
Functions definitions: called inside of simple_GHE
---------------------------------------------------*/
// reading data from JSON file
json_data read_json()
{
    json_data input;
    string path = "/Users/ryan/Downloads/simpleGHE/simpleGHE/test.json";
    // json_data.indexed_data needs to have vectors in the following order: lntts, g_func, q_load
    
    return input;
}

//Expanding g data as step function so that it has same length as q_load
tuple<vector<double>,vector<int>::size_type> g_expander(tuple <vector<double> ,vector<double>, vector<double>> test_data, double ts){
    tuple<vector<double>,vector<int>::size_type> (g_data);
    vector<double> lntts = get <0> (test_data);
    vector<double> g_func = get <1> (test_data);
    vector<double> q_load = get <2>(test_data);
    
    // from length of q_load, build 0 - 8760 integer vector called q_time.
    vector<int>::size_type length = q_load.size();
    vector<int> q_time(length);
    iota(begin(q_time),end(q_time),1);
    get<1>(g_data) = length ;
    // for each value of q_time calculate ln(t/ts)
    int i = 0;
    vector <double> q_lntts;
    for (int t : q_time){
        q_lntts[i] = log (t/ts);
        i++;}
    // Create a matching condition that replaces each lntts value from q_lnnts to the closest value from lnnts
    int j = 0;
    vector <double> q_lntts_new;
    for (double q_t : q_lntts){
        for ( double time : lntts){
            if (abs(q_t - time) < 0.2){q_lntts_new [j] = time ;}
        }
        j++;}
    // Match the values (which will now look like steps) of q_lnnts to the corresponding g values
    i = 0;
    j = 0;
    for(double q_t_new : q_lntts_new){
        if (q_t_new == lntts [j]){get<0>(g_data)[i] = g_func [j];}
        else {j++;}
        i++;}
    // Create a vector, g_data, with these values that is the same length as q_load and return g_data
    if (get<0>(g_data).size() == q_load.size()){return g_data;}
    else{puts("q_load size does not equal g_data size");
              exit (EXIT_FAILURE);}}

// eqn 1.11
double summation(double n, vector<double> q_data, vector<double> g_data)
{
    double total = 0;
    int i = 0;
    while ( i < n ){
        qi = q_data[i];
        qi1 = q_data[i-1];
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
    json_data input_data = read_json(); //this will be a function that reads and returns the data from the JSON file. See sndbx for variable type
    vector<double> q_data = get<2>(input_data.indexed_data);
    double cp = input_data.cp;
    double H = input_data.H;
    double Rb = input_data.Rb;
    double ks = input_data.ks;
    double Ts = input_data.Ts;
    
    //loading g_data
    tuple<vector<double>, vector<int>::size_type> g_data = g_expander(input_data.indexed_data, ts);
    
    //Defining variables for use in fucntion
    int n = get<1>(g_data);
    qn1 = q_data[n];
    n = n+1;
    ts = pow(H,2)/(9*alphas);
    alphas = 1*pow(10,-6);
    

    
    //Debugging print functions:
    printf("n is %d\n",n);
    printf("qn1 is %f\n",qn1);

    //eqn 1.2
    // need to ensure variables passed to g are cosistent with what is required
    gn = get<0>(g_data)[n];
    
    // eqn 1.3
    c0 = 1/(2 * M_PI * ks);
    
    //eqn 1.11
    c1 = summation(n, q_data, get<0>(g_data));

    /* Main functions to solve: 1.13, 1.12, 1.14 (In that order)*/
    // 1.13
    qn = ( (Tin - Ts) + ( ((qn1*gn)/c0) - (c1/c0) ) ) / ( (0.5 * (H/(mdot * cp))) + (gn / c0) + Rb);
    // 1.12
    Tf = Ts + c0*(((qn - qn1) * gn) + c1) + qn * Rb;
    //1.14
    Tout = Tf - 0.5*( (qn * H)/(mdot * cp) );
    
    printf("qn before export is %f\n",qn);
    printf("Tf before export is %f\n",Tf);
    printf("Tout before export is %f\n",Tout);
    
    //updating q data and writing to structure for export from function:
    q_data[n] = qn;
    output_data outputs;
    outputs.temp_out = Tout;
    outputs.q_out = q_data;
    
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
    // required to determine the index of the q data
    output_data outputs = simple_GHE(Tin, mdot);
   
    //reading output data
    int j = 0;
    while (j < 6){
        printf("q after export for index %d is %f\n",j,outputs.q_out[j]);
        j = j + 1;
    }
    printf("Tout after export is %f\n",outputs.temp_out);
    return 0;
}

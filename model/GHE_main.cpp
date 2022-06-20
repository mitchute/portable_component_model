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

// Definition of structures
struct output_data{
    std::vector<double> Tout; // outlet temperature
    std::vector<double> q_out;};

struct json_data{
    double Ts; // Soil temp
    double cp; // Specific heat (heat energy required to change temp of material)
    double H; // Active borehole length (Active length of pipe)
    double Rb; // Borehole Resistance
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
    
    //HARD CODED TEST DATA
    
    std::vector<double> lntts =  {-15.22015406,-15.08300806,-14.94586206,-14.80871605,-14.67157005,-14.53442405,-14.39727805,-14.26013205,-14.12298605,-13.98584005,-13.84869405,-13.71154804,-13.57440204,-13.43725604,-13.30011004,-13.16296404,-13.02581804,-12.88867204,-12.75152604,-12.61438004,-12.47723403,-12.34008803,-12.20294203,-12.06579603}; //length is 24
    std::vector<double> g_func = {-2.556919564,-2.483889186,-2.408186285,-2.329364731,-2.247153415,-2.161382726,-2.071953832,-1.978823254,-1.881993912,-1.781509211,-1.677448741,-1.569925155,-1.45908234,-1.34509531,-1.228172249,-1.108558858,-0.986544532,-0.862469138,-0.736728573,-0.609776928,-0.482123413,-0.354322868,-0.226959789,-0.100626916}; //length is 24
    std::vector<double> q_load = {1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000}; //length is 24
    std::tuple <std::vector<double> ,std::vector<double>, std::vector<double>> indexed_data;
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

//Expanding g data as step function so that it has same length as q_load
std::tuple<std::vector<double>,std::vector<int>::size_type> g_expander(std::tuple <std::vector<double> ,std::vector<double>, std::vector<double>> test_data, double ts){
    

    std::vector<double> lntts = std::get <0> (test_data);
    std::vector<double> g_func = std::get <1> (test_data);
    std::vector<double> q_load = std::get <2>(test_data);
    
    // from length of q_load, build 0 - 8760 integer vector called q_time.
    std::vector<int>::size_type n = q_load.size();
    std::vector<int> q_time(n);
    std::vector<double> output_g_values(n);
    int k = 0;
    while (k<n){
        q_time[k] = 3600*k;
        k++;}; //3600 is used here to convert hourly timesteps to seconds
    n = n-1; //converting n from size of vector to n as an index
    
    // for each value of q_time calculate ln(t/ts)
    int i = 0;
    double q_lntts[n];
    for (int t : q_time){
        q_lntts[i] = log (t/ts);
        i++;}
    // Create a matching condition that replaces each lntts value from q_lnnts to the closest value from lnnts
    double q_lntts_new[n];
    int j = 0;
    for (double q_t : q_lntts){
        for ( double time : lntts){
            if (abs(q_t - time) < 0.2){q_lntts_new [j] = time ;}
        }
        j++;}
    // Match the values (which will now look like steps) of q_lnnts to the corresponding g values
    i = 0;
    j = 0;
    std::vector<int>::size_type i_max = g_func.size();
    for (double q_lntts_temp : q_lntts_new){
        while (j<i_max+1){
            if (q_lntts_temp == lntts[j]){
                output_g_values[i] = g_func[j];
            }j++;
            } i++;}
    
    //Debugging print function
    i = 1;
    for (double output : q_lntts_new){
        std::cout<< "q_lntts_new at timestep " << i << " is " << output << std::endl;
        i++;};
    //Debugging print function
    int p = 1;
    for (double output : lntts){
        std::cout<< "lntts at timestep " << p << " is " << output << std::endl;
        p++;};
    //Debugging print function
    i = 0;
    for (double output : g_func){
        std::cout<< "g_func at index " << i << " is " << output << std::endl;
        i++;};
    //Debugging print function
    i = 0;
    for (double output : output_g_values){
        std::cout<< "G_value at timestep " << i << " is " << output << std::endl;
        i++;}
    
    
    //Write data to g_data structure and return
    std::tuple<std::vector<double>,std::vector<int>::size_type> (g_data);
    std::get<0>(g_data) = output_g_values;
    std::get<1>(g_data) = n;
    
    return g_data;}

// eqn 1.11
double summation(double n, std::vector<double> q_load, std::vector<double> g_data){
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
output_data simple_GHE(double mdot)
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
    double alpha_s = 1*pow(10,-7);
    double ts = pow(inputs.H,2)/(9*alpha_s);
    
    //loading g_data
    std::tuple<std::vector<double>, std::vector<int>::size_type> g_data = g_expander(inputs.indexed_data, ts);
    
    //Main Loop
    int n = 0;
    auto m = std::get<1>(g_data);
    std::vector<double> Tout (m+1);
    while (n < m+1){
        double qn1 = q_load[n];
        double qn = q_load[n+1];
        //eqn 1.2
        // need to ensure variables passed to g are cosistent with what is required
        double gn = std::get<0>(g_data)[n];
        
        // eqn 1.3
        double c0 = 1/(2 * M_PI * inputs.ks);
        
        //eqn 1.11
        double c1 = summation(n, q_load, std::get<0>(g_data));
        /* Main functions to solve: 1.13, 1.12, 1.14 (In that order)*/
        // 1.13
//        double qn = ( (temp_q - inputs.Ts) + ( ((qn1*gn)/c0) - (c1/c0) ) ) / ( (0.5 * (inputs.H/(mdot * inputs.cp))) + (gn / c0) + inputs.Rb); // TODO: This model assumes Ts is constant. Is that right?
        // 1.12
        double Tf = inputs.Ts + c0*(((qn - qn1) * gn) + c1) + qn * inputs.Rb;
        //1.14
        Tout[n] = Tf - 0.5*( (qn * inputs.H)/(mdot * inputs.cp) );
        
//        std::cout << "qn for time step " << n << " is "<< qn << std::endl;
        std::cout << "Tout for time step " << n << " is "<<Tout[n]<<std::endl;
        n++;};
    

    
//    //Debugging print functions:
//    std::cout << "n is " << n << std::endl;
//    std::cout << "qn1 is " << qn1 << std::endl;

    
//    //Debugging print functions:
//    std::cout << "qn before export is " << qn << std::endl;
//    std::cout << "Tf before export is " << Tf << std::endl;
//    std::cout << "Tout before export is " << Tout << std::endl;
    
    //Writing to structure for export from function:
    output_data outputs;
    outputs.Tout = Tout;
    outputs.q_out = q_load;
    
//    //Debugging print functions:
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
    
    double mdot = 0.2;
    output_data outputs = simple_GHE(mdot);
   
//    //Reading output data
//    int j = 0;
//    while (j < 24){
//        std::cout << "q after export for index " << j << "is "<< outputs.q_out[j] << std::endl;
//        j = j + 1;
//    }
//    std::vector<double>::size_type t_step = outputs.q_out.size();
//    std::cout << "Tout for timestep " << t_step << " is "<< outputs.Tout << std::endl;
    return 0;
}

#ifndef GHEh
#define GHEh

#include <fstream>
#include <tuple>
#include <vector>

class GHE {
    double Ts;  // Soil temp
    double cp;  // Specific heat (heat energy required to change temp of material)
    double H;   // Active borehole length (Active length of pipe)
    double Rb;  // Borehole Resistance
    double ks;  // Soil conductivity
    double rcp; // Rho cp
    double ts;  // Characteristic time
    double bldgload;
    std::vector<double> g_func;
    std::vector<double> lntts;
    std::vector<int> q_time;
    std::vector<double> q_lntts;
    std::vector<double> ghe_load;
    std::vector<double> g_data;
    void g_expander(int m);
    double summation(int n);
    [[nodiscard]] double HP(double ghe_Tout) const;
    std::ostream *out;
    std::ostream *debug;

  public:
    double mdot; // mass flow rate

    explicit GHE(std::ostream *main_output, std::ostream *debug_output, int m);
    void main_model(int m);
};
#endif

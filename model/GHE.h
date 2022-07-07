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
    double c0;  // Simplification term
    double bldgload;
    double qn1;

    std::vector<double> g_func;
    std::vector<double> lntts;

    void g_expander(int m);
    double summation(int n);
    [[nodiscard]] double HeatPump(double ghe_Tout) const;
    std::ostream *out;

  public:
    double qn;
    double ghe_Tin;
    double mdot;
    double c1;
    std::vector<int> q_time;
    std::vector<double> q_lntts;
    std::vector<double> ghe_load;
    std::vector<double> g_data;
    std::vector<double> ghe_Tout;
    std::vector<double> ghe_Tf;

    explicit GHE(std::ostream *main_output, int m);
    void main_model(int m);
};
#endif

//
//  sndbx.cpp
//  simpleGHE
//
//  Created by Ryan Davies on 6/15/22.
//

#include <stdio.h>
#include <string>
#include <tuple>
#include <vector>
#include <numeric>
#include <cmath>
using namespace std;

int main(){
    vector<int> v = {3,2,5,4,7,6,9,8};
    printf(" front index %d\n", v.front());
    printf(" back index %d\n", v.back());
    printf(" size %lu\n",v.size());
    auto index = (v.size()) - 1 ;
    printf("index is %lu\n",index);
    int length = 8760; // in the full function length will be determined by the size function of q_data
    vector<int> q_time(length);
    iota(begin(q_time),end(q_time),1);
//    int i = 8755;
//    while (i < 8761){
//        printf(" value in position %d is %d",i,q_time[i]);
//        i++;
//    }
//    vector<int>::size_type length2 = q_time.size();
//    printf(" does this size thing work? 8759 if yes: %lu\n",length2);
    
    double ts = 1*pow(10,-6); // dummy number for now
    int i = 0;
    vector <double> q_lntts;
    for (int t : q_time){
        q_lntts[i] = log (t/ts);
        i++;
    }
    int j = 0;
    vector <double> q_lntts_new;
    for (double q_t : q_lntts){
        for ( double time : lntts){
            if (abs(q_t - time) < 0.2){
                q_lntts_new [j] = time ;
            }
        }
        j++;
    }
    
// Match the values (which will now look like steps) of q_lnnts to the corresponding g values
    vector<double> (g_data);
    i = 0;
    j = 0;
    double temp_lntts;
    for(double q_t_new : q_lntts_new){
        if (q_t_new == lntts [j]){
            g_data [i] = g_func [j]
        }
        else {
            j++;
        }
        i++;
    }
    
    return 0;
}

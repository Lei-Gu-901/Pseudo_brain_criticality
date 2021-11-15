#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <iterator>
#include <iostream>
#include <assert.h>
#include <math.h>
#include "parameter.h"

using namespace std;
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_real_distribution<float> distribution(0.0,1.0);
std::uniform_int_distribution<int> uni(0,num_neuron-1);

int main()
{   
    
    vector <int> pool;
    for (int i=0;i<kin*num_neuron;i++){
        pool.push_back(i%num_neuron);
    }
    vector <int> connect;
    int residue = kin*num_neuron;
    for (int nn=0;nn<num_neuron; nn++){
        int temp[kin];
        int count = 0;
        do {
            int comp = 0;
            float f = distribution(generator);
            int tind = (int)floor(residue*f);
            int j = pool[tind];
            if (j!=nn){
                for (int c=0;c<count;c++){
                    if (temp[c]==j){
                        comp = 1;
                    }
                }
            } else{
                comp = 1;
            }
            if (comp==0){
                temp[count] = j;
                count++;
                pool.erase(pool.begin() + tind);
                residue--;
            }
        }
        while (count<kin);
        for (int ni=0;ni<kin; ni++){
            connect.push_back(temp[ni]);
        }
    }
    ofstream con("connection.dat", ios::binary);
    size_t isize = sizeof(int);
    int csize = connect.size();
    int *conarray;
    conarray = new int[csize];
    copy(connect.begin(), connect.end(), conarray);
    con.write((const char*)conarray, isize*csize);
    return 0;
}


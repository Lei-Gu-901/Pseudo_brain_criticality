#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <assert.h>
#include <algorithm>
#include <random>
#include <chrono>
#include <stdio.h>
#include <cmath>
#include "parameter.h"
using namespace std;

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_real_distribution<float> uniform(0.0,1.0);
std::uniform_int_distribution<int> uni(0,tr);
std::exponential_distribution<float> expdis(pe);
std::exponential_distribution<float> interval(pa);
std::exponential_distribution<float> last(pb);

/*
Here, we only record spikes of neurons in the range 128-255.
Because of the random connectivity, which neurons are subsampled does not impact the result.
The causal relations require full sampling, so these are neglected.
One may refer to the fullsampling for these.
*/

static const int dn = 127;
static const int up = 256;
int in_con[num_neuron][kin]={0};
int status[num_neuron]={0};
int presta[num_neuron]={0};
int nextexteral[num_neuron]={0};
float prob[kin]={0};
int clstart = 0;
int clend = 0;

void intialization()
{
    int *connect;
    connect = new int[num_neuron*kin];
    ifstream con("connection8192.dat", ios::binary);
    con.read((char*)connect, num_neuron*kin*sizeof(int));

    clend = (int)ceil(last(generator));
    for (int i=0;i<num_neuron;i++){
        for (int j=0;j<kin;j++){
            int ind = kin*i+j;
            in_con[i][j] = connect[ind];
        }
        presta[i] = uni(generator);
        int next = (int)ceil(expdis(generator));
        if (nextexteral[i]+next < clend){
            nextexteral[i] += next;
        } else {
            nextexteral[i] = clstart + next;
        }
    }
    
    float sum = 0.0;
    for (int i=0;i<kin;i++){
        sum += exp(-B*i);
    }

    for (int i=0;i<kin;i++){
        prob[i] = exp(-B*i)/sum;
    }

}


int main()
{   
    intialization();
    vector<int> timing;
    vector<int> index;
    /*
    Actually, only the timing of subsampled spikes is needed for the post-simulation analysis
    */
    
    int numerator = 0;
    for (int ts=0; ts<stimuN; ts++){
        if (ts==clstart){
            clstart = clend + (int)ceil(interval(generator)); 
        }
        if (ts==clend){
            clend = clstart + (int)ceil(last(generator)); 
        }
        for (int nn=0; nn<num_neuron; nn++){
            int sta = presta[nn];
            int fire = 0;
            int ext = 0;
            int valid = 0;
            float ran0 = 0.0;
            for (int ni=0;ni<kin; ni++){
                int inind = in_con[nn][ni];
                if (presta[inind]==1){
                    float ran = uniform(generator);
                    if (ran<kpa*prob[ni]){
                        fire++;
                        float vran = uniform(generator);
                        if (vran>ran0) {
                            ran0 = vran;
                            valid = inind;
                        }
                    }
                    ext = 1;
                }
            }
            if (ts==nextexteral[nn]){
                if (uniform(generator) > ran0){
                    if (sta == 0){
                        status[nn] = tp;
                    } else if (sta < 0 ) {
                        status[nn] = 1;
                        if (nn>dn&&nn<up){
                            timing.push_back(ts);
                            index.push_back(nn);
                        }
                    } else {
                        if (sta==tr){
                            status[nn]=0;
                        } else {
                            status[nn]=sta+1;
                        }
                    }
                } else {
                    if (sta==0){
                        status[nn] = tp;
                    } else if (sta < 0) {
                        status[nn] = 1;
                        if (nn>dn&&nn<up){
                            timing.push_back(ts);
                            index.push_back(nn);
                        }
                    } else {
                        if (sta==tr){
                            status[nn]=0;
                        } else {
                            status[nn]=sta+1;
                        }
                    }
                }
                int next = (int)ceil(expdis(generator));
                if (nextexteral[nn]+next < clend){
                    nextexteral[nn] += next;
                } else {
                    nextexteral[nn] = clstart + next;
                }
            } else {
                if (sta==0){
                    if (ext==1) {
                        status[nn] = tp;
                    }
                } else if (sta < 0) {
                    if (fire>0) {
                        status[nn] = 1;
                        if (nn>dn&&nn<up){
                            timing.push_back(ts);
                            index.push_back(nn);
                        }
                    } else {
                        status[nn] = sta + 1;
                    }
                } else {
                    if (sta==tr){
                        status[nn]=0;
                    } else {
                        status[nn]=sta+1;
                    }
                }
            }
        }
        std::copy(std::begin(status), std::end(status), std::begin(presta));
    }
    
    int ssize = timing.size();
    int *atiming, *aindex;
    atiming = new int[ssize];
    aindex = new int[ssize];

    ofstream ftiming("./timing.dat", ios::binary);
    ofstream findex("./index.dat", ios::binary);

    copy(timing.begin(), timing.end(), atiming);
    copy(index.begin(), index.end(), aindex);

    ftiming.write((const char*)atiming, sizeof(int)*ssize);
    findex.write((const char*)aindex, sizeof(int)*ssize);

    cout<<"Done"<< endl;

    return 0;
}
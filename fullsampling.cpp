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

// static const int dn = 128;
// static const int up = 257;
int in_con[num_neuron][kin]={0};
int status[num_neuron]={0};
int presta[num_neuron]={0};
int nextexteral[num_neuron]={0};
int currentind[num_neuron]={0};
int causal[num_neuron]={0};
int caused[num_neuron]={0};
float prob[kin]={0};
int clstart = 0;
int clend = 0;

void intialization()
{
    int *connect;
    connect = new int[num_neuron*kin];
    ifstream con("connection128.dat", ios::binary);
    con.read((char*)connect, num_neuron*kin*sizeof(int));

    clend = (int)ceil(last(generator));
    for (int i=0;i<num_neuron;i++){
        for (int j=0;j<kin;j++){
            int ind = kin*i+j;
            in_con[i][j] = connect[ind];
        }
        presta[i] = uni(generator);
        causal[i] = 0;
        caused[i] = 0;
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
    vector<int> avaind;
    vector<int> start_time;                              //starting time of avalenches
    vector<int> end_time;                                //ending time of avalenches
    
    /*
    In principle, the timing of every spike, the index of the spiking neuron for every spike, and the index of the causal avalanche
    that a spike belongs to contains sufficient information for the post-simulation analysis. The starting and ending time of causal
    avalanches are recorded at runtime, because this makes the calcualtion of duration of causal avalanches much faster.
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
                        timing.push_back(ts);
                        index.push_back(nn);
                        avaind.push_back(numerator);
                        currentind[nn] = numerator;
                        start_time.push_back(ts);
                        end_time.push_back(ts);
                        numerator++;
                        causal[nn] += 1;
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
                        timing.push_back(ts);
                        index.push_back(nn);
                        avaind.push_back(currentind[valid]);
                        currentind[nn] = currentind[valid];
                        end_time[currentind[valid]] = ts;
                        // caused[valid] += 1;
                        // causal[nn] += 1;
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
                        timing.push_back(ts);
                        index.push_back(nn);
                        avaind.push_back(currentind[valid]);
                        currentind[nn] = currentind[valid];
                        end_time[currentind[valid]] = ts;
                        // caused[valid] += 1;
                        // causal[nn] += 1;
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
    int *atiming, *aindex, *aavaind;
    atiming = new int[ssize];
    aindex = new int[ssize];
    aavaind = new int[ssize];

    int asize = start_time.size();
    int *astart, *aend;
    astart = new int[asize];
    aend = new int[asize];

    ofstream ftiming("./timing.dat", ios::binary);
    ofstream findex("./index.dat", ios::binary);
    ofstream favaind("./avaind.dat", ios::binary);
    // ofstream fcausal("./causal.dat", ios::binary);
    // ofstream fcaused("./caused.dat", ios::binary);
    ofstream fstart("./start.dat", ios::binary);
    ofstream fend("./end.dat", ios::binary);

    copy(timing.begin(), timing.end(), atiming);
    copy(index.begin(), index.end(), aindex);
    copy(avaind.begin(), avaind.end(), aavaind);
    copy(start_time.begin(), start_time.end(), astart);
    copy(end_time.begin(), end_time.end(), aend);
    ftiming.write((const char*)atiming, sizeof(int)*ssize);
    findex.write((const char*)aindex, sizeof(int)*ssize);
    favaind.write((const char*)aavaind, sizeof(int)*ssize);
    // fcausal.write((const char*)&causal, sizeof(int)*num_neuron);
    // fcaused.write((const char*)&caused, sizeof(int)*num_neuron);
    fstart.write((const char*)astart, sizeof(int)*asize);
    fend.write((const char*)aend, sizeof(int)*asize);
    cout<<"Done"<< endl;

    return 0;
}
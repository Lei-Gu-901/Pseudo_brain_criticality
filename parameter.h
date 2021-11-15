#ifndef PARAMETER_H
#define PARAMETER_H
    static const int stimuN = 100000;
    static const int num_neuron =8192;
    static const int tr = 5;
    static const int tp = -50;
    static const int kin = 10;
    static const float kpa = 1.2;
    static const float pe = 1.0E-2;             // rate of external stimuli
    static const float pb =  0.1*pe;            // 1/pb is the interval of stimuli (span of stimulus interupption)
    static const float pa = 0.5*pe;             // 1/pa is the span of consecutive stimuli
    static const float B = 0.5;                 // exponent for biasing the connectivity strenghs
#endif
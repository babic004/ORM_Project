#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

int analogNum;
int digitalNum;

struct AnalogModule{
    char name[MAX_LENGHT];
    unsigned int value; 
};


struct DigitalModule{
    char name[MAX_LENGHT];
    bool value; 
};

struct AnalogModule analogModule[MAX_MODULES];
struct DigitalModule digitalModule[MAX_MODULES];


#endif /* GLOBAL_VARIABLES_H */
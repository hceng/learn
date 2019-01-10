
#ifndef _AMERICAN_H
#define _AMERICAN_H

#include <iostream>
#include "Human.h"

using namespace std;

class American : public Human{
private:
    char addr[100];
    int age;
public:
    void eating(void);  
    void wearing(void);
    void driving(void);
    American();
    American(char *name, int age, char *addr);
    virtual ~American();
};

#endif
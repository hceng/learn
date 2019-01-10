#ifndef _HUMAN_H
#define _HUMAN_H

#include <iostream>
#include <string.h>
using namespace std;

class Human {
private:
    char *name;
public:
    void setName(char *name);
    char *getName(void);
    
    virtual void eating(void) = 0;
    virtual void wearing(void) = 0;
    virtual void driving(void) = 0;
    
    virtual ~Human();
};

Human& CreateAmerican(char *name, int age, char *addr);
Human& CreateChinese(char *name, int age, char *addr);

#endif
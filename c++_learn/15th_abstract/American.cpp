#include "American.h"

void American::eating(void) 
{
    cout << "Eating American food" << endl;
}

void American::wearing(void)
{
    cout << "Wearing American clothes" << endl;
}

void American::driving(void)
{
    cout << "Driving American car" << endl;
}

American::American()
{
    
}

American::American(char *name, int age, char *addr)
{
    setName(name);
    this->age = age;
    memset(this->addr, 0, 100);
    strcpy(this->addr, addr);
}

American::~American()
{
    cout << "~American()" << endl;
}

Human& CreateAmerican(char *name, int age, char *addr)
{
    return *(new American(name, age, addr));
}
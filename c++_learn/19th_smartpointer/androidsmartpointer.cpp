#include <iostream>
#include <stdlib.h>
#include "RefBase.h"

using namespace std;
using namespace android::RSC;

class Person : public LightRefBase<Person> {
public:
    Person() { cout<<"Person()"<<endl; }
    ~Person() { cout<<"~Person()"<<endl; }
    void printInfo(void) { cout<<"This is Person"<<endl; }
};

template<typename T>
void test_func(sp<T> &pt)
{
    sp<T> sp = pt;
    cout<<"In test_func:"<<sp->getStrongCount()<<endl;
    sp->printInfo(); //(*sp).printInfo();
}

int main(int argc, char **argv)
{
    int i;
    
    sp<Person> sp = new Person();
    cout<<"Before call test_func:"<<sp->getStrongCount()<<endl;
    for (i=0; i<2; i++)
    {
        test_func(sp);
        cout<<"After call test_func:"<<sp->getStrongCount()<<endl;
    }
    
    return 0;
}

#include <iostream>
#include <stdlib.h>

using namespace std;

class RefBase {
private:
    int count;
public:
    RefBase() : count(0) {}
    void incStrong() { count++; }
    void decStrong() { count--; }
    int getStrongCount() { return count; }  
};

class Person : public RefBase {
public:
    Person() { cout<<"Person()"<<endl; }
    ~Person() { cout<<"~Person()"<<endl; }
    void printInfo(void) { cout<<"This is Person"<<endl; }
};

template<typename T>
class SmartPointer {
private:
    T *sp;
    
public:
    SmartPointer() : sp(0) {}
    
    SmartPointer(T *pt)
    {
        cout<<"SmartPointer(T *pt)"<<endl;
        sp = pt;
        sp->incStrong();
    }
    
    SmartPointer(const SmartPointer &pt)
    {
        cout<<"SmartPointer(T &pt)"<<endl;
        sp = pt.sp;
        sp->incStrong();
    }
    
    ~SmartPointer()
    {
        cout<<"~SmartPointer()"<<endl;  
        if (sp)
        {
            sp->decStrong();
            if (sp->getStrongCount() == 0)
            {
                delete sp;
                sp = NULL;
            }
        }   
    }

    T *operator->() { return sp; }
    T& operator*() { return *sp; }
};

template<typename T>
void test_func(SmartPointer<T> &pt)
{
    SmartPointer<T> sp = pt;
    cout<<"In test_func:"<<sp->getStrongCount()<<endl;
    sp->printInfo(); //(*sp).printInfo();
}

int main(int argc, char **argv)
{
    int i;
    
    SmartPointer<Person> sp = new Person();
    cout<<"Before call test_func:"<<sp->getStrongCount()<<endl;
    for (i=0; i<2; i++)
    {
        test_func(sp);
        cout<<"After call test_func:"<<sp->getStrongCount()<<endl;
    }
    
    return 0;
}

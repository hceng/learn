#include <iostream>
#include <stdlib.h>

using namespace std;

class MyException {
public:
    virtual void printInfo(void)
    {
        cout<<"This is MyException"<<endl;
    }
};

class MySubException : public MyException {
public:
    void printInfo(void)
    {
        cout<<"This is MySubException"<<endl;
    }
};


//A->B->C
//void C(int in) throw(int, double) //C++11中丢弃
//noexcept(false)可能抛出异常;noexcept(true)/noexcept不会抛出异常;
void C(int in) noexcept(false) 
{
    int i = 1;
    double d = 1.1;
    
    if (0 == in)
        cout<<"No Exception"<<endl;
    else if (1 == in)
        throw i;
    else if (2 == in)
        throw d;
    else if (3 == in)
        throw MyException();
    else if (4 == in)
        throw MySubException();
}

void B(int i)
{	
    cout<<"run B start"<<endl;
    C(i);
    cout<<"run B end"<<endl;
}

void A(int i)
{
    try{
        B(i);
    } catch (int j) {
        cout<<"catch int style exception"<<endl;
    } catch (MyException &e) {
        e.printInfo();
    }//catch (...) {
        //cout<<"catch other style exception"<<endl;
    //}
}

void my_terminate_func()
{
    cout<<"run my_terminate_func"<<endl;
}

int main(int argc, char **argv)
{
    int i ;
    
    set_terminate(my_terminate_func);
    
    if (argc != 2)
    {
        cout<<"Usage: "<<endl;
        cout<<argv[0]<<" <0|1|2|3|4>"<<endl;
        return -1;
    }
    
    i = strtoul(argv[1], NULL, 0);
    
    A(i);
    
    return 0;
}



#include <iostream>

using namespace std;

/*
int& compare_max1(int& a, int& b)
{
    return (a < b) ? b : a;
}

double& compare_max2(double& a, double& b)
{
    return (a < b) ? b : a;
}
*/

template<typename T>
T& compare_max1(T& a, T& b)
{
    cout<<__PRETTY_FUNCTION__<<endl;
    return (a < b) ? b : a;
}

template<typename T>
const T& compare_max2(const T& a, const T& b)
{
    cout<<__PRETTY_FUNCTION__<<endl;
    return (a < b) ? b : a;
}

template<typename T>
T* compare_max3(T* a, T* b)
{
    cout<<__PRETTY_FUNCTION__<<endl;
    return (a < b) ? b : a;
}

template<typename T>
void test_func(T f)
{
    cout<<__PRETTY_FUNCTION__<<endl;
}

float f1(int a, int b)
{
    
}

/*******************************/
#if 1
int& overload_template(int& a, int& b)
{
    cout<<"1 "<<__PRETTY_FUNCTION__<<endl;
}
#endif

#if 1
template<typename T>
T& overload_template(T& a, T& b)
{
    cout<<"2 "<<__PRETTY_FUNCTION__<<endl;
}
#endif

#if 0
template<typename T>
T overload_template(T a, T b)
{
    cout<<"3 "<<__PRETTY_FUNCTION__<<endl;
}
#endif

#if 1
template<typename T>
const T& overload_template(const T& a, const T& b)
{
    cout<<"4 "<<__PRETTY_FUNCTION__<<endl;
}
#endif



int main(int argc, char **argv)
{
    int ia = 2, ib = 3;
    float fa = 2, fb = 3;
    
    compare_max1(ia,ib);
    compare_max1(fa,fb);

    cout<<"-------------------------"<<endl;
    
    compare_max2(ia,ib);
    
    cout<<"-------------------------"<<endl;
    
    char ca[] = "ca";
    char cb[] = "cb";
    char cc[]  = "cc123";
    
    compare_max1(ca, cb);
    //compare_max1(ca, cc);
    compare_max3(ca, cc);
    
    cout<<"-------------------------"<<endl;
    
    test_func(f1);
    test_func(*f1);
    
    cout<<"-------------------------"<<endl;
    
    overload_template(ia,ib);
    
    int *pa = &ia;
    int *pb = &ib;
    overload_template(pa, pb);
    
    return 0;
}

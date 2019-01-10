#include <iostream>

using namespace std;

template<typename T>
class Car {
private:
    T t;
public:
    void car_weight(const T &t);
    void printInfo(void);
};

template<typename T>
void Car<T>::car_weight(const T &t)
{
    this->t = t;
}

template<typename T>
void Car<T>::printInfo(void)
{
    cout<<"Car weight is:"<<t<<endl;
}

#if 1
//定做(重载)
template<>
class Car<char *> {
public:
    void car_weight(const char *t);
    void printInfo(void);
};

void Car<char *>::car_weight(const char *t)
{
    cout<<"Car weight is:"<<t<<endl;
}

void Car<char *>::printInfo(void)
{
}
#endif

int main(int argc, char **argv)
{
    Car<int> ic;
    ic.car_weight(1000);
    ic.printInfo();
    
    Car<double> dc;
    dc.car_weight(999.99);
    dc.printInfo();
    
    Car<char *> cc;
    cc.car_weight((char *)"1000kg");
    cc.printInfo();
    
    return 0;
}

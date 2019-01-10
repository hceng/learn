#include <iostream>

using namespace std;

void add1(int a)
{
    a = a + 1;
}

void add2(int *b) 
{
    *b = *b + 1; //指针操作
}

void add3(int &c) //引用
{
    c = c + 1;
}

int main(int argc, char **argv)
{
    int a = 99;
    int b = 99;
    int c = 99;
    int d = 99;
    int &e = d; //使用e代替d
    
    add1(a);
    cout<<"a = "<<a<<endl;
    
    add2(&b);
    cout<<"b = "<<b<<endl; 

    add3(c);
    cout<<"c = "<<c<<endl;   
    
    e++;
    cout<<"d = "<<d<<endl;  
    
    return 0;
}
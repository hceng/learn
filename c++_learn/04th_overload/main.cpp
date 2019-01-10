#include <iostream>

using namespace std;

int add(int a, int b)
{
    cout<<"add int+int = "<<a+b<<endl;
    
    return a+b;
}

double add(int a, double b)
{
    cout<<"add int+double = "<<a+b<<endl;
    
    return (double)a+b;
}

double add(double a, int b)
{
    cout<<"add double+int = "<<a+b<<endl;
    
    return (double)a+b;
}

int add(int a, int b, int c)
{
    cout<<"add int+int+int = "<<a+b+c<<endl;
    
    return a+b+c;
}


int main(int argc, char **argv)
{
    add(1, 1);
    add(1, 1.5);
	add(1.5, 1);
    add(1, 1, 1);
    
    return 0;
}
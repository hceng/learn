#include <iostream>
#include <unistd.h>
#include <string.h>

using namespace std;

class Point {
private: 
    int x;
    int y;
    
public:
    Point()
    {
        
    }
    
    Point(int a, int b) : x(a+1), y(b)
    {
        
    }
    
    int getX()
    { 
        return x; 
    }
    
    int getY()
    { 
        return y; 
    }
    
    void printInfo()
    {
        cout<<"sum: ("<<x<<", "<<y<<")"<<endl;
    }
    
    friend Point add(Point &p1, Point &p2);
};

Point add(Point &p1, Point &p2)
{
    Point p;
    
    p.x = p1.x + p2.x;
    p.y = p1.y + p2.y;
    
    return p;
}


int main(int argc, char **argv)
{
    Point p1(1, 2);
    cout<<"p1: ("<<p1.getX()<<", "<<p1.getY()<<")"<<endl;
    
    Point p2(2, 3);
    cout<<"p2: ("<<p2.getX()<<", "<<p2.getY()<<")"<<endl;

    Point sum = add(p1, p2);
    sum.printInfo();
    
    return 0;
}


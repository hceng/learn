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
    
    Point(int a, int b) : x(a), y(b)
    {
        //x = a;
        //y = b;
    }
    

    void printInfo()
    {
        cout<<"printInfo: ("<<x<<", "<<y<<")"<<endl;
    }
    
    
    friend Point operator+(Point &p1, Point &p2);
    friend Point& operator++(Point &p);
    friend Point operator++(Point &p, int a);
    friend ostream& operator<<(ostream &o, Point p);
    
#if 0   
    //a+b
    Point operator+(Point &p)
    {
        cout<<"operator+(Point &p)"<<endl;
        Point r;
        r.x = this->x + p.x;
        r.y = this->y + p.y;
        return r;
    }
    
    //++a
    Point& operator++(void)
    {
        cout<<"operator++(void)"<<endl;
        this->x += 1;
        this->y += 1;
        return *this;
    }
    
    //a++
    Point operator++(int a)
    {
        cout<<"operator++(int a)"<<endl;
        Point r;
        r = *this;
        this->x += 1;
        this->y += 1;
        return r;   
    }
#endif
};

//a+b
Point operator+(Point &p1, Point &p2)
{
    cout<<"operator+(Point &p1, Point &p2)"<<endl;
    
    Point r;
    
    r.x = p1.x + p2.x;
    r.y = p1.y + p2.y;
    
    return r;
}

//++a
Point& operator++(Point &p)
{
    cout<<"operator++(Point &p)"<<endl;
    
    p.x += 1;
    p.y += 1;
    
    return p;
}

//a++
Point operator++(Point &p, int a)
{
    cout<<"operator++(Point &p, int a)"<<endl;
    
    Point r;
    r = p;
    
    p.x += 1;
    p.y += 1;
    
    return r;
}
    
//<<
ostream& operator<<(ostream &o, Point p) //第一个参数不是Point类型,不能写到成员函数里
{
    cout<<"("<<p.x<<", "<<p.y<<")";
    return o;
}

int main(int argc, char **argv)
{
    Point p1(1, 2);
    Point p2(4, 5);
    Point p;

    p = p1 + p2;
    p.printInfo();
    
    p = ++p1;
    p.printInfo();
    p1.printInfo();
    
    p = p2++;
    p.printInfo();
    p2.printInfo();
    
    cout<<p<<p1<<p2<<endl;
    
    return 0;
}


#include <iostream>

using namespace std;

class Human {
public:
    virtual void eating(void) 
    {
        cout << "use hand to eat" << endl;
    }
};

class American : public Human {
public:
    void eating(void) 
    {
        cout << "use knife to eat" << endl;
    }
};

class Chinese : public Human {
public:
    void eating(void) 
    {
        cout << "use chopsticks to eat" << endl;
    }
};

class Chongqing : public Chinese {
public:
    void eating(void) 
    {
        cout << "use chopsticks to eat Chongqing" << endl;
    }
};


void show_type(Human& h)
{
    //Pointer 
    American *a;
    Chinese *c;
    Chongqing *cq;
    
    if (a = dynamic_cast<American *>(&h))
        cout << "this human is American" << endl;
    if (c = dynamic_cast<Chinese *>(&h))
        cout << "this human is Chinese" << endl;
    if (cq = dynamic_cast<Chongqing *>(&h))
        cout << "this human is Chongqing" << endl;
    
    //Reference
    if (cq = dynamic_cast<Chongqing *>(&h))
    {
        //American& am = dynamic_cast<American&>(h); //unable convert, generate aborted
        Chinese& ch = dynamic_cast<Chinese&>(h);    
        Chongqing& cqr = dynamic_cast<Chongqing&>(h);
    }

}


int main(int argc, char **argv)
{
    double a = 4.44;
    int b = a; //double to int
	int c = (int)a;
    int d = static_cast<int>(a);
    cout<<"b = "<<b<<"  "<<"c = "<<c<<"  "<<"d = "<<d<<endl;
    
    const char *e = "www.hceng.cn";
    char *f = const_cast<char *>(e); //clear const attribute
    cout<<"e = "<<e<<"  "<<"f = "<<f<<endl;
    unsigned int *g =reinterpret_cast<unsigned int *>(f); //char * to unsigned int * 
    cout<<"g = "<<g<<endl; 
    
    Human hu;
    American am;
    Chinese ch;
    Chongqing cq;
    
    show_type(hu);
    show_type(am);
    show_type(ch);
    show_type(cq);
    
    American *h1 =static_cast<American *>(&hu);
    //American *h2 = static_cast<American *>(&cq);
    Chinese *h3 = static_cast<Chinese *>(&cq);
    
    return 0;
}

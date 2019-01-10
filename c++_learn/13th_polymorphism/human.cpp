#include <iostream>

using namespace std;

class Human {
public:
    virtual void eating(void) 
    {
        cout << "use hand to eat" << endl;
    }
    
    virtual Human* test(void) 
    {
        cout << "Human test" << endl;
    }
    
    virtual ~Human() 
    {
        cout << "~Human" << endl;
    }
};

class American : public Human {
public:
    void eating(void) 
    {
        cout << "use knife to eat" << endl;
    }
    
    virtual American* test(void) 
    {
        cout << "American test" << endl;
    }
    
    virtual ~American() 
    {
        cout << "~American" << endl;
    }
};

class Chinese : public Human {
public:
    void eating(void) 
    {
        cout << "use chopsticks to eat" << endl;
    }
    
    virtual Chinese* test(void) 
    {
        cout << "Chinese test" << endl;
    }
    
    virtual ~Chinese() 
    {
        cout << "~Chinese" << endl;
    }
};

void test_eating(Human& h)
{
    h.eating();
}

void test_return(Human& h)
{
    h.test();
}

int main(int argc, char **argv)
{
    Human h;
    American a;
    Chinese c;
    
    test_eating(h);
    test_eating(a);
    test_eating(c);
    
    test_return(h);
    test_return(a);
    test_return(c);

    return 0;
}

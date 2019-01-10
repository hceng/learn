#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

//method 1
class Sofa {
private:
    int weight;
public:
    void watchTV(void)
    {
        cout<<"Sofa: watch TV"<<endl;
    }
    
    void setWeight(int weight)
    {
        this->weight = weight;
    }
    
    int getWeight(void)
    {
        return weight;
    }
};

class Bed {
private:
    int weight;
public:
    void sleep(void)
    {
        cout<<"Bed: sleep"<<endl;
    }
    
    void setWeight(int weight)
    {
        this->weight = weight;
    }
    
    int getWeight(void)
    {
        return weight;
    }
};

class Sofabed : public Sofa , public Bed {
    
}; 


//method 2
class Furniture {
private:
    int weight;
public:
    void setWeight(int weight)
    {
        this->weight = weight;
    }
    
    int getWeight(void)
    {
        return weight;
    }   
};

class Sofa1 : virtual public Furniture {
public:
    void watchTV(void)
    {
        cout<<"Sofa1: watch TV"<<endl;
    }
};

class Bed1 : virtual public Furniture {
public:
    void sleep(void)
    {
        cout<<"Bed1: sleep"<<endl;
    }
};

class Sofabed1 : public Sofa1 , public Bed1 {
    
}; 


int main(int argc, char **argv)
{
    Sofabed s;
    Sofabed1 s1;
    
    s.watchTV();
    s.sleep();
    //s.setWeight(100); //无法确定是哪一个基类的方法
    s.Sofa::setWeight(100);
    
    s1.watchTV();
    s1.sleep();
    s1.setWeight(100);
    
    return 0;
}
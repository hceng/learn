
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

class Father {
private:
    int money;
    
protected:
    int room_key;
    int room_car;
    
public:
    int phone;
    int address;
    
    void working(void)
    {
        cout<<"father working"<<endl;
    }
    
    int getMoney(void)
    {
        return money;
    }
    void setMoeny(int money)
    {
        this->money = money;
    }
};

class Son : public Father {
private:
    int book;
    using Father::phone;
    
public:
    using Father::room_car;
    //using Father::money; //无法改变父类private类型的属性

    void learning(void)
    {
        cout<<"son learning"<<endl;
    }
    
    void getKey(void)
    {
        //s.money = 10; //private属性,子类也无法访问
        room_key = 1;   //protected属性,子类可以访问
    }
};

class Daughter : protected Father {
private:

public:
    void dancing(void)
    {
        cout<<"daughter dancing"<<endl;
        room_key = 1; //不管何种继承,在派生类内部使用父类时无差别
    }
    
    void working(void) //覆写
    {
        cout<<"daughter working"<<endl;
    }
};

int main(int argc, char **argv)
{
    Son s;
    Daughter d;
    
    //s.money = 10; //private属性,外部无法访问
    s.setMoeny(10);
    
    //s.room_key = 1; //protected属性,外部无法访问
    s.getKey();
    
    s.working(); //public,外部可以直接访问
    s.learning();
    
    s.room_car = 1; //子类将其变为了public,使得可以访问
    //s.phone = 1; //子类将其变为了private,使得不能访问
    
    d.dancing(); //自己的属性没有
    //d.working(); //protected继承,父类方法变为protected,外部无法访问
    d.working();
    
    return 0;
}
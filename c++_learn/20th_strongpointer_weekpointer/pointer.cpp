#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <unistd.h>
#include <utils/RefBase.h>
#include <LightPointeRefBase.h>

using namespace std;

#define LIGHTPOINT 0 

#if LIGHTPOINT

using namespace android::RSC;

class Person : public LightRefBase<Person> {
private:
    sp<Person> father;
    sp<Person> son;
    
public:
    Person() { cout<<"Person()"<<endl; }
    ~Person() { cout<<"~Person()"<<endl; }
    void printInfo(void) { cout<<"This is Person"<<endl; }
    void setFather(sp<Person> &father) { this->father = father; }
    void setSon(sp<Person> &son) { this->son = son; }
};

#else

using namespace android;

class Person : public RefBase {
private:
    wp<Person> father;
    wp<Person> son;
    
public:
    Person() { cout<<"Person()"<<endl; }
    ~Person() { cout<<"~Person()"<<endl; }
    void printInfo(void) { cout<<"This is Person"<<endl; }
    void setFather(sp<Person> &father) { this->father = father; }
    void setSon(sp<Person> &son) { this->son = son; }
};

#endif

void test_func(int mode)
{
    if(mode)
    {
        sp<Person> father = new Person();
        sp<Person> son = new Person();
        father->setSon(son);    
    }
    else
    {
        sp<Person> father = new Person();
        sp<Person> son = new Person();
        father->setSon(son);
        son->setFather(father);     
    }

}

int main(int argc, char **argv)
{
    int mode = atoi(argv[1]);
    if (mode < 2)
        test_func(mode);
    else
    {
#if LIGHTPOINT  
#else
        wp<Person> s1 = new Person();
        //s1->printInfo(); //ERROR, "wp" no override "->"
        //(*s1).printInfo(); //ERROR, "wp" no override "*"

        sp<Person> s2 = s1.promote();
        if (s2 != 0)
            s2->printInfo();
#endif
    }
    
    return 0;
}

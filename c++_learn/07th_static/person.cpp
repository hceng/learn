#include <iostream>
#include <unistd.h>
#include <string.h>

using namespace std;

class Person {
private: 
    char *name; 
    int age;
    char *work;
    static int cnt;

public:
    static int getCount1(void)
    {
        return cnt;
    }
    
    static int getCount2(void); 
    
    Person()  
    {
        this->name = NULL;
        this->age = 0;
        this->work = NULL;
        cnt++;
    }
    
    Person(char *name, int age = 0, char *work = (char *)"none")  
    {
        //this->name = name;
        this->name = new char[strlen(name) + 1];  
        strcpy(this->name, name);

        //this->work = work;
        this->work = new char[strlen(work) + 1];
        strcpy(this->work, work);
        
        if(age < 0 || age > 150)
            this->age = 0;
        else
            this->age = age;
        
        cnt++;
    }
    
    //析构函数
    ~Person()
    {
        
    }
    
    //拷贝构造函数
    Person(Person &per)
    {       
        cnt++;
    }
};

int Person::cnt = 0; //cnt定义和初始化

int Person::getCount2(void) 
{ 
    return cnt; 
}


int main(int argc, char **argv)
{
    Person per0;  
    Person per1((char *)"jack");  
    Person *per2 = new Person((char *)"hceng", 23, (char *)"Embedded engineer");  
    Person per3(per1); 

    Person per4[10];
    Person *per5 = new Person[10];
    
    cout << "person number = "<<Person::getCount1()<<endl;
    cout << "person number = "<<per1.getCount1()<<endl;
    cout << "person number = "<<per5->getCount1()<<endl;
    cout << "person number = "<<per5->getCount2()<<endl;
    
    return 0;
}


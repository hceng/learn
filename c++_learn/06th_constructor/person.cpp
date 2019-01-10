#include <iostream>
#include <unistd.h>
#include <string.h>

using namespace std;

class Person {
private: 
    char *name; 
    int age;
    char *work;

public:
    void setName(char *name)
    {
        this->name = name;
    }

    int setAge(int age)
    {
        if(age < 0 || age > 150)
        {
            this->age = 0;
            return -1;
        }
        this->age = age;
        return 0;
    }
    
    void setWork(char *work)
    {
        this->work = work;
    }
    
    void printInfo(void)
    {
        if (this->name && this->work) 
            cout<<"name = "<<name<<", age = "<<age<<", work = "<<work<<endl;
        else
            cout<<"name = NULL"<<", age = "<<age<<", work = NULL"<<endl;
    }
    
    
    //构造函数:和类名相同,无返回值,可以设置默认值
    Person()  //一但有了其它构造函数,默认的就必须写出来才能使用
    {
        this->name = NULL;
        this->age = 0;
        this->work = NULL;
    }
    
    Person(char *name, int age = 0, char *work = (char *)"none") //age和work设置有默认值
    {
        //this->name = name;
        this->name = new char[strlen(name) + 1]; //为了测试析构函数的自动调用,这里使用开辟堆空间存放数据
        strcpy(this->name, name);

        //this->work = work;
        this->work = new char[strlen(work) + 1];
        strcpy(this->work, work);
        
        if(age < 0 || age > 150)
            this->age = 0;
        else
            this->age = age;
    }
    
    
    ~Person()
    {
        cout << "~Person()"<<endl;
        if (this->name) {
            cout<<"name = "<<name<<endl;
            delete this->name;
        }
        if (this->work) {
            cout<<"work = "<<work<<endl;
            delete this->work;
        }
    }
    
    
    //拷贝构造函数
    Person(Person &per)
    {       
        cout << "Person(Person &per)"<<endl;
        
        this->name = new char[strlen(per.name) + 1]; 
        strcpy(this->name, per.name);

        this->work = new char[strlen(per.work) + 1];
        strcpy(this->work, per.work);
        
        if(age < 0 || age > 150)
            this->age = 0;
        else
            this->age = per.age;
    }
};

void test_object()
{
    Person per0; //调用无参构造函数(后面无括号,有括号相当于声明)
    Person per1((char *)"jack"); //调用带参构造函数,未传入的未默认值
    Person *per2 = new Person((char *)"hceng", 23, (char *)"Embedded engineer"); //这种方式需要使用delete手动清除
    
    per0.printInfo();
    per1.printInfo();
    per2->printInfo();
    
    delete per2; //如果不手动删除,在调用完test_object()后,分配的堆空间不会被释放
    cout << "delete per2 end"<<endl;
}


int main(int argc, char **argv)
{
    test_object();
    cout << "run test_object end"<<endl;
    
    
    Person per1((char *)"hceng", 23, (char *)"Embedded engineer");
    Person per2(per1); 
    per2.printInfo();
    
    return 0;
}


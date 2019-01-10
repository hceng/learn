#include <stdio.h>

class Person {
private: //默认权限
    char const *name; 
    int age;
    char const *work;

public:
    void setName(char const *name)
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
    
    void setWork(char const *work)
    {
        this->work = work;
    }
    
    
    void printInfo(void)
    {
        printf("name = %6s, age = %2d, work = %s\n", name, age, work); //函数实现直接写在了里面,且可访问数据
    }
};

int main(int argc, char **argv)
{
    int ret = 0;
    Person per;
    //per.name = "hceng"; //因为是private属性,无法直接访问
    
    per.setName("hceng"); //通过类里的函数间接访问
    ret = per.setAge(-23); //间接访问,可以实现对传入参数的控制
    per.setWork("Embedded engineer"); 
    per.printInfo();

    return ret;
}


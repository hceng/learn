#include <stdio.h>

//struct person {
class person {
public:
    char const *name; //warning: ISO C++ forbids converting a string constant to ‘char*’ [-Wwrite-strings]
    int age;
    char const *work;

    void printf_info(void)
    {
        printf("name = %6s, age = %2d, work = %s\n", name, age, work); //函数实现直接写在了里面,且可访问数据
    }
};

int main(int argc, char **argv)
{
    struct person person[] = {
        {"hceng", 23, "Embedded engineer"},
        {"jack", 21, "Graphic Designer"}
    };

    person[0].printf_info();
    person[1].printf_info();

    return 0;
}


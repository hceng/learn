#include <stdio.h>

struct person {
    char *name;
    int age;
    char *work;
    
    void (*printf_info)(struct person *per); //函数指针
};

void printf_info(struct person *per) 
{
    printf("name = %6s, age = %2d, work = %s\n", per->name, per->age, per->work);
}

int main(int argc, char **argv)
{
    struct person person[] = {
        {"hceng", 23, "Embedded engineer", printf_info},
        {"jack", 21, "Graphic Designer", printf_info}
    };
    
    person[0].printf_info(&person[0]);
    person[1].printf_info(&person[1]);
    
    return 0;
}
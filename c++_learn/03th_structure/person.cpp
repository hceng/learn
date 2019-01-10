#include "person.h"
#include <iostream>

namespace P {
	
void Person::setName(char const *name)
{
	this->name = name;
}

int Person::setAge(int age)
{
	if(age < 0 || age > 150)
	{
		this->age = 0;
		return -1;
	}
	this->age = age;
	return 0;
}

void Person::setWork(char const *work)
{
	this->work = work;
}


void Person::printInfo(void)
{
	//printf("name = %6s, age = %2d, work = %s\n", name, age, work);
	std::cout<<"name = "<<name<<", age  = "<<age<<", work = "<<work<<std::endl;
}

void printVersion(void)
{
	//printf("Person cpp, v1\n");
	std::cout<<"Person cpp, v1"<<std::endl;
}

}
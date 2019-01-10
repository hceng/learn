#include "dog.h"
#include <iostream>

namespace D {
	
using namespace std;
	
void Dog::setName(char const *name)
{
	this->name = name;
}

int Dog::setAge(int age)
{
	if(age < 0 || age > 40)
	{
		this->age = 0;
		return -1;
	}
	this->age = age;
	return 0;
}

void Dog::printInfo(void)
{
	//printf("name = %6s, age = %2d\n", name, age);
	cout<<"name = "<<name<<", age  = "<<age<<endl;
}

void printVersion(void)
{
	//printf("Dog cpp, v1\n");
	cout<<"Dog cpp, v1"<<endl;
}

}
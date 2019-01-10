//#include <stdio.h>

namespace P {
	
class Person {
private: 
    char const *name; 
    int age;
    char const *work;

public:
    void setName(char const *name);
    int setAge(int age);
    void setWork(char const *work);
    void printInfo(void);
};

void printVersion(void);

}
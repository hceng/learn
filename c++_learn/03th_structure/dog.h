#include <stdio.h>

namespace D {
	
class Dog {
private: 
    char const *name; 
    int age;

public:
    void setName(char const *name);
    int setAge(int age);
    void setWork(char const *work);
    void printInfo(void);
};

void printVersion(void);

}
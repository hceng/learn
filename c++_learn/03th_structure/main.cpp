#include <stdio.h>
#include "person.h"
#include "dog.h"


using namespace D; //把命名空间D全部导入,后面可以使用Dog来表示D:Dog 
//using D::Dog; //把D::Dog放入global namespace, 后面可以使用Dog来表示D:Dog 


int main(int argc, char **argv)
{
    P::Person per; //指定调用哪个命名空间的Person
    per.setName("hceng"); 
	per.setAge(23); 
    per.setWork("Embedded engineer"); 
    per.printInfo();
	
	Dog dog;
	dog.setName("gougou"); 
    dog.setAge(4); 
    dog.printInfo();
	
	P::printVersion(); 
	D::printVersion();
	
    return 0;
}


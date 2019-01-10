#include "Chinese.h"

void Chinese::eating(void)  
{
    cout << "Eating Chinese food" << endl;
}

void Chinese::wearing(void)
{
    cout << "Wearing Chinese clothes" << endl;
}

void Chinese::driving(void)
{
    cout << "Driving Chinese car" << endl;
}

Chinese::~Chinese()
{
    cout << "~Chinese()" << endl;
}
Human& CreateChinese(char *name, int age, char *addr)
{
    return *(new Chinese());
}
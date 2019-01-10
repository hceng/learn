#include "Human.h"

void test_eating(Human *h)
{
    h->eating();
}

int main(int argc, char **argv)
{
    Human& a = CreateAmerican((char *)"jk", 22, (char *)"America");
    Human& c = CreateChinese((char *)"hceng", 23, (char *)"Chine");
    
    Human* h[2] = {&a, &c};
    int i;
    for (i=0; i<2; i++)
        test_eating(h[i]);
    delete &a;
    delete &c;
    
    return 0;
}
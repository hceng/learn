#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

class Furniture {
public:
    Furniture(void)
    {
        cout << "Furniture(void)" << endl;
    }
};

class Verification {
public:
    Verification(void)
    {
        cout << "Verification(void)" << endl;
    }
};

class Sofa : virtual public Furniture, virtual public Verification {
public:
    Sofa(void)
    {
        cout << "Sofa(void)" << endl;
    }
};

class Bed : virtual public Furniture, virtual public Verification {
public:
    Bed(void)
    {
        cout << "Bed(void)" << endl;
    }
};

class Sofabed : public Sofa , public Bed {
public:
    Sofabed(void)
    {
        cout << "Sofabed(void)" << endl;
    }  

    Sofabed(char *s)
    {
        cout << "Sofabed(char *s)" << endl;
    }       
}; 

class Company {
public:
    Company(void)
    {
        cout << "Company(void)" << endl;
    }

    Company(char *s)
    {
        cout << "Company(char *s)" << endl;
    }   
};

class Type {
public:
    Type(void)
    {
        cout << "Type(void)" << endl;
    }   
};

class Date {
public:
    Date(void)
    {
        cout << "Date(void)" << endl;
    }
    
    Date(char *s)
    {
        cout << "Date(char *s)" << endl;
    }       
};

class CompanySofabed : public Sofabed, virtual public Company {
private:
    Type type;
    Date date;
    
public:
    CompanySofabed(void)
    {
        cout << "CompanySofabed(void)" << endl;
    }
    
    CompanySofabed(char *s1, char *s2, char *s3): Sofabed(s1), Company(s2), date(s3)
    {
        cout << "CompanySofabed(char *s)" << endl;
    }   
};

int main(int argc, char **argv)
{
    CompanySofabed c((char *)"good", (char *)"xxx", (char *)"2018");
    
    return 0;
}
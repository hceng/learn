#include <iostream>

using namespace std;

class OS {
public:
    virtual void Install() = 0;
};

class LinuxOS : public OS {
public:
    virtual void Install() { cout<<"Install Linux OS"<<endl; };
};

class WindowsOS : public OS {
public:
    virtual void Install() { cout<<"Install Windows OS"<<endl; };
};



class Company {
public:
    virtual void Use() = 0;
    virtual void InstallOS() = 0;
};

class Msi : public Company {
private:
    OS *impl;
public:
    Msi(OS *impl) { this->impl = impl; };
    virtual void Use() { cout<<"Msi computer "; };
    virtual void InstallOS() { Use(); impl->Install(); };
};

class Dell : public Company {
private:
    OS *impl;
public:
    Dell(OS *impl) { this->impl = impl; };
    virtual void Use() { cout<<"Dell computer "; };
    virtual void InstallOS() { Use(); impl->Install(); };
};



int main(int argc, char **argv)
{
    OS *lin = new LinuxOS();
    OS *win = new WindowsOS();
    
    Company *a = new Msi(lin);
    Company *b = new Dell(win);
    
    a->InstallOS();
    b->InstallOS();
    
    return 0;
}
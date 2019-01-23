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
};

class Msi : public Company {
public:
    virtual void Use() { cout<<"Msi computer "; };
};

class Dell : public Company {
public:
    virtual void Use() { cout<<"Dell computer "; };
};



class MsiUseLinuxOS : public LinuxOS, public Msi {
public:
    void InstallOS() { Use(); Install(); }
};

class DellUseWindowsOS : public WindowsOS, public Dell {
public:
    void InstallOS() { Use(); Install(); }
};

class MsiUseWindowsOS : public WindowsOS, public Msi {
public:
    void InstallOS() { Use(); Install(); }
};

class DellUseLinuxOS : public LinuxOS, public Dell {
public:
    void InstallOS() { Use(); Install(); }
};

int main(int argc, char **argv)
{
    MsiUseLinuxOS a;
    a.InstallOS();
    
    DellUseWindowsOS b;
    b.InstallOS();
    
    return 0;
}
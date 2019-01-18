#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#if 0

//hungry mode
class Singleton {
private:
    static Singleton *gInstance;

private:
    Singleton() { cout<<"Singleton()"<<endl; }
    
public:
    static Singleton *getInstance() { return gInstance; }
    
    void printInfo() { cout<<"This is Singleton"<<endl; }
};

Singleton *Singleton::gInstance = new Singleton;

#else
    
//Lazy mode 
class Singleton {
private:
    static Singleton *gInstance;
    static pthread_mutex_t g_tMutex;

private:
    Singleton() { cout<<"Singleton()"<<endl; }
    
public:
    static Singleton *getInstance()
    {
    #if 0
        if (NULL == gInstance)
        {
            pthread_mutex_lock(&g_tMutex);
            if (NULL == gInstance)
                gInstance = new Singleton;
            pthread_mutex_unlock(&g_tMutex);
        }
    #else
        pthread_mutex_lock(&g_tMutex);
        static Singleton *gInstance;
        pthread_mutex_unlock(&g_tMutex);
    #endif  
        return gInstance;
    }
    
    void printInfo() { cout<<"This is Singleton"<<endl; }
};

Singleton *Singleton::gInstance = NULL;
pthread_mutex_t Singleton::g_tMutex = PTHREAD_MUTEX_INITIALIZER;

#endif

void *start_routine_thread1(void *arg)
{
    cout<<"this is thread 1 ……"<<endl;
    Singleton *s = Singleton::getInstance();
    s->printInfo();
    
    return NULL;
}

void *start_routine_thread2(void *arg)
{
    cout<<"this is thread 2 ……"<<endl;
    Singleton *s = Singleton::getInstance();
    s->printInfo();
    
    return NULL;
}

int main(int argc, char **argv)
{
    Singleton *s1 = Singleton::getInstance();
    s1->printInfo();
    
    Singleton *s2 = Singleton::getInstance();
    s2->printInfo();
    
    //Singleton *s3 = new getInstance();
    //Singleton s4;
    
    pthread_t threadID1;
    pthread_t threadID2;
    
    pthread_create(&threadID1, NULL, start_routine_thread1, NULL);
    pthread_create(&threadID2, NULL, start_routine_thread2, NULL);
    
    sleep(2);
    
    return 0;
}

#ifndef _CHINESE_H
#define _CHINESE_H

#include "Human.h"

using namespace std;

class Chinese : public Human{
public:
    void eating(void);  
    void wearing(void);
    void driving(void);
    virtual ~Chinese();
};


#endif
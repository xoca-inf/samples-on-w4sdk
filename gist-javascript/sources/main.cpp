#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

// simple.js - simple calls example

W4_JS_IMPORT void simpleF1(const char*);
W4_JS_IMPORT int  simpleF2(int);


struct testJSApp : public IGame
{
    void onStart() override
    {

        simpleF1("test from C++");
       
        gui::createWidget<Label>(nullptr, utils::format("Simple F2 : %d", simpleF2(41)), ivec2(540, 100));
        
    }    

private:

};

W4_RUN(testJSApp)

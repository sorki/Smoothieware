#ifndef DACONTROL_H
#define DACONTROL_H

#include "Module.h"

class DACBase;

class DAControl : public Module {
    public:
        DAControl();
        virtual ~DAControl() {};

        void on_module_loaded();
        void on_gcode_received(void *);

    private:
        DACBase* dac;

};

#endif

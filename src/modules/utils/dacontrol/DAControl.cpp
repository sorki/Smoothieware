#include "DAControl.h"
#include "libs/Kernel.h"
#include "libs/nuts_bolts.h"
#include "libs/utils.h"
#include "ConfigValue.h"
#include "libs/StreamOutput.h"

#include "Gcode.h"
#include "Config.h"
#include "checksumm.h"
#include "DACBase.h"

#include "AD5667.h"

#include <string>
using namespace std;

#define dacontrol_module_enable_checksum   CHECKSUM("dacontrol_module_enable")

DAControl::DAControl()
{
    dac = NULL;
}

void DAControl::on_module_loaded()
{
    if( !THEKERNEL->config->value( dacontrol_module_enable_checksum )->by_default(false)->as_bool() ) {
        // as this module is not needed free up the resource
        delete this;
        return;
    }

    // allocate dac, if already allocated delete it first
    delete dac;

    dac = new AD5667();

    this->register_for_event(ON_GCODE_RECEIVED);
}


void DAControl::on_gcode_received(void *argument)
{
    Gcode *gcode = static_cast<Gcode*>(argument);
    if (gcode->has_m) {
        if (gcode->m == 444) {
            if (gcode->has_letter('C') && gcode->has_letter('S')) {
                int channel = gcode->get_int('C');
                int value = gcode->get_int('S');
                this->dac->set_value(channel, value);
                gcode->stream->printf("DAC channel %d set to %d\n", channel, value);
            }
            gcode->mark_as_taken();
        }
        if (gcode->m == 445) {
            gcode->stream->printf("DAC value set to %d\n", this->dac->get_value());
            gcode->mark_as_taken();
        }
    }
}

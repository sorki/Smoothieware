#ifndef DACBASE_H
#define DACBASE_H

class DACBase {
    public:
        DACBase(){}
        virtual ~DACBase(){}

        virtual void set_value( uint8_t channel, uint16_t value )= 0;
        virtual uint16_t get_value( )= 0;
};


#endif

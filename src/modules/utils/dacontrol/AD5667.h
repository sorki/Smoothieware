#ifndef AD5667_H
#define AD5667_H

#include "libs/Kernel.h"
#include "libs/MODI2C.h"
#include "libs/utils.h"
#include "DACBase.h"
#include <string>
#include <math.h>

// Driver for AD5667 Dual 16bit DAC (I2C)
// Datasheet
// http://www.analog.com/static/imported-files/data_sheets/AD5627R_5647R_5667R_5627_5667.pdf

#define AD5667_ADDRESS_GND          0x0F // address pin low (GND)
#define AD5667_ADDRESS_VDD          0x0C // address pin high (VCC)
#define AD5667_ADDRESS_NOCON        0x0E // address pin left open
#define AD5667_DEFAULT_ADDRESS      AD5667_ADDRESS_GND

// Command byte
// -----------------------------------------------------------------------------
// bit DB23 is reserved always 0 and DB22 is to enable multiple byte operations
#define AD5667_CMD_MULTIPLE_ON      0x40
#define AD5667_CMD_MULTIPLE_OFF     0x00
// bits DB21,DB20,DB19 are C2,C1,C0 (Table 8) define the operation mode
#define AD5667_CMD_WRITE_REG        0x00
#define AD5667_CMD_UPDATE_REG       0x08
#define AD5667_CMD_WRITE_UPDATE_ALL 0x10
#define AD5667_CMD_WRITE_UPDATE_DAC 0x18
#define AD5667_CMD_POWERDOWN        0x20
#define AD5667_CMD_RESET            0x28
#define AD5667_CMD_LDAC             0x30
#define AD5667_CMD_REFERENCE        0x38
// bits DB18,DB17,DB16 (Table 9) define the operating DAC
#define AD5667_CMD_DAC_A            0x00
#define AD5667_CMD_DAC_B            0x01
#define AD5667_CMD_DAC_BOTH         0x07

// Power Down Mode DB5 and DB4, DB1 AND DB0 define which DAC !!!LAST BYTE!!!
// -----------------------------------------------------------------------------
#define AD5667_PWRDWN_NORMAL        0x00
#define AD5667_PWRDWN_1K            0x10
#define AD5667_PWRDWN_100K          0x20
#define AD5667_PWRDWN_HIGHZ         0x30
 
#define AD5667_PWRDWN_DAC_A         0x01
#define AD5667_PWRDWN_DAC_B         0x02
#define AD5667_PWRDWN_DAC_BOTH      0x03

// LDAC  DB0 and DB1
// -----------------------------------------------------------------------------
#define AD5667_LDAC_ON              0x00
#define AD5667_LDAC_OFF             0x03

// RESET  DB0
// -----------------------------------------------------------------------------
#define AD5667_RESET_DAC            0x00
#define AD5667_RESET_ALL            0x01

class AD5667 : public DACBase {
    public:
        AD5667(uint8_t address = AD5667_DEFAULT_ADDRESS){
            this->i2c = new MODI2C(p9, p10);
            this->i2c_address = address;

            this->i2c->frequency(100000); // 100 KHz
            //this->i2c->frequency(400000); // 400 KHz
        }

        ~AD5667(){
            delete this->i2c;
        }

        void set_value( uint8_t channel, uint16_t value )
        {
            // cmd: 00011(A2)(A0)(A1)
            // write: 0x18
            // msb: 8b
            // lsb: 8b
            // Vout = Vrefin * (D/2^16)
            // D \in (0 .. 65535) (16bit)

            char data[3];
            data[0] = AD5667_CMD_WRITE_UPDATE_DAC;

            switch (channel){
              case (0):
                data[0] |= AD5667_CMD_DAC_A;
                break;
              case (1):
                data[0] |= AD5667_CMD_DAC_B;
                break;
              case (2):
                data[0] |= AD5667_CMD_DAC_BOTH;
                break;
            }

            data[1] = (uint8_t)(value >> 8);
            data[2] = (uint8_t)(value & 0xFF);
            this->i2c->write(this->i2c_address, data, 3);
        }

        uint16_t get_value() {
            char data[3];
            this->i2c->read(this->i2c_address, data, 3);
            return (0x0 | data[1] << 8 | data[2]);
        }

    private:


        /*
        char current_to_wiper( float current ){
          return char(ceil(float((this->factor*current))));
          this->i2c->start();
          this->i2c->stop();
        }
        */

        MODI2C* i2c;
        uint8_t i2c_address;
};


#endif

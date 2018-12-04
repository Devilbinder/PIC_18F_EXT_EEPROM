#include "i2c_layer.h"

void i2c_is_idle(void){
    while((SSPCON2 & 0x1F) || (SSPSTAT & 0x04) );
}

void i2c_start(void){
    i2c_is_idle();
    SSPCON2bits.SEN = 1;
}

void i2c_rep_start(void){
    i2c_is_idle();
    SSPCON2bits.RSEN = 1;
}

void i2c_stop(void){
    i2c_is_idle();
    SSPCON2bits.PEN = 1;
}

void i2c_write(uint8_t i2c_data){
    i2c_is_idle();
    SSPBUF = i2c_data;
    while(SSPSTATbits.BF != 0);
    while(SSPCON2bits.ACKSTAT != 0);
}

uint8_t i2c_read(uint8_t ack){
    i2c_is_idle();
    SSPCON2bits.RCEN = 1;
    while(SSPSTATbits.BF != 1);
    SSPCON2bits.ACKEN = ack;
    return SSPBUF;
}

void i2c_init(void){
    TRISCbits.RC3 = 1;
    TRISCbits.RC4 = 1;
    SSPSTATbits.SMP = 1;
    SSPSTATbits.CKE = 0;
    SSPCON1bits.SSPM = 0x08;
    SSPADD = 19;
    SSPCON1bits.SSPEN = 1;
}

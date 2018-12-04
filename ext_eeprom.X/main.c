#include <xc.h>
#include <p18f4520.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conbits.h"
#include "uart_layer.h"
#include "i2c_layer.h"

uint8_t uart_data = 0;
bool uart_got_data_bool = false;
uint8_t print_buffer[256] = {0};

uint8_t block_buff[256] = {0};
uint8_t block_sample_pos[8] = {0};
uint8_t block_sample[8] = {0};

typedef struct{
    uint8_t rw:1;
    uint8_t block:3;
    uint8_t addr:4;
}ext_eeprom;

ext_eeprom ext_eeprom_acc;

void interrupt high_isr(void);
void interrupt low_priority low_isr(void);



void main(void) {
    uint16_t i = 0;
    uint16_t j = 0;
    uint16_t l = 0;
    uint16_t u = 0;
    uint8_t acc = 0;
    uint8_t addr = 0;
    uint8_t ran = 0;
    OSCCONbits.IRCF = 0x07;
    OSCCONbits.SCS = 0x03;
    while(OSCCONbits.IOFS!=1);
    
    ext_eeprom_acc.addr = 0x0A;
    srand(46538);

    TRISB=0;
    TRISDbits.RD2=0;
    LATB=0x00;
    LATDbits.LD2=0;
    
    uart_init(51,0,1,0);//baud 9600
    i2c_init();
    
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1; 
    INTCONbits.GIEL = 1;
    
    __delay_ms(2000);
    sprintf(print_buffer,"Program Start\n\r");
    uart_send_string(print_buffer);
    uint8_t k = 0;
    
    
    
    uint32_t last = 0;
    
    while(1){
        
        ext_eeprom_acc.rw = 0;

        for(l = 0 ; l < 8 ; l++){
            block_sample_pos[l]=rand() % 256;
            ext_eeprom_acc.block = l;
            memcpy(&acc,&ext_eeprom_acc,sizeof(uint8_t));

            for(i = 0 ; i < 16 ; i++){
                i2c_start();
                i2c_write(acc);
                i2c_write(i*16);
                for(j = 0 ; j < 16 ; j++){
                   ran=rand() % 256;
                   i2c_write(ran);
                   if(u == block_sample_pos[l]){
                       block_sample[l] = ran;
                   }
                   u++;
                }
                i2c_stop(); 
                __delay_ms(5);
            }
            u=0;
        }

        //sprintf(print_buffer,"Prom Write Done\n\r");
        //uart_send_string(print_buffer);

        for(l = 0 ; l < 8 ; l++){
            //sprintf(print_buffer,"Block number %d\n\r",l);
            //uart_send_string(print_buffer);
            ext_eeprom_acc.block = l;
            ext_eeprom_acc.rw = 0;
            memcpy(&acc,&ext_eeprom_acc,sizeof(uint8_t));
            i2c_start();
            i2c_write(acc);
            i2c_write(0x00);
            ext_eeprom_acc.rw = 1;
            memcpy(&acc,&ext_eeprom_acc,sizeof(uint8_t));
            i2c_rep_start();
            i2c_write(acc);
            for(i = 0; i < 255 ; i++){
                block_buff[i] = i2c_read(1);
            }
            block_buff[255] = i2c_read(0);
            i2c_stop();

            for(i = 0; i < 256 ; i++){
                if(i == block_sample_pos[l]){
                  // sprintf(print_buffer,"0x%02x 0x%02x\n\r",block_sample[l],block_buff[i]);
                  // uart_send_string(print_buffer);
                   if(block_sample[l] != block_buff[i]){
                       LATB = 0xFF;
                        sprintf(print_buffer,"lasted cycles %d\n\r",last);
                        uart_send_string(print_buffer);
                       while(1);
                   }
                }
            }
            /*sprintf(print_buffer,"\n\r");
            uart_send_string(print_buffer);*/
        }  
        last++;
    } 
}

void interrupt high_isr(void){
    INTCONbits.GIEH = 0;
    if(PIR1bits.RCIF){
        uart_receiver(&uart_data,&uart_got_data_bool);
       PIR1bits.RCIF=0;
    }
    
    INTCONbits.GIEH = 1;
}

void interrupt low_priority low_isr(void){
    INTCONbits.GIEH = 0;
    
    INTCONbits.GIEH = 1;
}




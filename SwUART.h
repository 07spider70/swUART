#ifndef _SWUART_H
#define _SWUART_H

#include "mbed.h"
#include <string> 
#define RECEIVE_BUFFER_SIZE 255

namespace UART {
    class SwUart {

        public:
            typedef enum {
                EVEN,
                ODD,
                NONE
            } parityType;

            typedef enum {
                ONE,
                TWO,
                ONE_AND_HALF
            } stopBitsCount;

        public:
            SwUart(PinName pinRx, PinName pinTx);
            ~SwUart();

            void init();
            void setParity(parityType parity=NONE);
            void setStopBit(stopBitsCount conf=ONE);
            //nepouzite
            //bool setBaudrate(int baud);
            bool avaible();
            uint8_t readByte();
            void sendChar(char data);
            void sendString(std::string data);
            
            void sendByte(uint8_t data);
        private:

            void setTxPinHigh();
            void setTxPinLow();
            int getRxPinStatus();

            void sendStartBit();
            void sendStopBit();
            void sendParityBit(int numOfOnes);

            void changeOnRx();
            uint8_t receiveByte(); //read byte from link

            

            void computeWaitTime();

            void waitFull(); //for transmit
            void waitHalf(); //for receive

            void custWait(long waitTime);
            bool getBit(uint8_t byte, int position);
            uint8_t setOrClearBit(uint8_t variable, uint8_t bit_position, uint8_t setOrClear);



        private:
            DigitalOut* mTxPin;
            DigitalIn* mRxPin;
            InterruptIn* mReceiveInt;
            parityType mParity;
            stopBitsCount mStopBit;
            int mBaudRate;
            long mWaitTime;
            int mReceiveBufIndex;
            int mReceiveBufLen;
            int mReceiveByteBit;
            uint8_t mReceiveBuffer[RECEIVE_BUFFER_SIZE];

    };
}


#endif
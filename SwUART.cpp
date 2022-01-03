#include "SwUART.h"
using namespace UART;
SwUart::SwUart(PinName pinRx, PinName pinTx) {
    this->mTxPin = new DigitalOut(pinTx);
    this->mRxPin = new DigitalIn(pinRx);

    this->mReceiveInt = new InterruptIn(pinRx);


    this->mReceiveBufIndex = 0;
    this->mReceiveBufLen = 0;
    this->mReceiveByteBit = 0;
    for(int i =0; i < RECEIVE_BUFFER_SIZE; i++) {
        mReceiveBuffer[i] = 0;
    }

    this->mParity = NONE;
   

    this->mBaudRate = 9600;
    this->mWaitTime = 0;

    this->mStopBit = ONE;
}

SwUart::~SwUart() {
    if(this->mReceiveInt) {
        delete this->mReceiveInt;
    }

    if(this->mRxPin) {
        delete this->mRxPin;
    }

    if(this->mTxPin) {
        delete this->mTxPin;
    }

}

void SwUart::init() {

    //zapnutie preruseni
    this->mReceiveInt->fall(callback(this, &SwUart::changeOnRx));
    //vyratanie cakania
    computeWaitTime();
}

void SwUart::setParity(parityType parity) {
    
    this->mParity = parity;
}
/*
bool SwUart::setBaudrate(int baud) {
    if(baud > 256000 || baud <= 0) {
        this->mBaudRate = 9600;
        return false;
    }
    this->mBaudRate = baud;
    return true;
}
*/
void SwUart::setStopBit(stopBitsCount conf) {
    this->mStopBit = conf;
}

void SwUart::computeWaitTime() {
   
    this->mWaitTime = 1000000/this->mBaudRate;
    if(this->mWaitTime == 0) {
        this->mWaitTime = 104; //default 9600 baud -> 104 us
    }
}

void SwUart::setTxPinHigh() {
    *(this->mTxPin) = 1;
}

void SwUart::setTxPinLow() {
    *(this->mTxPin) = 0;
}

int SwUart::getRxPinStatus() {
    return this->mRxPin->read();
}

void SwUart::waitFull() {
    custWait(this->mWaitTime);
}

void SwUart::waitHalf() {
    custWait(this->mWaitTime/2);
}

void SwUart::custWait(long waitTime) {
    for(int i = 0; i < waitTime; i++) {
        wait_us(1);
    }
}

bool SwUart::avaible() {
    if(this->mReceiveBufLen > 0) {
        return true;
    } 
    return false;
}

void SwUart::sendStartBit() {
    setTxPinHigh();
    setTxPinLow();
    waitFull();
}

void SwUart::sendStopBit() {
    //po ukonceni sa caka optimalne dva cykly
    setTxPinHigh();
    switch(this->mStopBit) {
        case ONE:
            waitFull();
            break;
        case TWO:
            waitFull();
            waitFull();
            break;
        case ONE_AND_HALF:
            waitFull();
            waitHalf();
            break;
    }
    
}

uint8_t SwUart::setOrClearBit(uint8_t variable, uint8_t bit_position, uint8_t setOrClear)
{
    if(setOrClear) {
        variable |= (1<<bit_position); 
    }
    else {
        variable &= ~(1<<bit_position);
    }
    return variable;
}

void SwUart::changeOnRx() {
    waitHalf();
    if(getRxPinStatus() != 0) {
        return;
    } 

    for(int i = 0; i <8 ;i++) {
        waitFull();
        mReceiveBuffer[mReceiveBufIndex] = setOrClearBit(mReceiveBuffer[mReceiveBufIndex],i,getRxPinStatus());
    }

    if(this->mParity != NONE) {
        waitFull();
        //zisti paritu
        uint8_t n = mReceiveBuffer[mReceiveBufIndex];
        int count = 0;
        while (n) {
            count += n & 1;
            n >>= 1;
        }
        //zisti aka ma byt parita 
        bool set = false;
        switch(this->mParity) {
            case NONE:
                break;
            case EVEN:
                if(count%2 == 0) {
                    set = false;
                } else {
                    set = true;
                }
                break;

            case ODD:
                if(count%2 != 0) {
                    set = false;
                } else {
                    set = true;
                }
                break;
        }
        //porovna paritu
        if(getRxPinStatus() == set) {
            //parita je ok
        } else {
            return;
        }
    }

    waitFull();
    if(getRxPinStatus() == 1) {
        mReceiveBufLen++;
        mReceiveBufIndex++;
        if(mReceiveBufLen >= RECEIVE_BUFFER_SIZE) {
            mReceiveBufLen = 0; //neprecitali sme data, zmazeme ich a ideme odznova    
        }  
    } else {
        return;
    }
    
}

void SwUart::sendParityBit(int numOfOnes) {
    switch(this->mParity) {
        case NONE:
            break;
        case EVEN:
            if(numOfOnes%2 == 0) {
                setTxPinLow();
                waitFull();
            } else {
                 setTxPinHigh();
                waitFull();
            }
            break;

        case ODD:
            if(numOfOnes%2 != 0) {
                setTxPinLow();
                waitFull();
            } else {
                setTxPinHigh();
                waitFull();
            }
            break;
    }
}

void SwUart::sendString(std::string data) {
    for(int i = 0; i < data.length(); i++) {
        sendChar(data.at(i));
    }
}

void SwUart::sendChar(char data) {
    sendByte((uint8_t) data);
}

//range 0-7
bool SwUart::getBit(uint8_t byte, int position)
{
    return (byte >> position) & 0x1;
}

void SwUart::sendByte(uint8_t data) {
    //posleme start bit
    sendStartBit();
    int numOnes = 0;
    for(int bit= 0; bit < 8; bit++) {
        if(getBit(data,bit)) {
            setTxPinHigh();
            numOnes++;
        } else {
            setTxPinLow();
        }
        waitFull();
    }
    sendParityBit(numOnes);
    sendStopBit();
    
}

uint8_t SwUart::readByte() {
    //printf("size of avaib data: %d\n\r", mReceiveBufIndex);
    uint8_t retVal = 0;
    if(avaible()) {
        retVal = mReceiveBuffer[mReceiveBufLen-1];
        mReceiveBufIndex--;
        mReceiveBufLen--;

    }
    return retVal;
}
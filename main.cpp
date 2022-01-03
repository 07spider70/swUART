#include "mbed.h"
#include "SwUART.h"
// main() runs in its own thread in the OS

using namespace UART;



int main()
{
    SwUart swUart(PA_1,PA_2);
    //swUart.setBaudrate(9600);
    swUart.init();
    while (true) {
        swUart.sendString("Ahoj, ako sa mas?\n\r");
        
        if(swUart.avaible()) {
            //printf("data incom: \n\r");
            while(swUart.avaible()) {
                swUart.sendByte(swUart.readByte());
                //printf("znak: %c\n\r", (char) swUart.readByte());
            }
            //printf("\n\r");
        }
       ThisThread::sleep_for(5s);
    }
}


#include "mbed.h"
#include "SwUART.h"
// main() runs in its own thread in the OS

using namespace UART;



int main()
{
    SwUart swUart(PA_1,PA_2);
    swUart.init();
    while (true) {
        //swUart.sendString("hello\n\r");
        if(swUart.avaible()) {
            printf("data incom: \n\r");
            while(swUart.avaible()) {
                printf("%c\n\r",(char)swUart.readByte());
            }
            printf("\n\r");
        }
       // ThisThread::sleep_for(5s);
    }
}


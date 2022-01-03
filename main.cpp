#include "mbed.h"
#include "SwUART.h"
// main() runs in its own thread in the OS

using namespace UART;

// Maximum number of element the application buffer can contain
#define MAXIMUM_BUFFER_SIZE                                                  32

// Create a DigitalOutput object to toggle an LED whenever data is received.
static DigitalOut led(LED1);
static DigitalOut led2(LED2);
// Create a BufferedSerial object with a default baud rate.
static BufferedSerial serial_port(PB_6, PB_7);

int main()
{

     // Set desired properties (9600-8-N-1).
    serial_port.set_baud(9600);
    serial_port.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );

    // Application buffer to receive the data
    char buf[MAXIMUM_BUFFER_SIZE] = {0};
    char bufSend[1] = {0};

    SwUart swUart(PA_1,PA_2);
    //swUart.setBaudrate(9600);
    swUart.init();
    while (true) {
        swUart.sendString("o");
        
        if (uint32_t num = serial_port.read(buf, sizeof(buf))) {
            if(buf[0] == 'o') {
                 led = !led;
            }
        }
        bufSend[0] = {'a'};

        serial_port.write(bufSend,1);
        while(swUart.avaible()) {
            if(swUart.readByte() == 'a') {
                led2 = !led2;
            }
        }
        ThisThread::sleep_for(5s);
        }
       
    }



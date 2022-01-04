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

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        if(a[i] == 0) {
            return s;
        }
        s = s + a[i];
    }
    return s;
}


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

    SwUart swUart(PA_1,PA_2);
    //swUart.setBaudrate(9600);
    swUart.init();
    while (true) {
        swUart.sendString("hello");
        
        if (uint32_t num = serial_port.read(buf, sizeof(buf))) {
            string recData =  convertToString(buf, sizeof(buf));
           
            if(recData == "hello") {
                 led = !led;
            }
        }

        string swData = "ahoj";
        serial_port.write(swData.c_str(),swData.length());
        while(swUart.avaible()) {
            string rec = swUart.readString();
            if(rec == "ahoj") {
                led2 = !led2;
            }
        }
        ThisThread::sleep_for(5s);
        }
       
    }



#include "device.h"

Device::Device(Display &display, Scale &scale, RFID &rfid) : display(display), scale(scale), rfid(rfid), server(80)
{
    
}
#pragma once
#include <systemc.h>
#include "spi_defines.h"

#define READ_EDGE false

SC_MODULE(TB_SPI)
{

    sc_in<bool> sclk;
    sc_in<bool> sdi;
    sc_in<bool> sdo;

    bool in_bit;
    bool out_bit;

    sc_int<8> addr;
    sc_int<8> data;

    int edge_counter;
    bool read;

    SC_CTOR(TB_SPI)
    {
        SC_THREAD(spi_slave);
        sensitive << sclk.pos() << sclk.neg();
        dont_initialize();
        edge_counter = 0;
    }

    void spi_slave()
    {
        while(true)
        {
            wait();
            if (sclk.read() == READ_EDGE)
            {
                if (edge_counter < 8)
                {
                    in_bit = sdi.read();
                    if (edge_counter == 0) read = in_bit;
                    addr << (int)in_bit;
                    edge_counter++;
                }
                else if (!read)
                {
                    in_bit = sdi.read();
                    switch (addr)
                    {
                        case BW_RATE_READ:
                            break;
                        
                        default:
                            break;
                    }
                    edge_counter++;
                }
            }
            else
            {
                if (edge_counter >= 8 && read)
                {

                }
            }
        }
    }

};
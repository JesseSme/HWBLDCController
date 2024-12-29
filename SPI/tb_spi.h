#pragma once
#include <systemc.h>
#include <random>
#include <bitset>
#include <time.h>
#include <iomanip>

#include "spi_defines.h"

#define READ_EDGE false

SC_MODULE(TB_SPI)
{
    sc_in<bool> clk;
    sc_in<bool> arst_n;

    sc_in<bool> sclk;
    sc_in<bool> sdi;
    sc_out<bool> sdo;

    sc_fifo_out<sc_int<16> > fifo_out;
    sc_fifo_in<sc_int<8> > fifo_in;

    sc_int<8> read_default;
    sc_int<8> write_reg;

    bool reset_active = false;
    bool first;
    bool mode;

    int counter;

    SC_CTOR(TB_SPI)
    {
        SC_THREAD(data_and_result);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(spi_slave);
        sensitive << sclk.pos() << sclk.neg();
        dont_initialize();

        SC_METHOD(handle_reset);
        sensitive << arst_n;
        dont_initialize();

        first = true;
        srand(time(NULL));
        counter = 0;
        read_default = 255;
        write_reg = 0;
    }

    void handle_reset()
    {
        if (arst_n.read() == false)
        {
            reset_active = true;
            cout << "Asynchronous reset asserted at time " << sc_time_stamp() << endl;
        }
        else
        {
            reset_active = false;
            cout << "Asynchronous reset deasserted at time " << sc_time_stamp() << endl;
        }
    }

    void data_and_result()
    {
        static bool result_wait = false;
        static int16_t addr_data;

        while(true)
        {
            wait();
            if (reset_active == true)
            {
                cout << "Reset active at time " << sc_time_stamp() << endl;
                continue;
            }

            if (result_wait)
            {
                if (fifo_in.num_available() > 0)
                {
                    sc_int<8> result = fifo_in.read();
                    int result_tmp = result.to_int();
                    unsigned char result_int = (unsigned char)result_tmp;
                    cout << "Result received at " << sc_time_stamp() << endl;
                    cout << "Addr data: " 
                        << "0b" << std::bitset<16>(addr_data)
                        << endl;
                    cout << "Result: " 
                        << std::setfill('0') 
                        << std::setw(2)
                        << std::hex 
                        << " 0x"
                        << (int)result_int
                        << " 0b" << std::bitset<8>(result_int) << std::endl;
                    sc_stop();
                }
            }
            else
            {
                cout << "Writing result at " << sc_time_stamp() << endl;

                addr_data = rand();
                sc_int<16> tmp = addr_data;
                cout << "Int written: " << addr_data << endl;
                fifo_out.write(tmp);
                result_wait = true;
            }
        }
    }

    void spi_slave()
    {
        bool sclk_val = sclk.read();
        

        if (first)
        {
            first = !first;
        }
        else
        {
            cout << "SCLK changed at " << sc_time_stamp() << endl;
            cout << "SCLK at the time " << sclk_val << endl;
            bool sdi_val = sdi.read();
            cout << "SDI at the time " << sdi_val << endl;
            
            if (counter == 0)
            {
                mode = sdi_val;
                cout << "Mode: " << mode << endl;
            }

            if (mode && counter > 7)
            {
                sdo.write(read_default[(counter-8)]);
                // cout << "Read_default: " << read_default[(counter-8)] << endl;
            }
            else if (counter > 7)
            {
                write_reg[(counter-8)] = sdi_val;
            }
            // cout << "Write reg: " << write_reg << endl;
            // cout << "Counter " << counter++ << endl;
            counter++;

            if (counter >= 16)
            {
                counter = 0;
            }



        }


    }

};
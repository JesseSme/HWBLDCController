#pragma once
#include <systemc.h>
#include "spi_defines.h"

SC_MODULE(HelloWorld)
{
    SC_CTOR(HelloWorld)
    {
        SC_THREAD(say_hello);
    }

    void say_hello()
    {
        cout << "Hello World" << endl;
        sc_stop();
    }
};

SC_MODULE(SPI_Master)
{

    private:
        bool reset_active = false;
        bool active = false;
        bool transaction_done = false;
        bool rw = false;
        const int CLK_HZ = 50000000;
        const int BAUD_RATE = 115200;
        const int CLOCK_CYCLES = CLK_HZ / BAUD_RATE;
        int cycles;
        int edges;
        sc_int<16> temp;
        bool reading_en;
        sc_int<8> reading;


    public:
        sc_in<bool> clk;
        sc_in<bool> arst_n;

        sc_out<bool> cs;
        sc_out<bool> sclk;
        sc_out<bool> sdo;
        sc_in<bool> sdi;

        sc_fifo_in<sc_int<16> > fifo_in;
        sc_fifo_out<sc_int<8> > fifo_out;
        
        bool sclk_cur;
        bool sdo_cur;

    SC_CTOR(SPI_Master)
    {
        SC_THREAD(clock_thread);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(handle_reset);
        sensitive << arst_n;
        dont_initialize();

        SC_THREAD(handle_io);
        sensitive << clk.pos();
        dont_initialize();

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

    void handle_io()
    {
        static sc_int<16> store = 0;
        while(true)
        {

            wait();
            if (active)
            {
                if (transaction_done)
                {
                    sc_int<8> out_tmp;
                    if (reading_en)
                    {
                        out_tmp = reading;
                    }
                    else
                    {
                        out_tmp = store.range(7,0);
                    }
                    
                    fifo_out.write(out_tmp);
                    transaction_done = false;
                    active = false;
                }
            }
            else
            {
                cout << "Waiting for input..." << std::endl;
                cout << "Data in fifo: " << fifo_in.num_available() << endl;
                if (fifo_in.num_available() > 0)
                {
                    temp = fifo_in.read();
                    reading_en = temp[15];
                    store = temp;
                    cout << "Input received: " << temp << endl;
                    active = true;
                }
            }
        }
    }

    void clock_thread()
    {
        sclk.write(true);
        sdo.write(true);

        while (true)
        {
            wait();
            if (reset_active == true)
            {
                cycles = 0;
                edges = 0;
                sclk_cur = true;
                sdo_cur = true;
                cs.write(true);
                sclk.write(true);
                sdo.write(true);
                cout << "Reset active at time " << sc_time_stamp() << endl;
                continue;
            }

            if (active)
            {
                cycles++;
                if (cycles >= CLOCK_CYCLES)
                {
                    sclk_cur = !sclk_cur;
                    if (sclk_cur == false)
                    {
                        sdo_cur = temp[15];
                        temp <<= 1;
                    }
                    if (sclk_cur == true)
                    {
                        bool sdi_tmp = sdi.read();
                        reading <<= 1;
                        reading[0] = sdi_tmp;
                    }
                    cycles = 0;
                    edges++;
                }

                sclk.write(sclk_cur);
                sdo.write(sdo_cur);
                // cout << "Thread triggered at time " << sc_time_stamp() << endl;
            
                if (edges >= 16)
                {
                    transaction_done = true;
                    edges = 0;
                }
            }

            
        }
    }
};

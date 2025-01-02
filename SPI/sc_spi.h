#pragma once
#include <systemc.h>
#include "spi_defines.h"

/*
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
*/

SC_MODULE(SPI_Master)
{

    private:
        sc_signal<bool> reset_active;
        sc_signal<bool> active;
        sc_signal<bool> transaction_done;
        // sc_signal<bool> transaction_done_rdy;
        sc_signal<bool> rw;
        
        const int CLK_HZ = 50000000;
        const int BAUD_RATE = 115200;
        const int CLOCK_CYCLES = CLK_HZ / BAUD_RATE;
        
        
        sc_signal<sc_int<16> > temp;
        sc_signal<bool> reading_en;
        sc_signal<sc_int<8> > reading;


    public:
        sc_in<bool> clk;
        sc_in<bool> arst_n;

        sc_out<bool> cs;
        sc_out<bool> sclk;
        sc_out<bool> sdo;
        sc_in<bool> sdi;

        sc_fifo_in<sc_int<16> > fifo_in;
        sc_fifo_out<sc_int<8> > fifo_out;

    SC_CTOR(SPI_Master)
    {
        SC_THREAD(clock_thread);
        sensitive << clk.pos();
        reset_signal_is(arst_n, false);
        dont_initialize();

        SC_THREAD(handle_io);
        sensitive << clk.pos();
        reset_signal_is(arst_n, false);
        dont_initialize();

        active.write(false);
        reset_active.write(false);
        transaction_done.write(false);
        // transaction_done_rdy.write(false);
        rw.write(false);
    }

    void handle_io()
    {
        static sc_int<16> store = 0;

        while(true)
        {
            wait();
            if (arst_n.read() == false)
            {
                store = 0;
                active.write(false);
                temp.write(0);
                // transaction_done_rdy.write(false);
                continue;
            }

            if (active.read())
            {
                if (transaction_done.read())
                {
                    sc_int<8> out_tmp;
                    if (reading_en.read())
                    {
                        out_tmp = reading.read();
                    }
                    else
                    {
                        out_tmp = store.range(7,0);
                    }
                    
                    fifo_out.write(out_tmp);
                    active.write(false);
                    // transaction_done_rdy.write(true);
                }
            }
            else
            {
                cout << "Waiting for input..." << std::endl;
                cout << "Data in fifo: " << fifo_in.num_available() << endl;
                if (fifo_in.num_available() > 0)
                {
                    sc_int<16> inval = fifo_in.read();
                    reading_en.write(inval[15]);
                    temp.write(inval);
                    store = inval;
                    cout << "Input received: " << temp.read() << endl;
                    active.write(true);
                }
            }
        }
    }

    void clock_thread()
    {
        static sc_int<16> cur_data;
        static bool cur_data_read = false;
        static sc_int<8> reading_tmp;
        static bool sclk_cur = true;
        static bool sdo_cur = true;
        static int cycles = 0;
        static sc_uint<5> edges = 0;
        sclk.write(true);
        sdo.write(true);

        while (true)
        {
            wait();
            if (arst_n.read() == false)
            {
                cycles = 0;
                edges = 0;
                sclk_cur = true;
                sdo_cur = true;
                cs.write(true);
                sclk.write(true);
                sdo.write(true);
                transaction_done.write(false);
                cout << "Reset active at time " << sc_time_stamp() << endl;
                continue;
            }

            if (active.read())
            {

                cycles++;
                if (cycles >= CLOCK_CYCLES)
                {
                    if (!cur_data_read)
                    {
                        cur_data = temp.read();
                        cur_data_read = true;
                    }

                    sclk_cur = !sclk_cur;
                    if (sclk_cur == false)
                    {
                        sdo_cur = cur_data[15];
                        cur_data <<= 1;
                    }
                    if (sclk_cur == true)
                    {
                        bool sdi_tmp = sdi.read();
                        reading_tmp <<= 1;
                        reading_tmp[0] = sdi_tmp;
                    }
                    cycles = 0;
                    edges++;
                }

                sclk.write(sclk_cur);
                sdo.write(sdo_cur);
                // cout << "Thread triggered at time " << sc_time_stamp() << endl;
            
                if (edges >= 16)
                {
                    cout << "Transaction done at " << sc_time_stamp() << "!!!!!" << endl;
                    transaction_done.write(true);
                    reading.write(reading_tmp);
                    edges = 0;
                    continue;
                }
            }

            transaction_done.write(false);
            
        }
    }
};

#include <systemc.h>
#include "sc_spi.h"
#include "tb_spi.h"


int sc_main(int argc, char* argv[])
{
    sc_clock clock("clock", 10, SC_NS, 0.5);
    sc_signal<bool> reset_signal;
    sc_signal<bool> mosi;
    sc_signal<bool> miso;
    sc_signal<bool> sclk;
    sc_signal<bool> cs;

    // Data IO
    sc_fifo<sc_int<16> > fifo_input(4);
    sc_fifo<sc_int<8> > fifo_output(4);
    // HelloWorld hello("Hello_world");
    SPI_Master master("SPI_Master");
    TB_SPI tb("TB");

    master.clk(clock);
    master.arst_n(reset_signal);
    // SPI Signals
    master.cs(cs);
    master.sclk(sclk);
    master.sdo(mosi);
    master.sdi(miso);
    // SPI input and output
    master.fifo_in(fifo_input);
    master.fifo_out(fifo_output);

    // TB
    tb.clk(clock);
    tb.arst_n(reset_signal);
    tb.sclk(sclk);
    tb.sdo(miso);
    tb.sdi(mosi);
    tb.fifo_in(fifo_output);
    tb.fifo_out(fifo_input);

    sc_start(0, SC_NS);
    reset_signal.write(true);
    sc_start(1, SC_NS);
    reset_signal.write(false);
    sc_start(20, SC_NS);
    reset_signal.write(true);
    

    sc_start(1000000, SC_NS);

    return 0;
}
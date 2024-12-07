library ieee;
context ieee.ieee_std_context;
use ieee.math_real.all;

entity BLDCCtrl_pwm is
    generic (
        clk_frq : integer := 50_000_000;
        pwm_frq : integer := 20_000;
        adc_sample_width : integer := 8
    );
    port (
        clk   : in std_logic;
        reset : in std_logic;
        adc_vld : in std_logic;
        adc_in : in std_logic_vector(7 downto 0);
        phase_U_PWM_out : out std_logic;
        phase_V_PWM_out : out std_logic;
        phase_W_PWM_out : out std_logic
    );
end entity;

architecture rtl of BLDCCtrl_pwm is

    constant clk_tick_max : integer := clk_frq / pwm_frq;
    constant adc_sample_count : integer := 8;
    signal s_adc_array : std_logic_vector((adc_sample_count*adc_sample_width)-1 downto 0);
    signal s_adc : std_logic_vector(7 downto 0);

begin

    process (adc_vld, reset)
    begin
        if reset = '1' then
            s_adc <= (others => '0');
        else
        
        end if;
    end process;

end architecture;
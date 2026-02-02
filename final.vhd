-- ECE 532 Final Project Top Level
-- Connor Prikkel
-- 8/1/24

library ieee;
use ieee.std_logic_1164.all;

entity final is
  port
  (
   i_clk      : in    std_logic;
   i_rst_n    : in    std_logic;
	i_button	  : in    std_logic;
	i_switches : in    std_logic_vector(17 downto 0);
	o_hex0     : out   std_logic_vector(6 downto 0);
	o_hex1     : out   std_logic_vector(6 downto 0);
	o_hex2     : out   std_logic_vector(6 downto 0);
	o_hex3     : out   std_logic_vector(6 downto 0);
	o_hex4     : out   std_logic_vector(6 downto 0);
	o_hex5     : out   std_logic_vector(6 downto 0);
	o_hex6     : out   std_logic_vector(6 downto 0);
	o_hex7     : out   std_logic_vector(6 downto 0);
	o_gpio_leds : out  std_logic_vector(7 downto 0);
	o_rpio_leds : out std_logic_vector(17 downto 0);
	-- for final green LED
	o_gpio_final : out std_logic;
	
	-- I2C pins (bidirectional)
	b_i2c_scl  : inout std_logic;
	b_i2c_sda  : inout std_logic;
	i_uart_rxd : in    std_logic;
	o_uart_txd : out   std_logic;
	
	-- SDRAM pins
	o_sdram_addr      : out   std_logic_vector(12 downto 0);                    -- addr
   o_sdram_ba        : out   std_logic_vector(1 downto 0);                     -- ba
   o_sdram_cas_n     : out   std_logic;                                        -- cas_n
   o_sdram_cke       : out   std_logic;                                        -- cke
   o_sdram_cs_n      : out   std_logic;                                        -- cs_n
   b_sdram_dq        : inout std_logic_vector(31 downto 0);                    -- dq
   o_sdram_dqm       : out   std_logic_vector(3 downto 0);                     -- dqm
   o_sdram_ras_n     : out   std_logic;                                        -- ras_n
   o_sdram_we_n      : out   std_logic;
	o_sdram_clk			: out    std_logic
	
);
end final;

architecture sch of final is

--instantiate decoder for each hex display
component decoder is
 port (
	decode				: out std_logic_vector(6 downto 0);
	code					: in std_logic_vector(3 downto 0);
	clk					: in std_logic
);
end component decoder;
    
component final_cpu is
   port (
      clk_clk          : in  std_logic;                   
      reset_reset_n    : in  std_logic;
		button_export    : in  std_logic;
		gpio_final_export : out std_logic;
      switches_export  : in  std_logic_vector(17 downto 0);
		gpio_leds_export : out std_logic_vector(7 downto 0);
		rpio_leds_export : out std_logic_vector(17 downto 0);
      hex0_export      : out std_logic_vector(3 downto 0);                    
      hex1_export      : out std_logic_vector(3 downto 0);                    
      hex2_export      : out std_logic_vector(3 downto 0);                    
      hex3_export      : out std_logic_vector(3 downto 0);                    
      hex4_export      : out std_logic_vector(3 downto 0);                    
      hex5_export      : out std_logic_vector(3 downto 0);                    
      hex6_export      : out std_logic_vector(3 downto 0);                    
      hex7_export      : out std_logic_vector(3 downto 0);
		-- needs default val to avoid error
		uart_rxd         : in  std_logic			:= 'X';                                 
      uart_txd         : out std_logic;
		i2c_sda_in       : in std_logic        := 'X';
		i2c_scl_in       : in std_logic        := 'X';
		i2c_sda_oe       : out std_logic;
		i2c_scl_oe       : out std_logic;
		-- SDRAM pins
		sdram_addr       : out   std_logic_vector(12 downto 0);                    -- addr
		sdram_ba         : out   std_logic_vector(1 downto 0);                     -- ba
		sdram_cas_n      : out   std_logic;                                        -- cas_n
		sdram_cke        : out   std_logic;                                        -- cke
		sdram_cs_n       : out   std_logic;                                        -- cs_n
		sdram_dq         : inout std_logic_vector(31 downto 0) := (others => 'X'); -- dq
		sdram_dqm        : out   std_logic_vector(3 downto 0);                     -- dqm
		sdram_ras_n      : out   std_logic;                                        -- ras_n
		sdram_we_n       : out   std_logic;                                        -- we_n
		sdram_clk_clk    : out   std_logic		
    );
    end component final_cpu;
	 
	 signal hex0_val    :  std_logic_vector(3 downto 0);
	 signal hex1_val    :  std_logic_vector(3 downto 0);
	 signal hex2_val    :  std_logic_vector(3 downto 0);
	 signal hex3_val    :  std_logic_vector(3 downto 0);
	 signal hex4_val    :  std_logic_vector(3 downto 0);
	 signal hex5_val    :  std_logic_vector(3 downto 0);
	 signal hex6_val    :  std_logic_vector(3 downto 0);
	 signal hex7_val    :  std_logic_vector(3 downto 0);
	 
	 signal cpu_clk	  :  std_logic;
	 signal w_gpio_final : std_logic;
	 signal w_gpio_leds  : std_logic_vector(7 downto 0);
	 signal w_rpio_leds  : std_logic_vector(17 downto 0);
	 
	 --wiring signals hook up to controller
	 signal w_i2c_sda_in : std_logic;
	 signal w_i2c_scl_in : std_logic;
	 signal w_i2c_sda_oe : std_logic;
	 signal w_i2c_scl_oe : std_logic;

	 
	 

begin
	
	-- for eighteen red LEDs, OR w/ switch input so that switches can allow knight rider
	o_rpio_leds <= w_rpio_leds or i_switches;
	o_gpio_leds <= w_gpio_leds;
	o_gpio_final <= w_gpio_final;
	
	-- map signal clk to i_clk
	cpu_clk <= i_clk;
	
	-- bidirectional signals
	b_i2c_scl <= '0' when w_i2c_scl_oe = '1' else 'Z';
	b_i2c_sda <= '0' when w_i2c_sda_oe = '1' else 'Z';
	w_i2c_scl_in <= b_i2c_scl;
	w_i2c_sda_in <= b_i2c_sda;
	
  u0 : component final_cpu
  port map 
  (
   clk_clk         => i_clk,
   reset_reset_n   => i_rst_n,
   button_export   => i_button,
	switches_export  => i_switches,
	gpio_leds_export => w_gpio_leds,
	rpio_leds_export => w_rpio_leds,
	gpio_final_export => w_gpio_final,
	--map exports to decoder's signals
	hex0_export	  => hex0_val,
	hex1_export   => hex1_val,
	hex2_export   => hex2_val,
	hex3_export   => hex3_val,
	hex4_export   => hex4_val,
	hex5_export   => hex5_val,
	hex6_export   => hex6_val,
	hex7_export   => hex7_val,
	uart_rxd      => i_uart_rxd,
	uart_txd		  => o_uart_txd,
	i2c_sda_in    => w_i2c_sda_in,
	i2c_scl_in    => w_i2c_scl_in,
	i2c_sda_oe    => w_i2c_sda_oe,
	i2c_scl_oe    => w_i2c_scl_oe,
	--sdram
	sdram_addr       => o_sdram_addr,       --     sdram.addr
   sdram_ba         => o_sdram_ba,         --          .ba
   sdram_cas_n      => o_sdram_cas_n,      --          .cas_n
   sdram_cke        => o_sdram_cke,        --          .cke
   sdram_cs_n       => o_sdram_cs_n,       --          .cs_n
   sdram_dq         => b_sdram_dq,         --          .dq
   sdram_dqm        => o_sdram_dqm,        --          .dqm
   sdram_ras_n      => o_sdram_ras_n,      --          .ras_n
   sdram_we_n       => o_sdram_we_n,       --          .we_n
   sdram_clk_clk    => o_sdram_clk    		 -- sdram_clk.clk
  );
  
  --map from decoder.vhd to defs
  dec0 : component decoder
	port map (code => hex0_val, decode => o_hex0, clk => cpu_clk);
  dec1 : component decoder
	port map (code => hex1_val, decode => o_hex1, clk => cpu_clk);
  dec2 : component decoder
	port map (code => hex2_val, decode => o_hex2, clk => cpu_clk);
  dec3 : component decoder
	port map (code => hex3_val, decode => o_hex3, clk => cpu_clk);
  dec4 : component decoder
	port map (code => hex4_val, decode => o_hex4, clk => cpu_clk);
  dec5 : component decoder
	port map (code => hex5_val, decode => o_hex5, clk => cpu_clk);
  dec6: component decoder
	port map (code => hex6_val, decode => o_hex6, clk => cpu_clk);
  dec7 : component decoder
	port map (code => hex7_val, decode => o_hex7, clk => cpu_clk);


end sch;

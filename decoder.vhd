-- ECE 532 Final Project Decoder Process
-- Connor Prikkel
-- 8/1/24

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity decoder is
port (
	decode				: out std_logic_vector(6 downto 0);
	code					: in std_logic_vector(3 downto 0);
	clk					: in std_logic
);
end decoder;

architecture behavior of decoder is

begin 

	decode_proc: process(clk, code)
   begin
		  --update 7-segment only on rising_edge of clk
        if(rising_edge(clk)) then
		  
				--matches hex switch statement in C
            case code is
				   
					when "0000" =>
					
						--display switch statement val, let decoder handle binary -> hex
						--decode <= not std_logic_vector(x"3F");
						--each bit of output corresponds to segment-encoding 6,5,4,3,2,1,0
						--ex: for 0, only segment 6 is off, or "1"
						decode <= "1000000";
					when "0001" =>
						decode <= "1111001";
					when "0010" =>
						decode <= "0100100";
					when "0011" =>
						decode <= "0110000";
					when "0100" =>
						decode <= "0011001";
					when "0101" =>
						decode <= "0010010";
					when "0110" =>
						decode <= "0000010";
					when "0111" =>
						decode <= "1111000";
					when "1000" =>
						decode <= "0000000";
					when "1001" =>
						decode <= "0011000";
					when "1010" =>
						decode <= "0001000";
					when "1011" =>
						decode <= "0000011";
					when "1100" =>
						decode <= "1000110";
					when "1101" =>
						decode <= "0100001";
					when "1110" =>
						decode <= "0000110";
					when "1111" =>
						decode <= "0001110";
				end case;
        end if;
    end process decode_proc;
end behavior;

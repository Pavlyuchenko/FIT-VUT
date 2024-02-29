-- uart_rx.vhd: UART controller - receiving (RX) side
-- Author(s): Michal Pavlíček (xpavlim00)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



-- Entity declaration (DO NOT ALTER THIS PART!)
entity UART_RX is
    port(
        CLK      : in std_logic;
        RST      : in std_logic;
        DIN      : in std_logic;
        DOUT     : out std_logic_vector(7 downto 0);
        DOUT_VLD : out std_logic
    );
end entity;


-- Architecture implementation (INSERT YOUR IMPLEMENTATION HERE)
architecture behavioral of UART_RX is
	signal clk_cnt		: std_logic_vector(4 downto 0);
	signal bit_cnt		: std_logic_vector(2 downto 0);
	signal activate_clk_cnt : std_logic = '0';
	signal activate_bit_cnt : std_logic = '0';
		
begin
	-- Instance of RX FSM
	fsm: entity work.UART_RX_FSM
	port map (
		CLK => 		CLK,
		RST => 		RST,
		DIN => 		DIN,
		CNT_CLK =>	clk_cnt,
		CNT_BIT =>	bit_cnt,

		-- Activation bits 
		ACTIVATE_CLK_CNT => activate_clk_cnt,
		ACTIVATE_BIT_CNT => activate_bit_cnt,
		DATA_VALID => 	    DOUT_VLD
	);
	
	process (CLK) begin
		if RST = '1' then
			DOUT <= "00000000";
			DOUT_VLD <= '0';
			clk_cnt <= "00000";
			bit_cnt <= "000";
		elsif rising_edge(CLK)
			if activate_clk_cnt = '1' then
				clk_cnt <= clk_cnt + 1;
			else 
				clk_cnt <= "00000";
			end if;

			if activate_bit_cnt = '1' then
				if clk_cnt >= "10000" then
					clk_cnt <= "00000";

					case bit_cnt is
						when "000" =>
							bit_cnt <= "001";
							DOUT[0] <= DIN;
						when "001" =>
							bit_cnt <= "010";
							DOUT[1] <= DIN;
						when "010" =>
							bit_cnt <= "011";
							DOUT[2] <= DIN;
						when "011" =>
							bit_cnt <= "100";
							DOUT[3] <= DIN;
						when "100" =>
							bit_cnt <= "101";
							DOUT[4] <= DIN;
						when "101" =>
							bit_cnt <= "110";
							DOUT[5] <= DIN;
						when "110" =>
							bit_cnt <= "111";
							DOUT[6] <= DIN;
						when "111" =>
							bit_cnt <= "000";
							DOUT[7] <= DIN;
					end case;
				end if;
			end if;
		end if;
	end process;

end architecture;

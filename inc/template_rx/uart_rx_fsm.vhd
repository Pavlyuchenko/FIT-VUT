-- uart_rx_fsm.vhd: UART controller - finite state machine controlling RX side
-- Author(s): Michal Pavlicek  (xpavlim00)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;


entity UART_RX_FSM is
	port(
		CLK     : in std_logic;
		RST     : in std_logic;
		DIN     : in std_logic;
		CNT_CLK : in std_logic_vector(4 downto 0); -- keeps track of how many clock cycles happened (0 - 24)
		CNT_BIT : in std_logic_vector(3 downto 0); -- tracks how many bits were received (0 - 7)
		
		-- Activation bits
		ACTIVATE_CLK_CNT	: out std_logic;
		ACTIVATE_BIT_CNT	: out std_logic;
		DATA_VALID			: out std_logic
	);
end entity;


architecture behavioral of UART_RX_FSM is
type FSM_STATES is (
	Idle,
	Start,
	Data,
	Stop,
	Valid
);
signal curr_state : FSM_STATES := Idle;
begin
DATA_VALID <= '1' when curr_state = Valid else '0';
ACTIVATE_CLK_CNT <= '0' when curr_state = Idle or curr_state = Valid else '1';
ACTIVATE_BIT_CNT <= '1' when curr_state = Data else '0'; 
process (CLK) begin
	if RST = '1' then
		curr_state <= Idle;
	elsif rising_edge(CLK) then
		case curr_state is
			when Idle =>
				if DIN = '0' then
					curr_state <= Start;
				end if;
			when Start =>
				if CNT_CLK = "11000" then -- 24
					curr_state <= Data;
				end if;
			when Data =>
				if CNT_BIT = "1000" then -- 7
					curr_state <= Stop;
				end if;
			when Stop =>
				if DIN = '1' then
					curr_state <= Valid;
				end if;
			when Valid =>
				curr_state <= Idle;
		end case;
	end if;
end process;
end architecture;

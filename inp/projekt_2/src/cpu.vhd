-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2023 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): jmeno <login AT stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic;                      -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'

   -- stavove signaly
   READY    : out std_logic;                      -- hodnota 1 znamena, ze byl procesor inicializovan a zacina vykonavat program
   DONE     : out std_logic                       -- hodnota 1 znamena, ze procesor ukoncil vykonavani programu (narazil na instrukci halt)
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
	--- PC ---
	signal pc_inc : std_logic;
	signal pc_dec : std_logic;
	signal pc_reg : std_logic_vector(12 downto 0) := (others => '0');
	--- PC ---


	--- PTR ---
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	signal ptr_reg : std_logic_vector(12 downto 0) := (others => '0');
	--- PTR ---


	--- MX1 ---
	signal mx1_sel : std_logic;
	signal mx1_out : std_logic_vector(12 downto 0);
	--- MX1 ---


	--- MX2 ---
	signal mx2_sel : std_logic_vector(1 downto 0);
	signal mx2_out : std_logic_vector(7 downto 0);
	--- MX2 ---

	
	--- FSM ---
	type state_type is (
		START,
		FETCH,
		DECODE,

		S_POINTER_INC,
		S_POINTER_DEC,

		S_VALUE_INC,
		S_VALUE_INC_2,
		S_VALUE_INC_3,

		S_VALUE_DEC,
		S_VALUE_DEC_2,
		S_VALUE_DEC_3,

		S_WHILE,
		S_END_WHILE,

		S_WRITE,
		S_WRITE_2,
		S_LOAD,
		S_LOAD_2,

		S_AT,

		S_NULL
	);
	signal state : state_type := START;
	signal next_state : state_type;
	--- FSM ---
begin


	--- PC ---
	pc: process(CLK, RESET, pc_inc, pc_dec) is
	begin
		if (RESET = '1') then
			pc_reg <= (others => '0');
		elsif rising_edge(CLK) then
			if (pc_inc = '1') then
				pc_reg <= pc_reg + 1;
			elsif (pc_dec = '1') then
				pc_reg <= pc_reg - 1;
			end if;	
		end if;
	end process;
	--- PC ---


	--- PTR ---
	ptr: process(CLK, RESET, ptr_inc, ptr_dec) is 
	begin
		if (RESET = '1') then
			ptr_reg <= (others => '0');
		elsif rising_edge(CLK) then
			if (ptr_inc = '1') then
				ptr_reg <= ptr_reg + 1;
			elsif (ptr_dec = '1') then
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;
	--- PTR ---
	

	--- MX1 ---
	mx1: process(CLK, RESET, mx1_sel) is
	begin
		if (RESET = '1') then
			mx1_out <= (others => '0');
		elsif rising_edge(CLK) then
			if (mx1_sel = '0') then
				mx1_out <= pc_reg;
			elsif (mx1_sel = '1') then
				mx1_out <= ptr_reg;
			end if;
		end if;
	end process;
	DATA_ADDR <= mx1_out;
	--- MX1 ---


	--- MX2 ---
	mx2: process(CLK, RESET, mx2_sel) is
	begin
		if (RESET = '1') then
			mx2_out <= (others => '0');
		elsif rising_edge(CLK) then
			case mx2_sel is
				when "00" 	=> mx2_out <= IN_DATA;
				when "01" 	=> mx2_out <= DATA_RDATA + 1;
				when "10" 	=> mx2_out <= DATA_RDATA - 1;
				when others => mx2_out <= (others => '0');
			end case;
		end if;
	end process;
	DATA_WDATA <= mx2_out;
	--- MX2 ---


	--- FSM ---
	fsm_start: process(CLK, RESET, EN) is
	begin
		if (RESET = '1') then
			state <= START;
		elsif rising_edge(CLK) then
			if EN = '1' then
				state <= next_state;
			end if;
		end if;
	end process;

	fsm: process(state, IN_VLD, OUT_BUSY, DATA_RDATA) is
	begin
		pc_inc <= '0';
		pc_dec <= '0';
		ptr_inc <= '0';
		ptr_dec <= '0';
		mx1_sel <= '0';
		mx2_sel <= "00";

		DATA_RDWR <= '0';
		DATA_EN <= '1';
		IN_REQ <= '0';
		OUT_WE <= '0';

		DONE <= '0';
		READY <= '0';
		
		case state is
			when START =>
				next_state <= FETCH;
			when FETCH =>
				report "FETCH";
				next_state <= DECODE;
			when DECODE => 
				report "DECODE";
				case DATA_RDATA is
					when X"3E" =>
						report "1";
						next_state <= S_POINTER_INC;
					when X"3C" =>
						report "2";
						next_state <= S_POINTER_DEC;
					when X"2B" =>
						report "3";
						next_state <= S_VALUE_INC;
					when X"2D" =>
						report "4";
						next_state <= S_VALUE_DEC;
					when X"5B" => next_state <= S_WHILE;
						report "5";
					when X"5D" => next_state <= S_END_WHILE;
						report "6";
					when X"2E" => next_state <= S_WRITE;
						report "7";
					when X"2C" =>
						next_state <= S_LOAD;
						report "8";
					when X"40" =>
						report "9";
						next_state <= S_AT;
					when X"00" =>
						report "00";
						next_state <= S_NULL;
					when others =>
						report "0thers";
						pc_inc <= '1';
						next_state <= S_NULL;
				end case;
			when S_AT =>
				report "ENDING";
				READY <= '1';
				DONE <= '1';
				pc_inc <= '1';
				next_state <= FETCH;

			when S_POINTER_INC =>
				report "S_POINTER_INC";
				pc_inc <= '1';
				ptr_inc <= '1';
				next_state <= FETCH;
			when S_POINTER_DEC =>
				pc_dec <= '1';
				ptr_dec <= '1';
				next_state <= FETCH;

			when S_VALUE_INC =>
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				next_state <= S_VALUE_INC_2;
			when S_VALUE_INC_2 =>
				mx2_sel <= "01";
				next_state <= S_VALUE_INC_3;
			when S_VALUE_INC_3 =>
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				pc_inc <= '1';
				next_state <= FETCH;

			when S_VALUE_DEC =>
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				next_state <= S_VALUE_DEC_2;
			when S_VALUE_DEC_2 =>
				mx2_sel <= "10";
				next_state <= S_VALUE_DEC_3;
			when S_VALUE_DEC_3 =>
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				pc_inc <= '1';
				next_state <= FETCH;

			when S_WHILE =>
			when S_END_WHILE =>

			when S_WRITE =>
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				next_state <= S_WRITE_2;
			when S_WRITE_2 =>
				if OUT_BUSY = '1' then
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					next_state <= S_WRITE_2;
				else 
					OUT_WE <= '1';
					pc_inc <= '1';
					next_state <= FETCH;
				end if;

			when S_LOAD =>
				report "S_LOAD";
				IN_REQ <= '1';
				if (IN_VLD = '1') then
					report "INPUT RECEIVED";
					next_state <= S_LOAD_2;
				else
					report "WAITING FOR INPUT";
					next_state <= S_LOAD;
				end if;
			when S_LOAD_2 =>
				pc_inc <= '1';
				mx1_sel <= '1';
				mx2_sel <= "00";
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				next_state <= FETCH;

			when S_NULL =>
				next_state <= S_NULL;
				report "NULL 2";
			when others => next_state <= S_NULL;
		end case;
	end process;
	--- FSM ---
end behavioral;



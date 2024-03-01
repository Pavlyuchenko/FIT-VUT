library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-- Entity declaration
entity UART_RX is
    port(
        CLK      : in std_logic;
        RST      : in std_logic;
        DIN      : in std_logic;
        DOUT     : out std_logic_vector(7 downto 0);
        DOUT_VLD : out std_logic
    );
end entity;

-- Architecture implementation
architecture behavioral of UART_RX is
    signal clk_cnt          : std_logic_vector(4 downto 0) := "00000";
    signal bit_cnt          : std_logic_vector(3 downto 0) := "0000";
    signal activate_clk_cnt : std_logic := '0';
    signal activate_bit_cnt : std_logic := '0';
    signal valid_data       : std_logic := '0';
begin

    -- Instance of RX FSM
    fsm: entity work.UART_RX_FSM
    port map (
        CLK             => CLK,
        RST             => RST,
        DIN             => DIN,
        CNT_CLK         => clk_cnt,
        CNT_BIT         => bit_cnt,
        ACTIVATE_CLK_CNT => activate_clk_cnt,
        ACTIVATE_BIT_CNT => activate_bit_cnt,
        DATA_VALID      => valid_data
    );

    process (CLK) begin
        if RST = '1' then
            DOUT <= "00000000";
            DOUT_VLD <= '0';
            clk_cnt <= "00001";
            bit_cnt <= "0000";
        elsif rising_edge(CLK) then
            DOUT_VLD <= '0'; -- Assuming data is invalid by default
            if activate_clk_cnt = '1' then
                clk_cnt <= clk_cnt + 1;
            else 
                clk_cnt <= "00001";
            end if;
            
            if bit_cnt = "1000" and valid_data = '1' then
                DOUT_VLD <= '1';
                bit_cnt <= "0000";
            end if;

            if activate_bit_cnt = '1' and clk_cnt >= "10000" then
                clk_cnt <= "00001";

                case bit_cnt is
                    when "0000" =>
                        bit_cnt <= "0001";
                        DOUT(0) <= DIN;
                    when "0001" =>
                        bit_cnt <= "0010";
                        DOUT(1) <= DIN;
                    when "0010" =>
                        bit_cnt <= "0011";
                        DOUT(2) <= DIN;
                    when "0011" =>
                        bit_cnt <= "0100";
                        DOUT(3) <= DIN;
                    when "0100" =>
                        bit_cnt <= "0101";
                        DOUT(4) <= DIN;
                    when "0101" =>
                        bit_cnt <= "0110";
                        DOUT(5) <= DIN;
                    when "0110" =>
                        bit_cnt <= "0111";
                        DOUT(6) <= DIN;
                    when "0111" =>
                        bit_cnt <= "1000";
                        DOUT(7) <= DIN;
                    when others =>
                        null;
                end case;
            end if;
        end if;
    end process;
end architecture;


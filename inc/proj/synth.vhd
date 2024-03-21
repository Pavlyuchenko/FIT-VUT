library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
entity UART_RX is
  port (
    CLK: in std_logic;
    RST: in std_logic;
    DIN: in std_logic;
    DOUT: out std_logic_vector (7 downto 0) := "00000000";
    DOUT_VLD: out std_logic := '0'
  );
end entity;
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity uart_rx_fsm is
  port (
    clk : in std_logic;
    rst : in std_logic;
    din : in std_logic;
    cnt_clk : in std_logic_vector (4 downto 0);
    cnt_bit : in std_logic_vector (3 downto 0);
    activate_clk_cnt : out std_logic;
    activate_bit_cnt : out std_logic;
    data_valid : out std_logic);
end entity uart_rx_fsm;

architecture rtl of uart_rx_fsm is
  signal curr_state : std_logic_vector (2 downto 0);
  signal n100_o : std_logic;
  signal n101_o : std_logic;
  signal n105_o : std_logic;
  signal n107_o : std_logic;
  signal n108_o : std_logic;
  signal n109_o : std_logic;
  signal n113_o : std_logic;
  signal n114_o : std_logic;
  signal n119_o : std_logic;
  signal n121_o : std_logic_vector (2 downto 0);
  signal n123_o : std_logic;
  signal n125_o : std_logic;
  signal n127_o : std_logic_vector (2 downto 0);
  signal n129_o : std_logic;
  signal n131_o : std_logic;
  signal n133_o : std_logic_vector (2 downto 0);
  signal n135_o : std_logic;
  signal n137_o : std_logic_vector (2 downto 0);
  signal n139_o : std_logic;
  signal n141_o : std_logic;
  signal n142_o : std_logic_vector (4 downto 0);
  signal n145_o : std_logic_vector (2 downto 0);
  signal n150_q : std_logic_vector (2 downto 0) := "000";
begin
  activate_clk_cnt <= n109_o;
  activate_bit_cnt <= n114_o;
  data_valid <= n101_o;
  -- uart_rx_fsm.vhd:33:8
  curr_state <= n150_q; -- (isignal)
  -- uart_rx_fsm.vhd:35:35
  n100_o <= '1' when curr_state = "100" else '0';
  -- uart_rx_fsm.vhd:35:19
  n101_o <= '0' when n100_o = '0' else '1';
  -- uart_rx_fsm.vhd:36:41
  n105_o <= '1' when curr_state = "000" else '0';
  -- uart_rx_fsm.vhd:36:62
  n107_o <= '1' when curr_state = "100" else '0';
  -- uart_rx_fsm.vhd:36:48
  n108_o <= n105_o or n107_o;
  -- uart_rx_fsm.vhd:36:25
  n109_o <= '1' when n108_o = '0' else '0';
  -- uart_rx_fsm.vhd:37:41
  n113_o <= '1' when curr_state = "010" else '0';
  -- uart_rx_fsm.vhd:37:25
  n114_o <= '0' when n113_o = '0' else '1';
  -- uart_rx_fsm.vhd:44:40
  n119_o <= not din;
  -- uart_rx_fsm.vhd:44:33
  n121_o <= curr_state when n119_o = '0' else "001";
  -- uart_rx_fsm.vhd:43:25
  n123_o <= '1' when curr_state = "000" else '0';
  -- uart_rx_fsm.vhd:48:44
  n125_o <= '1' when unsigned (cnt_clk) >= unsigned'("11000") else '0';
  -- uart_rx_fsm.vhd:48:33
  n127_o <= curr_state when n125_o = '0' else "010";
  -- uart_rx_fsm.vhd:47:25
  n129_o <= '1' when curr_state = "001" else '0';
  -- uart_rx_fsm.vhd:52:44
  n131_o <= '1' when unsigned (cnt_bit) >= unsigned'("1000") else '0';
  -- uart_rx_fsm.vhd:52:33
  n133_o <= curr_state when n131_o = '0' else "011";
  -- uart_rx_fsm.vhd:51:25
  n135_o <= '1' when curr_state = "010" else '0';
  -- uart_rx_fsm.vhd:56:33
  n137_o <= curr_state when din = '0' else "100";
  -- uart_rx_fsm.vhd:55:25
  n139_o <= '1' when curr_state = "011" else '0';
  -- uart_rx_fsm.vhd:59:25
  n141_o <= '1' when curr_state = "100" else '0';
  n142_o <= n141_o & n139_o & n135_o & n129_o & n123_o;
  -- uart_rx_fsm.vhd:42:17
  with n142_o select n145_o <=
    "000" when "10000",
    n137_o when "01000",
    n133_o when "00100",
    n127_o when "00010",
    n121_o when "00001",
    "XXX" when others;
  -- uart_rx_fsm.vhd:41:9
  process (clk, rst)
  begin
    if rst = '1' then
      n150_q <= "000";
    elsif rising_edge (clk) then
      n150_q <= n145_o;
    end if;
  end process;
end rtl;


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

architecture rtl of uart_rx is
  signal wrap_CLK: std_logic;
  signal wrap_RST: std_logic;
  signal wrap_DIN: std_logic;
  subtype typwrap_DOUT is std_logic_vector (7 downto 0);
  signal wrap_DOUT: typwrap_DOUT;
  signal wrap_DOUT_VLD: std_logic;
  signal clk_cnt : std_logic_vector (4 downto 0);
  signal bit_cnt : std_logic_vector (3 downto 0);
  signal activate_clk_cnt : std_logic;
  signal activate_bit_cnt : std_logic;
  signal valid_data : std_logic;
  signal fsm_c_activate_clk_cnt : std_logic;
  signal fsm_c_activate_bit_cnt : std_logic;
  signal fsm_c_data_valid : std_logic;
  signal n16_o : std_logic_vector (4 downto 0);
  signal n18_o : std_logic_vector (4 downto 0);
  signal n20_o : std_logic;
  signal n21_o : std_logic;
  signal n24_o : std_logic;
  signal n27_o : std_logic_vector (3 downto 0);
  signal n29_o : std_logic;
  signal n30_o : std_logic;
  signal n32_o : std_logic_vector (2 downto 0);
  signal n36_o : std_logic_vector (3 downto 0);
  signal n39_o : std_logic_vector (4 downto 0);
  signal n40_o : std_logic_vector (3 downto 0);
  signal n54_q : std_logic_vector (4 downto 0) := "00000";
  signal n55_q : std_logic_vector (3 downto 0) := "0000";
  signal n56_o : std_logic_vector (7 downto 0);
  signal n57_q : std_logic_vector (7 downto 0) := "00000000";
  signal n58_q : std_logic := '0';
  signal n59_o : std_logic;
  signal n60_o : std_logic;
  signal n61_o : std_logic;
  signal n62_o : std_logic;
  signal n63_o : std_logic;
  signal n64_o : std_logic;
  signal n65_o : std_logic;
  signal n66_o : std_logic;
  signal n67_o : std_logic;
  signal n68_o : std_logic;
  signal n69_o : std_logic;
  signal n70_o : std_logic;
  signal n71_o : std_logic;
  signal n72_o : std_logic;
  signal n73_o : std_logic;
  signal n74_o : std_logic;
  signal n75_o : std_logic;
  signal n76_o : std_logic;
  signal n77_o : std_logic;
  signal n78_o : std_logic;
  signal n79_o : std_logic;
  signal n80_o : std_logic;
  signal n81_o : std_logic;
  signal n82_o : std_logic;
  signal n83_o : std_logic;
  signal n84_o : std_logic;
  signal n85_o : std_logic;
  signal n86_o : std_logic;
  signal n87_o : std_logic;
  signal n88_o : std_logic;
  signal n89_o : std_logic;
  signal n90_o : std_logic;
  signal n91_o : std_logic;
  signal n92_o : std_logic;
  signal n93_o : std_logic_vector (7 downto 0);
begin
  wrap_clk <= clk;
  wrap_rst <= rst;
  wrap_din <= din;
  dout <= wrap_dout;
  dout_vld <= wrap_dout_vld;
  wrap_DOUT <= n57_q;
  wrap_DOUT_VLD <= n58_q;
  -- uart_rx.vhd:20:12
  clk_cnt <= n54_q; -- (isignal)
  -- uart_rx.vhd:21:12
  bit_cnt <= n55_q; -- (isignal)
  -- uart_rx.vhd:22:12
  activate_clk_cnt <= fsm_c_activate_clk_cnt; -- (isignal)
  -- uart_rx.vhd:23:12
  activate_bit_cnt <= fsm_c_activate_bit_cnt; -- (isignal)
  -- uart_rx.vhd:24:12
  valid_data <= fsm_c_data_valid; -- (isignal)
  -- uart_rx.vhd:28:5
  fsm : entity work.uart_rx_fsm port map (
    clk => wrap_CLK,
    rst => wrap_RST,
    din => wrap_DIN,
    cnt_clk => clk_cnt,
    cnt_bit => bit_cnt,
    activate_clk_cnt => fsm_c_activate_clk_cnt,
    activate_bit_cnt => fsm_c_activate_bit_cnt,
    data_valid => fsm_c_data_valid);
  -- uart_rx.vhd:49:36
  n16_o <= std_logic_vector (unsigned (clk_cnt) + unsigned'("00001"));
  -- uart_rx.vhd:48:13
  n18_o <= "00010" when activate_clk_cnt = '0' else n16_o;
  -- uart_rx.vhd:54:24
  n20_o <= '1' when bit_cnt = "1000" else '0';
  -- uart_rx.vhd:54:33
  n21_o <= n20_o and valid_data;
  -- uart_rx.vhd:54:13
  n24_o <= '0' when n21_o = '0' else '1';
  -- uart_rx.vhd:54:13
  n27_o <= bit_cnt when n21_o = '0' else "0000";
  -- uart_rx.vhd:59:51
  n29_o <= '1' when unsigned (clk_cnt) >= unsigned'("01111") else '0';
  -- uart_rx.vhd:59:39
  n30_o <= activate_bit_cnt and n29_o;
  -- uart_rx.vhd:62:22
  n32_o <= bit_cnt (2 downto 0);  --  trunc
  -- uart_rx.vhd:63:36
  n36_o <= std_logic_vector (unsigned (bit_cnt) + unsigned'("0001"));
  -- uart_rx.vhd:59:13
  n39_o <= n18_o when n30_o = '0' else "00000";
  -- uart_rx.vhd:59:13
  n40_o <= n27_o when n30_o = '0' else n36_o;
  -- uart_rx.vhd:46:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n54_q <= "00000";
    elsif rising_edge (wrap_CLK) then
      n54_q <= n39_o;
    end if;
  end process;
  -- uart_rx.vhd:46:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n55_q <= "0000";
    elsif rising_edge (wrap_CLK) then
      n55_q <= n40_o;
    end if;
  end process;
  -- uart_rx.vhd:46:9
  n56_o <= n57_q when n30_o = '0' else n93_o;
  -- uart_rx.vhd:46:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n57_q <= "00000000";
    elsif rising_edge (wrap_CLK) then
      n57_q <= n56_o;
    end if;
  end process;
  -- uart_rx.vhd:46:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n58_q <= '0';
    elsif rising_edge (wrap_CLK) then
      n58_q <= n24_o;
    end if;
  end process;
  -- uart_rx.vhd:62:17
  n59_o <= n32_o (2);
  -- uart_rx.vhd:62:17
  n60_o <= not n59_o;
  -- uart_rx.vhd:62:17
  n61_o <= n32_o (1);
  -- uart_rx.vhd:62:17
  n62_o <= not n61_o;
  -- uart_rx.vhd:62:17
  n63_o <= n60_o and n62_o;
  -- uart_rx.vhd:62:17
  n64_o <= n60_o and n61_o;
  -- uart_rx.vhd:62:17
  n65_o <= n59_o and n62_o;
  -- uart_rx.vhd:62:17
  n66_o <= n59_o and n61_o;
  -- uart_rx.vhd:62:17
  n67_o <= n32_o (0);
  -- uart_rx.vhd:62:17
  n68_o <= not n67_o;
  -- uart_rx.vhd:62:17
  n69_o <= n63_o and n68_o;
  -- uart_rx.vhd:62:17
  n70_o <= n63_o and n67_o;
  -- uart_rx.vhd:62:17
  n71_o <= n64_o and n68_o;
  -- uart_rx.vhd:62:17
  n72_o <= n64_o and n67_o;
  -- uart_rx.vhd:62:17
  n73_o <= n65_o and n68_o;
  -- uart_rx.vhd:62:17
  n74_o <= n65_o and n67_o;
  -- uart_rx.vhd:62:17
  n75_o <= n66_o and n68_o;
  -- uart_rx.vhd:62:17
  n76_o <= n66_o and n67_o;
  n77_o <= n57_q (0);
  -- uart_rx.vhd:62:17
  n78_o <= n77_o when n69_o = '0' else wrap_DIN;
  n79_o <= n57_q (1);
  -- uart_rx.vhd:62:17
  n80_o <= n79_o when n70_o = '0' else wrap_DIN;
  n81_o <= n57_q (2);
  -- uart_rx.vhd:62:17
  n82_o <= n81_o when n71_o = '0' else wrap_DIN;
  n83_o <= n57_q (3);
  -- uart_rx.vhd:62:17
  n84_o <= n83_o when n72_o = '0' else wrap_DIN;
  n85_o <= n57_q (4);
  -- uart_rx.vhd:62:17
  n86_o <= n85_o when n73_o = '0' else wrap_DIN;
  n87_o <= n57_q (5);
  -- uart_rx.vhd:62:17
  n88_o <= n87_o when n74_o = '0' else wrap_DIN;
  n89_o <= n57_q (6);
  -- uart_rx.vhd:62:17
  n90_o <= n89_o when n75_o = '0' else wrap_DIN;
  n91_o <= n57_q (7);
  -- uart_rx.vhd:62:17
  n92_o <= n91_o when n76_o = '0' else wrap_DIN;
  n93_o <= n92_o & n90_o & n88_o & n86_o & n84_o & n82_o & n80_o & n78_o;
end rtl;

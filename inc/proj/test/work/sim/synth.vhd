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
  signal n120_o : std_logic;
  signal n121_o : std_logic;
  signal n123_o : std_logic_vector (2 downto 0);
  signal n125_o : std_logic;
  signal n127_o : std_logic;
  signal n129_o : std_logic_vector (2 downto 0);
  signal n131_o : std_logic;
  signal n133_o : std_logic;
  signal n135_o : std_logic_vector (2 downto 0);
  signal n137_o : std_logic;
  signal n139_o : std_logic_vector (2 downto 0);
  signal n141_o : std_logic;
  signal n143_o : std_logic_vector (2 downto 0);
  signal n144_o : std_logic_vector (2 downto 0);
  signal n145_o : std_logic_vector (2 downto 0);
  signal n146_o : std_logic_vector (2 downto 0);
  signal n147_o : std_logic_vector (2 downto 0);
  signal n152_q : std_logic_vector (2 downto 0) := "000";
begin
  activate_clk_cnt <= n109_o;
  activate_bit_cnt <= n114_o;
  data_valid <= n101_o;
  -- work/uart_rx_fsm.vhd:32:16
  curr_state <= n152_q; -- (isignal)
  -- work/uart_rx_fsm.vhd:34:51
  n100_o <= '1' when curr_state = "100" else '0';
  -- work/uart_rx_fsm.vhd:34:35
  n101_o <= '0' when n100_o = '0' else '1';
  -- work/uart_rx_fsm.vhd:35:57
  n105_o <= '1' when curr_state = "000" else '0';
  -- work/uart_rx_fsm.vhd:35:78
  n107_o <= '1' when curr_state = "100" else '0';
  -- work/uart_rx_fsm.vhd:35:64
  n108_o <= n105_o or n107_o;
  -- work/uart_rx_fsm.vhd:35:41
  n109_o <= '1' when n108_o = '0' else '0';
  -- work/uart_rx_fsm.vhd:36:57
  n113_o <= '1' when curr_state = "010" else '0';
  -- work/uart_rx_fsm.vhd:36:41
  n114_o <= '0' when n113_o = '0' else '1';
  -- work/uart_rx_fsm.vhd:41:39
  n120_o <= '1' when curr_state = "000" else '0';
  -- work/uart_rx_fsm.vhd:42:40
  n121_o <= not din;
  -- work/uart_rx_fsm.vhd:42:33
  n123_o <= curr_state when n121_o = '0' else "001";
  -- work/uart_rx_fsm.vhd:45:42
  n125_o <= '1' when curr_state = "001" else '0';
  -- work/uart_rx_fsm.vhd:46:44
  n127_o <= '1' when unsigned (cnt_clk) >= unsigned'("11000") else '0';
  -- work/uart_rx_fsm.vhd:46:33
  n129_o <= curr_state when n127_o = '0' else "010";
  -- work/uart_rx_fsm.vhd:49:42
  n131_o <= '1' when curr_state = "010" else '0';
  -- work/uart_rx_fsm.vhd:50:44
  n133_o <= '1' when unsigned (cnt_bit) >= unsigned'("1000") else '0';
  -- work/uart_rx_fsm.vhd:50:33
  n135_o <= curr_state when n133_o = '0' else "011";
  -- work/uart_rx_fsm.vhd:53:42
  n137_o <= '1' when curr_state = "011" else '0';
  -- work/uart_rx_fsm.vhd:54:33
  n139_o <= curr_state when din = '0' else "100";
  -- work/uart_rx_fsm.vhd:57:42
  n141_o <= '1' when curr_state = "100" else '0';
  -- work/uart_rx_fsm.vhd:57:25
  n143_o <= curr_state when n141_o = '0' else "000";
  -- work/uart_rx_fsm.vhd:53:25
  n144_o <= n143_o when n137_o = '0' else n139_o;
  -- work/uart_rx_fsm.vhd:49:25
  n145_o <= n144_o when n131_o = '0' else n135_o;
  -- work/uart_rx_fsm.vhd:45:25
  n146_o <= n145_o when n125_o = '0' else n129_o;
  -- work/uart_rx_fsm.vhd:41:25
  n147_o <= n146_o when n120_o = '0' else n123_o;
  -- work/uart_rx_fsm.vhd:40:17
  process (clk, rst)
  begin
    if rst = '1' then
      n152_q <= "000";
    elsif rising_edge (clk) then
      n152_q <= n147_o;
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
  -- work/uart_rx.vhd:20:12
  clk_cnt <= n54_q; -- (isignal)
  -- work/uart_rx.vhd:21:12
  bit_cnt <= n55_q; -- (isignal)
  -- work/uart_rx.vhd:22:12
  activate_clk_cnt <= fsm_c_activate_clk_cnt; -- (isignal)
  -- work/uart_rx.vhd:23:12
  activate_bit_cnt <= fsm_c_activate_bit_cnt; -- (isignal)
  -- work/uart_rx.vhd:24:12
  valid_data <= fsm_c_data_valid; -- (isignal)
  -- work/uart_rx.vhd:28:5
  fsm : entity work.uart_rx_fsm port map (
    clk => wrap_CLK,
    rst => wrap_RST,
    din => wrap_DIN,
    cnt_clk => clk_cnt,
    cnt_bit => bit_cnt,
    activate_clk_cnt => fsm_c_activate_clk_cnt,
    activate_bit_cnt => fsm_c_activate_bit_cnt,
    data_valid => fsm_c_data_valid);
  -- work/uart_rx.vhd:49:36
  n16_o <= std_logic_vector (unsigned (clk_cnt) + unsigned'("00001"));
  -- work/uart_rx.vhd:48:13
  n18_o <= "00010" when activate_clk_cnt = '0' else n16_o;
  -- work/uart_rx.vhd:54:28
  n20_o <= '1' when bit_cnt = "1000" else '0';
  -- work/uart_rx.vhd:54:37
  n21_o <= n20_o and valid_data;
  -- work/uart_rx.vhd:54:17
  n24_o <= '0' when n21_o = '0' else '1';
  -- work/uart_rx.vhd:54:17
  n27_o <= bit_cnt when n21_o = '0' else "0000";
  -- work/uart_rx.vhd:59:55
  n29_o <= '1' when unsigned (clk_cnt) >= unsigned'("01111") else '0';
  -- work/uart_rx.vhd:59:43
  n30_o <= activate_bit_cnt and n29_o;
  -- work/uart_rx.vhd:62:30
  n32_o <= bit_cnt (2 downto 0);  --  trunc
  -- work/uart_rx.vhd:63:44
  n36_o <= std_logic_vector (unsigned (bit_cnt) + unsigned'("0001"));
  -- work/uart_rx.vhd:59:17
  n39_o <= n18_o when n30_o = '0' else "00000";
  -- work/uart_rx.vhd:59:17
  n40_o <= n27_o when n30_o = '0' else n36_o;
  -- work/uart_rx.vhd:46:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n54_q <= "00000";
    elsif rising_edge (wrap_CLK) then
      n54_q <= n39_o;
    end if;
  end process;
  -- work/uart_rx.vhd:46:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n55_q <= "0000";
    elsif rising_edge (wrap_CLK) then
      n55_q <= n40_o;
    end if;
  end process;
  -- work/uart_rx.vhd:46:9
  n56_o <= n57_q when n30_o = '0' else n93_o;
  -- work/uart_rx.vhd:46:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n57_q <= "00000000";
    elsif rising_edge (wrap_CLK) then
      n57_q <= n56_o;
    end if;
  end process;
  -- work/uart_rx.vhd:46:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n58_q <= '0';
    elsif rising_edge (wrap_CLK) then
      n58_q <= n24_o;
    end if;
  end process;
  -- work/uart_rx.vhd:62:25
  n59_o <= n32_o (2);
  -- work/uart_rx.vhd:62:25
  n60_o <= not n59_o;
  -- work/uart_rx.vhd:62:25
  n61_o <= n32_o (1);
  -- work/uart_rx.vhd:62:25
  n62_o <= not n61_o;
  -- work/uart_rx.vhd:62:25
  n63_o <= n60_o and n62_o;
  -- work/uart_rx.vhd:62:25
  n64_o <= n60_o and n61_o;
  -- work/uart_rx.vhd:62:25
  n65_o <= n59_o and n62_o;
  -- work/uart_rx.vhd:62:25
  n66_o <= n59_o and n61_o;
  -- work/uart_rx.vhd:62:25
  n67_o <= n32_o (0);
  -- work/uart_rx.vhd:62:25
  n68_o <= not n67_o;
  -- work/uart_rx.vhd:62:25
  n69_o <= n63_o and n68_o;
  -- work/uart_rx.vhd:62:25
  n70_o <= n63_o and n67_o;
  -- work/uart_rx.vhd:62:25
  n71_o <= n64_o and n68_o;
  -- work/uart_rx.vhd:62:25
  n72_o <= n64_o and n67_o;
  -- work/uart_rx.vhd:62:25
  n73_o <= n65_o and n68_o;
  -- work/uart_rx.vhd:62:25
  n74_o <= n65_o and n67_o;
  -- work/uart_rx.vhd:62:25
  n75_o <= n66_o and n68_o;
  -- work/uart_rx.vhd:62:25
  n76_o <= n66_o and n67_o;
  n77_o <= n57_q (0);
  -- work/uart_rx.vhd:62:25
  n78_o <= n77_o when n69_o = '0' else wrap_DIN;
  n79_o <= n57_q (1);
  -- work/uart_rx.vhd:62:25
  n80_o <= n79_o when n70_o = '0' else wrap_DIN;
  n81_o <= n57_q (2);
  -- work/uart_rx.vhd:62:25
  n82_o <= n81_o when n71_o = '0' else wrap_DIN;
  n83_o <= n57_q (3);
  -- work/uart_rx.vhd:62:25
  n84_o <= n83_o when n72_o = '0' else wrap_DIN;
  n85_o <= n57_q (4);
  -- work/uart_rx.vhd:62:25
  n86_o <= n85_o when n73_o = '0' else wrap_DIN;
  n87_o <= n57_q (5);
  -- work/uart_rx.vhd:62:25
  n88_o <= n87_o when n74_o = '0' else wrap_DIN;
  n89_o <= n57_q (6);
  -- work/uart_rx.vhd:62:25
  n90_o <= n89_o when n75_o = '0' else wrap_DIN;
  n91_o <= n57_q (7);
  -- work/uart_rx.vhd:62:25
  n92_o <= n91_o when n76_o = '0' else wrap_DIN;
  n93_o <= n92_o & n90_o & n88_o & n86_o & n84_o & n82_o & n80_o & n78_o;
end rtl;

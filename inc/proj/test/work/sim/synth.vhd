library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
entity UART_RX is
  port (
    CLK: in std_logic;
    RST: in std_logic;
    DIN: in std_logic;
    DOUT: out std_logic_vector (7 downto 0);
    DOUT_VLD: out std_logic
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
  signal n98_o : std_logic;
  signal n99_o : std_logic;
  signal n103_o : std_logic;
  signal n105_o : std_logic;
  signal n106_o : std_logic;
  signal n107_o : std_logic;
  signal n111_o : std_logic;
  signal n112_o : std_logic;
  signal n117_o : std_logic;
  signal n119_o : std_logic_vector (2 downto 0);
  signal n121_o : std_logic;
  signal n123_o : std_logic;
  signal n125_o : std_logic_vector (2 downto 0);
  signal n127_o : std_logic;
  signal n129_o : std_logic;
  signal n131_o : std_logic_vector (2 downto 0);
  signal n133_o : std_logic;
  signal n135_o : std_logic_vector (2 downto 0);
  signal n137_o : std_logic;
  signal n139_o : std_logic;
  signal n140_o : std_logic_vector (4 downto 0);
  signal n143_o : std_logic_vector (2 downto 0);
  signal n148_q : std_logic_vector (2 downto 0) := "000";
begin
  activate_clk_cnt <= n107_o;
  activate_bit_cnt <= n112_o;
  data_valid <= n99_o;
  -- work/uart_rx_fsm.vhd:33:8
  curr_state <= n148_q; -- (isignal)
  -- work/uart_rx_fsm.vhd:35:35
  n98_o <= '1' when curr_state = "100" else '0';
  -- work/uart_rx_fsm.vhd:35:19
  n99_o <= '0' when n98_o = '0' else '1';
  -- work/uart_rx_fsm.vhd:36:41
  n103_o <= '1' when curr_state = "000" else '0';
  -- work/uart_rx_fsm.vhd:36:62
  n105_o <= '1' when curr_state = "100" else '0';
  -- work/uart_rx_fsm.vhd:36:48
  n106_o <= n103_o or n105_o;
  -- work/uart_rx_fsm.vhd:36:25
  n107_o <= '1' when n106_o = '0' else '0';
  -- work/uart_rx_fsm.vhd:37:41
  n111_o <= '1' when curr_state = "010" else '0';
  -- work/uart_rx_fsm.vhd:37:25
  n112_o <= '0' when n111_o = '0' else '1';
  -- work/uart_rx_fsm.vhd:44:40
  n117_o <= not din;
  -- work/uart_rx_fsm.vhd:44:33
  n119_o <= curr_state when n117_o = '0' else "001";
  -- work/uart_rx_fsm.vhd:43:25
  n121_o <= '1' when curr_state = "000" else '0';
  -- work/uart_rx_fsm.vhd:48:44
  n123_o <= '1' when unsigned (cnt_clk) >= unsigned'("11000") else '0';
  -- work/uart_rx_fsm.vhd:48:33
  n125_o <= curr_state when n123_o = '0' else "010";
  -- work/uart_rx_fsm.vhd:47:25
  n127_o <= '1' when curr_state = "001" else '0';
  -- work/uart_rx_fsm.vhd:52:44
  n129_o <= '1' when unsigned (cnt_bit) >= unsigned'("1000") else '0';
  -- work/uart_rx_fsm.vhd:52:33
  n131_o <= curr_state when n129_o = '0' else "011";
  -- work/uart_rx_fsm.vhd:51:25
  n133_o <= '1' when curr_state = "010" else '0';
  -- work/uart_rx_fsm.vhd:56:33
  n135_o <= curr_state when din = '0' else "100";
  -- work/uart_rx_fsm.vhd:55:25
  n137_o <= '1' when curr_state = "011" else '0';
  -- work/uart_rx_fsm.vhd:59:25
  n139_o <= '1' when curr_state = "100" else '0';
  n140_o <= n139_o & n137_o & n133_o & n127_o & n121_o;
  -- work/uart_rx_fsm.vhd:42:17
  with n140_o select n143_o <=
    "000" when "10000",
    n135_o when "01000",
    n131_o when "00100",
    n125_o when "00010",
    n119_o when "00001",
    "XXX" when others;
  -- work/uart_rx_fsm.vhd:41:9
  process (clk, rst)
  begin
    if rst = '1' then
      n148_q <= "000";
    elsif rising_edge (clk) then
      n148_q <= n143_o;
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
  signal n14_o : std_logic_vector (4 downto 0);
  signal n16_o : std_logic_vector (4 downto 0);
  signal n18_o : std_logic;
  signal n19_o : std_logic;
  signal n22_o : std_logic;
  signal n25_o : std_logic_vector (3 downto 0);
  signal n27_o : std_logic;
  signal n28_o : std_logic;
  signal n30_o : std_logic_vector (2 downto 0);
  signal n34_o : std_logic_vector (3 downto 0);
  signal n37_o : std_logic_vector (4 downto 0);
  signal n38_o : std_logic_vector (3 downto 0);
  signal n52_q : std_logic_vector (4 downto 0) := "00000";
  signal n53_q : std_logic_vector (3 downto 0) := "0000";
  signal n54_o : std_logic_vector (7 downto 0);
  signal n55_q : std_logic_vector (7 downto 0);
  signal n56_q : std_logic;
  signal n57_o : std_logic;
  signal n58_o : std_logic;
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
  signal n91_o : std_logic_vector (7 downto 0);
begin
  wrap_clk <= clk;
  wrap_rst <= rst;
  wrap_din <= din;
  dout <= wrap_dout;
  dout_vld <= wrap_dout_vld;
  wrap_DOUT <= n55_q;
  wrap_DOUT_VLD <= n56_q;
  -- work/uart_rx.vhd:18:12
  clk_cnt <= n52_q; -- (isignal)
  -- work/uart_rx.vhd:19:12
  bit_cnt <= n53_q; -- (isignal)
  -- work/uart_rx.vhd:20:12
  activate_clk_cnt <= fsm_c_activate_clk_cnt; -- (isignal)
  -- work/uart_rx.vhd:21:12
  activate_bit_cnt <= fsm_c_activate_bit_cnt; -- (isignal)
  -- work/uart_rx.vhd:22:12
  valid_data <= fsm_c_data_valid; -- (isignal)
  -- work/uart_rx.vhd:26:5
  fsm : entity work.uart_rx_fsm port map (
    clk => wrap_CLK,
    rst => wrap_RST,
    din => wrap_DIN,
    cnt_clk => clk_cnt,
    cnt_bit => bit_cnt,
    activate_clk_cnt => fsm_c_activate_clk_cnt,
    activate_bit_cnt => fsm_c_activate_bit_cnt,
    data_valid => fsm_c_data_valid);
  -- work/uart_rx.vhd:47:36
  n14_o <= std_logic_vector (unsigned (clk_cnt) + unsigned'("00001"));
  -- work/uart_rx.vhd:46:13
  n16_o <= "00010" when activate_clk_cnt = '0' else n14_o;
  -- work/uart_rx.vhd:52:24
  n18_o <= '1' when bit_cnt = "1000" else '0';
  -- work/uart_rx.vhd:52:33
  n19_o <= n18_o and valid_data;
  -- work/uart_rx.vhd:52:13
  n22_o <= '0' when n19_o = '0' else '1';
  -- work/uart_rx.vhd:52:13
  n25_o <= bit_cnt when n19_o = '0' else "0000";
  -- work/uart_rx.vhd:57:51
  n27_o <= '1' when unsigned (clk_cnt) >= unsigned'("01111") else '0';
  -- work/uart_rx.vhd:57:39
  n28_o <= activate_bit_cnt and n27_o;
  -- work/uart_rx.vhd:60:22
  n30_o <= bit_cnt (2 downto 0);  --  trunc
  -- work/uart_rx.vhd:61:36
  n34_o <= std_logic_vector (unsigned (bit_cnt) + unsigned'("0001"));
  -- work/uart_rx.vhd:57:13
  n37_o <= n16_o when n28_o = '0' else "00000";
  -- work/uart_rx.vhd:57:13
  n38_o <= n25_o when n28_o = '0' else n34_o;
  -- work/uart_rx.vhd:44:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n52_q <= "00000";
    elsif rising_edge (wrap_CLK) then
      n52_q <= n37_o;
    end if;
  end process;
  -- work/uart_rx.vhd:44:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n53_q <= "0000";
    elsif rising_edge (wrap_CLK) then
      n53_q <= n38_o;
    end if;
  end process;
  -- work/uart_rx.vhd:44:9
  n54_o <= n55_q when n28_o = '0' else n91_o;
  -- work/uart_rx.vhd:44:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n55_q <= "00000000";
    elsif rising_edge (wrap_CLK) then
      n55_q <= n54_o;
    end if;
  end process;
  -- work/uart_rx.vhd:44:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n56_q <= '0';
    elsif rising_edge (wrap_CLK) then
      n56_q <= n22_o;
    end if;
  end process;
  -- work/uart_rx.vhd:60:17
  n57_o <= n30_o (2);
  -- work/uart_rx.vhd:60:17
  n58_o <= not n57_o;
  -- work/uart_rx.vhd:60:17
  n59_o <= n30_o (1);
  -- work/uart_rx.vhd:60:17
  n60_o <= not n59_o;
  -- work/uart_rx.vhd:60:17
  n61_o <= n58_o and n60_o;
  -- work/uart_rx.vhd:60:17
  n62_o <= n58_o and n59_o;
  -- work/uart_rx.vhd:60:17
  n63_o <= n57_o and n60_o;
  -- work/uart_rx.vhd:60:17
  n64_o <= n57_o and n59_o;
  -- work/uart_rx.vhd:60:17
  n65_o <= n30_o (0);
  -- work/uart_rx.vhd:60:17
  n66_o <= not n65_o;
  -- work/uart_rx.vhd:60:17
  n67_o <= n61_o and n66_o;
  -- work/uart_rx.vhd:60:17
  n68_o <= n61_o and n65_o;
  -- work/uart_rx.vhd:60:17
  n69_o <= n62_o and n66_o;
  -- work/uart_rx.vhd:60:17
  n70_o <= n62_o and n65_o;
  -- work/uart_rx.vhd:60:17
  n71_o <= n63_o and n66_o;
  -- work/uart_rx.vhd:60:17
  n72_o <= n63_o and n65_o;
  -- work/uart_rx.vhd:60:17
  n73_o <= n64_o and n66_o;
  -- work/uart_rx.vhd:60:17
  n74_o <= n64_o and n65_o;
  n75_o <= n55_q (0);
  -- work/uart_rx.vhd:60:17
  n76_o <= n75_o when n67_o = '0' else wrap_DIN;
  n77_o <= n55_q (1);
  -- work/uart_rx.vhd:60:17
  n78_o <= n77_o when n68_o = '0' else wrap_DIN;
  n79_o <= n55_q (2);
  -- work/uart_rx.vhd:60:17
  n80_o <= n79_o when n69_o = '0' else wrap_DIN;
  n81_o <= n55_q (3);
  -- work/uart_rx.vhd:60:17
  n82_o <= n81_o when n70_o = '0' else wrap_DIN;
  n83_o <= n55_q (4);
  -- work/uart_rx.vhd:60:17
  n84_o <= n83_o when n71_o = '0' else wrap_DIN;
  n85_o <= n55_q (5);
  -- work/uart_rx.vhd:60:17
  n86_o <= n85_o when n72_o = '0' else wrap_DIN;
  n87_o <= n55_q (6);
  -- work/uart_rx.vhd:60:17
  n88_o <= n87_o when n73_o = '0' else wrap_DIN;
  n89_o <= n55_q (7);
  -- work/uart_rx.vhd:60:17
  n90_o <= n89_o when n74_o = '0' else wrap_DIN;
  n91_o <= n90_o & n88_o & n86_o & n84_o & n82_o & n80_o & n78_o & n76_o;
end rtl;

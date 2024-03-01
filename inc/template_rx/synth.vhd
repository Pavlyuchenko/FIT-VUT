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
  n125_o <= '1' when cnt_clk = "11000" else '0';
  -- uart_rx_fsm.vhd:48:33
  n127_o <= curr_state when n125_o = '0' else "010";
  -- uart_rx_fsm.vhd:47:25
  n129_o <= '1' when curr_state = "001" else '0';
  -- uart_rx_fsm.vhd:52:44
  n131_o <= '1' when cnt_bit = "1000" else '0';
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
  signal n14_o : std_logic_vector (4 downto 0);
  signal n16_o : std_logic_vector (4 downto 0);
  signal n18_o : std_logic;
  signal n19_o : std_logic;
  signal n22_o : std_logic;
  signal n25_o : std_logic_vector (3 downto 0);
  signal n27_o : std_logic;
  signal n28_o : std_logic;
  signal n30_o : std_logic;
  signal n32_o : std_logic;
  signal n34_o : std_logic;
  signal n36_o : std_logic;
  signal n38_o : std_logic;
  signal n40_o : std_logic;
  signal n42_o : std_logic;
  signal n44_o : std_logic;
  signal n45_o : std_logic_vector (7 downto 0);
  signal n46_o : std_logic;
  signal n47_o : std_logic;
  signal n48_o : std_logic;
  signal n49_o : std_logic;
  signal n50_o : std_logic;
  signal n51_o : std_logic;
  signal n52_o : std_logic;
  signal n53_o : std_logic;
  signal n54_o : std_logic;
  signal n55_o : std_logic;
  signal n56_o : std_logic;
  signal n57_o : std_logic;
  signal n58_o : std_logic;
  signal n59_o : std_logic;
  signal n60_o : std_logic;
  signal n61_o : std_logic;
  signal n70_o : std_logic_vector (3 downto 0);
  signal n71_o : std_logic_vector (7 downto 0);
  signal n74_o : std_logic_vector (4 downto 0);
  signal n75_o : std_logic_vector (3 downto 0);
  signal n89_q : std_logic_vector (4 downto 0) := "00000";
  signal n90_q : std_logic_vector (3 downto 0) := "0000";
  signal n91_o : std_logic_vector (7 downto 0);
  signal n92_q : std_logic_vector (7 downto 0);
  signal n93_q : std_logic;
begin
  wrap_clk <= clk;
  wrap_rst <= rst;
  wrap_din <= din;
  dout <= wrap_dout;
  dout_vld <= wrap_dout_vld;
  wrap_DOUT <= n92_q;
  wrap_DOUT_VLD <= n93_q;
  -- uart_rx.vhd:18:12
  clk_cnt <= n89_q; -- (isignal)
  -- uart_rx.vhd:19:12
  bit_cnt <= n90_q; -- (isignal)
  -- uart_rx.vhd:20:12
  activate_clk_cnt <= fsm_c_activate_clk_cnt; -- (isignal)
  -- uart_rx.vhd:21:12
  activate_bit_cnt <= fsm_c_activate_bit_cnt; -- (isignal)
  -- uart_rx.vhd:22:12
  valid_data <= fsm_c_data_valid; -- (isignal)
  -- uart_rx.vhd:26:5
  fsm : entity work.uart_rx_fsm port map (
    clk => wrap_CLK,
    rst => wrap_RST,
    din => wrap_DIN,
    cnt_clk => clk_cnt,
    cnt_bit => bit_cnt,
    activate_clk_cnt => fsm_c_activate_clk_cnt,
    activate_bit_cnt => fsm_c_activate_bit_cnt,
    data_valid => fsm_c_data_valid);
  -- uart_rx.vhd:47:36
  n14_o <= std_logic_vector (unsigned (clk_cnt) + unsigned'("00001"));
  -- uart_rx.vhd:46:13
  n16_o <= "00001" when activate_clk_cnt = '0' else n14_o;
  -- uart_rx.vhd:52:24
  n18_o <= '1' when bit_cnt = "1000" else '0';
  -- uart_rx.vhd:52:33
  n19_o <= n18_o and valid_data;
  -- uart_rx.vhd:52:13
  n22_o <= '0' when n19_o = '0' else '1';
  -- uart_rx.vhd:52:13
  n25_o <= bit_cnt when n19_o = '0' else "0000";
  -- uart_rx.vhd:57:51
  n27_o <= '1' when unsigned (clk_cnt) >= unsigned'("10000") else '0';
  -- uart_rx.vhd:57:39
  n28_o <= activate_bit_cnt and n27_o;
  -- uart_rx.vhd:61:21
  n30_o <= '1' when bit_cnt = "0000" else '0';
  -- uart_rx.vhd:64:21
  n32_o <= '1' when bit_cnt = "0001" else '0';
  -- uart_rx.vhd:67:21
  n34_o <= '1' when bit_cnt = "0010" else '0';
  -- uart_rx.vhd:70:21
  n36_o <= '1' when bit_cnt = "0011" else '0';
  -- uart_rx.vhd:73:21
  n38_o <= '1' when bit_cnt = "0100" else '0';
  -- uart_rx.vhd:76:21
  n40_o <= '1' when bit_cnt = "0101" else '0';
  -- uart_rx.vhd:79:21
  n42_o <= '1' when bit_cnt = "0110" else '0';
  -- uart_rx.vhd:82:21
  n44_o <= '1' when bit_cnt = "0111" else '0';
  n45_o <= n44_o & n42_o & n40_o & n38_o & n36_o & n34_o & n32_o & n30_o;
  n46_o <= n92_q (0);
  -- uart_rx.vhd:60:17
  with n45_o select n47_o <=
    n46_o when "10000000",
    n46_o when "01000000",
    n46_o when "00100000",
    n46_o when "00010000",
    n46_o when "00001000",
    n46_o when "00000100",
    n46_o when "00000010",
    wrap_DIN when "00000001",
    n46_o when others;
  n48_o <= n92_q (1);
  -- uart_rx.vhd:60:17
  with n45_o select n49_o <=
    n48_o when "10000000",
    n48_o when "01000000",
    n48_o when "00100000",
    n48_o when "00010000",
    n48_o when "00001000",
    n48_o when "00000100",
    wrap_DIN when "00000010",
    n48_o when "00000001",
    n48_o when others;
  n50_o <= n92_q (2);
  -- uart_rx.vhd:60:17
  with n45_o select n51_o <=
    n50_o when "10000000",
    n50_o when "01000000",
    n50_o when "00100000",
    n50_o when "00010000",
    n50_o when "00001000",
    wrap_DIN when "00000100",
    n50_o when "00000010",
    n50_o when "00000001",
    n50_o when others;
  n52_o <= n92_q (3);
  -- uart_rx.vhd:60:17
  with n45_o select n53_o <=
    n52_o when "10000000",
    n52_o when "01000000",
    n52_o when "00100000",
    n52_o when "00010000",
    wrap_DIN when "00001000",
    n52_o when "00000100",
    n52_o when "00000010",
    n52_o when "00000001",
    n52_o when others;
  n54_o <= n92_q (4);
  -- uart_rx.vhd:60:17
  with n45_o select n55_o <=
    n54_o when "10000000",
    n54_o when "01000000",
    n54_o when "00100000",
    wrap_DIN when "00010000",
    n54_o when "00001000",
    n54_o when "00000100",
    n54_o when "00000010",
    n54_o when "00000001",
    n54_o when others;
  n56_o <= n92_q (5);
  -- uart_rx.vhd:60:17
  with n45_o select n57_o <=
    n56_o when "10000000",
    n56_o when "01000000",
    wrap_DIN when "00100000",
    n56_o when "00010000",
    n56_o when "00001000",
    n56_o when "00000100",
    n56_o when "00000010",
    n56_o when "00000001",
    n56_o when others;
  n58_o <= n92_q (6);
  -- uart_rx.vhd:60:17
  with n45_o select n59_o <=
    n58_o when "10000000",
    wrap_DIN when "01000000",
    n58_o when "00100000",
    n58_o when "00010000",
    n58_o when "00001000",
    n58_o when "00000100",
    n58_o when "00000010",
    n58_o when "00000001",
    n58_o when others;
  n60_o <= n92_q (7);
  -- uart_rx.vhd:60:17
  with n45_o select n61_o <=
    wrap_DIN when "10000000",
    n60_o when "01000000",
    n60_o when "00100000",
    n60_o when "00010000",
    n60_o when "00001000",
    n60_o when "00000100",
    n60_o when "00000010",
    n60_o when "00000001",
    n60_o when others;
  -- uart_rx.vhd:60:17
  with n45_o select n70_o <=
    "1000" when "10000000",
    "0111" when "01000000",
    "0110" when "00100000",
    "0101" when "00010000",
    "0100" when "00001000",
    "0011" when "00000100",
    "0010" when "00000010",
    "0001" when "00000001",
    n25_o when others;
  n71_o <= n61_o & n59_o & n57_o & n55_o & n53_o & n51_o & n49_o & n47_o;
  -- uart_rx.vhd:57:13
  n74_o <= n16_o when n28_o = '0' else "00001";
  -- uart_rx.vhd:57:13
  n75_o <= n25_o when n28_o = '0' else n70_o;
  -- uart_rx.vhd:44:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n89_q <= "00001";
    elsif rising_edge (wrap_CLK) then
      n89_q <= n74_o;
    end if;
  end process;
  -- uart_rx.vhd:44:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n90_q <= "0000";
    elsif rising_edge (wrap_CLK) then
      n90_q <= n75_o;
    end if;
  end process;
  -- uart_rx.vhd:44:9
  n91_o <= n92_q when n28_o = '0' else n71_o;
  -- uart_rx.vhd:44:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n92_q <= "00000000";
    elsif rising_edge (wrap_CLK) then
      n92_q <= n91_o;
    end if;
  end process;
  -- uart_rx.vhd:44:9
  process (wrap_CLK, wrap_RST)
  begin
    if wrap_RST = '1' then
      n93_q <= '0';
    elsif rising_edge (wrap_CLK) then
      n93_q <= n22_o;
    end if;
  end process;
end rtl;

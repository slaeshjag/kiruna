<?xml version="1.0" encoding="UTF-8"?>
<drawing version="7">
    <attr value="xc9500xl" name="DeviceFamilyName">
        <trait delete="all:0" />
        <trait editname="all:0" />
        <trait edittrait="all:0" />
    </attr>
    <netlist>
        <signal name="CAMERA_DATA(7:0)" />
        <signal name="XLXN_6(7:0)" />
        <signal name="XLXN_8" />
        <signal name="shift_out(7:0)" />
        <signal name="shift_out(7)" />
        <signal name="nSLAVE_SELECT" />
        <signal name="XLXN_38" />
        <signal name="READ_STROBE" />
        <signal name="MOSI" />
        <signal name="SCK" />
        <signal name="XLXN_56" />
        <signal name="MOSI_P(7:0)" />
        <signal name="MOSI_P(0)" />
        <signal name="MOSI_P(1)" />
        <signal name="MOSI_P(2)" />
        <signal name="MOSI_P(3)" />
        <signal name="MOSI_P(4)" />
        <signal name="MOSI_P(5)" />
        <signal name="MOSI_P(6)" />
        <signal name="MOSI_P(7)" />
        <signal name="MISO" />
        <signal name="XLXN_69" />
        <signal name="FIFO_RESET" />
        <signal name="XLXN_85" />
        <signal name="XLXN_86" />
        <signal name="XLXN_90" />
        <signal name="XLXN_94" />
        <signal name="XLXN_99" />
        <signal name="XLXN_105" />
        <signal name="XLXN_106" />
        <signal name="XLXN_107" />
        <port polarity="Input" name="CAMERA_DATA(7:0)" />
        <port polarity="Input" name="nSLAVE_SELECT" />
        <port polarity="Output" name="READ_STROBE" />
        <port polarity="Input" name="MOSI" />
        <port polarity="Input" name="SCK" />
        <port polarity="Output" name="MISO" />
        <port polarity="Output" name="FIFO_RESET" />
        <blockdef name="ibuf8">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <rect width="96" x="128" y="-44" height="24" />
            <line x2="128" y1="-32" y2="-32" x1="224" />
            <rect width="64" x="0" y="-44" height="24" />
            <line x2="64" y1="0" y2="-64" x1="64" />
            <line x2="64" y1="-32" y2="0" x1="128" />
            <line x2="128" y1="-64" y2="-32" x1="64" />
            <line x2="64" y1="-32" y2="-32" x1="0" />
        </blockdef>
        <blockdef name="sr8cle">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <line x2="320" y1="-384" y2="-384" x1="384" />
            <line x2="192" y1="-64" y2="-32" x1="192" />
            <line x2="64" y1="-32" y2="-32" x1="192" />
            <line x2="80" y1="-112" y2="-128" x1="64" />
            <line x2="64" y1="-128" y2="-144" x1="80" />
            <line x2="64" y1="-32" y2="-32" x1="0" />
            <line x2="64" y1="-128" y2="-128" x1="0" />
            <line x2="64" y1="-192" y2="-192" x1="0" />
            <line x2="64" y1="-320" y2="-320" x1="0" />
            <rect width="64" x="320" y="-396" height="24" />
            <line x2="64" y1="-576" y2="-576" x1="0" />
            <rect width="64" x="0" y="-524" height="24" />
            <line x2="64" y1="-512" y2="-512" x1="0" />
            <rect width="256" x="64" y="-640" height="576" />
        </blockdef>
        <blockdef name="gnd">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <line x2="64" y1="-128" y2="-96" x1="64" />
            <line x2="64" y1="-64" y2="-80" x1="64" />
            <line x2="40" y1="-64" y2="-64" x1="88" />
            <line x2="60" y1="-32" y2="-32" x1="68" />
            <line x2="52" y1="-48" y2="-48" x1="76" />
            <line x2="64" y1="-64" y2="-96" x1="64" />
        </blockdef>
        <blockdef name="obufe">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <line x2="64" y1="-96" y2="-96" x1="0" />
            <line x2="64" y1="-32" y2="-32" x1="0" />
            <line x2="128" y1="-32" y2="-32" x1="224" />
            <line x2="64" y1="-96" y2="-96" x1="96" />
            <line x2="96" y1="-48" y2="-96" x1="96" />
            <line x2="64" y1="-64" y2="0" x1="64" />
            <line x2="64" y1="-32" y2="-64" x1="128" />
            <line x2="128" y1="0" y2="-32" x1="64" />
        </blockdef>
        <blockdef name="ibuf">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <line x2="64" y1="0" y2="-64" x1="64" />
            <line x2="64" y1="-32" y2="0" x1="128" />
            <line x2="128" y1="-64" y2="-32" x1="64" />
            <line x2="128" y1="-32" y2="-32" x1="224" />
            <line x2="64" y1="-32" y2="-32" x1="0" />
        </blockdef>
        <blockdef name="sr8ce">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <rect width="256" x="64" y="-384" height="320" />
            <line x2="64" y1="-320" y2="-320" x1="0" />
            <line x2="64" y1="-32" y2="-32" x1="0" />
            <line x2="192" y1="-64" y2="-32" x1="192" />
            <line x2="64" y1="-32" y2="-32" x1="192" />
            <line x2="64" y1="-128" y2="-128" x1="0" />
            <line x2="320" y1="-256" y2="-256" x1="384" />
            <line x2="80" y1="-112" y2="-128" x1="64" />
            <line x2="64" y1="-128" y2="-144" x1="80" />
            <line x2="64" y1="-192" y2="-192" x1="0" />
            <rect width="64" x="320" y="-268" height="24" />
        </blockdef>
        <blockdef name="inv">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <line x2="64" y1="-32" y2="-32" x1="0" />
            <line x2="160" y1="-32" y2="-32" x1="224" />
            <line x2="128" y1="-64" y2="-32" x1="64" />
            <line x2="64" y1="-32" y2="0" x1="128" />
            <line x2="64" y1="0" y2="-64" x1="64" />
            <circle r="16" cx="144" cy="-32" />
        </blockdef>
        <blockdef name="obuf">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <line x2="64" y1="0" y2="-64" x1="64" />
            <line x2="64" y1="-32" y2="0" x1="128" />
            <line x2="128" y1="-64" y2="-32" x1="64" />
            <line x2="64" y1="-32" y2="-32" x1="0" />
            <line x2="128" y1="-32" y2="-32" x1="224" />
        </blockdef>
        <blockdef name="m2_1">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <line x2="96" y1="-64" y2="-192" x1="96" />
            <line x2="96" y1="-96" y2="-64" x1="256" />
            <line x2="256" y1="-160" y2="-96" x1="256" />
            <line x2="256" y1="-192" y2="-160" x1="96" />
            <line x2="96" y1="-32" y2="-32" x1="176" />
            <line x2="176" y1="-80" y2="-32" x1="176" />
            <line x2="96" y1="-32" y2="-32" x1="0" />
            <line x2="256" y1="-128" y2="-128" x1="320" />
            <line x2="96" y1="-96" y2="-96" x1="0" />
            <line x2="96" y1="-160" y2="-160" x1="0" />
        </blockdef>
        <blockdef name="nor8">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <arc ex="48" ey="-336" sx="48" sy="-240" r="56" cx="16" cy="-288" />
            <line x2="64" y1="-336" y2="-336" x1="128" />
            <line x2="64" y1="-240" y2="-240" x1="128" />
            <arc ex="208" ey="-288" sx="128" sy="-240" r="88" cx="132" cy="-328" />
            <arc ex="128" ey="-336" sx="208" sy="-288" r="88" cx="132" cy="-248" />
            <line x2="228" y1="-288" y2="-288" x1="256" />
            <circle r="10" cx="218" cy="-286" />
            <line x2="48" y1="-64" y2="-64" x1="0" />
            <line x2="48" y1="-128" y2="-128" x1="0" />
            <line x2="48" y1="-192" y2="-192" x1="0" />
            <line x2="48" y1="-384" y2="-384" x1="0" />
            <line x2="48" y1="-448" y2="-448" x1="0" />
            <line x2="48" y1="-512" y2="-512" x1="0" />
            <line x2="64" y1="-320" y2="-320" x1="0" />
            <line x2="64" y1="-256" y2="-256" x1="0" />
            <line x2="48" y1="-336" y2="-512" x1="48" />
            <line x2="48" y1="-64" y2="-240" x1="48" />
            <line x2="48" y1="-336" y2="-336" x1="72" />
            <line x2="52" y1="-240" y2="-240" x1="72" />
        </blockdef>
        <blockdef name="fd">
            <timestamp>2000-1-1T10:10:10</timestamp>
            <line x2="80" y1="-112" y2="-128" x1="64" />
            <line x2="64" y1="-128" y2="-144" x1="80" />
            <line x2="320" y1="-256" y2="-256" x1="384" />
            <line x2="64" y1="-256" y2="-256" x1="0" />
            <line x2="64" y1="-128" y2="-128" x1="0" />
            <rect width="256" x="64" y="-320" height="256" />
        </blockdef>
        <block symbolname="ibuf8" name="XLXI_1">
            <blockpin signalname="CAMERA_DATA(7:0)" name="I(7:0)" />
            <blockpin signalname="XLXN_6(7:0)" name="O(7:0)" />
        </block>
        <block symbolname="sr8cle" name="XLXI_26">
            <blockpin signalname="XLXN_90" name="C" />
            <blockpin signalname="XLXN_94" name="CE" />
            <blockpin signalname="XLXN_8" name="CLR" />
            <blockpin signalname="XLXN_6(7:0)" name="D(7:0)" />
            <blockpin signalname="XLXN_38" name="L" />
            <blockpin signalname="XLXN_105" name="SLI" />
            <blockpin signalname="shift_out(7:0)" name="Q(7:0)" />
        </block>
        <block symbolname="gnd" name="XLXI_27">
            <blockpin signalname="XLXN_105" name="G" />
        </block>
        <block symbolname="gnd" name="XLXI_29">
            <blockpin signalname="XLXN_8" name="G" />
        </block>
        <block symbolname="ibuf" name="XLXI_31">
            <blockpin signalname="nSLAVE_SELECT" name="I" />
            <blockpin signalname="XLXN_38" name="O" />
        </block>
        <block symbolname="sr8ce" name="XLXI_47">
            <blockpin signalname="XLXN_90" name="C" />
            <blockpin signalname="XLXN_94" name="CE" />
            <blockpin signalname="XLXN_85" name="CLR" />
            <blockpin signalname="XLXN_56" name="SLI" />
            <blockpin signalname="MOSI_P(7:0)" name="Q(7:0)" />
        </block>
        <block symbolname="inv" name="XLXI_52">
            <blockpin signalname="XLXN_38" name="I" />
            <blockpin signalname="XLXN_94" name="O" />
        </block>
        <block symbolname="obuf" name="XLXI_53">
            <blockpin signalname="XLXN_69" name="I" />
            <blockpin signalname="READ_STROBE" name="O" />
        </block>
        <block symbolname="m2_1" name="XLXI_56">
            <blockpin signalname="XLXN_86" name="D0" />
            <blockpin signalname="XLXN_94" name="D1" />
            <blockpin signalname="XLXN_106" name="S0" />
            <blockpin signalname="XLXN_69" name="O" />
        </block>
        <block symbolname="nor8" name="XLXI_57">
            <blockpin signalname="MOSI_P(7)" name="I0" />
            <blockpin signalname="MOSI_P(6)" name="I1" />
            <blockpin signalname="MOSI_P(5)" name="I2" />
            <blockpin signalname="MOSI_P(4)" name="I3" />
            <blockpin signalname="MOSI_P(3)" name="I4" />
            <blockpin signalname="MOSI_P(2)" name="I5" />
            <blockpin signalname="MOSI_P(1)" name="I6" />
            <blockpin signalname="MOSI_P(0)" name="I7" />
            <blockpin signalname="XLXN_99" name="O" />
        </block>
        <block symbolname="ibuf" name="XLXI_45">
            <blockpin signalname="MOSI" name="I" />
            <blockpin signalname="XLXN_56" name="O" />
        </block>
        <block symbolname="ibuf" name="XLXI_32">
            <blockpin signalname="SCK" name="I" />
            <blockpin signalname="XLXN_90" name="O" />
        </block>
        <block symbolname="fd" name="XLXI_60">
            <blockpin signalname="XLXN_38" name="C" />
            <blockpin signalname="XLXN_99" name="D" />
            <blockpin signalname="XLXN_106" name="Q" />
        </block>
        <block symbolname="obuf" name="XLXI_61">
            <blockpin signalname="XLXN_107" name="I" />
            <blockpin signalname="FIFO_RESET" name="O" />
        </block>
        <block symbolname="obufe" name="XLXI_30">
            <blockpin signalname="XLXN_94" name="E" />
            <blockpin signalname="shift_out(7)" name="I" />
            <blockpin signalname="MISO" name="O" />
        </block>
        <block symbolname="gnd" name="XLXI_62">
            <blockpin signalname="XLXN_85" name="G" />
        </block>
        <block symbolname="gnd" name="XLXI_63">
            <blockpin signalname="XLXN_86" name="G" />
        </block>
        <block symbolname="inv" name="XLXI_65">
            <blockpin signalname="XLXN_106" name="I" />
            <blockpin signalname="XLXN_107" name="O" />
        </block>
    </netlist>
    <sheet sheetnum="1" width="5382" height="3801">
        <attr value="CM" name="LengthUnitName" />
        <attr value="4" name="GridsPerUnit" />
        <instance x="544" y="608" name="XLXI_1" orien="R0" />
        <branch name="CAMERA_DATA(7:0)">
            <wire x2="544" y1="576" y2="576" x1="512" />
        </branch>
        <iomarker fontsize="28" x="512" y="576" name="CAMERA_DATA(7:0)" orien="R180" />
        <instance x="1440" y="1088" name="XLXI_26" orien="R0" />
        <branch name="XLXN_6(7:0)">
            <wire x2="1440" y1="576" y2="576" x1="768" />
        </branch>
        <instance x="1328" y="448" name="XLXI_27" orien="R0" />
        <branch name="XLXN_8">
            <wire x2="1440" y1="1056" y2="1104" x1="1440" />
        </branch>
        <instance x="1376" y="1232" name="XLXI_29" orien="R0" />
        <branch name="shift_out(7:0)">
            <wire x2="1920" y1="704" y2="704" x1="1824" />
            <wire x2="1968" y1="704" y2="704" x1="1920" />
        </branch>
        <bustap x2="1920" y1="704" y2="608" x1="1920" />
        <branch name="shift_out(7)">
            <wire x2="2080" y1="544" y2="544" x1="1920" />
            <wire x2="1920" y1="544" y2="608" x1="1920" />
        </branch>
        <branch name="nSLAVE_SELECT">
            <wire x2="384" y1="1264" y2="1264" x1="368" />
        </branch>
        <iomarker fontsize="28" x="368" y="1264" name="nSLAVE_SELECT" orien="R180" />
        <instance x="384" y="1296" name="XLXI_31" orien="R0" />
        <instance x="736" y="1280" name="XLXI_52" orien="R90" />
        <branch name="XLXN_38">
            <wire x2="768" y1="1264" y2="1264" x1="608" />
            <wire x2="768" y1="1264" y2="1280" x1="768" />
            <wire x2="880" y1="1264" y2="1264" x1="768" />
            <wire x2="880" y1="1264" y2="1552" x1="880" />
            <wire x2="1168" y1="1552" y2="1552" x1="880" />
            <wire x2="1168" y1="1472" y2="1488" x1="1168" />
            <wire x2="1168" y1="1488" y2="1552" x1="1168" />
            <wire x2="1296" y1="1488" y2="1488" x1="1168" />
            <wire x2="1440" y1="768" y2="768" x1="1296" />
            <wire x2="1296" y1="768" y2="1488" x1="1296" />
        </branch>
        <instance x="672" y="704" name="XLXI_53" orien="R180" />
        <branch name="READ_STROBE">
            <wire x2="448" y1="736" y2="736" x1="416" />
        </branch>
        <iomarker fontsize="28" x="416" y="736" name="READ_STROBE" orien="R180" />
        <instance x="848" y="2256" name="XLXI_47" orien="R0" />
        <instance x="368" y="1936" name="XLXI_45" orien="R0" />
        <branch name="MOSI">
            <wire x2="368" y1="1904" y2="1904" x1="336" />
        </branch>
        <instance x="368" y="2128" name="XLXI_32" orien="R0" />
        <branch name="SCK">
            <wire x2="368" y1="2096" y2="2096" x1="336" />
        </branch>
        <iomarker fontsize="28" x="336" y="1904" name="MOSI" orien="R180" />
        <iomarker fontsize="28" x="336" y="2096" name="SCK" orien="R180" />
        <branch name="XLXN_56">
            <wire x2="720" y1="1904" y2="1904" x1="592" />
            <wire x2="720" y1="1904" y2="1936" x1="720" />
            <wire x2="848" y1="1936" y2="1936" x1="720" />
        </branch>
        <branch name="MOSI_P(7:0)">
            <wire x2="1264" y1="2000" y2="2000" x1="1232" />
            <wire x2="1328" y1="2000" y2="2000" x1="1264" />
            <wire x2="1392" y1="2000" y2="2000" x1="1328" />
            <wire x2="1456" y1="2000" y2="2000" x1="1392" />
            <wire x2="1520" y1="2000" y2="2000" x1="1456" />
            <wire x2="1584" y1="2000" y2="2000" x1="1520" />
            <wire x2="1648" y1="2000" y2="2000" x1="1584" />
            <wire x2="1712" y1="2000" y2="2000" x1="1648" />
            <wire x2="1792" y1="2000" y2="2000" x1="1712" />
        </branch>
        <instance x="1776" y="1776" name="XLXI_57" orien="R270" />
        <bustap x2="1264" y1="2000" y2="1904" x1="1264" />
        <branch name="MOSI_P(0)">
            <wire x2="1264" y1="1776" y2="1904" x1="1264" />
        </branch>
        <bustap x2="1328" y1="2000" y2="1904" x1="1328" />
        <branch name="MOSI_P(1)">
            <wire x2="1328" y1="1776" y2="1904" x1="1328" />
        </branch>
        <bustap x2="1392" y1="2000" y2="1904" x1="1392" />
        <branch name="MOSI_P(2)">
            <wire x2="1392" y1="1776" y2="1904" x1="1392" />
        </branch>
        <bustap x2="1456" y1="2000" y2="1904" x1="1456" />
        <branch name="MOSI_P(3)">
            <wire x2="1456" y1="1776" y2="1904" x1="1456" />
        </branch>
        <bustap x2="1520" y1="2000" y2="1904" x1="1520" />
        <branch name="MOSI_P(4)">
            <wire x2="1520" y1="1776" y2="1904" x1="1520" />
        </branch>
        <bustap x2="1584" y1="2000" y2="1904" x1="1584" />
        <branch name="MOSI_P(5)">
            <wire x2="1584" y1="1776" y2="1904" x1="1584" />
        </branch>
        <bustap x2="1648" y1="2000" y2="1904" x1="1648" />
        <branch name="MOSI_P(6)">
            <wire x2="1648" y1="1776" y2="1904" x1="1648" />
        </branch>
        <bustap x2="1712" y1="2000" y2="1904" x1="1712" />
        <branch name="MOSI_P(7)">
            <wire x2="1712" y1="1776" y2="1904" x1="1712" />
        </branch>
        <branch name="MISO">
            <wire x2="2320" y1="544" y2="544" x1="2304" />
            <wire x2="2336" y1="480" y2="480" x1="2320" />
            <wire x2="2320" y1="480" y2="544" x1="2320" />
        </branch>
        <iomarker fontsize="28" x="2336" y="480" name="MISO" orien="R0" />
        <instance x="928" y="1072" name="XLXI_56" orien="R270" />
        <branch name="XLXN_69">
            <wire x2="800" y1="736" y2="736" x1="672" />
            <wire x2="800" y1="736" y2="752" x1="800" />
        </branch>
        <instance x="1296" y="1472" name="XLXI_60" orien="R270" />
        <instance x="672" y="624" name="XLXI_61" orien="R180" />
        <branch name="FIFO_RESET">
            <wire x2="448" y1="656" y2="656" x1="416" />
        </branch>
        <iomarker fontsize="28" x="416" y="656" name="FIFO_RESET" orien="R180" />
        <instance x="2080" y="512" name="XLXI_30" orien="M180" />
        <instance x="1344" y="2336" name="XLXI_62" orien="R270" />
        <branch name="XLXN_85">
            <wire x2="848" y1="2224" y2="2272" x1="848" />
            <wire x2="1216" y1="2272" y2="2272" x1="848" />
        </branch>
        <instance x="512" y="1216" name="XLXI_63" orien="R0" />
        <branch name="XLXN_86">
            <wire x2="576" y1="1008" y2="1088" x1="576" />
            <wire x2="656" y1="1008" y2="1008" x1="576" />
            <wire x2="656" y1="1008" y2="1136" x1="656" />
            <wire x2="768" y1="1136" y2="1136" x1="656" />
            <wire x2="768" y1="1072" y2="1136" x1="768" />
        </branch>
        <branch name="XLXN_90">
            <wire x2="672" y1="2096" y2="2096" x1="592" />
            <wire x2="832" y1="2096" y2="2096" x1="672" />
            <wire x2="832" y1="2096" y2="2128" x1="832" />
            <wire x2="848" y1="2128" y2="2128" x1="832" />
            <wire x2="672" y1="2096" y2="2448" x1="672" />
            <wire x2="1824" y1="2448" y2="2448" x1="672" />
            <wire x2="1440" y1="960" y2="960" x1="1344" />
            <wire x2="1344" y1="960" y2="1328" x1="1344" />
            <wire x2="1824" y1="1328" y2="1328" x1="1344" />
            <wire x2="1824" y1="1328" y2="2448" x1="1824" />
        </branch>
        <branch name="XLXN_94">
            <wire x2="832" y1="1168" y2="1168" x1="688" />
            <wire x2="688" y1="1168" y2="1520" x1="688" />
            <wire x2="768" y1="1520" y2="1520" x1="688" />
            <wire x2="768" y1="1520" y2="2064" x1="768" />
            <wire x2="848" y1="2064" y2="2064" x1="768" />
            <wire x2="768" y1="2064" y2="2352" x1="768" />
            <wire x2="2080" y1="2352" y2="2352" x1="768" />
            <wire x2="768" y1="1504" y2="1520" x1="768" />
            <wire x2="832" y1="1072" y2="1168" x1="832" />
            <wire x2="1440" y1="896" y2="896" x1="1328" />
            <wire x2="1328" y1="896" y2="1088" x1="1328" />
            <wire x2="2080" y1="1088" y2="1088" x1="1328" />
            <wire x2="2080" y1="1088" y2="2352" x1="2080" />
            <wire x2="2080" y1="608" y2="1088" x1="2080" />
        </branch>
        <branch name="XLXN_99">
            <wire x2="1040" y1="1472" y2="1520" x1="1040" />
            <wire x2="1488" y1="1520" y2="1520" x1="1040" />
        </branch>
        <text style="fontsize:24;fontname:Arial" x="992" y="1308">State Machine of Doom</text>
        <branch name="XLXN_105">
            <wire x2="1392" y1="256" y2="256" x1="1296" />
            <wire x2="1392" y1="256" y2="320" x1="1392" />
            <wire x2="1296" y1="256" y2="512" x1="1296" />
            <wire x2="1440" y1="512" y2="512" x1="1296" />
        </branch>
        <branch name="XLXN_106">
            <wire x2="992" y1="1072" y2="1072" x1="896" />
            <wire x2="1040" y1="1072" y2="1072" x1="992" />
            <wire x2="1040" y1="1072" y2="1088" x1="1040" />
            <wire x2="1040" y1="912" y2="1072" x1="1040" />
        </branch>
        <instance x="1072" y="912" name="XLXI_65" orien="R270" />
        <branch name="XLXN_107">
            <wire x2="1040" y1="656" y2="656" x1="672" />
            <wire x2="1040" y1="656" y2="688" x1="1040" />
        </branch>
    </sheet>
</drawing>
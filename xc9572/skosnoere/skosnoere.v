////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995-2012 Xilinx, Inc.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////
//   ____  ____ 
//  /   /\/   / 
// /___/  \  /    Vendor: Xilinx 
// \   \   \/     Version : 14.3
//  \   \         Application : sch2hdl
//  /   /         Filename : skosnoere.vf
// /___/   /\     Timestamp : 05/12/2014 13:13:10
// \   \  /  \ 
//  \___\/\___\ 
//
//Command: sch2hdl -intstyle ise -family xc9500xl -verilog /home/axel/Projekt/kiruna/xc9572/skosnoere/skosnoere.vf -w /home/axel/Projekt/kiruna/xc9572/skosnoere/skosnoere.sch
//Design Name: skosnoere
//Device: xc9500xl
//Purpose:
//    This verilog netlist is translated from an ECS schematic.It can be 
//    synthesized and simulated, but it should not be modified. 
//
`timescale 1ns / 1ps

module FD_MXILINX_skosnoere(C, 
                            D, 
                            Q);

   parameter INIT = 1'b0;
   
    input C;
    input D;
   output Q;
   
   wire XLXN_4;
   
   GND  I_36_43 (.G(XLXN_4));
   FDCP  U0 (.C(C), 
            .CLR(XLXN_4), 
            .D(D), 
            .PRE(XLXN_4), 
            .Q(Q));
endmodule
`timescale 1ns / 1ps

module NOR8_MXILINX_skosnoere(I0, 
                              I1, 
                              I2, 
                              I3, 
                              I4, 
                              I5, 
                              I6, 
                              I7, 
                              O);

    input I0;
    input I1;
    input I2;
    input I3;
    input I4;
    input I5;
    input I6;
    input I7;
   output O;
   
   wire S0;
   wire S1;
   
   OR4  I_36_110 (.I0(I0), 
                 .I1(I1), 
                 .I2(I2), 
                 .I3(I3), 
                 .O(S0));
   OR4  I_36_127 (.I0(I4), 
                 .I1(I5), 
                 .I2(I6), 
                 .I3(I7), 
                 .O(S1));
   NOR2  I_36_140 (.I0(S0), 
                  .I1(S1), 
                  .O(O));
endmodule
`timescale 1ns / 1ps

module M2_1_MXILINX_skosnoere(D0, 
                              D1, 
                              S0, 
                              O);

    input D0;
    input D1;
    input S0;
   output O;
   
   wire M0;
   wire M1;
   
   AND2B1  I_36_7 (.I0(S0), 
                  .I1(D0), 
                  .O(M0));
   OR2  I_36_8 (.I0(M1), 
               .I1(M0), 
               .O(O));
   AND2  I_36_9 (.I0(D1), 
                .I1(S0), 
                .O(M1));
endmodule
`timescale 1ns / 1ps

module SR8CE_MXILINX_skosnoere(C, 
                               CE, 
                               CLR, 
                               SLI, 
                               Q);

    input C;
    input CE;
    input CLR;
    input SLI;
   output [7:0] Q;
   
   wire [7:0] Q_DUMMY;
   
   assign Q[7:0] = Q_DUMMY[7:0];
   FDCE #( .INIT(1'b0) ) Q0 (.C(C), 
            .CE(CE), 
            .CLR(CLR), 
            .D(SLI), 
            .Q(Q_DUMMY[0]));
   FDCE #( .INIT(1'b0) ) Q1 (.C(C), 
            .CE(CE), 
            .CLR(CLR), 
            .D(Q_DUMMY[0]), 
            .Q(Q_DUMMY[1]));
   FDCE #( .INIT(1'b0) ) Q2 (.C(C), 
            .CE(CE), 
            .CLR(CLR), 
            .D(Q_DUMMY[1]), 
            .Q(Q_DUMMY[2]));
   FDCE #( .INIT(1'b0) ) Q3 (.C(C), 
            .CE(CE), 
            .CLR(CLR), 
            .D(Q_DUMMY[2]), 
            .Q(Q_DUMMY[3]));
   FDCE #( .INIT(1'b0) ) Q4 (.C(C), 
            .CE(CE), 
            .CLR(CLR), 
            .D(Q_DUMMY[3]), 
            .Q(Q_DUMMY[4]));
   FDCE #( .INIT(1'b0) ) Q5 (.C(C), 
            .CE(CE), 
            .CLR(CLR), 
            .D(Q_DUMMY[4]), 
            .Q(Q_DUMMY[5]));
   FDCE #( .INIT(1'b0) ) Q6 (.C(C), 
            .CE(CE), 
            .CLR(CLR), 
            .D(Q_DUMMY[5]), 
            .Q(Q_DUMMY[6]));
   FDCE #( .INIT(1'b0) ) Q7 (.C(C), 
            .CE(CE), 
            .CLR(CLR), 
            .D(Q_DUMMY[6]), 
            .Q(Q_DUMMY[7]));
endmodule
`timescale 1ns / 1ps

module SR8CLE_MXILINX_skosnoere(C, 
                                CE, 
                                CLR, 
                                D, 
                                L, 
                                SLI, 
                                Q);

    input C;
    input CE;
    input CLR;
    input [7:0] D;
    input L;
    input SLI;
   output [7:0] Q;
   
   wire L_OR_CE;
   wire MD0;
   wire MD1;
   wire MD2;
   wire MD3;
   wire MD4;
   wire MD5;
   wire MD6;
   wire MD7;
   wire [7:0] Q_DUMMY;
   
   assign Q[7:0] = Q_DUMMY[7:0];
   OR2  I_36_12 (.I0(L), 
                .I1(CE), 
                .O(L_OR_CE));
   (* HU_SET = "MQ0_2" *) 
   M2_1_MXILINX_skosnoere  MQ0 (.D0(SLI), 
                               .D1(D[0]), 
                               .S0(L), 
                               .O(MD0));
   (* HU_SET = "MQ1_3" *) 
   M2_1_MXILINX_skosnoere  MQ1 (.D0(Q_DUMMY[0]), 
                               .D1(D[1]), 
                               .S0(L), 
                               .O(MD1));
   (* HU_SET = "MQ2_4" *) 
   M2_1_MXILINX_skosnoere  MQ2 (.D0(Q_DUMMY[1]), 
                               .D1(D[2]), 
                               .S0(L), 
                               .O(MD2));
   (* HU_SET = "MQ3_1" *) 
   M2_1_MXILINX_skosnoere  MQ3 (.D0(Q_DUMMY[2]), 
                               .D1(D[3]), 
                               .S0(L), 
                               .O(MD3));
   (* HU_SET = "MQ4_0" *) 
   M2_1_MXILINX_skosnoere  MQ4 (.D0(Q_DUMMY[3]), 
                               .D1(D[4]), 
                               .S0(L), 
                               .O(MD4));
   (* HU_SET = "MQ5_5" *) 
   M2_1_MXILINX_skosnoere  MQ5 (.D0(Q_DUMMY[4]), 
                               .D1(D[5]), 
                               .S0(L), 
                               .O(MD5));
   (* HU_SET = "MQ6_7" *) 
   M2_1_MXILINX_skosnoere  MQ6 (.D0(Q_DUMMY[5]), 
                               .D1(D[6]), 
                               .S0(L), 
                               .O(MD6));
   (* HU_SET = "MQ7_6" *) 
   M2_1_MXILINX_skosnoere  MQ7 (.D0(Q_DUMMY[6]), 
                               .D1(D[7]), 
                               .S0(L), 
                               .O(MD7));
   FDCE #( .INIT(1'b0) ) Q0 (.C(C), 
            .CE(L_OR_CE), 
            .CLR(CLR), 
            .D(MD0), 
            .Q(Q_DUMMY[0]));
   FDCE #( .INIT(1'b0) ) Q1 (.C(C), 
            .CE(L_OR_CE), 
            .CLR(CLR), 
            .D(MD1), 
            .Q(Q_DUMMY[1]));
   FDCE #( .INIT(1'b0) ) Q2 (.C(C), 
            .CE(L_OR_CE), 
            .CLR(CLR), 
            .D(MD2), 
            .Q(Q_DUMMY[2]));
   FDCE #( .INIT(1'b0) ) Q3 (.C(C), 
            .CE(L_OR_CE), 
            .CLR(CLR), 
            .D(MD3), 
            .Q(Q_DUMMY[3]));
   FDCE #( .INIT(1'b0) ) Q4 (.C(C), 
            .CE(L_OR_CE), 
            .CLR(CLR), 
            .D(MD4), 
            .Q(Q_DUMMY[4]));
   FDCE #( .INIT(1'b0) ) Q5 (.C(C), 
            .CE(L_OR_CE), 
            .CLR(CLR), 
            .D(MD5), 
            .Q(Q_DUMMY[5]));
   FDCE #( .INIT(1'b0) ) Q6 (.C(C), 
            .CE(L_OR_CE), 
            .CLR(CLR), 
            .D(MD6), 
            .Q(Q_DUMMY[6]));
   FDCE #( .INIT(1'b0) ) Q7 (.C(C), 
            .CE(L_OR_CE), 
            .CLR(CLR), 
            .D(MD7), 
            .Q(Q_DUMMY[7]));
endmodule
`timescale 1ns / 1ps

module IBUF8_MXILINX_skosnoere(I, 
                               O);

    input [7:0] I;
   output [7:0] O;
   
   
   IBUF  I_36_30 (.I(I[4]), 
                 .O(O[4]));
   IBUF  I_36_31 (.I(I[5]), 
                 .O(O[5]));
   IBUF  I_36_32 (.I(I[6]), 
                 .O(O[6]));
   IBUF  I_36_33 (.I(I[7]), 
                 .O(O[7]));
   IBUF  I_36_34 (.I(I[3]), 
                 .O(O[3]));
   IBUF  I_36_35 (.I(I[2]), 
                 .O(O[2]));
   IBUF  I_36_36 (.I(I[1]), 
                 .O(O[1]));
   IBUF  I_36_37 (.I(I[0]), 
                 .O(O[0]));
endmodule
`timescale 1ns / 1ps

module skosnoere(CAMERA_DATA, 
                 MOSI, 
                 nSLAVE_SELECT, 
                 SCK, 
                 FIFO_RESET, 
                 MISO, 
                 READ_STROBE);

    input [7:0] CAMERA_DATA;
    input MOSI;
    input nSLAVE_SELECT;
    input SCK;
   output FIFO_RESET;
   output MISO;
   output READ_STROBE;
   
   wire [7:0] MOSI_P;
   wire [7:0] shift_out;
   wire spi_clock;
   wire [7:0] XLXN_6;
   wire XLXN_8;
   wire XLXN_56;
   wire XLXN_69;
   wire XLXN_85;
   wire XLXN_99;
   wire XLXN_105;
   wire XLXN_106;
   wire XLXN_107;
   wire XLXN_108;
   wire XLXN_117;
   wire XLXN_121;
   
   (* HU_SET = "XLXI_1_8" *) 
   IBUF8_MXILINX_skosnoere  XLXI_1 (.I(CAMERA_DATA[7:0]), 
                                   .O(XLXN_6[7:0]));
   (* HU_SET = "XLXI_26_9" *) 
   SR8CLE_MXILINX_skosnoere  XLXI_26 (.C(spi_clock), 
                                     .CE(XLXN_108), 
                                     .CLR(XLXN_8), 
                                     .D(XLXN_6[7:0]), 
                                     .L(XLXN_117), 
                                     .SLI(XLXN_105), 
                                     .Q(shift_out[7:0]));
   GND  XLXI_27 (.G(XLXN_105));
   GND  XLXI_29 (.G(XLXN_8));
   IBUF  XLXI_31 (.I(nSLAVE_SELECT), 
                 .O(XLXN_117));
   IBUF  XLXI_32 (.I(SCK), 
                 .O(spi_clock));
   IBUF  XLXI_45 (.I(MOSI), 
                 .O(XLXN_56));
   (* HU_SET = "XLXI_47_10" *) 
   SR8CE_MXILINX_skosnoere  XLXI_47 (.C(spi_clock), 
                                    .CE(XLXN_108), 
                                    .CLR(XLXN_85), 
                                    .SLI(XLXN_56), 
                                    .Q(MOSI_P[7:0]));
   INV  XLXI_52 (.I(XLXN_117), 
                .O(XLXN_108));
   (* SLEW = "SLOW" *) 
   OBUF  XLXI_53 (.I(XLXN_69), 
                 .O(READ_STROBE));
   (* HU_SET = "XLXI_56_11" *) 
   M2_1_MXILINX_skosnoere  XLXI_56 (.D0(XLXN_108), 
                                   .D1(spi_clock), 
                                   .S0(XLXN_106), 
                                   .O(XLXN_69));
   (* HU_SET = "XLXI_57_12" *) 
   NOR8_MXILINX_skosnoere  XLXI_57 (.I0(MOSI_P[7]), 
                                   .I1(MOSI_P[6]), 
                                   .I2(MOSI_P[5]), 
                                   .I3(MOSI_P[4]), 
                                   .I4(MOSI_P[3]), 
                                   .I5(MOSI_P[2]), 
                                   .I6(MOSI_P[1]), 
                                   .I7(MOSI_P[0]), 
                                   .O(XLXN_99));
   (* HU_SET = "XLXI_60_13" *) 
   FD_MXILINX_skosnoere #( .INIT(1'b0) ) XLXI_60 (.C(XLXN_117), 
                                 .D(XLXN_99), 
                                 .Q(XLXN_106));
   (* SLEW = "SLOW" *) 
   OBUF  XLXI_61 (.I(XLXN_107), 
                 .O(FIFO_RESET));
   GND  XLXI_62 (.G(XLXN_85));
   INV  XLXI_65 (.I(XLXN_106), 
                .O(XLXN_107));
   (* SLEW = "SLOW" *) 
   OBUF  XLXI_66 (.I(XLXN_121), 
                 .O(MISO));
   INV  XLXI_68 (.I(shift_out[7]), 
                .O(XLXN_121));
endmodule

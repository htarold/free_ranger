v 20130925 2
C 40000 40000 0 0 0 title-A3.sym
C 42600 43200 1 0 0 capacitor-v-1.sym
{
T 42800 43900 5 10 0 0 0 0 1
device=CAPACITOR
T 43000 43900 5 10 1 1 180 0 1
refdes=C1
T 42900 43400 5 10 1 1 180 0 1
value=0.47uF
T 42900 43400 5 10 0 1 0 0 1
footprint=0805
T 42800 44100 5 10 0 0 0 0 1
symversion=0.1
}
C 43000 42200 1 0 0 gnd-1.sym
C 48800 42000 1 0 0 npn-4.sym
{
T 49400 42500 5 10 0 0 0 0 1
device=NPN_TRANSISTOR
T 49400 42500 5 10 1 1 0 0 1
refdes=Q2
T 48800 42000 5 10 0 0 0 0 1
footprint=SOT23
T 49100 42300 5 10 1 1 0 0 1
value=MMBT4401
}
C 49300 43300 1 90 0 coil-1.sym
{
T 48900 43500 5 10 0 0 90 0 1
device=COIL
T 49100 43700 5 10 1 1 180 0 1
refdes=L1
T 48700 43500 5 10 0 0 90 0 1
symversion=0.1
T 48600 43800 5 10 1 1 0 0 1
value=470uH
T 49300 43300 5 10 0 0 0 0 1
footprint=COIL_SMD_10X10
}
N 49300 43300 49300 43000 4
N 49300 43300 50000 43300 4
N 49300 44300 49300 44400 4
C 48700 43400 1 180 0 resistor-v-1.sym
{
T 48400 43000 5 10 0 0 180 0 1
device=RESISTOR
T 48500 42900 5 10 1 1 180 0 1
refdes=R9
T 48600 43200 5 10 1 1 180 0 1
value=4k7
T 48400 43200 5 10 0 1 180 0 1
footprint=0805
}
C 49200 41700 1 0 0 gnd-1.sym
C 46800 43300 1 0 0 terminal-1.sym
{
T 47110 44050 5 10 0 0 0 0 1
device=terminal
T 47110 43900 5 10 0 0 0 0 1
footprint=CONNECTOR 1 1
T 46850 43750 5 10 1 1 180 6 1
refdes=T6
}
C 46800 47100 1 0 0 lm358-1.sym
{
T 47475 47700 5 10 0 0 0 0 1
device=LM358
T 47500 48450 5 10 0 0 0 0 1
footprint=SO8
T 47000 48000 5 10 1 1 0 0 1
refdes=U1
T 46800 47100 5 10 0 0 0 0 1
slot=1
}
C 44800 47300 1 0 0 lm358-1.sym
{
T 45475 47900 5 10 0 0 0 0 1
device=LM358
T 45500 48650 5 10 0 0 0 0 1
footprint=SO8
T 45000 48200 5 10 1 1 0 0 1
refdes=U1
T 44800 47300 5 10 0 0 0 0 1
slot=2
}
C 42300 47400 1 0 0 resistor-h-1.sym
{
T 42600 47800 5 10 0 0 0 0 1
device=RESISTOR
T 42800 47600 5 10 1 1 0 0 1
refdes=R1
T 42500 47600 5 10 1 1 0 0 1
value=22k
T 42600 47600 5 10 0 1 0 0 1
footprint=0805
}
C 43600 47300 1 0 0 capacitor-h-1.sym
{
T 43800 48000 5 10 0 0 0 0 1
device=CAPACITOR
T 44100 47600 5 10 1 1 0 0 1
refdes=C3
T 43600 47600 5 10 1 1 0 0 1
value=120p
T 43900 47500 5 10 0 1 0 0 1
footprint=0805
T 43800 48200 5 10 0 0 0 0 1
symversion=0.1
}
C 43100 46600 1 0 0 capacitor-v-1.sym
{
T 43300 47300 5 10 0 0 0 0 1
device=CAPACITOR
T 43500 47400 5 10 1 1 180 0 1
refdes=C2
T 43500 46900 5 10 1 1 180 0 1
value=120p
T 43400 46800 5 10 0 1 0 0 1
footprint=0805
T 43300 47500 5 10 0 0 0 0 1
symversion=0.1
}
N 44500 47500 44800 47500 4
C 45100 46600 1 0 1 resistor-v-1.sym
{
T 44800 47000 5 10 0 0 0 6 1
device=RESISTOR
T 44800 47300 5 10 1 1 180 6 1
refdes=R3
T 44900 47000 5 10 1 1 180 6 1
value=270k
T 44800 46800 5 10 0 1 0 6 1
footprint=0805
}
N 43600 46600 45800 46600 4
N 45800 46600 45800 47700 4
N 43200 47500 43600 47500 4
C 42800 47500 1 0 0 resistor-v-1.sym
{
T 43100 47900 5 10 0 0 0 0 1
device=RESISTOR
T 43100 48200 5 10 1 1 180 0 1
refdes=R2
T 43100 48000 5 10 1 1 180 0 1
value=10k
T 43100 47700 5 10 0 1 0 0 1
footprint=0805
}
N 43300 48400 44800 48400 4
{
T 43900 48400 5 10 1 1 0 0 1
netname=vhalf
}
N 44800 48400 44800 47900 4
C 42900 44300 1 0 0 vcc-1.sym
C 45100 48100 1 0 0 vcc-1.sym
N 43100 42500 43100 43200 4
N 43100 44100 43100 44300 4
C 44100 43400 1 0 1 resistor-v-1.sym
{
T 43800 43800 5 10 0 0 0 6 1
device=RESISTOR
T 43800 44100 5 10 1 1 180 6 1
refdes=R6
T 43800 43800 5 10 1 1 180 6 1
value=2k2
T 43800 43600 5 10 0 1 0 6 1
footprint=0805
}
N 44300 42500 43100 42500 4
N 43800 44300 43100 44300 4
C 44800 42500 1 0 1 capacitor-v-1.sym
{
T 44600 43200 5 10 0 0 0 6 1
device=CAPACITOR
T 44400 43200 5 10 1 1 180 6 1
refdes=C8
T 44500 42700 5 10 1 1 180 6 1
value=0.47uF
T 44500 42700 5 10 0 1 0 6 1
footprint=0805
T 44600 43400 5 10 0 0 0 6 1
symversion=0.1
}
N 44300 43400 43600 43400 4
{
T 44300 43400 5 10 1 1 0 0 1
netname=vhalf
}
C 45200 47000 1 0 0 gnd-1.sym
C 46100 46300 1 0 0 resistor-v-1.sym
{
T 46400 46700 5 10 0 0 0 0 1
device=RESISTOR
T 46400 47000 5 10 1 1 180 0 1
refdes=R4
T 46300 46700 5 10 1 1 180 0 1
value=4k7
T 46400 46500 5 10 0 1 0 0 1
footprint=0805
}
C 46100 45400 1 0 0 capacitor-v-1.sym
{
T 46300 46100 5 10 0 0 0 0 1
device=CAPACITOR
T 46500 46200 5 10 1 1 180 0 1
refdes=C7
T 46400 45600 5 10 1 1 180 0 1
value=10n
T 46400 45600 5 10 0 1 0 0 1
footprint=0805
T 46300 46300 5 10 0 0 0 0 1
symversion=0.1
}
C 46500 45100 1 0 0 gnd-1.sym
C 49100 44400 1 0 0 vcc-1.sym
T 41700 44900 9 10 1 0 0 0 1
Vcc assumed to be 2 cells = 7.8V
C 43500 43400 1 270 0 led-2.sym
{
T 43700 42600 5 10 1 1 0 0 1
refdes=D1
T 44100 43300 5 10 0 0 270 0 1
device=LED
T 43500 43400 5 10 0 1 0 0 1
footprint=LED3
T 43600 43000 5 10 1 1 0 0 1
value=Green
}
N 48200 42500 48800 42500 4
N 47700 43400 48200 43400 4
C 44700 44100 1 0 1 schottky-1.sym
{
T 44378 44772 5 10 0 0 0 6 1
device=DIODE
T 44400 44600 5 10 1 1 0 6 1
refdes=D3
T 44359 44932 5 10 0 1 0 6 1
footprint=SOD323
T 44200 43900 5 10 1 1 0 0 1
value=PMEG3010
}
N 45400 44300 44700 44300 4
C 49900 42600 1 0 0 gnd-1.sym
C 42200 46800 1 0 0 gnd-1.sym
C 45400 43300 1 0 0 header2-1.sym
{
T 46400 43950 5 10 0 1 0 0 1
device=HEADER2
T 45800 44600 5 10 1 1 0 0 1
refdes=J3
T 45400 43300 5 10 0 1 0 0 1
footprint=HEADER2_1
}
C 45300 43600 1 0 0 gnd-1.sym
C 45900 48100 1 0 0 capacitor-v-1.sym
{
T 46100 48800 5 10 0 0 0 0 1
device=CAPACITOR
T 46300 48800 5 10 1 1 180 0 1
refdes=C9
T 46300 48400 5 10 1 1 180 0 1
value=0.47uF
T 46200 48300 5 10 0 1 0 0 1
footprint=0805
T 46100 49000 5 10 0 0 0 0 1
symversion=0.1
}
C 46200 49000 1 0 0 vcc-1.sym
C 46300 47800 1 0 0 gnd-1.sym
N 42200 47500 42300 47500 4
N 42200 47100 42300 47100 4
C 42200 48100 1 180 0 header2-1.sym
{
T 41200 47450 5 10 0 1 180 0 1
device=HEADER2
T 41900 47900 5 10 1 1 180 0 1
refdes=J1
T 42200 48100 5 10 0 1 180 6 1
footprint=JST_XH_2
}
C 50000 43900 1 180 1 header2-1.sym
{
T 51000 43250 5 10 0 1 180 6 1
device=HEADER2
T 50400 43700 5 10 1 1 180 6 1
refdes=J2
T 50000 43900 5 10 0 1 180 6 1
footprint=JST_XH_2
}
T 49600 40700 9 10 1 0 0 0 1
free_ranger.sch
T 50500 40100 9 10 1 0 0 0 1
1
T 51000 40100 9 10 1 0 0 0 1
1
T 53500 40400 9 10 1 0 0 0 1
Harold Tay
T 53400 40100 9 10 1 0 0 0 1
htarold@gmail.com
C 48800 47000 1 0 0 npn-4.sym
{
T 49400 47500 5 10 0 0 0 0 1
device=NPN_TRANSISTOR
T 49400 47500 5 10 1 1 0 0 1
refdes=Q1
T 48800 47000 5 10 0 0 0 0 1
footprint=SOT23
T 49100 47300 5 10 1 1 0 0 1
value=BC846
}
C 48800 46100 1 0 0 resistor-v-1.sym
{
T 49100 46500 5 10 0 0 0 0 1
device=RESISTOR
T 49000 46500 5 10 1 1 180 0 1
value=150
T 49100 46300 5 10 0 1 0 0 1
footprint=0805
T 48800 46100 5 10 1 1 0 0 1
refdes=R13
}
C 48800 48400 1 0 0 resistor-v-1.sym
{
T 49100 48800 5 10 0 0 0 0 1
device=RESISTOR
T 49100 49100 5 10 1 1 180 0 1
refdes=R10
T 49000 48800 5 10 1 1 180 0 1
value=4k7
T 49100 48600 5 10 0 1 0 0 1
footprint=0805
}
N 49300 48400 49300 48000 4
C 47100 48200 1 0 0 resistor-h-1.sym
{
T 47400 48600 5 10 0 0 0 0 1
device=RESISTOR
T 47600 48400 5 10 1 1 0 0 1
refdes=R7
T 47100 48400 5 10 1 1 0 0 1
value=470k
T 47400 48400 5 10 0 1 0 0 1
footprint=0805
}
N 46800 47700 46600 47700 4
N 46600 47200 46600 48300 4
N 46800 47300 45800 47300 4
N 46600 48300 47100 48300 4
N 48000 48300 49300 48300 4
C 49200 45800 1 0 0 gnd-1.sym
C 49100 49300 1 0 0 vcc-1.sym
T 42900 49500 9 10 1 0 0 0 1
22k/5k6/330k/120p(35kHz) -> Q=4.2,K=-7.5
T 42900 49700 9 10 1 0 0 0 1
22k/10k/270k/120p(31kHz) -> Q=3.1, K=-6.1
N 50200 48000 49300 48000 4
C 47800 47400 1 0 0 resistor-h-1.sym
{
T 48100 47800 5 10 0 0 0 0 1
device=RESISTOR
T 48300 47600 5 10 1 1 0 0 1
refdes=R14
T 47800 47600 5 10 1 1 0 0 1
value=47k
T 48100 47600 5 10 0 1 0 0 1
footprint=0805
}
N 48700 47500 48800 47500 4
C 51700 46200 1 0 1 resistor-v-1.sym
{
T 51400 46600 5 10 0 0 0 6 1
device=RESISTOR
T 51400 46400 5 10 0 1 0 6 1
footprint=0805
T 51400 46900 5 10 1 1 180 6 1
refdes=R5
T 51500 46600 5 10 1 1 180 6 1
value=10k
}
C 52400 45300 1 0 1 capacitor-v-1.sym
{
T 52200 46000 5 10 0 0 0 6 1
device=CAPACITOR
T 52100 45500 5 10 0 1 0 6 1
footprint=0805
T 52200 46200 5 10 0 0 0 6 1
symversion=0.1
T 52000 46100 5 10 1 1 180 6 1
refdes=C4
T 52100 45500 5 10 1 1 180 6 1
value=10n
}
C 51500 45300 1 0 1 resistor-v-1.sym
{
T 51200 45700 5 10 0 0 0 6 1
device=RESISTOR
T 51200 45500 5 10 0 1 0 6 1
footprint=0805
T 51200 46000 5 10 1 1 180 6 1
refdes=R8
T 51200 45700 5 10 1 1 180 6 1
value=68k
}
N 50600 46200 51900 46200 4
C 52000 45000 1 0 1 gnd-1.sym
C 51100 45000 1 0 1 gnd-1.sym
N 50600 46200 50600 47600 4
C 50200 47900 1 0 0 resistor-h-1.sym
{
T 50500 48300 5 10 0 0 0 0 1
device=RESISTOR
T 50500 48100 5 10 0 1 0 0 1
footprint=0805
T 50700 48100 5 10 1 1 0 0 1
refdes=R12
T 50200 48100 5 10 1 1 0 0 1
value=2k2
}
N 51900 48000 51100 48000 4
{
T 50600 47800 5 10 1 1 0 0 1
netname=comparator_neg_ain1
}
C 51900 47000 1 0 0 header2-1.sym
{
T 52900 47650 5 10 0 1 0 0 1
device=HEADER2
T 51900 47000 5 10 0 1 0 0 1
footprint=HEADER2_1
T 52300 48300 5 10 1 1 0 0 1
refdes=J4
}
N 50600 47600 51900 47600 4
{
T 50600 47400 5 10 1 1 0 0 1
netname=comparator_pos_ain0
}
N 51200 47100 51900 47100 4
{
T 51200 47100 5 10 1 1 0 0 1
netname=LPF_in
}
C 52800 47000 1 0 1 terminal-1.sym
{
T 52490 47750 5 10 0 0 0 6 1
device=terminal
T 52490 47600 5 10 0 0 0 6 1
footprint=CONNECTOR 1 1
T 52550 47050 5 10 1 1 0 0 1
refdes=T1
}

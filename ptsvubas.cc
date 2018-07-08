#if(0)
c++ -pipe -xc++ -fpermissive -w --include ptsvubas.cc -o "${0%.*}.exe" "$0" ; exit $?
/* BASIC language subset/dialect in C++

Authors: NASZVADI, Peter and pts@fazekas.hu, 2017

All rights reserved, free for educational purpose only, which must be
noncommercial! Cannot remove or alter lines 3-8! */
/*

This is a standalone c++ file, which allows basic code to be compiled as
c++ code. There are at least two ways. The first:

    # c++ -pipe -xc++ -fpermissive -w --include ptsvubas.cc -o YOURFILE.exe FOOBAR.bas

from CLI should be invoked the upper line in order to complie the
FOOBAR.bas basic code. In that case, do not include "ptsvubas.cc" from basic source!

The second way is: eliminate "--include ptsvubas.cc" from the command
line above and make sure that the basic source contains

    #include "ptsvubas.cc"

in its first lines before "BEGINBASIC(...)" line.

Implemented a BASIC language dialect/subset, with the following restrictions:

- All variables and statements must be allcaps
- All lines must begin with a line number from 1 to 32767
- All lines must have a trailing semicolon
- One statement per line (except IF-THEN)
- "THEN" must NOT be followed by a "GOTO" nor "GOSUB"!
- Implemented keywords:
--- IF <C-style expression with "==" in case of equality> THEN <label>
--- GOTO <label>
--- GOSUB <label> / RETURN
--- STOP [optional errorlevel/return value]
--- END [optional errorlevel/return value]
--- LET <variable>=<expression>
--- PRINT <strings and/or variables in arbitrary order>
--- INPUT <one or more variables>
--- DIM <array variables with number of elements, p.ex. "NP(42)">
--- FOR/NEXT loop (NEXT's argument is mandatory, exactly one variable)
- PRINT interprets comma as semicolon, except on the ENDS
--- thus PRINT does NOT interpret comma as tab injection
--- basically, PRINT is some kind of writeln()
--- but can be tricked: if CHR$(0) is inserted somewhere, it won't print
---  the remaining part including the trailing newline!
- variables are all float types, predeclared, and their names at most 2 chars long
- (numerical) arrays can be used, must be declared before their first usage
--- Array numberings: DIM A(8) means that A(0) till A(7) are declared this way

example code:

    #include "ptsvubas.cc"
    BEGINBASIC(int,main,(int argc, char* argv[]))
    10 PRINT "Hello, BASIC!";
    20 GOTO 40;
    30 PRINT "OOPS, I am skipped :(";
    40 END;
    ENDBASIC

The BEGINBASIC(,,) macro creates a function with type given in its first
argument. So the created function can be invoked from another C++ function,
and BASIC could pass return values to the caller function
*/
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef NTYPE
#define NTYPE float
#endif
#ifndef NOT
#define NOT !
#endif
#ifndef AND
#define AND &&
#endif
#ifndef OR
#define OR ||
#endif
#ifndef XOR
#define XOR ^
#endif
#ifndef SHL
#define SHL <<
#endif
#ifndef SHR
#define SHR >>
#endif
#ifndef CHR$
#define CHR$(x) (char)(x)
#endif
#ifndef RND
#define RND(x) (x>0?rand()%x:((float)(rand())/((float)RAND_MAX)))
#endif
struct _LABEL {
    int _label;
    void _SET(int x) { _label=x; }
    int _GET() { return(_label); }
};
const _LABEL &operator,(int x, const _LABEL &p) { p._SET(x); return p; }
const _LABEL &operator,(const _LABEL &p, int &x) { x=p._GET(); return p; }
struct _PRINT {
    char _row[256];
    int _length = 0;
    void _SETS(const char *x) { strcpy((_row+_length), x); _length += strlen(x); }
    void _SETC(char x) { _row[_length++] = (char)x; }
    ~_PRINT() { _row[_length]='\n';_row[_length+1]='\0'; printf("%s", _row); }
};
const _PRINT &operator,(const _PRINT &p, NTYPE x) {
    char _s[256];
    snprintf(_s, 255, "%.12g", (double)x);
    p._SETS(_s);
    return p;
};
const _PRINT &operator,(const _PRINT &p, char x) { p._SETC(x); return p; }
const _PRINT &operator,(const _PRINT &p, const char *s) { p._SETS(s); return p; }
#ifndef PRINT
#define PRINT ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl)_PRINT(),
#endif
struct _INPUT { _INPUT() { putchar('?');putchar(' '); } };
const _INPUT &operator,(const _INPUT &p, NTYPE &x) { scanf("%f", &x); return p; }
#ifndef INPUT
#define INPUT ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl)_INPUT(),
#endif
#ifndef GOTO
#define GOTO ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl)fstack.nextl=0x7FFF+
#endif
#ifndef GOSUB
#define GOSUB ,_LABEL(),currentlabel;\
 if(currentlabel==fstack.nextl)if((gstack[++gsp]=fstack.nextl)||1)fstack.nextl=0x7FFF+
#endif
#ifndef RETURN
#define RETURN ,_LABEL(),currentlabel;\
 if(currentlabel==fstack.nextl)fstack.nextl=0x8000+gstack[gsp--]
#endif
struct _fstype {
    char ptr;
    int nextl;
    int lbl[256];
    NTYPE val[256];
};
#ifndef FOR
#define FOR ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl)if((
#endif
#ifndef _TO
#define _TO )||1)if((++fstack.ptr||1)&&(fstack.lbl[fstack.ptr]=0x8000|fstack.nextl)||1)fstack.val[fstack.ptr]=
#endif
#ifndef TO
#define TO _TO
#endif
struct _NEXT {
    _fstype* fp;
    void _SET(_fstype &x){ fp = &x; };
};
const _NEXT &operator,(const _NEXT &p, _fstype &fs) {
    p._SET(fs);
    return p;
};
const _NEXT &operator,(const _NEXT &p, NTYPE &x) {
    if(p.fp->val[p.fp->ptr] < x+1) p.fp->ptr--;
    else {
        x=x+1;
        p.fp->nextl = p.fp->lbl[p.fp->ptr];
    }
    return p;
};
#ifndef NEXT
#define NEXT ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl)_NEXT(),fstack,
#endif
#ifndef STOP
#define STOP ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl)return
#endif
#ifndef END
#define END ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl)return
#endif
#ifndef LET
#define LET ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl)
#endif
#ifndef IF
#define IF ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl && (
#endif
#ifndef THEN
#define THEN ))fstack.nextl=0x7FFF+
#endif
#ifndef DIM
#define DIM ,_LABEL(),currentlabel;NTYPE
#endif
#ifndef RANDOMIZE
#define RANDOMIZE ,_LABEL(),currentlabel;if(currentlabel==fstack.nextl)srand(time(0))
#endif
#ifndef ENDBASIC
#define ENDBASIC }while(fstack.nextl=(fstack.nextl+1)&0x7FFF);return(0);}
#endif
#ifndef BEGINBASIC
#define BEGINBASIC(rettype,main,fargs) rettype main fargs{\
 char gsp;int gstack[256];gsp=0;int currentlabel=0;\
 _fstype fstack; fstack.ptr=0; fstack.nextl=1;\
 NTYPE A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE,\
 AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,BA,BB,BC,\
 BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO,BP,BQ,BR,BS,BT,BU,BV,BW,BX,BY,BZ,CA,\
 CB,CC,CD,CE,CF,CG,CH,CI,CJ,CK,CL,CM,CN,CO,CP,CQ,CR,CS,CT,CU,CV,CW,CX,CY,\
 CZ,DA,DB,DC,DD,DE,DF,DG,DH,DI,DJ,DK,DL,DM,DN,DP,DQ,DR,DS,DT,DU,DV,DW,DX,\
 DY,DZ,EA,EB,EC,ED,EE,EF,EG,EH,EI,EJ,EK,EL,EM,EN,EO,EP,EQ,ER,ES,ET,EU,EV,\
 EW,EX,EY,EZ,FA,FB,FC,FD,FE,FF,FG,FH,FI,FJ,FK,FL,FM,FN,FO,FP,FQ,FR,FS,FT,\
 FU,FV,FW,FX,FY,FZ,GA,GB,GC,GD,GE,GF,GG,GH,GI,GJ,GK,GL,GM,GN,GO,GP,GQ,GR,\
 GS,GT,GU,GV,GW,GX,GY,GZ,HA,HB,HC,HD,HE,HF,HG,HH,HI,HJ,HK,HL,HM,HN,HO,HP,\
 HQ,HR,HS,HT,HU,HV,HW,HX,HY,HZ,IA,IB,IC,ID,IE,IG,IH,II,IJ,IK,IL,IM,IN,IO,\
 IP,IQ,IR,IS,IT,IU,IV,IW,IX,IY,IZ,JA,JB,JC,JD,JE,JF,JG,JH,JI,JJ,JK,JL,JM,\
 JN,JO,JP,JQ,JR,JS,JT,JU,JV,JW,JX,JY,JZ,KA,KB,KC,KD,KE,KF,KG,KH,KI,KJ,KK,\
 KL,KM,KN,KO,KP,KQ,KR,KS,KT,KU,KV,KW,KX,KY,KZ,LA,LB,LC,LD,LE,LF,LG,LH,LI,\
 LJ,LK,LL,LM,LN,LO,LP,LQ,LR,LS,LT,LU,LV,LW,LX,LY,LZ,MA,MB,MC,MD,ME,MF,MG,\
 MH,MI,MJ,MK,ML,MM,MN,MO,MP,MQ,MR,MS,MT,MU,MV,MW,MX,MY,MZ,NA,NB,NC,ND,NE,\
 NF,NG,NH,NI,NJ,NK,NL,NM,NN,NO,NP,NQ,NR,NS,NT,NU,NV,NW,NX,NY,NZ,OA,OB,OC,\
 OD,OE,OF,OG,OH,OI,OJ,OK,OL,OM,ON,OO,OP,OQ,OS,OT,OU,OV,OW,OX,OY,OZ,PA,PB,\
 PC,PD,PE,PF,PG,PH,PI,PJ,PK,PL,PM,PN,PO,PP,PQ,PR,PS,PT,PU,PV,PW,PX,PY,PZ,\
 QA,QB,QC,QD,QE,QF,QG,QH,QI,QJ,QK,QL,QM,QN,QO,QP,QQ,QR,QS,QT,QU,QV,QW,QX,\
 QY,QZ,RA,RB,RC,RD,RE,RF,RG,RH,RI,RJ,RK,RL,RM,RN,RO,RP,RQ,RR,RS,RT,RU,RV,\
 RW,RX,RY,RZ,SA,SB,SC,SD,SE,SF,SG,SH,SI,SJ,SK,SL,SM,SN,SO,SP,SQ,SR,SS,ST,\
 SU,SV,SW,SX,SY,SZ,TA,TB,TC,TD,TE,TF,TG,TH,TI,TJ,TK,TL,TM,TN,TP,TQ,TR,\
 TS,TT,TU,TV,TW,TX,TY,TZ,UA,UB,UC,UD,UE,UF,UG,UH,UI,UJ,UK,UL,UM,UN,UO,UP,\
 UQ,UR,US,UT,UU,UV,UW,UX,UY,UZ,VA,VB,VC,VD,VE,VF,VG,VH,VI,VJ,VK,VL,VM,VN,\
 VO,VP,VQ,VR,VS,VT,VU,VV,VW,VX,VY,VZ,WA,WB,WC,WD,WE,WF,WG,WH,WI,WJ,WK,WL,\
 WM,WN,WO,WP,WQ,WR,WS,WT,WU,WV,WW,WX,WY,WZ,XA,XB,XC,XD,XE,XF,XG,XH,XI,XJ,\
 XK,XL,XM,XN,XO,XP,XQ,XR,XS,XT,XU,XV,XW,XX,XY,XZ,YA,YB,YC,YD,YE,YF,YG,YH,\
 YI,YJ,YK,YL,YM,YN,YO,YP,YQ,YR,YS,YT,YU,YV,YW,YX,YY,YZ,ZA,ZB,ZC,ZD,ZE,ZF,\
 ZG,ZH,ZI,ZJ,ZK,ZL,ZM,ZN,ZO,ZP,ZQ,ZR,ZS,ZT,ZU,ZV,ZW,ZX,ZY,ZZ,A0,A1,A2,A3,\
 A4,A5,A6,A7,A8,A9,B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,C0,C1,C2,C3,C4,C5,C6,C7,\
 C8,C9,D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,F0,F1,F2,F3,F4,F5,F6,F7,F8,F9,G0,G1,\
 G2,G3,G4,G5,G6,G7,G8,G9,H0,H1,H2,H3,H4,H5,H6,H7,H8,H9,I0,I1,I2,I3,I4,I5,\
 I6,I7,I8,I9,J0,J1,J2,J3,J4,J5,J6,J7,J8,J9,K0,K1,K2,K3,K4,K5,K6,K7,K8,K9,\
 L0,L1,L2,L3,L4,L5,L6,L7,L8,L9,M0,M1,M2,M3,M4,M5,M6,M7,M8,M9,N0,N1,N2,N3,\
 N4,N5,N6,N7,N8,N9,O0,O1,O2,O3,O4,O5,O6,O7,O8,O9,P0,P1,P2,P3,P4,P5,P6,P7,\
 P8,P9,Q0,Q1,Q2,Q3,Q4,Q5,Q6,Q7,Q8,Q9,R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,S0,S1,\
 S2,S3,S4,S5,S6,S7,S8,S9,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,U0,U1,U2,U3,U4,U5,\
 U6,U7,U8,U9,V0,V1,V2,V3,V4,V5,V6,V7,V8,V9,W0,W1,W2,W3,W4,W5,W6,W7,W8,W9,\
 X0,X1,X2,X3,X4,X5,X6,X7,X8,X9,Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8,Y9,Z0,Z1,Z2,Z3,\
 Z4,Z5,Z6,Z7,Z8,Z9;do{
#endif
#define A(x) A[(unsigned int)(x)]
#define B(x) B[(unsigned int)(x)]
#define C(x) C[(unsigned int)(x)]
#define D(x) D[(unsigned int)(x)]
#define E(x) E[(unsigned int)(x)]
#define F(x) F[(unsigned int)(x)]
#define G(x) G[(unsigned int)(x)]
#define H(x) H[(unsigned int)(x)]
#define I(x) I[(unsigned int)(x)]
#define J(x) J[(unsigned int)(x)]
#define K(x) K[(unsigned int)(x)]
#define L(x) L[(unsigned int)(x)]
#define M(x) M[(unsigned int)(x)]
#define N(x) N[(unsigned int)(x)]
#define O(x) O[(unsigned int)(x)]
#define P(x) P[(unsigned int)(x)]
#define Q(x) Q[(unsigned int)(x)]
#define R(x) R[(unsigned int)(x)]
#define S(x) S[(unsigned int)(x)]
#define T(x) T[(unsigned int)(x)]
#define U(x) U[(unsigned int)(x)]
#define V(x) V[(unsigned int)(x)]
#define W(x) W[(unsigned int)(x)]
#define X(x) X[(unsigned int)(x)]
#define Y(x) Y[(unsigned int)(x)]
#define Z(x) Z[(unsigned int)(x)]
#define AA(x) AA[(unsigned int)(x)]
#define AB(x) AB[(unsigned int)(x)]
#define AC(x) AC[(unsigned int)(x)]
#define AD(x) AD[(unsigned int)(x)]
#define AE(x) AE[(unsigned int)(x)]
#define AF(x) AF[(unsigned int)(x)]
#define AG(x) AG[(unsigned int)(x)]
#define AH(x) AH[(unsigned int)(x)]
#define AI(x) AI[(unsigned int)(x)]
#define AJ(x) AJ[(unsigned int)(x)]
#define AK(x) AK[(unsigned int)(x)]
#define AL(x) AL[(unsigned int)(x)]
#define AM(x) AM[(unsigned int)(x)]
#define AN(x) AN[(unsigned int)(x)]
#define AO(x) AO[(unsigned int)(x)]
#define AP(x) AP[(unsigned int)(x)]
#define AQ(x) AQ[(unsigned int)(x)]
#define AR(x) AR[(unsigned int)(x)]
#define AS(x) AS[(unsigned int)(x)]
#define AT(x) AT[(unsigned int)(x)]
#define AU(x) AU[(unsigned int)(x)]
#define AV(x) AV[(unsigned int)(x)]
#define AW(x) AW[(unsigned int)(x)]
#define AX(x) AX[(unsigned int)(x)]
#define AY(x) AY[(unsigned int)(x)]
#define AZ(x) AZ[(unsigned int)(x)]
#define BA(x) BA[(unsigned int)(x)]
#define BB(x) BB[(unsigned int)(x)]
#define BC(x) BC[(unsigned int)(x)]
#define BD(x) BD[(unsigned int)(x)]
#define BE(x) BE[(unsigned int)(x)]
#define BF(x) BF[(unsigned int)(x)]
#define BG(x) BG[(unsigned int)(x)]
#define BH(x) BH[(unsigned int)(x)]
#define BI(x) BI[(unsigned int)(x)]
#define BJ(x) BJ[(unsigned int)(x)]
#define BK(x) BK[(unsigned int)(x)]
#define BL(x) BL[(unsigned int)(x)]
#define BM(x) BM[(unsigned int)(x)]
#define BN(x) BN[(unsigned int)(x)]
#define BO(x) BO[(unsigned int)(x)]
#define BP(x) BP[(unsigned int)(x)]
#define BQ(x) BQ[(unsigned int)(x)]
#define BR(x) BR[(unsigned int)(x)]
#define BS(x) BS[(unsigned int)(x)]
#define BT(x) BT[(unsigned int)(x)]
#define BU(x) BU[(unsigned int)(x)]
#define BV(x) BV[(unsigned int)(x)]
#define BW(x) BW[(unsigned int)(x)]
#define BX(x) BX[(unsigned int)(x)]
#define BY(x) BY[(unsigned int)(x)]
#define BZ(x) BZ[(unsigned int)(x)]
#define CA(x) CA[(unsigned int)(x)]
#define CB(x) CB[(unsigned int)(x)]
#define CC(x) CC[(unsigned int)(x)]
#define CD(x) CD[(unsigned int)(x)]
#define CE(x) CE[(unsigned int)(x)]
#define CF(x) CF[(unsigned int)(x)]
#define CG(x) CG[(unsigned int)(x)]
#define CH(x) CH[(unsigned int)(x)]
#define CI(x) CI[(unsigned int)(x)]
#define CJ(x) CJ[(unsigned int)(x)]
#define CK(x) CK[(unsigned int)(x)]
#define CL(x) CL[(unsigned int)(x)]
#define CM(x) CM[(unsigned int)(x)]
#define CN(x) CN[(unsigned int)(x)]
#define CO(x) CO[(unsigned int)(x)]
#define CP(x) CP[(unsigned int)(x)]
#define CQ(x) CQ[(unsigned int)(x)]
#define CR(x) CR[(unsigned int)(x)]
#define CS(x) CS[(unsigned int)(x)]
#define CT(x) CT[(unsigned int)(x)]
#define CU(x) CU[(unsigned int)(x)]
#define CV(x) CV[(unsigned int)(x)]
#define CW(x) CW[(unsigned int)(x)]
#define CX(x) CX[(unsigned int)(x)]
#define CY(x) CY[(unsigned int)(x)]
#define CZ(x) CZ[(unsigned int)(x)]
#define DA(x) DA[(unsigned int)(x)]
#define DB(x) DB[(unsigned int)(x)]
#define DC(x) DC[(unsigned int)(x)]
#define DD(x) DD[(unsigned int)(x)]
#define DE(x) DE[(unsigned int)(x)]
#define DF(x) DF[(unsigned int)(x)]
#define DG(x) DG[(unsigned int)(x)]
#define DH(x) DH[(unsigned int)(x)]
#define DI(x) DI[(unsigned int)(x)]
#define DJ(x) DJ[(unsigned int)(x)]
#define DK(x) DK[(unsigned int)(x)]
#define DL(x) DL[(unsigned int)(x)]
#define DM(x) DM[(unsigned int)(x)]
#define DN(x) DN[(unsigned int)(x)]
#define DP(x) DP[(unsigned int)(x)]
#define DQ(x) DQ[(unsigned int)(x)]
#define DR(x) DR[(unsigned int)(x)]
#define DS(x) DS[(unsigned int)(x)]
#define DT(x) DT[(unsigned int)(x)]
#define DU(x) DU[(unsigned int)(x)]
#define DV(x) DV[(unsigned int)(x)]
#define DW(x) DW[(unsigned int)(x)]
#define DX(x) DX[(unsigned int)(x)]
#define DY(x) DY[(unsigned int)(x)]
#define DZ(x) DZ[(unsigned int)(x)]
#define EA(x) EA[(unsigned int)(x)]
#define EB(x) EB[(unsigned int)(x)]
#define EC(x) EC[(unsigned int)(x)]
#define ED(x) ED[(unsigned int)(x)]
#define EE(x) EE[(unsigned int)(x)]
#define EF(x) EF[(unsigned int)(x)]
#define EG(x) EG[(unsigned int)(x)]
#define EH(x) EH[(unsigned int)(x)]
#define EI(x) EI[(unsigned int)(x)]
#define EJ(x) EJ[(unsigned int)(x)]
#define EK(x) EK[(unsigned int)(x)]
#define EL(x) EL[(unsigned int)(x)]
#define EM(x) EM[(unsigned int)(x)]
#define EN(x) EN[(unsigned int)(x)]
#define EO(x) EO[(unsigned int)(x)]
#define EP(x) EP[(unsigned int)(x)]
#define EQ(x) EQ[(unsigned int)(x)]
#define ER(x) ER[(unsigned int)(x)]
#define ES(x) ES[(unsigned int)(x)]
#define ET(x) ET[(unsigned int)(x)]
#define EU(x) EU[(unsigned int)(x)]
#define EV(x) EV[(unsigned int)(x)]
#define EW(x) EW[(unsigned int)(x)]
#define EX(x) EX[(unsigned int)(x)]
#define EY(x) EY[(unsigned int)(x)]
#define EZ(x) EZ[(unsigned int)(x)]
#define FA(x) FA[(unsigned int)(x)]
#define FB(x) FB[(unsigned int)(x)]
#define FC(x) FC[(unsigned int)(x)]
#define FD(x) FD[(unsigned int)(x)]
#define FE(x) FE[(unsigned int)(x)]
#define FF(x) FF[(unsigned int)(x)]
#define FG(x) FG[(unsigned int)(x)]
#define FH(x) FH[(unsigned int)(x)]
#define FI(x) FI[(unsigned int)(x)]
#define FJ(x) FJ[(unsigned int)(x)]
#define FK(x) FK[(unsigned int)(x)]
#define FL(x) FL[(unsigned int)(x)]
#define FM(x) FM[(unsigned int)(x)]
#define FN(x) FN[(unsigned int)(x)]
#define FO(x) FO[(unsigned int)(x)]
#define FP(x) FP[(unsigned int)(x)]
#define FQ(x) FQ[(unsigned int)(x)]
#define FR(x) FR[(unsigned int)(x)]
#define FS(x) FS[(unsigned int)(x)]
#define FT(x) FT[(unsigned int)(x)]
#define FU(x) FU[(unsigned int)(x)]
#define FV(x) FV[(unsigned int)(x)]
#define FW(x) FW[(unsigned int)(x)]
#define FX(x) FX[(unsigned int)(x)]
#define FY(x) FY[(unsigned int)(x)]
#define FZ(x) FZ[(unsigned int)(x)]
#define GA(x) GA[(unsigned int)(x)]
#define GB(x) GB[(unsigned int)(x)]
#define GC(x) GC[(unsigned int)(x)]
#define GD(x) GD[(unsigned int)(x)]
#define GE(x) GE[(unsigned int)(x)]
#define GF(x) GF[(unsigned int)(x)]
#define GG(x) GG[(unsigned int)(x)]
#define GH(x) GH[(unsigned int)(x)]
#define GI(x) GI[(unsigned int)(x)]
#define GJ(x) GJ[(unsigned int)(x)]
#define GK(x) GK[(unsigned int)(x)]
#define GL(x) GL[(unsigned int)(x)]
#define GM(x) GM[(unsigned int)(x)]
#define GN(x) GN[(unsigned int)(x)]
#define GO(x) GO[(unsigned int)(x)]
#define GP(x) GP[(unsigned int)(x)]
#define GQ(x) GQ[(unsigned int)(x)]
#define GR(x) GR[(unsigned int)(x)]
#define GS(x) GS[(unsigned int)(x)]
#define GT(x) GT[(unsigned int)(x)]
#define GU(x) GU[(unsigned int)(x)]
#define GV(x) GV[(unsigned int)(x)]
#define GW(x) GW[(unsigned int)(x)]
#define GX(x) GX[(unsigned int)(x)]
#define GY(x) GY[(unsigned int)(x)]
#define GZ(x) GZ[(unsigned int)(x)]
#define HA(x) HA[(unsigned int)(x)]
#define HB(x) HB[(unsigned int)(x)]
#define HC(x) HC[(unsigned int)(x)]
#define HD(x) HD[(unsigned int)(x)]
#define HE(x) HE[(unsigned int)(x)]
#define HF(x) HF[(unsigned int)(x)]
#define HG(x) HG[(unsigned int)(x)]
#define HH(x) HH[(unsigned int)(x)]
#define HI(x) HI[(unsigned int)(x)]
#define HJ(x) HJ[(unsigned int)(x)]
#define HK(x) HK[(unsigned int)(x)]
#define HL(x) HL[(unsigned int)(x)]
#define HM(x) HM[(unsigned int)(x)]
#define HN(x) HN[(unsigned int)(x)]
#define HO(x) HO[(unsigned int)(x)]
#define HP(x) HP[(unsigned int)(x)]
#define HQ(x) HQ[(unsigned int)(x)]
#define HR(x) HR[(unsigned int)(x)]
#define HS(x) HS[(unsigned int)(x)]
#define HT(x) HT[(unsigned int)(x)]
#define HU(x) HU[(unsigned int)(x)]
#define HV(x) HV[(unsigned int)(x)]
#define HW(x) HW[(unsigned int)(x)]
#define HX(x) HX[(unsigned int)(x)]
#define HY(x) HY[(unsigned int)(x)]
#define HZ(x) HZ[(unsigned int)(x)]
#define IA(x) IA[(unsigned int)(x)]
#define IB(x) IB[(unsigned int)(x)]
#define IC(x) IC[(unsigned int)(x)]
#define ID(x) ID[(unsigned int)(x)]
#define IE(x) IE[(unsigned int)(x)]
#define IG(x) IG[(unsigned int)(x)]
#define IH(x) IH[(unsigned int)(x)]
#define II(x) II[(unsigned int)(x)]
#define IJ(x) IJ[(unsigned int)(x)]
#define IK(x) IK[(unsigned int)(x)]
#define IL(x) IL[(unsigned int)(x)]
#define IM(x) IM[(unsigned int)(x)]
#define IN(x) IN[(unsigned int)(x)]
#define IO(x) IO[(unsigned int)(x)]
#define IP(x) IP[(unsigned int)(x)]
#define IQ(x) IQ[(unsigned int)(x)]
#define IR(x) IR[(unsigned int)(x)]
#define IS(x) IS[(unsigned int)(x)]
#define IT(x) IT[(unsigned int)(x)]
#define IU(x) IU[(unsigned int)(x)]
#define IV(x) IV[(unsigned int)(x)]
#define IW(x) IW[(unsigned int)(x)]
#define IX(x) IX[(unsigned int)(x)]
#define IY(x) IY[(unsigned int)(x)]
#define IZ(x) IZ[(unsigned int)(x)]
#define JA(x) JA[(unsigned int)(x)]
#define JB(x) JB[(unsigned int)(x)]
#define JC(x) JC[(unsigned int)(x)]
#define JD(x) JD[(unsigned int)(x)]
#define JE(x) JE[(unsigned int)(x)]
#define JF(x) JF[(unsigned int)(x)]
#define JG(x) JG[(unsigned int)(x)]
#define JH(x) JH[(unsigned int)(x)]
#define JI(x) JI[(unsigned int)(x)]
#define JJ(x) JJ[(unsigned int)(x)]
#define JK(x) JK[(unsigned int)(x)]
#define JL(x) JL[(unsigned int)(x)]
#define JM(x) JM[(unsigned int)(x)]
#define JN(x) JN[(unsigned int)(x)]
#define JO(x) JO[(unsigned int)(x)]
#define JP(x) JP[(unsigned int)(x)]
#define JQ(x) JQ[(unsigned int)(x)]
#define JR(x) JR[(unsigned int)(x)]
#define JS(x) JS[(unsigned int)(x)]
#define JT(x) JT[(unsigned int)(x)]
#define JU(x) JU[(unsigned int)(x)]
#define JV(x) JV[(unsigned int)(x)]
#define JW(x) JW[(unsigned int)(x)]
#define JX(x) JX[(unsigned int)(x)]
#define JY(x) JY[(unsigned int)(x)]
#define JZ(x) JZ[(unsigned int)(x)]
#define KA(x) KA[(unsigned int)(x)]
#define KB(x) KB[(unsigned int)(x)]
#define KC(x) KC[(unsigned int)(x)]
#define KD(x) KD[(unsigned int)(x)]
#define KE(x) KE[(unsigned int)(x)]
#define KF(x) KF[(unsigned int)(x)]
#define KG(x) KG[(unsigned int)(x)]
#define KH(x) KH[(unsigned int)(x)]
#define KI(x) KI[(unsigned int)(x)]
#define KJ(x) KJ[(unsigned int)(x)]
#define KK(x) KK[(unsigned int)(x)]
#define KL(x) KL[(unsigned int)(x)]
#define KM(x) KM[(unsigned int)(x)]
#define KN(x) KN[(unsigned int)(x)]
#define KO(x) KO[(unsigned int)(x)]
#define KP(x) KP[(unsigned int)(x)]
#define KQ(x) KQ[(unsigned int)(x)]
#define KR(x) KR[(unsigned int)(x)]
#define KS(x) KS[(unsigned int)(x)]
#define KT(x) KT[(unsigned int)(x)]
#define KU(x) KU[(unsigned int)(x)]
#define KV(x) KV[(unsigned int)(x)]
#define KW(x) KW[(unsigned int)(x)]
#define KX(x) KX[(unsigned int)(x)]
#define KY(x) KY[(unsigned int)(x)]
#define KZ(x) KZ[(unsigned int)(x)]
#define LA(x) LA[(unsigned int)(x)]
#define LB(x) LB[(unsigned int)(x)]
#define LC(x) LC[(unsigned int)(x)]
#define LD(x) LD[(unsigned int)(x)]
#define LE(x) LE[(unsigned int)(x)]
#define LF(x) LF[(unsigned int)(x)]
#define LG(x) LG[(unsigned int)(x)]
#define LH(x) LH[(unsigned int)(x)]
#define LI(x) LI[(unsigned int)(x)]
#define LJ(x) LJ[(unsigned int)(x)]
#define LK(x) LK[(unsigned int)(x)]
#define LL(x) LL[(unsigned int)(x)]
#define LM(x) LM[(unsigned int)(x)]
#define LN(x) LN[(unsigned int)(x)]
#define LO(x) LO[(unsigned int)(x)]
#define LP(x) LP[(unsigned int)(x)]
#define LQ(x) LQ[(unsigned int)(x)]
#define LR(x) LR[(unsigned int)(x)]
#define LS(x) LS[(unsigned int)(x)]
#define LT(x) LT[(unsigned int)(x)]
#define LU(x) LU[(unsigned int)(x)]
#define LV(x) LV[(unsigned int)(x)]
#define LW(x) LW[(unsigned int)(x)]
#define LX(x) LX[(unsigned int)(x)]
#define LY(x) LY[(unsigned int)(x)]
#define LZ(x) LZ[(unsigned int)(x)]
#define MA(x) MA[(unsigned int)(x)]
#define MB(x) MB[(unsigned int)(x)]
#define MC(x) MC[(unsigned int)(x)]
#define MD(x) MD[(unsigned int)(x)]
#define ME(x) ME[(unsigned int)(x)]
#define MF(x) MF[(unsigned int)(x)]
#define MG(x) MG[(unsigned int)(x)]
#define MH(x) MH[(unsigned int)(x)]
#define MI(x) MI[(unsigned int)(x)]
#define MJ(x) MJ[(unsigned int)(x)]
#define MK(x) MK[(unsigned int)(x)]
#define ML(x) ML[(unsigned int)(x)]
#define MM(x) MM[(unsigned int)(x)]
#define MN(x) MN[(unsigned int)(x)]
#define MO(x) MO[(unsigned int)(x)]
#define MP(x) MP[(unsigned int)(x)]
#define MQ(x) MQ[(unsigned int)(x)]
#define MR(x) MR[(unsigned int)(x)]
#define MS(x) MS[(unsigned int)(x)]
#define MT(x) MT[(unsigned int)(x)]
#define MU(x) MU[(unsigned int)(x)]
#define MV(x) MV[(unsigned int)(x)]
#define MW(x) MW[(unsigned int)(x)]
#define MX(x) MX[(unsigned int)(x)]
#define MY(x) MY[(unsigned int)(x)]
#define MZ(x) MZ[(unsigned int)(x)]
#define NA(x) NA[(unsigned int)(x)]
#define NB(x) NB[(unsigned int)(x)]
#define NC(x) NC[(unsigned int)(x)]
#define ND(x) ND[(unsigned int)(x)]
#define NE(x) NE[(unsigned int)(x)]
#define NF(x) NF[(unsigned int)(x)]
#define NG(x) NG[(unsigned int)(x)]
#define NH(x) NH[(unsigned int)(x)]
#define NI(x) NI[(unsigned int)(x)]
#define NJ(x) NJ[(unsigned int)(x)]
#define NK(x) NK[(unsigned int)(x)]
#define NL(x) NL[(unsigned int)(x)]
#define NM(x) NM[(unsigned int)(x)]
#define NN(x) NN[(unsigned int)(x)]
#define NO(x) NO[(unsigned int)(x)]
#define NP(x) NP[(unsigned int)(x)]
#define NQ(x) NQ[(unsigned int)(x)]
#define NR(x) NR[(unsigned int)(x)]
#define NS(x) NS[(unsigned int)(x)]
#define NT(x) NT[(unsigned int)(x)]
#define NU(x) NU[(unsigned int)(x)]
#define NV(x) NV[(unsigned int)(x)]
#define NW(x) NW[(unsigned int)(x)]
#define NX(x) NX[(unsigned int)(x)]
#define NY(x) NY[(unsigned int)(x)]
#define NZ(x) NZ[(unsigned int)(x)]
#define OA(x) OA[(unsigned int)(x)]
#define OB(x) OB[(unsigned int)(x)]
#define OC(x) OC[(unsigned int)(x)]
#define OD(x) OD[(unsigned int)(x)]
#define OE(x) OE[(unsigned int)(x)]
#define OF(x) OF[(unsigned int)(x)]
#define OG(x) OG[(unsigned int)(x)]
#define OH(x) OH[(unsigned int)(x)]
#define OI(x) OI[(unsigned int)(x)]
#define OJ(x) OJ[(unsigned int)(x)]
#define OK(x) OK[(unsigned int)(x)]
#define OL(x) OL[(unsigned int)(x)]
#define OM(x) OM[(unsigned int)(x)]
#define ON(x) ON[(unsigned int)(x)]
#define OO(x) OO[(unsigned int)(x)]
#define OP(x) OP[(unsigned int)(x)]
#define OQ(x) OQ[(unsigned int)(x)]
#define OS(x) OS[(unsigned int)(x)]
#define OT(x) OT[(unsigned int)(x)]
#define OU(x) OU[(unsigned int)(x)]
#define OV(x) OV[(unsigned int)(x)]
#define OW(x) OW[(unsigned int)(x)]
#define OX(x) OX[(unsigned int)(x)]
#define OY(x) OY[(unsigned int)(x)]
#define OZ(x) OZ[(unsigned int)(x)]
#define PA(x) PA[(unsigned int)(x)]
#define PB(x) PB[(unsigned int)(x)]
#define PC(x) PC[(unsigned int)(x)]
#define PD(x) PD[(unsigned int)(x)]
#define PE(x) PE[(unsigned int)(x)]
#define PF(x) PF[(unsigned int)(x)]
#define PG(x) PG[(unsigned int)(x)]
#define PH(x) PH[(unsigned int)(x)]
#define PI(x) PI[(unsigned int)(x)]
#define PJ(x) PJ[(unsigned int)(x)]
#define PK(x) PK[(unsigned int)(x)]
#define PL(x) PL[(unsigned int)(x)]
#define PM(x) PM[(unsigned int)(x)]
#define PN(x) PN[(unsigned int)(x)]
#define PO(x) PO[(unsigned int)(x)]
#define PP(x) PP[(unsigned int)(x)]
#define PQ(x) PQ[(unsigned int)(x)]
#define PR(x) PR[(unsigned int)(x)]
#define PS(x) PS[(unsigned int)(x)]
#define PT(x) PT[(unsigned int)(x)]
#define PU(x) PU[(unsigned int)(x)]
#define PV(x) PV[(unsigned int)(x)]
#define PW(x) PW[(unsigned int)(x)]
#define PX(x) PX[(unsigned int)(x)]
#define PY(x) PY[(unsigned int)(x)]
#define PZ(x) PZ[(unsigned int)(x)]
#define QA(x) QA[(unsigned int)(x)]
#define QB(x) QB[(unsigned int)(x)]
#define QC(x) QC[(unsigned int)(x)]
#define QD(x) QD[(unsigned int)(x)]
#define QE(x) QE[(unsigned int)(x)]
#define QF(x) QF[(unsigned int)(x)]
#define QG(x) QG[(unsigned int)(x)]
#define QH(x) QH[(unsigned int)(x)]
#define QI(x) QI[(unsigned int)(x)]
#define QJ(x) QJ[(unsigned int)(x)]
#define QK(x) QK[(unsigned int)(x)]
#define QL(x) QL[(unsigned int)(x)]
#define QM(x) QM[(unsigned int)(x)]
#define QN(x) QN[(unsigned int)(x)]
#define QO(x) QO[(unsigned int)(x)]
#define QP(x) QP[(unsigned int)(x)]
#define QQ(x) QQ[(unsigned int)(x)]
#define QR(x) QR[(unsigned int)(x)]
#define QS(x) QS[(unsigned int)(x)]
#define QT(x) QT[(unsigned int)(x)]
#define QU(x) QU[(unsigned int)(x)]
#define QV(x) QV[(unsigned int)(x)]
#define QW(x) QW[(unsigned int)(x)]
#define QX(x) QX[(unsigned int)(x)]
#define QY(x) QY[(unsigned int)(x)]
#define QZ(x) QZ[(unsigned int)(x)]
#define RA(x) RA[(unsigned int)(x)]
#define RB(x) RB[(unsigned int)(x)]
#define RC(x) RC[(unsigned int)(x)]
#define RD(x) RD[(unsigned int)(x)]
#define RE(x) RE[(unsigned int)(x)]
#define RF(x) RF[(unsigned int)(x)]
#define RG(x) RG[(unsigned int)(x)]
#define RH(x) RH[(unsigned int)(x)]
#define RI(x) RI[(unsigned int)(x)]
#define RJ(x) RJ[(unsigned int)(x)]
#define RK(x) RK[(unsigned int)(x)]
#define RL(x) RL[(unsigned int)(x)]
#define RM(x) RM[(unsigned int)(x)]
#define RN(x) RN[(unsigned int)(x)]
#define RO(x) RO[(unsigned int)(x)]
#define RP(x) RP[(unsigned int)(x)]
#define RQ(x) RQ[(unsigned int)(x)]
#define RR(x) RR[(unsigned int)(x)]
#define RS(x) RS[(unsigned int)(x)]
#define RT(x) RT[(unsigned int)(x)]
#define RU(x) RU[(unsigned int)(x)]
#define RV(x) RV[(unsigned int)(x)]
#define RW(x) RW[(unsigned int)(x)]
#define RX(x) RX[(unsigned int)(x)]
#define RY(x) RY[(unsigned int)(x)]
#define RZ(x) RZ[(unsigned int)(x)]
#define SA(x) SA[(unsigned int)(x)]
#define SB(x) SB[(unsigned int)(x)]
#define SC(x) SC[(unsigned int)(x)]
#define SD(x) SD[(unsigned int)(x)]
#define SE(x) SE[(unsigned int)(x)]
#define SF(x) SF[(unsigned int)(x)]
#define SG(x) SG[(unsigned int)(x)]
#define SH(x) SH[(unsigned int)(x)]
#define SI(x) SI[(unsigned int)(x)]
#define SJ(x) SJ[(unsigned int)(x)]
#define SK(x) SK[(unsigned int)(x)]
#define SL(x) SL[(unsigned int)(x)]
#define SM(x) SM[(unsigned int)(x)]
#define SN(x) SN[(unsigned int)(x)]
#define SO(x) SO[(unsigned int)(x)]
#define SP(x) SP[(unsigned int)(x)]
#define SQ(x) SQ[(unsigned int)(x)]
#define SR(x) SR[(unsigned int)(x)]
#define SS(x) SS[(unsigned int)(x)]
#define ST(x) ST[(unsigned int)(x)]
#define SU(x) SU[(unsigned int)(x)]
#define SV(x) SV[(unsigned int)(x)]
#define SW(x) SW[(unsigned int)(x)]
#define SX(x) SX[(unsigned int)(x)]
#define SY(x) SY[(unsigned int)(x)]
#define SZ(x) SZ[(unsigned int)(x)]
#define TA(x) TA[(unsigned int)(x)]
#define TB(x) TB[(unsigned int)(x)]
#define TC(x) TC[(unsigned int)(x)]
#define TD(x) TD[(unsigned int)(x)]
#define TE(x) TE[(unsigned int)(x)]
#define TF(x) TF[(unsigned int)(x)]
#define TG(x) TG[(unsigned int)(x)]
#define TH(x) TH[(unsigned int)(x)]
#define TI(x) TI[(unsigned int)(x)]
#define TJ(x) TJ[(unsigned int)(x)]
#define TK(x) TK[(unsigned int)(x)]
#define TL(x) TL[(unsigned int)(x)]
#define TM(x) TM[(unsigned int)(x)]
#define TN(x) TN[(unsigned int)(x)]
#define TP(x) TP[(unsigned int)(x)]
#define TQ(x) TQ[(unsigned int)(x)]
#define TR(x) TR[(unsigned int)(x)]
#define TS(x) TS[(unsigned int)(x)]
#define TT(x) TT[(unsigned int)(x)]
#define TU(x) TU[(unsigned int)(x)]
#define TV(x) TV[(unsigned int)(x)]
#define TW(x) TW[(unsigned int)(x)]
#define TX(x) TX[(unsigned int)(x)]
#define TY(x) TY[(unsigned int)(x)]
#define TZ(x) TZ[(unsigned int)(x)]
#define UA(x) UA[(unsigned int)(x)]
#define UB(x) UB[(unsigned int)(x)]
#define UC(x) UC[(unsigned int)(x)]
#define UD(x) UD[(unsigned int)(x)]
#define UE(x) UE[(unsigned int)(x)]
#define UF(x) UF[(unsigned int)(x)]
#define UG(x) UG[(unsigned int)(x)]
#define UH(x) UH[(unsigned int)(x)]
#define UI(x) UI[(unsigned int)(x)]
#define UJ(x) UJ[(unsigned int)(x)]
#define UK(x) UK[(unsigned int)(x)]
#define UL(x) UL[(unsigned int)(x)]
#define UM(x) UM[(unsigned int)(x)]
#define UN(x) UN[(unsigned int)(x)]
#define UO(x) UO[(unsigned int)(x)]
#define UP(x) UP[(unsigned int)(x)]
#define UQ(x) UQ[(unsigned int)(x)]
#define UR(x) UR[(unsigned int)(x)]
#define US(x) US[(unsigned int)(x)]
#define UT(x) UT[(unsigned int)(x)]
#define UU(x) UU[(unsigned int)(x)]
#define UV(x) UV[(unsigned int)(x)]
#define UW(x) UW[(unsigned int)(x)]
#define UX(x) UX[(unsigned int)(x)]
#define UY(x) UY[(unsigned int)(x)]
#define UZ(x) UZ[(unsigned int)(x)]
#define VA(x) VA[(unsigned int)(x)]
#define VB(x) VB[(unsigned int)(x)]
#define VC(x) VC[(unsigned int)(x)]
#define VD(x) VD[(unsigned int)(x)]
#define VE(x) VE[(unsigned int)(x)]
#define VF(x) VF[(unsigned int)(x)]
#define VG(x) VG[(unsigned int)(x)]
#define VH(x) VH[(unsigned int)(x)]
#define VI(x) VI[(unsigned int)(x)]
#define VJ(x) VJ[(unsigned int)(x)]
#define VK(x) VK[(unsigned int)(x)]
#define VL(x) VL[(unsigned int)(x)]
#define VM(x) VM[(unsigned int)(x)]
#define VN(x) VN[(unsigned int)(x)]
#define VO(x) VO[(unsigned int)(x)]
#define VP(x) VP[(unsigned int)(x)]
#define VQ(x) VQ[(unsigned int)(x)]
#define VR(x) VR[(unsigned int)(x)]
#define VS(x) VS[(unsigned int)(x)]
#define VT(x) VT[(unsigned int)(x)]
#define VU(x) VU[(unsigned int)(x)]
#define VV(x) VV[(unsigned int)(x)]
#define VW(x) VW[(unsigned int)(x)]
#define VX(x) VX[(unsigned int)(x)]
#define VY(x) VY[(unsigned int)(x)]
#define VZ(x) VZ[(unsigned int)(x)]
#define WA(x) WA[(unsigned int)(x)]
#define WB(x) WB[(unsigned int)(x)]
#define WC(x) WC[(unsigned int)(x)]
#define WD(x) WD[(unsigned int)(x)]
#define WE(x) WE[(unsigned int)(x)]
#define WF(x) WF[(unsigned int)(x)]
#define WG(x) WG[(unsigned int)(x)]
#define WH(x) WH[(unsigned int)(x)]
#define WI(x) WI[(unsigned int)(x)]
#define WJ(x) WJ[(unsigned int)(x)]
#define WK(x) WK[(unsigned int)(x)]
#define WL(x) WL[(unsigned int)(x)]
#define WM(x) WM[(unsigned int)(x)]
#define WN(x) WN[(unsigned int)(x)]
#define WO(x) WO[(unsigned int)(x)]
#define WP(x) WP[(unsigned int)(x)]
#define WQ(x) WQ[(unsigned int)(x)]
#define WR(x) WR[(unsigned int)(x)]
#define WS(x) WS[(unsigned int)(x)]
#define WT(x) WT[(unsigned int)(x)]
#define WU(x) WU[(unsigned int)(x)]
#define WV(x) WV[(unsigned int)(x)]
#define WW(x) WW[(unsigned int)(x)]
#define WX(x) WX[(unsigned int)(x)]
#define WY(x) WY[(unsigned int)(x)]
#define WZ(x) WZ[(unsigned int)(x)]
#define XA(x) XA[(unsigned int)(x)]
#define XB(x) XB[(unsigned int)(x)]
#define XC(x) XC[(unsigned int)(x)]
#define XD(x) XD[(unsigned int)(x)]
#define XE(x) XE[(unsigned int)(x)]
#define XF(x) XF[(unsigned int)(x)]
#define XG(x) XG[(unsigned int)(x)]
#define XH(x) XH[(unsigned int)(x)]
#define XI(x) XI[(unsigned int)(x)]
#define XJ(x) XJ[(unsigned int)(x)]
#define XK(x) XK[(unsigned int)(x)]
#define XL(x) XL[(unsigned int)(x)]
#define XM(x) XM[(unsigned int)(x)]
#define XN(x) XN[(unsigned int)(x)]
#define XO(x) XO[(unsigned int)(x)]
#define XP(x) XP[(unsigned int)(x)]
#define XQ(x) XQ[(unsigned int)(x)]
#define XR(x) XR[(unsigned int)(x)]
#define XS(x) XS[(unsigned int)(x)]
#define XT(x) XT[(unsigned int)(x)]
#define XU(x) XU[(unsigned int)(x)]
#define XV(x) XV[(unsigned int)(x)]
#define XW(x) XW[(unsigned int)(x)]
#define XX(x) XX[(unsigned int)(x)]
#define XY(x) XY[(unsigned int)(x)]
#define XZ(x) XZ[(unsigned int)(x)]
#define YA(x) YA[(unsigned int)(x)]
#define YB(x) YB[(unsigned int)(x)]
#define YC(x) YC[(unsigned int)(x)]
#define YD(x) YD[(unsigned int)(x)]
#define YE(x) YE[(unsigned int)(x)]
#define YF(x) YF[(unsigned int)(x)]
#define YG(x) YG[(unsigned int)(x)]
#define YH(x) YH[(unsigned int)(x)]
#define YI(x) YI[(unsigned int)(x)]
#define YJ(x) YJ[(unsigned int)(x)]
#define YK(x) YK[(unsigned int)(x)]
#define YL(x) YL[(unsigned int)(x)]
#define YM(x) YM[(unsigned int)(x)]
#define YN(x) YN[(unsigned int)(x)]
#define YO(x) YO[(unsigned int)(x)]
#define YP(x) YP[(unsigned int)(x)]
#define YQ(x) YQ[(unsigned int)(x)]
#define YR(x) YR[(unsigned int)(x)]
#define YS(x) YS[(unsigned int)(x)]
#define YT(x) YT[(unsigned int)(x)]
#define YU(x) YU[(unsigned int)(x)]
#define YV(x) YV[(unsigned int)(x)]
#define YW(x) YW[(unsigned int)(x)]
#define YX(x) YX[(unsigned int)(x)]
#define YY(x) YY[(unsigned int)(x)]
#define YZ(x) YZ[(unsigned int)(x)]
#define ZA(x) ZA[(unsigned int)(x)]
#define ZB(x) ZB[(unsigned int)(x)]
#define ZC(x) ZC[(unsigned int)(x)]
#define ZD(x) ZD[(unsigned int)(x)]
#define ZE(x) ZE[(unsigned int)(x)]
#define ZF(x) ZF[(unsigned int)(x)]
#define ZG(x) ZG[(unsigned int)(x)]
#define ZH(x) ZH[(unsigned int)(x)]
#define ZI(x) ZI[(unsigned int)(x)]
#define ZJ(x) ZJ[(unsigned int)(x)]
#define ZK(x) ZK[(unsigned int)(x)]
#define ZL(x) ZL[(unsigned int)(x)]
#define ZM(x) ZM[(unsigned int)(x)]
#define ZN(x) ZN[(unsigned int)(x)]
#define ZO(x) ZO[(unsigned int)(x)]
#define ZP(x) ZP[(unsigned int)(x)]
#define ZQ(x) ZQ[(unsigned int)(x)]
#define ZR(x) ZR[(unsigned int)(x)]
#define ZS(x) ZS[(unsigned int)(x)]
#define ZT(x) ZT[(unsigned int)(x)]
#define ZU(x) ZU[(unsigned int)(x)]
#define ZV(x) ZV[(unsigned int)(x)]
#define ZW(x) ZW[(unsigned int)(x)]
#define ZX(x) ZX[(unsigned int)(x)]
#define ZY(x) ZY[(unsigned int)(x)]
#define ZZ(x) ZZ[(unsigned int)(x)]
#define A0(x) A0[(unsigned int)(x)]
#define A1(x) A1[(unsigned int)(x)]
#define A2(x) A2[(unsigned int)(x)]
#define A3(x) A3[(unsigned int)(x)]
#define A4(x) A4[(unsigned int)(x)]
#define A5(x) A5[(unsigned int)(x)]
#define A6(x) A6[(unsigned int)(x)]
#define A7(x) A7[(unsigned int)(x)]
#define A8(x) A8[(unsigned int)(x)]
#define A9(x) A9[(unsigned int)(x)]
#define B0(x) B0[(unsigned int)(x)]
#define B1(x) B1[(unsigned int)(x)]
#define B2(x) B2[(unsigned int)(x)]
#define B3(x) B3[(unsigned int)(x)]
#define B4(x) B4[(unsigned int)(x)]
#define B5(x) B5[(unsigned int)(x)]
#define B6(x) B6[(unsigned int)(x)]
#define B7(x) B7[(unsigned int)(x)]
#define B8(x) B8[(unsigned int)(x)]
#define B9(x) B9[(unsigned int)(x)]
#define C0(x) C0[(unsigned int)(x)]
#define C1(x) C1[(unsigned int)(x)]
#define C2(x) C2[(unsigned int)(x)]
#define C3(x) C3[(unsigned int)(x)]
#define C4(x) C4[(unsigned int)(x)]
#define C5(x) C5[(unsigned int)(x)]
#define C6(x) C6[(unsigned int)(x)]
#define C7(x) C7[(unsigned int)(x)]
#define C8(x) C8[(unsigned int)(x)]
#define C9(x) C9[(unsigned int)(x)]
#define D0(x) D0[(unsigned int)(x)]
#define D1(x) D1[(unsigned int)(x)]
#define D2(x) D2[(unsigned int)(x)]
#define D3(x) D3[(unsigned int)(x)]
#define D4(x) D4[(unsigned int)(x)]
#define D5(x) D5[(unsigned int)(x)]
#define D6(x) D6[(unsigned int)(x)]
#define D7(x) D7[(unsigned int)(x)]
#define D8(x) D8[(unsigned int)(x)]
#define D9(x) D9[(unsigned int)(x)]
#define F0(x) F0[(unsigned int)(x)]
#define F1(x) F1[(unsigned int)(x)]
#define F2(x) F2[(unsigned int)(x)]
#define F3(x) F3[(unsigned int)(x)]
#define F4(x) F4[(unsigned int)(x)]
#define F5(x) F5[(unsigned int)(x)]
#define F6(x) F6[(unsigned int)(x)]
#define F7(x) F7[(unsigned int)(x)]
#define F8(x) F8[(unsigned int)(x)]
#define F9(x) F9[(unsigned int)(x)]
#define G0(x) G0[(unsigned int)(x)]
#define G1(x) G1[(unsigned int)(x)]
#define G2(x) G2[(unsigned int)(x)]
#define G3(x) G3[(unsigned int)(x)]
#define G4(x) G4[(unsigned int)(x)]
#define G5(x) G5[(unsigned int)(x)]
#define G6(x) G6[(unsigned int)(x)]
#define G7(x) G7[(unsigned int)(x)]
#define G8(x) G8[(unsigned int)(x)]
#define G9(x) G9[(unsigned int)(x)]
#define H0(x) H0[(unsigned int)(x)]
#define H1(x) H1[(unsigned int)(x)]
#define H2(x) H2[(unsigned int)(x)]
#define H3(x) H3[(unsigned int)(x)]
#define H4(x) H4[(unsigned int)(x)]
#define H5(x) H5[(unsigned int)(x)]
#define H6(x) H6[(unsigned int)(x)]
#define H7(x) H7[(unsigned int)(x)]
#define H8(x) H8[(unsigned int)(x)]
#define H9(x) H9[(unsigned int)(x)]
#define I0(x) I0[(unsigned int)(x)]
#define I1(x) I1[(unsigned int)(x)]
#define I2(x) I2[(unsigned int)(x)]
#define I3(x) I3[(unsigned int)(x)]
#define I4(x) I4[(unsigned int)(x)]
#define I5(x) I5[(unsigned int)(x)]
#define I6(x) I6[(unsigned int)(x)]
#define I7(x) I7[(unsigned int)(x)]
#define I8(x) I8[(unsigned int)(x)]
#define I9(x) I9[(unsigned int)(x)]
#define J0(x) J0[(unsigned int)(x)]
#define J1(x) J1[(unsigned int)(x)]
#define J2(x) J2[(unsigned int)(x)]
#define J3(x) J3[(unsigned int)(x)]
#define J4(x) J4[(unsigned int)(x)]
#define J5(x) J5[(unsigned int)(x)]
#define J6(x) J6[(unsigned int)(x)]
#define J7(x) J7[(unsigned int)(x)]
#define J8(x) J8[(unsigned int)(x)]
#define J9(x) J9[(unsigned int)(x)]
#define K0(x) K0[(unsigned int)(x)]
#define K1(x) K1[(unsigned int)(x)]
#define K2(x) K2[(unsigned int)(x)]
#define K3(x) K3[(unsigned int)(x)]
#define K4(x) K4[(unsigned int)(x)]
#define K5(x) K5[(unsigned int)(x)]
#define K6(x) K6[(unsigned int)(x)]
#define K7(x) K7[(unsigned int)(x)]
#define K8(x) K8[(unsigned int)(x)]
#define K9(x) K9[(unsigned int)(x)]
#define L0(x) L0[(unsigned int)(x)]
#define L1(x) L1[(unsigned int)(x)]
#define L2(x) L2[(unsigned int)(x)]
#define L3(x) L3[(unsigned int)(x)]
#define L4(x) L4[(unsigned int)(x)]
#define L5(x) L5[(unsigned int)(x)]
#define L6(x) L6[(unsigned int)(x)]
#define L7(x) L7[(unsigned int)(x)]
#define L8(x) L8[(unsigned int)(x)]
#define L9(x) L9[(unsigned int)(x)]
#define M0(x) M0[(unsigned int)(x)]
#define M1(x) M1[(unsigned int)(x)]
#define M2(x) M2[(unsigned int)(x)]
#define M3(x) M3[(unsigned int)(x)]
#define M4(x) M4[(unsigned int)(x)]
#define M5(x) M5[(unsigned int)(x)]
#define M6(x) M6[(unsigned int)(x)]
#define M7(x) M7[(unsigned int)(x)]
#define M8(x) M8[(unsigned int)(x)]
#define M9(x) M9[(unsigned int)(x)]
#define N0(x) N0[(unsigned int)(x)]
#define N1(x) N1[(unsigned int)(x)]
#define N2(x) N2[(unsigned int)(x)]
#define N3(x) N3[(unsigned int)(x)]
#define N4(x) N4[(unsigned int)(x)]
#define N5(x) N5[(unsigned int)(x)]
#define N6(x) N6[(unsigned int)(x)]
#define N7(x) N7[(unsigned int)(x)]
#define N8(x) N8[(unsigned int)(x)]
#define N9(x) N9[(unsigned int)(x)]
#define O0(x) O0[(unsigned int)(x)]
#define O1(x) O1[(unsigned int)(x)]
#define O2(x) O2[(unsigned int)(x)]
#define O3(x) O3[(unsigned int)(x)]
#define O4(x) O4[(unsigned int)(x)]
#define O5(x) O5[(unsigned int)(x)]
#define O6(x) O6[(unsigned int)(x)]
#define O7(x) O7[(unsigned int)(x)]
#define O8(x) O8[(unsigned int)(x)]
#define O9(x) O9[(unsigned int)(x)]
#define P0(x) P0[(unsigned int)(x)]
#define P1(x) P1[(unsigned int)(x)]
#define P2(x) P2[(unsigned int)(x)]
#define P3(x) P3[(unsigned int)(x)]
#define P4(x) P4[(unsigned int)(x)]
#define P5(x) P5[(unsigned int)(x)]
#define P6(x) P6[(unsigned int)(x)]
#define P7(x) P7[(unsigned int)(x)]
#define P8(x) P8[(unsigned int)(x)]
#define P9(x) P9[(unsigned int)(x)]
#define Q0(x) Q0[(unsigned int)(x)]
#define Q1(x) Q1[(unsigned int)(x)]
#define Q2(x) Q2[(unsigned int)(x)]
#define Q3(x) Q3[(unsigned int)(x)]
#define Q4(x) Q4[(unsigned int)(x)]
#define Q5(x) Q5[(unsigned int)(x)]
#define Q6(x) Q6[(unsigned int)(x)]
#define Q7(x) Q7[(unsigned int)(x)]
#define Q8(x) Q8[(unsigned int)(x)]
#define Q9(x) Q9[(unsigned int)(x)]
#define R0(x) R0[(unsigned int)(x)]
#define R1(x) R1[(unsigned int)(x)]
#define R2(x) R2[(unsigned int)(x)]
#define R3(x) R3[(unsigned int)(x)]
#define R4(x) R4[(unsigned int)(x)]
#define R5(x) R5[(unsigned int)(x)]
#define R6(x) R6[(unsigned int)(x)]
#define R7(x) R7[(unsigned int)(x)]
#define R8(x) R8[(unsigned int)(x)]
#define R9(x) R9[(unsigned int)(x)]
#define S0(x) S0[(unsigned int)(x)]
#define S1(x) S1[(unsigned int)(x)]
#define S2(x) S2[(unsigned int)(x)]
#define S3(x) S3[(unsigned int)(x)]
#define S4(x) S4[(unsigned int)(x)]
#define S5(x) S5[(unsigned int)(x)]
#define S6(x) S6[(unsigned int)(x)]
#define S7(x) S7[(unsigned int)(x)]
#define S8(x) S8[(unsigned int)(x)]
#define S9(x) S9[(unsigned int)(x)]
#define T0(x) T0[(unsigned int)(x)]
#define T1(x) T1[(unsigned int)(x)]
#define T2(x) T2[(unsigned int)(x)]
#define T3(x) T3[(unsigned int)(x)]
#define T4(x) T4[(unsigned int)(x)]
#define T5(x) T5[(unsigned int)(x)]
#define T6(x) T6[(unsigned int)(x)]
#define T7(x) T7[(unsigned int)(x)]
#define T8(x) T8[(unsigned int)(x)]
#define T9(x) T9[(unsigned int)(x)]
#define U0(x) U0[(unsigned int)(x)]
#define U1(x) U1[(unsigned int)(x)]
#define U2(x) U2[(unsigned int)(x)]
#define U3(x) U3[(unsigned int)(x)]
#define U4(x) U4[(unsigned int)(x)]
#define U5(x) U5[(unsigned int)(x)]
#define U6(x) U6[(unsigned int)(x)]
#define U7(x) U7[(unsigned int)(x)]
#define U8(x) U8[(unsigned int)(x)]
#define U9(x) U9[(unsigned int)(x)]
#define V0(x) V0[(unsigned int)(x)]
#define V1(x) V1[(unsigned int)(x)]
#define V2(x) V2[(unsigned int)(x)]
#define V3(x) V3[(unsigned int)(x)]
#define V4(x) V4[(unsigned int)(x)]
#define V5(x) V5[(unsigned int)(x)]
#define V6(x) V6[(unsigned int)(x)]
#define V7(x) V7[(unsigned int)(x)]
#define V8(x) V8[(unsigned int)(x)]
#define V9(x) V9[(unsigned int)(x)]
#define W0(x) W0[(unsigned int)(x)]
#define W1(x) W1[(unsigned int)(x)]
#define W2(x) W2[(unsigned int)(x)]
#define W3(x) W3[(unsigned int)(x)]
#define W4(x) W4[(unsigned int)(x)]
#define W5(x) W5[(unsigned int)(x)]
#define W6(x) W6[(unsigned int)(x)]
#define W7(x) W7[(unsigned int)(x)]
#define W8(x) W8[(unsigned int)(x)]
#define W9(x) W9[(unsigned int)(x)]
#define X0(x) X0[(unsigned int)(x)]
#define X1(x) X1[(unsigned int)(x)]
#define X2(x) X2[(unsigned int)(x)]
#define X3(x) X3[(unsigned int)(x)]
#define X4(x) X4[(unsigned int)(x)]
#define X5(x) X5[(unsigned int)(x)]
#define X6(x) X6[(unsigned int)(x)]
#define X7(x) X7[(unsigned int)(x)]
#define X8(x) X8[(unsigned int)(x)]
#define X9(x) X9[(unsigned int)(x)]
#define Y0(x) Y0[(unsigned int)(x)]
#define Y1(x) Y1[(unsigned int)(x)]
#define Y2(x) Y2[(unsigned int)(x)]
#define Y3(x) Y3[(unsigned int)(x)]
#define Y4(x) Y4[(unsigned int)(x)]
#define Y5(x) Y5[(unsigned int)(x)]
#define Y6(x) Y6[(unsigned int)(x)]
#define Y7(x) Y7[(unsigned int)(x)]
#define Y8(x) Y8[(unsigned int)(x)]
#define Y9(x) Y9[(unsigned int)(x)]
#define Z0(x) Z0[(unsigned int)(x)]
#define Z1(x) Z1[(unsigned int)(x)]
#define Z2(x) Z2[(unsigned int)(x)]
#define Z3(x) Z3[(unsigned int)(x)]
#define Z4(x) Z4[(unsigned int)(x)]
#define Z5(x) Z5[(unsigned int)(x)]
#define Z6(x) Z6[(unsigned int)(x)]
#define Z7(x) Z7[(unsigned int)(x)]
#define Z8(x) Z8[(unsigned int)(x)]
#define Z9(x) Z9[(unsigned int)(x)]

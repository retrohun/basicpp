#if 0
c++ -pipe -xc++ -fpermissive -w -o "${0%.*}".exe "$0" && ./"${0%.*}.exe" ; exit $?
#endif

#include "ptsvubas.cc"

BEGINBASIC(int,main,())
10 DIM C(100);
20 DIM D(100);
30 PRINT "YET ANOTHER GAME OF LIFE IMPLEMENTATION";
40 PRINT " BY NASZVADI, PETER. 2017, FOR BASIC++";
50 FOR I=0 TO 99;
60 LET C(I)=0;
70 NEXT I;
80 GOSUB 430;
90 PRINT "1-100 TOGGLE CELL, 0=NEXTGEN, 222=QUITS";
100 INPUT A;
110 IF (A>0) AND (A<=100) THEN 350;
120 IF A==0 THEN 150;
130 IF A==222 THEN 370;
140 GOTO 80;
150 FOR I=0 TO 99;
160 LET B=-C(I);
170 FOR J=-1 TO 1;
180 FOR K=-1 TO 1;
190 LET F=10*J+I+K;
200 IF F>=0 THEN 240;
210 LET F=F+100;
220 IF F<100 THEN 240;
230 LET F=F-100;
240 LET B=2*C(F)+B;
250 NEXT K;
260 NEXT J;
270 IF B<5 THEN 310;
280 IF B>7 THEN 310;
290 LET D(I)=1;
300 GOTO 320;
310 LET D(I)=0;
320 NEXT I;
330 GOSUB 390;
340 GOTO 80;
350 LET C(A-1)=1-C(A-1);
360 GOTO 80;
370 PRINT "EXITING, HAVE A NICE DAY!";
380 END;
390 FOR I=0 TO 99;
400 LET C(I)=D(I);
410 NEXT I;
420 RETURN;
430 FOR I=0 TO 9;
440 FOR J=0 TO 9;
450 IF C(I*10+J)<1 THEN 480;
460 PRINT "X",CHR$(0);
470 GOTO 490;
480 PRINT ".",CHR$(0);
490 NEXT J;
500 PRINT "";
510 NEXT I;
520 RETURN;
ENDBASIC

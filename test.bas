#if 0
c++ -pipe -xc++ -fpermissive -w -o "${0%.*}".exe "$0" && ./"${0%.*}.exe" ; exit $?
#endif
#include "ptsvubas.cc"

BEGINBASIC(int,main,())
10 DIM A(5);
20 PRINT "FULL BASIC DEMO, 2017";
30 PRINT "ENTER LOOP"," LENGTH:",CHR$(0);
40 INPUT L;
50 IF L<5 THEN 80;
60 PRINT "ERROR! L is at least 5, namely: '", L, "'; Retry please!";
70 GOTO 30;
80 FOR I=1 TO L;
90 PRINT "OLD VALUE=", A(I), CHR$(0);
100 LET A(I)=I*I;
110 PRINT ", NEW VALUE=", A(I);
120 NEXT I;
130 GOSUB 160;
140 PRINT "THE END.";
150 GOTO 200;
160 PRINT "THIS IS A SUBROUTINE!";
170 RETURN;
200 END 0;
ENDBASIC

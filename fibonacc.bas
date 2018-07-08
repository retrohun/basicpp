#if(0)
c++ -pipe -xc++ -fpermissive -w -o "${0%.*}.exe" "$0" && ./"${0%.*}.exe" ; exit $?
#endif
#include "ptsvubas.cc"

BEGINBASIC(int,main,())
10 PRINT "Fibonacci series calculator in BASIC";
20 PRINT "What is the upper bound?";
30 INPUT D;
35 LET I=1;
40 LET A=0;
50 LET B=1;
60 IF I>D THEN 130;
70 LET C=B+A;
80 LET A=B;
90 LET B=C;
100 PRINT I,"th number is: ", A;
110 LET I=I+1;
120 GOTO 60;
130 END;
ENDBASIC

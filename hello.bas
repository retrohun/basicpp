#if 0
c++ -pipe -xc++ -fpermissive -w -o "${0%.*}".exe "$0" && ./"${0%.*}.exe" ; exit $?
#endif
#include "ptsvubas.cc"

BEGINBASIC(int,main,(int argc, char* argv[]))
10 PRINT "Hello, world!";
20 GOTO 40;
30 PRINT "I am not printed";
40 END;
ENDBASIC

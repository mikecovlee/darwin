#/bin/sh
rm ./headers/*.gch
rm ./sources/*.gch
rm ./tests/*.gch
astyle ./headers/*.*
astyle ./sources/*.*
astyle ./tests/*.*
astyle -A4 -N -t ./headers/*.*
astyle -A4 -N -t ./sources/*.*
astyle -A4 -N -t ./tests/*.*
rm ./headers/*.orig
rm ./sources/*.orig
rm ./tests/*.orig
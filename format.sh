#/bin/sh
rm ./examples/*.gch
rm ./sources/*.gch
rm ./tests/*.gch
astyle ./examples/*.*
astyle ./sources/*.*
astyle ./tests/*.*
rm ./examples/*.orig
rm ./sources/*.orig
rm ./tests/*.orig
astyle -A4 -N -t ./examples/*.*
astyle -A4 -N -t ./sources/*.*
astyle -A4 -N -t ./tests/*.*
rm ./examples/*.orig
rm ./sources/*.orig
rm ./tests/*.orig
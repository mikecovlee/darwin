#/bin/sh
rm ./examples/*.gch
rm ./darwin/*.gch
rm ./tests/*.gch
astyle ./examples/*.*
astyle ./darwin/*.*
astyle ./tests/*.*
rm ./examples/*.orig
rm ./darwin/*.orig
rm ./tests/*.orig
astyle -A4 -N -t ./examples/*.*
astyle -A4 -N -t ./darwin/*.*
astyle -A4 -N -t ./tests/*.*
rm ./examples/*.orig
rm ./darwin/*.orig
rm ./tests/*.orig
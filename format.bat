@del /F /Q .\examples\*.gch
@del /F /Q .\darwin\*.gch
@del /F /Q .\tests\*.gch
@astyle .\examples\*.*
@astyle .\darwin\*.*
@astyle .\tests\*.*
@del /F /Q .\examples\*.orig
@del /F /Q .\darwin\*.orig
@del /F /Q .\tests\*.orig
@astyle -A4 -N -t .\examples\*.*
@astyle -A4 -N -t .\darwin\*.*
@astyle -A4 -N -t .\tests\*.*
@del /F /Q .\examples\*.orig
@del /F /Q .\darwin\*.orig
@del /F /Q .\tests\*.orig
@del /F /Q .\examples\*.gch
@del /F /Q .\sources\*.gch
@del /F /Q .\tests\*.gch
@astyle .\examples\*.*
@astyle .\sources\*.*
@astyle .\tests\*.*
@del /F /Q .\examples\*.orig
@del /F /Q .\sources\*.orig
@del /F /Q .\tests\*.orig
@astyle -A4 -N -t .\examples\*.*
@astyle -A4 -N -t .\sources\*.*
@astyle -A4 -N -t .\tests\*.*
@del /F /Q .\examples\*.orig
@del /F /Q .\sources\*.orig
@del /F /Q .\tests\*.orig
@echo off
if not exist build mkdir build

cd build
echo Building...

cmake .. -G "MinGW Makefiles"
mingw32-make

copy HappySquare.exe ..\HappySquare.exe
cd ..

echo Сборка завершена успешно!
pause

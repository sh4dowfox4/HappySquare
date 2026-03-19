#!/bin/bash

mkdir -p build
cd build

echo "Сборка..."
cmake ..
make

cp HappySquare ../
cd ..

echo "Сборка завершена!"
echo "Исполняемый файл находится в: $(pwd)/HappySquare"

# Scrabble en C++

Proyecto final de Introducción a la Programación.
Versión del juego Scrabble en castellano para 2 jugadores, en consola.

## Integrantes
- Daniella Sofía Arias Henríquez
- Tomás Alejandro Ramírez Reina
- Jhon Jairo Ramos

Profesor: Edward Andrés Cortés Peña

## Compilar y ejecutar
g++ -std=c++11 -Wall -o scrabble scrabble.cpp
./scrabble        (en Windows: scrabble.exe)

## Archivos
- scrabble.cpp: código fuente (guardar en UTF-8).
- registro.txt: lo crea el programa; guarda ganadas, perdidas y empatadas de cada jugador.

## Reglas de diseño
- Sin apuntadores ni paso por referencia.
- Datos del juego en variables globales; funciones con parámetros por valor.
- Sin diccionario: el jugador rival aprueba cada palabra formada.
#include <iostream>
#include <string>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// Constantes del sistema
const int TAMANOTABLERO = 15;
const int CENTRO = 7;
const int TOTALFICHAS = 98;
const int TOTALPORTAFICHAS = 7;
const int CANTMAXREGISTRO = 100;
const int MAXCANTPALABRAS = 15;
const int MAXPALABRASFORMADAS = 16;

// Estructuras
struct Ficha{
    string letra;
    int valor;
};

struct Jugador{
    string nombrejug;
    int puntajejug;
    Ficha Atril[TOTALPORTAFICHAS];
    int CantFichasAct;
    int TurnosSaltados;
};

struct RegistorJugador{
    string nombre;
    int ganadas;
    int perdidas;
    int empatadas;
};

// Variables globales

// Prototipos

// main
int main(){
    #ifdef _WIN32
        SetConsoleOutputCP(65001);
    #endif

    cout << "=========================================" << endl;
    cout << "         SCRABBLE EN CASTELLANO" << endl;
    cout << "=========================================" << endl;

    cout << "\nGracias por jugar Scrabble. Hasta pronto!" << endl;
    return 0;
}
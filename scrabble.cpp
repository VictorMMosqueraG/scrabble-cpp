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
int LeerOpcion(int minimo, int maximo);

// main
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif

    cout << "=========================================" << endl;
    cout << "         SCRABBLE EN CASTELLANO" << endl;
    cout << "=========================================" << endl;

    int opcion = 0;
    while (opcion != 4) {
        cout << "\n============ MENU PRINCIPAL ============" << endl;
        cout << "1. Cambiar nombre de jugadores" << endl;
        cout << "2. Iniciar juego" << endl;
        cout << "3. Mostrar registro de resultados" << endl;
        cout << "4. Salir del juego" << endl;
        cout << "Opcion: ";
        opcion = LeerOpcion(1, 4);

        if (opcion == 1) {
            cout << "Opcion en construccion." << endl;
        } else if (opcion == 2) {
            cout << "Opcion en construccion." << endl;
        } else if (opcion == 3) {
            cout << "Opcion en construccion." << endl;
        }
    }

    cout << "\nGracias por jugar Scrabble. Hasta pronto!" << endl;
    return 0;
}

//Lee un numero entre minimo y maximo
int LeerOpcion(int minimo, int maximo) {
    int opcion;
    bool valida = false;
    while (!valida) {
        cin >> opcion;
        if (cin.eof()) {
            cout << "\nFin de la entrada. Se cierra el programa." << endl;
            exit(0);
        }
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Debe escribir un numero entre " << minimo << " y " << maximo << ": ";
        } else {
            cin.ignore(10000, '\n');
            if (opcion < minimo || opcion > maximo) {
                cout << "Opcion fuera de rango. Escriba un numero entre "
                     << minimo << " y " << maximo << ": ";
            } else {
                valida = true;
            }
        }
    }
    return opcion;
}
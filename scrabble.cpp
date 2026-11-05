#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
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

struct RegistroJugador{
    string nombre;
    int ganadas;
    int perdidas;
    int empatadas;
};

// Variables globales
RegistroJugador Registro[CANTMAXREGISTRO];
int CantRegistrados = 0;

// Prototipos
int LeerOpcion(int minimo, int maximo);
string NormalizarTexto(string texto);
bool CargarRegistro();
void GuardarRegistro();
int BuscarJugadorRegistro(string nombre);

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

// Normalizar texto de mayusculas, sin tildes ni dieresis, Ñ unificada
string NormalizarTexto(string texto) {
    string salida = "";
    int i = 0;
    int n = texto.length();
    while (i < n) {
        unsigned char c = texto[i];
        unsigned char sig = 0;
        if (i + 1 < n) sig = texto[i + 1];

        if (c == 0xC3 && sig != 0) {
            if (sig == 0xA1 || sig == 0x81) salida += 'A';
            else if (sig == 0xA9 || sig == 0x89) salida += 'E';
            else if (sig == 0xAD || sig == 0x8D) salida += 'I';
            else if (sig == 0xB3 || sig == 0x93) salida += 'O';
            else if (sig == 0xBA || sig == 0x9A || sig == 0xBC || sig == 0x9C) salida += 'U';
            else if (sig == 0xB1 || sig == 0x91) salida += "Ñ";
            else {
                salida += texto[i];
                salida += texto[i + 1];
            }
            i += 2;
        } else {
            if (c == 0xA0 || c == 0xB5) salida += 'A';
            else if (c == 0x82 || c == 0x90) salida += 'E';
            else if (c == 0xA1 || c == 0xD6) salida += 'I';
            else if (c == 0xA2 || c == 0xE0) salida += 'O';
            else if (c == 0xA3 || c == 0xE9 || c == 0x81 || c == 0x9A) salida += 'U';
            else if (c == 0xA4 || c == 0xA5) salida += "Ñ";
            else if (c >= 'a' && c <= 'z') salida += (char)(c - 'a' + 'A');
            else salida += texto[i];
            i++;
        }
    }
    return salida;
}

// ============================================================================
// CargarRegistro: lee registro.txt. Falso si no existe (primera ejecucion)
// ============================================================================
bool CargarRegistro() {
    ifstream archivo("registro.txt");
    CantRegistrados = 0;
    if (!archivo.is_open()) {
        return false;
    }
    string nombre;
    int g, p, e;
    while (CantRegistrados < CANTMAXREGISTRO && archivo >> nombre >> g >> p >> e) {
        Registro[CantRegistrados].nombre = nombre;
        Registro[CantRegistrados].ganadas = g;
        Registro[CantRegistrados].perdidas = p;
        Registro[CantRegistrados].empatadas = e;
        CantRegistrados++;
    }
    archivo.close();
    return true;
}

// Reescribe registro.txt completo
void GuardarRegistro() {
    ofstream archivo("registro.txt");
    if (!archivo.is_open()) {
        cout << "No se pudo guardar el registro de resultados." << endl;
        return;
    }
    for (int i = 0; i < CantRegistrados; i++) {
        archivo << Registro[i].nombre << " " << Registro[i].ganadas << " "
                << Registro[i].perdidas << " " << Registro[i].empatadas << endl;
    }
    archivo.close();
}

// Posicion del nombre en Registro, o -1
int BuscarJugadorRegistro(string nombre) {
    string buscado = NormalizarTexto(nombre);
    for (int i = 0; i < CantRegistrados; i++) {
        if (NormalizarTexto(Registro[i].nombre) == buscado) {
            return i;
        }
    }
    return -1;
}
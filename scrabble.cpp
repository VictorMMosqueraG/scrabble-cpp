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
Jugador Jugadores[2];
bool nombresConfigurados = false;
Ficha Bolsa[TOTALFICHAS];
int FichasEnBolsa = 0;

// Metodos
int LeerOpcion(int minimo, int maximo);
string NormalizarTexto(string texto);
bool CargarRegistro();
void GuardarRegistro();
int BuscarJugadorRegistro(string nombre);
void CambiarNombres();
void MostrarRegistro();
int ValorLetra(string letra);
void LlenarBolsa();

// main
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);   
#endif

    cout << "=========================================" << endl;
    cout << "         SCRABBLE EN CASTELLANO" << endl;
    cout << "=========================================" << endl;

    if (!CargarRegistro()) {
        cout << "\nEs la primera vez que se ejecuta el juego." << endl;
        cout << "Antes de continuar, escriban los nombres de los jugadores." << endl;
        CambiarNombres();
    }

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
            CambiarNombres();
        } else if (opcion == 2) {
            cout << "Opcion en construccion." << endl;
        } else if (opcion == 3) {
            MostrarRegistro();
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

//FIXME: bug cuando se cambia nombre (esta duplicando el registro de un jugador)
// CambiarNombres: pide los dos nombres y los asocia al registro
void CambiarNombres() {
    string nombres[2];
    bool validos = false;
    while (!validos) {
        for (int i = 0; i < 2; i++) {
            cout << "Nombre del jugador " << (i + 1) << ": ";
            getline(cin, nombres[i]);
            if (cin.eof()) {
                cout << "\nFin de la entrada. Se cierra el programa." << endl;
                exit(0);
            }
            string limpio = "";
            for (int k = 0; k < (int)nombres[i].length(); k++) {
                char c = nombres[i][k];
                if (c == '\r' || c == '\t') c = ' ';
                limpio += c;
            }
            int ini = 0;
            int fin = (int)limpio.length() - 1;
            while (ini <= fin && limpio[ini] == ' ') ini++;
            while (fin >= ini && limpio[fin] == ' ') fin--;
            string recortado = "";
            for (int k = ini; k <= fin; k++) {
                if (limpio[k] == ' ') recortado += '_';
                else recortado += limpio[k];
            }
            nombres[i] = recortado;
        }
        if (nombres[0] == "" || nombres[1] == "") {
            cout << "Los nombres no pueden estar vacios. Escribanlos de nuevo." << endl;
        } else if (NormalizarTexto(nombres[0]) == NormalizarTexto(nombres[1])) {
            cout << "Los dos jugadores deben tener nombres distintos. Escribanlos de nuevo." << endl;
        } else {
            validos = true;
        }
    }

    for (int i = 0; i < 2; i++) {
        int pos = BuscarJugadorRegistro(nombres[i]);
        if (pos != -1) {
            nombres[i] = Registro[pos].nombre;
            cout << "Bienvenido de nuevo, " << nombres[i] << ". Ganadas: " << Registro[pos].ganadas
                 << "  Perdidas: " << Registro[pos].perdidas
                 << "  Empatadas: " << Registro[pos].empatadas << endl;
        } else if (CantRegistrados < CANTMAXREGISTRO) {
            Registro[CantRegistrados].nombre = nombres[i];
            Registro[CantRegistrados].ganadas = 0;
            Registro[CantRegistrados].perdidas = 0;
            Registro[CantRegistrados].empatadas = 0;
            CantRegistrados++;
            cout << "Jugador nuevo registrado: " << nombres[i] << endl;
        } else {
            cout << "El registro esta lleno; " << nombres[i]
                 << " no se podra guardar." << endl;
        }
        Jugadores[i].nombrejug = nombres[i];
    }
    GuardarRegistro();
    nombresConfigurados = true;
}

// Tabla de resultados historicos
void MostrarRegistro() {
    if (CantRegistrados == 0) {
        cout << "\nNo hay jugadores registrados." << endl;
        return;
    }
    cout << "\n=========== REGISTRO DE RESULTADOS ===========" << endl;
    cout << "Jugador              Ganadas  Perdidas  Empatadas" << endl;
    int i = 0;
    while (i < CantRegistrados) {
        string nombre = Registro[i].nombre;
        cout << nombre;
        for (int k = nombre.length(); k < 21; k++) cout << " ";
        cout << Registro[i].ganadas << "\t " << Registro[i].perdidas << "\t   "
             << Registro[i].empatadas << endl;
        i = i + 1;
    }
}

// Puntos de una ficha segun la tabla oficial
int ValorLetra(string letra) {
    if (letra == "A" || letra == "E" || letra == "I" || letra == "L" || letra == "N" ||
        letra == "O" || letra == "R" || letra == "S" || letra == "T" || letra == "U") {
        return 1;
    }
    if (letra == "D" || letra == "G") return 2;
    if (letra == "B" || letra == "C" || letra == "M" || letra == "P") return 3;
    if (letra == "F" || letra == "H" || letra == "V" || letra == "Y") return 4;
    if (letra == "CH" || letra == "Q") return 5;
    if (letra == "J" || letra == "LL" || letra == "Ñ" || letra == "RR" || letra == "X") return 8;
    if (letra == "Z") return 10;
    return 0;
}

// Crea las 98 fichas
void LlenarBolsa() {
    string letras[28] = {"A", "E", "I", "L", "N", "O", "R", "S", "T", "U",
                         "D", "G",
                         "B", "C", "M", "P",
                         "F", "H", "V", "Y",
                         "CH", "Q",
                         "J", "LL", "Ñ", "RR", "X",
                         "Z"};
    int cantidades[28] = {12, 12, 6, 4, 5, 9, 5, 6, 4, 5,
                          5, 2,
                          2, 4, 2, 2,
                          1, 2, 1, 1,
                          1, 1,
                          1, 1, 1, 1, 1,
                          1};
    FichasEnBolsa = 0;
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < cantidades[i]; j++) {
            Bolsa[FichasEnBolsa].letra = letras[i];
            Bolsa[FichasEnBolsa].valor = ValorLetra(letras[i]);
            FichasEnBolsa++;
        }
    }
}
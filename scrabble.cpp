#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <ctime>
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
const int MAXCANTPALABRA = 15;
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
bool AtrilUsado[TOTALPORTAFICHAS];
string Tablero[TAMANOTABLERO][TAMANOTABLERO]; 
string PalabraJugada;
int fila = 0, columna = 0;
char sentido = 'H';
string palabraFichas[MAXCANTPALABRA];
int CantFichasPalabra = 0;
int TurnoActual = 0;
bool PrimeraJugada = true;
string CopiaTablero[TAMANOTABLERO][TAMANOTABLERO]; 
string PalabrasFormadas[MAXPALABRASFORMADAS];
int FilaPalabra[MAXPALABRASFORMADAS];
int ColumnaPalabra[MAXPALABRASFORMADAS];
char SentidoPalabra[MAXPALABRASFORMADAS];
int LargoPalabra[MAXPALABRASFORMADAS];
int CantPalabrasFormadas = 0;
int PuntosTurno = 0;
bool PartidaTerminada = false;

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
void ReponerPortafichas(int numJugador);
void MostrarTablero();
void MostrarPortafichas(int numJugador);
void PedirPalabra();
void PartirPalabra(string palabra);
bool ValidarJugadaTablero();
bool ValidarConRival(string palabra);
bool ArmarYValidarPalabras();
int CalcularPuntos();
bool CambiarFichas();
bool ProbarJugada();
void JugarTurno();
void ActualizarResultados();
void JugarPartida();

// main
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);   // para que la Ñ se vea bien en la consola
#endif
    srand(time(0));

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
            if (!nombresConfigurados) {
                cout << "\nPrimero escriban los nombres de los jugadores." << endl;
                CambiarNombres();
            }
            JugarPartida();
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

// Lee registro.txt. Falso si no existe (primera ejecucion)
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

// Pide los dos nombres y los asocia al registro
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

// Quita las fichas usadas y completa hasta siete
void ReponerPortafichas(int numJugador) {
    int n = 0;
    for (int j = 0; j < Jugadores[numJugador].CantFichasAct; j++) {
        if (!AtrilUsado[j]) {
            Jugadores[numJugador].Atril[n] = Jugadores[numJugador].Atril[j];
            n++;
        }
    }
    Jugadores[numJugador].CantFichasAct = n;

    while (Jugadores[numJugador].CantFichasAct < TOTALPORTAFICHAS && FichasEnBolsa > 0) {
        int pos = rand() % FichasEnBolsa;
        Ficha tomada = Bolsa[pos];
        Bolsa[pos] = Bolsa[FichasEnBolsa - 1];
        FichasEnBolsa--;
        Jugadores[numJugador].Atril[Jugadores[numJugador].CantFichasAct] = tomada;
        Jugadores[numJugador].CantFichasAct++;
    }
    for (int k = Jugadores[numJugador].CantFichasAct; k < TOTALPORTAFICHAS; k++) {
        Jugadores[numJugador].Atril[k].letra = "";
        Jugadores[numJugador].Atril[k].valor = 0;
    }
    for (int k = 0; k < TOTALPORTAFICHAS; k++) {
        AtrilUsado[k] = false;
    }
}

// Imprime el tablero con coordenadas de 1 a 15
void MostrarTablero() {
    cout << endl << "    ";
    for (int c = 0; c < TAMANOTABLERO; c++) {
        if (c + 1 < 10) cout << "  " << (c + 1);
        else cout << " " << (c + 1);
    }
    cout << endl;
    for (int r = 0; r < TAMANOTABLERO; r++) {
        if (r + 1 < 10) cout << "  " << (r + 1) << " ";
        else cout << " " << (r + 1) << " ";
        for (int c = 0; c < TAMANOTABLERO; c++) {
            string casilla = Tablero[r][c];
            if (casilla == "") {
                if (r == CENTRO && c == CENTRO) casilla = "*";
                else casilla = ".";
            }
            int ancho = casilla.length();
            if (casilla == "Ñ") ancho = 1;      // 2 bytes, 1 caracter en pantalla
            for (int k = ancho; k < 3; k++) cout << " ";
            cout << casilla;
        }
        cout << endl;
    }
    cout << "Fichas en la bolsa: " << FichasEnBolsa << endl;
}

// Fichas del jugador con su valor
void MostrarPortafichas(int numJugador) {
    cout << "Portafichas de " << Jugadores[numJugador].nombrejug << ": ";
    if (Jugadores[numJugador].CantFichasAct == 0) {
        cout << "(vacio)";
    }
    for (int k = 0; k < Jugadores[numJugador].CantFichasAct; k++) {
        cout << "[" << Jugadores[numJugador].Atril[k].letra << "("
             << Jugadores[numJugador].Atril[k].valor << ")] ";
    }
    cout << endl;
}

// Lee la palabra, la fila, la columna y el sentido
void PedirPalabra() {
    PalabraJugada = "";
    while (PalabraJugada == "") {
        cout << "Escriba la palabra: ";
        string leida;
        getline(cin, leida);
        if (cin.eof()) {
            cout << "\nFin de la entrada. Se cierra el programa." << endl;
            exit(0);
        }
        string sinEspacios = "";
        for (int k = 0; k < (int)leida.length(); k++) {
            if (leida[k] != ' ' && leida[k] != '\r' && leida[k] != '\t') {
                sinEspacios += leida[k];
            }
        }
        PalabraJugada = NormalizarTexto(sinEspacios);
    }
    cout << "Fila de la primera letra (1-15): ";
    fila = LeerOpcion(1, 15) - 1;
    cout << "Columna de la primera letra (1-15): ";
    columna = LeerOpcion(1, 15) - 1;

    sentido = ' ';
    while (sentido != 'H' && sentido != 'V') {
        cout << "Sentido (H = horizontal, V = vertical): ";
        string s;
        getline(cin, s);
        if (cin.eof()) {
            cout << "\nFin de la entrada. Se cierra el programa." << endl;
            exit(0);
        }
        if (s.length() > 0) {
            sentido = s[0];
            if (sentido == 'h') sentido = 'H';
            if (sentido == 'v') sentido = 'V';
        }
        if (sentido != 'H' && sentido != 'V') {
            cout << "Sentido no valido." << endl;
        }
    }
}

// Separa la palabra en fichas (CH, LL, RR y Ñ = una ficha)
void PartirPalabra(string palabra) {
    CantFichasPalabra = 0;
    int i = 0;
    int n = palabra.length();
    while (i < n) {
        string ficha;
        unsigned char c = palabra[i];
        unsigned char sig = 0;
        if (i + 1 < n) sig = palabra[i + 1];

        if (c == 0xC3 && sig == 0x91) {
            ficha = "Ñ";  
            i += 2;
        } else if ((c == 'C' && sig == 'H') || (c == 'L' && sig == 'L') ||
                   (c == 'R' && sig == 'R')) {
            ficha = palabra.substr(i, 2);
            i += 2;
        } else {
            ficha = palabra.substr(i, 1);
            i++;
        }
        if (CantFichasPalabra < MAXCANTPALABRA) {
            palabraFichas[CantFichasPalabra] = ficha;
        }
        CantFichasPalabra++;
    }
}

// Revisa la jugada en el tablero, en seis pasos
bool ValidarJugadaTablero() {
    for (int k = 0; k < TOTALPORTAFICHAS; k++) {
        AtrilUsado[k] = false;
    }

    if (CantFichasPalabra < 2) {
        cout << "La palabra debe tener al menos dos fichas." << endl;
        return false;
    }
    if (fila < 0 || fila >= TAMANOTABLERO || columna < 0 || columna >= TAMANOTABLERO ||
        (sentido == 'H' && columna + CantFichasPalabra > TAMANOTABLERO) ||
        (sentido == 'V' && fila + CantFichasPalabra > TAMANOTABLERO)) {
        cout << "La palabra no cabe en el tablero." << endl;
        return false;
    }

    int dr = 0, dc = 0;
    if (sentido == 'H') dc = 1;
    else dr = 1;

    bool pasaCentro = false;
    bool conecta = false;
    for (int k = 0; k < CantFichasPalabra; k++) {
        int r = fila + dr * k;
        int c = columna + dc * k;
        if (r == CENTRO && c == CENTRO) pasaCentro = true;
        if (Tablero[r][c] != "") conecta = true;
        if (r > 0 && Tablero[r - 1][c] != "") conecta = true;
        if (r < TAMANOTABLERO - 1 && Tablero[r + 1][c] != "") conecta = true;
        if (c > 0 && Tablero[r][c - 1] != "") conecta = true;
        if (c < TAMANOTABLERO - 1 && Tablero[r][c + 1] != "") conecta = true;
    }
    if (PrimeraJugada && !pasaCentro) {
        cout << "La primera palabra debe pasar por la casilla central (fila 8, columna 8)." << endl;
        return false;
    }
    if (!PrimeraJugada && !conecta) {
        cout << "La palabra debe usar o tocar al menos una letra del tablero." << endl;
        return false;
    }

    int numJ = TurnoActual;
    int nuevas = 0;
    for (int k = 0; k < CantFichasPalabra; k++) {
        int r = fila + dr * k;
        int c = columna + dc * k;
        if (Tablero[r][c] != "") {
            if (Tablero[r][c] != palabraFichas[k]) {
                cout << "En la fila " << (r + 1) << ", columna " << (c + 1)
                     << " ya hay una " << Tablero[r][c] << " y la palabra pide "
                     << palabraFichas[k] << "." << endl;
                return false;
            }
        } else {
            bool hallada = false;
            for (int j = 0; j < Jugadores[numJ].CantFichasAct && !hallada; j++) {
                if (!AtrilUsado[j] && Jugadores[numJ].Atril[j].letra == palabraFichas[k]) {
                    AtrilUsado[j] = true;
                    hallada = true;
                }
            }
            if (!hallada) {
                cout << "No tiene la ficha " << palabraFichas[k]
                     << " en su portafichas." << endl;
                return false;
            }
            nuevas++;
        }
    }
    if (nuevas == 0) {
        cout << "Debe poner al menos una ficha de su portafichas." << endl;
        return false;
    }
    return true;
}

// El jugador que no tiene el turno dice si la palabra vale
bool ValidarConRival(string palabra) {
    int rival = 1 - TurnoActual;
    cout << Jugadores[rival].nombrejug << ", la palabra \"" << palabra
         << "\" es valida?  1. Si   2. No" << endl;
    cout << "Opcion: ";
    int respuesta = LeerOpcion(1, 2);
    return respuesta == 1;
}

// ArmarYValidarPalabras: arma la principal y las cruzadas, revisa las fichas
// dobles sueltas y le pide al rival que apruebe cada palabra
bool ArmarYValidarPalabras() {
    CantPalabrasFormadas = 0;
    int dr = 0, dc = 0;
    if (sentido == 'H') dc = 1;
    else dr = 1;

    // palabra principal
    int r = fila, c = columna;
    while (r - dr >= 0 && c - dc >= 0 && Tablero[r - dr][c - dc] != "") {
        r -= dr;
        c -= dc;
    }
    int r0 = r, c0 = c;
    string texto = "";
    int largo = 0;
    while (r < TAMANOTABLERO && c < TAMANOTABLERO && Tablero[r][c] != "") {
        texto += Tablero[r][c];
        largo++;
        r += dr;
        c += dc;
    }
    PalabrasFormadas[0] = texto;
    FilaPalabra[0] = r0;
    ColumnaPalabra[0] = c0;
    SentidoPalabra[0] = sentido;
    LargoPalabra[0] = largo;
    CantPalabrasFormadas = 1;

    int pr = dc, pc = dr;
    char sentidoCruz = 'H';
    if (sentido == 'H') sentidoCruz = 'V';
    for (int k = 0; k < CantFichasPalabra; k++) {
        int fr = fila + dr * k;
        int fc = columna + dc * k;
        if (CopiaTablero[fr][fc] == "") {
            int rr = fr, cc = fc;
            while (rr - pr >= 0 && cc - pc >= 0 && Tablero[rr - pr][cc - pc] != "") {
                rr -= pr;
                cc -= pc;
            }
            int ir = rr, ic = cc;
            string cruzada = "";
            int largoCruz = 0;
            while (rr < TAMANOTABLERO && cc < TAMANOTABLERO && Tablero[rr][cc] != "") {
                cruzada += Tablero[rr][cc];
                largoCruz++;
                rr += pr;
                cc += pc;
            }
            if (largoCruz >= 2 && CantPalabrasFormadas < MAXPALABRASFORMADAS) {
                PalabrasFormadas[CantPalabrasFormadas] = cruzada;
                FilaPalabra[CantPalabrasFormadas] = ir;
                ColumnaPalabra[CantPalabrasFormadas] = ic;
                SentidoPalabra[CantPalabrasFormadas] = sentidoCruz;
                LargoPalabra[CantPalabrasFormadas] = largoCruz;
                CantPalabrasFormadas++;
            }
        }
    }

    for (int i = 0; i < CantPalabrasFormadas; i++) {
        int wr = 0, wc = 0;
        if (SentidoPalabra[i] == 'H') wc = 1;
        else wr = 1;
        for (int k = 0; k + 1 < LargoPalabra[i]; k++) {
            string a = Tablero[FilaPalabra[i] + wr * k][ColumnaPalabra[i] + wc * k];
            string b = Tablero[FilaPalabra[i] + wr * (k + 1)][ColumnaPalabra[i] + wc * (k + 1)];
            if ((a == "C" && b == "H") || (a == "L" && b == "L") || (a == "R" && b == "R")) {
                cout << "La palabra " << PalabrasFormadas[i] << " forma " << a << b
                     << " con dos fichas sueltas; " << a << b
                     << " debe ser una sola ficha." << endl;
                return false;
            }
        }
    }

    cout << "\n----- VALIDACION DEL RIVAL -----" << endl;
    cout << Jugadores[TurnoActual].nombrejug << " formo " << CantPalabrasFormadas
         << " palabra(s): ";
    for (int i = 0; i < CantPalabrasFormadas; i++) {
        cout << PalabrasFormadas[i];
        if (i < CantPalabrasFormadas - 1) cout << ", ";
    }
    cout << endl;
    for (int i = 0; i < CantPalabrasFormadas; i++) {
        if (!ValidarConRival(PalabrasFormadas[i])) {
            cout << Jugadores[1 - TurnoActual].nombrejug << " rechazo la palabra "
                 << PalabrasFormadas[i] << "." << endl;
            return false;
        }
    }
    cout << "Todas las palabras fueron aceptadas." << endl;
    return true;
}

// Suma las fichas de cada palabra leyendolas del tablero
int CalcularPuntos() {
    int total = 0;
    for (int i = 0; i < CantPalabrasFormadas; i++) {
        int wr = 0, wc = 0;
        if (SentidoPalabra[i] == 'H') wc = 1;
        else wr = 1;
        for (int k = 0; k < LargoPalabra[i]; k++) {
            total += ValorLetra(Tablero[FilaPalabra[i] + wr * k][ColumnaPalabra[i] + wc * k]);
        }
    }
    return total;
}

// Cambia todas las fichas del jugador en turno
bool CambiarFichas() {
    int numJ = TurnoActual;
    int n = Jugadores[numJ].CantFichasAct;
    if (n == 0 || FichasEnBolsa < n) {
        return false;
    }
    Ficha temporal[TOTALPORTAFICHAS];
    for (int k = 0; k < n; k++) {
        temporal[k] = Jugadores[numJ].Atril[k];
    }
    for (int k = 0; k < n; k++) {
        int pos = rand() % FichasEnBolsa;
        Jugadores[numJ].Atril[k] = Bolsa[pos];
        Bolsa[pos] = Bolsa[FichasEnBolsa - 1];
        FichasEnBolsa--;
    }
    for (int k = 0; k < n; k++) {
        Bolsa[FichasEnBolsa] = temporal[k];
        FichasEnBolsa++;
    }
    return true;
}

// Pide, valida y coloca una palabra, con reintento
bool ProbarJugada() {
    while (true) {
        PedirPalabra();
        PartirPalabra(PalabraJugada);

        bool valida = ValidarJugadaTablero();
        if (valida) {
            for (int r = 0; r < TAMANOTABLERO; r++)
                for (int c = 0; c < TAMANOTABLERO; c++)
                    CopiaTablero[r][c] = Tablero[r][c];

            for (int k = 0; k < CantFichasPalabra; k++) {
                if (sentido == 'H') Tablero[fila][columna + k] = palabraFichas[k];
                else Tablero[fila + k][columna] = palabraFichas[k];
            }
            MostrarTablero();

            valida = ArmarYValidarPalabras();
            if (!valida) {
                for (int r = 0; r < TAMANOTABLERO; r++)
                    for (int c = 0; c < TAMANOTABLERO; c++)
                        Tablero[r][c] = CopiaTablero[r][c];
                cout << "La palabra se quito del tablero." << endl;
            }
        }

        if (valida) {
            PuntosTurno = CalcularPuntos();
            Jugadores[TurnoActual].puntajejug += PuntosTurno;
            ReponerPortafichas(TurnoActual);
            PrimeraJugada = false;
            return true;
        }

        for (int k = 0; k < TOTALPORTAFICHAS; k++) AtrilUsado[k] = false;
        cout << "1. Intentar otra palabra   2. Saltar el turno" << endl;
        cout << "Opcion: ";
        if (LeerOpcion(1, 2) == 2) {
            return false;
        }
        MostrarPortafichas(TurnoActual);
    }
}

// Controla el turno completo de un jugador
void JugarTurno() {
    int numJ = TurnoActual;
    PuntosTurno = 0;

    cout << "\n=========================================" << endl;
    cout << "Turno de " << Jugadores[numJ].nombrejug << endl;
    cout << "=========================================" << endl;
    MostrarTablero();
    MostrarPortafichas(numJ);

    bool terminado = false;
    while (!terminado) {
        cout << "1. Colocar palabra   2. Cambiar todas las fichas   3. Saltar turno" << endl;
        cout << "Opcion: ";
        int opcion = LeerOpcion(1, 3);
        if (opcion == 1) {
            if (ProbarJugada()) Jugadores[numJ].TurnosSaltados = 0;
            else Jugadores[numJ].TurnosSaltados++;
            terminado = true;
        } else if (opcion == 2) {
            if (CambiarFichas()) {
                cout << "Se cambiaron todas las fichas." << endl;
                Jugadores[numJ].TurnosSaltados = 0;
                terminado = true;
            } else {
                cout << "No hay suficientes fichas en la bolsa para cambiar. Elija otra accion." << endl;
            }
        } else {
            Jugadores[numJ].TurnosSaltados++;
            terminado = true;
        }
    }

    cout << "\nPuntos de esta jugada: " << PuntosTurno << endl;
    cout << "Puntaje total -> " << Jugadores[0].nombrejug << ": " << Jugadores[0].puntajejug
         << "   " << Jugadores[1].nombrejug << ": " << Jugadores[1].puntajejug << endl;
    MostrarPortafichas(numJ);
}

// Ganador o empate y registro
void ActualizarResultados() {
    int p0 = Jugadores[0].puntajejug;
    int p1 = Jugadores[1].puntajejug;
    int pos0 = BuscarJugadorRegistro(Jugadores[0].nombrejug);
    int pos1 = BuscarJugadorRegistro(Jugadores[1].nombrejug);

    cout << "\n=============== FIN DEL JUEGO ===============" << endl;
    cout << Jugadores[0].nombrejug << ": " << p0 << " puntos" << endl;
    cout << Jugadores[1].nombrejug << ": " << p1 << " puntos" << endl;

    if (p0 > p1) {
        cout << "Gana " << Jugadores[0].nombrejug << "!" << endl;
        if (pos0 != -1) Registro[pos0].ganadas++;
        if (pos1 != -1) Registro[pos1].perdidas++;
    } else if (p1 > p0) {
        cout << "Gana " << Jugadores[1].nombrejug << "!" << endl;
        if (pos1 != -1) Registro[pos1].ganadas++;
        if (pos0 != -1) Registro[pos0].perdidas++;
    } else {
        cout << "Empate!" << endl;
        if (pos0 != -1) Registro[pos0].empatadas++;
        if (pos1 != -1) Registro[pos1].empatadas++;
    }
    GuardarRegistro();
}

// Prepara la partida, repite los turnos y cierra
void JugarPartida() {
    for (int r = 0; r < TAMANOTABLERO; r++)
        for (int c = 0; c < TAMANOTABLERO; c++)
            Tablero[r][c] = "";   // la estrella central solo la dibuja MostrarTablero

    LlenarBolsa();

    for (int i = 0; i < 2; i++) {
        Jugadores[i].puntajejug = 0;
        Jugadores[i].CantFichasAct = 0;
        Jugadores[i].TurnosSaltados = 0;
        for (int k = 0; k < TOTALPORTAFICHAS; k++) AtrilUsado[k] = true;
        ReponerPortafichas(i);
    }

    PrimeraJugada = true;
    PartidaTerminada = false;
    TurnoActual = 0;

    while (!PartidaTerminada) {
        JugarTurno();
        bool sinFichas = FichasEnBolsa == 0 && Jugadores[TurnoActual].CantFichasAct == 0;
        bool ambosSaltaron = Jugadores[0].TurnosSaltados >= 2 && Jugadores[1].TurnosSaltados >= 2;
        if (sinFichas || ambosSaltaron) {
            PartidaTerminada = true;
        } else {
            TurnoActual = 1 - TurnoActual;
        }
    }
    ActualizarResultados();
}
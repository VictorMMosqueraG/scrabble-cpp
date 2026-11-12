//  Una libreria es codigo que ya viene hecho y que podemos usar.
//  #include "pega" ese codigo en nuestro programa antes de compilar.
#include <iostream>   // Trae cin (leer del teclado) y cout (escribir en pantalla)
#include <string>     // Trae el tipo string, que sirve para guardar textos
#include <cstdlib>    // Trae rand() (numero al azar), srand() (semilla del azar) y exit() (cerrar el programa)
#include <fstream>    // Trae ifstream y ofstream, para leer y escribir archivos
#include <ctime>      // Trae time(0), que da la hora actual en segundos
#ifdef _WIN32         // "Si este programa se esta compilando en Windows..."
#include <windows.h>  // ...incluir windows.h, que trae SetConsoleOutputCP (para ver bien la Ñ)
#endif                // Fin del "si". En Mac o Linux las dos lineas anteriores se ignoran

using namespace std;  // Permite escribir cout en vez de std::cout (y lo mismo con cin, string, etc.)


//  "const" significa que el valor NO se puede cambiar despues.
//  Se usan para no escribir numeros sueltos por todo el codigo:
//  si algun dia el tablero fuera de 21, solo se cambia aqui.
// Constantes del sistema
const int TAMANOTABLERO = 15;       // El tablero tiene 15 filas y 15 columnas
const int CENTRO = 7;               // La casilla central es la posicion 7 (la fila 8 que ve el usuario)
const int TOTALFICHAS = 98;         // La bolsa empieza con 98 fichas
const int TOTALPORTAFICHAS = 7;     // Cada jugador tiene 7 fichas en su portafichas (atril)
const int CANTMAXREGISTRO = 100;    // El historial guarda como maximo 100 jugadores
const int MAXCANTPALABRA = 15;      // Una palabra puede tener como maximo 15 fichas (el ancho del tablero)
const int MAXPALABRASFORMADAS = 16; // En una jugada se forman como maximo 16 palabras (1 principal + 15 cruzadas)


//  Un struct agrupa varias variables bajo un mismo nombre.
//  Es como una ficha tecnica: "una Ficha tiene letra y valor".
//  Para usar un campo se escribe variable.campo, por ejemplo f.letra
// Estructuras
struct Ficha{           // Una ficha del juego
    string letra;       // La letra: "A", "CH", "Ñ"... Es string porque "CH" son 2 letras
    int valor;          // Cuantos puntos vale (A = 1, Z = 10, etc.)
};                      // El struct siempre termina con punto y coma

struct Jugador{                      // Todo lo que se sabe de un jugador durante la partida
    string nombrejug;                // Su nombre
    int puntajejug;                  // Sus puntos acumulados en esta partida
    Ficha Atril[TOTALPORTAFICHAS];   // Su portafichas: un arreglo de 7 fichas
    int CantFichasAct;               // Cuantas de esas 7 posiciones tienen ficha de verdad
    int TurnosSaltados;              // Cuantos turnos seguidos lleva sin jugar
};

struct RegistroJugador{   // Una linea del historial (archivo registro.txt)
    string nombre;        // Nombre del jugador
    int ganadas;          // Partidas ganadas
    int perdidas;         // Partidas perdidas
    int empatadas;        // Partidas empatadas
};


//  Se declaran fuera de todas las funciones, por eso CUALQUIER funcion
//  las puede leer y cambiar. El proyecto no podia usar apuntadores ni
//  paso por referencia, asi que esta fue la forma de compartir datos.
// Variables globales
RegistroJugador Registro[CANTMAXREGISTRO];    // El historial: espacio para 100 jugadores
int CantRegistrados = 0;                      // Cuantos de esos 100 espacios estan ocupados
Jugador Jugadores[2];                         // Los dos jugadores: Jugadores[0] y Jugadores[1]
bool nombresConfigurados = false;             // true cuando ya se escribieron los nombres
Ficha Bolsa[TOTALFICHAS];                     // La bolsa con las fichas que aun no se han sacado
int FichasEnBolsa = 0;                        // Cuantas fichas quedan en la bolsa
bool AtrilUsado[TOTALPORTAFICHAS];            // AtrilUsado[j] = true si la ficha j del atril se usa en esta jugada
string Tablero[TAMANOTABLERO][TAMANOTABLERO]; // El tablero: matriz de 15x15. "" = casilla vacia, "A" = hay una A
string PalabraJugada;                         // La palabra que escribio el jugador (ya en mayusculas)
int fila = 0, columna = 0;                    // Donde va la primera letra de la palabra
char sentido = 'H';                           // 'H' = horizontal, 'V' = vertical
string palabraFichas[MAXCANTPALABRA];         // La palabra separada en fichas. "CHURRO" -> CH, U, RR, O
int CantFichasPalabra = 0;                    // Cuantas fichas tiene la palabra
int TurnoActual = 0;                          // De quien es el turno: 0 = jugador 1, 1 = jugador 2
bool PrimeraJugada = true;                    // true si todavia no se ha puesto ninguna palabra
string CopiaTablero[TAMANOTABLERO][TAMANOTABLERO]; // Copia de respaldo del tablero, para deshacer una jugada
// Los siguientes 5 arreglos son "paralelos": la posicion i de todos habla de la MISMA palabra
string PalabrasFormadas[MAXPALABRASFORMADAS]; // El texto de cada palabra formada en la jugada
int FilaPalabra[MAXPALABRASFORMADAS];         // Fila donde empieza cada palabra
int ColumnaPalabra[MAXPALABRASFORMADAS];      // Columna donde empieza cada palabra
char SentidoPalabra[MAXPALABRASFORMADAS];     // Sentido de cada palabra ('H' o 'V')
int LargoPalabra[MAXPALABRASFORMADAS];        // Cuantas fichas tiene cada palabra
int CantPalabrasFormadas = 0;                 // Cuantas palabras se formaron en la jugada
int PuntosTurno = 0;                          // Puntos ganados en el turno actual
bool PartidaTerminada = false;                // Se pone en true cuando termina la partida


//  C++ lee el archivo de arriba hacia abajo. main() usa funciones que
//  estan escritas MAS ABAJO, asi que primero hay que "anunciarlas":
//  decir su nombre, que reciben y que devuelven.
//  Formato:  tipoQueDevuelve NombreFuncion(parametros);
//  "void" significa que la funcion no devuelve nada.
// Metodos
int LeerOpcion(int minimo, int maximo);   // Lee un numero entre minimo y maximo
string NormalizarTexto(string texto);     // Pasa un texto a mayusculas y sin tildes
bool CargarRegistro();                    // Lee registro.txt
void GuardarRegistro();                   // Escribe registro.txt
int BuscarJugadorRegistro(string nombre); // Busca un jugador en el historial
void CambiarNombres();                    // Pide los nombres de los jugadores
void MostrarRegistro();                   // Muestra la tabla de resultados
int ValorLetra(string letra);             // Dice cuantos puntos vale una letra
void LlenarBolsa();                       // Crea las 98 fichas
void ReponerPortafichas(int numJugador);  // Completa el atril del jugador hasta 7 fichas
void MostrarTablero();                    // Dibuja el tablero en pantalla
void MostrarPortafichas(int numJugador);  // Muestra las fichas de un jugador
void PedirPalabra();                      // Pide palabra, fila, columna y sentido
void PartirPalabra(string palabra);       // Separa la palabra en fichas
bool ValidarJugadaTablero();              // Revisa las reglas del tablero
bool ValidarConRival(string palabra);     // El rival dice si la palabra existe
bool ArmarYValidarPalabras();             // Busca todas las palabras formadas y las valida
int CalcularPuntos();                     // Suma los puntos de la jugada
bool CambiarFichas();                     // Cambia todas las fichas del jugador
bool ProbarJugada();                      // Maneja un intento completo de poner una palabra
void JugarTurno();                        // Maneja un turno completo
void ActualizarResultados();              // Dice quien gano y lo guarda
void JugarPartida();                      // Maneja la partida completa


//  Toda aplicacion en C++ arranca en la funcion main.
// main
int main() {
#ifdef _WIN32                    // Solo en Windows:
    SetConsoleOutputCP(65001);   // para que la Ñ se vea bien en la consola (65001 = UTF-8)
#endif
    srand(time(0));              // Prepara los numeros al azar usando la hora actual como "semilla".
                                 // Sin esto, rand() daria siempre los mismos numeros y todas las
                                 // partidas repartirian las mismas fichas.

    cout << "=========================================" << endl; // endl = salto de linea
    cout << "         SCRABBLE EN CASTELLANO" << endl;            // Titulo del juego
    cout << "=========================================" << endl;

    if (!CargarRegistro()) {     // ! significa "no". Si NO se pudo cargar el archivo (no existe)...
        cout << "\nEs la primera vez que se ejecuta el juego." << endl;  // \n tambien es salto de linea
        cout << "Antes de continuar, escriban los nombres de los jugadores." << endl;
        CambiarNombres();        // ...se piden los nombres antes de mostrar el menu
    }

    int opcion = 0;              // Guarda la opcion del menu. Empieza en 0 para que entre al while
    while (opcion != 4) {        // Repetir el menu mientras la opcion sea distinta (!=) de 4 (Salir)
        cout << "\n============ MENU PRINCIPAL ============" << endl;
        cout << "1. Cambiar nombre de jugadores" << endl;
        cout << "2. Iniciar juego" << endl;
        cout << "3. Mostrar registro de resultados" << endl;
        cout << "4. Salir del juego" << endl;
        cout << "Opcion: ";
        opcion = LeerOpcion(1, 4);   // Lee un numero y solo acepta 1, 2, 3 o 4

        if (opcion == 1) {           // == compara: "si la opcion es igual a 1"
            CambiarNombres();        // Cambiar los nombres
        } else if (opcion == 2) {    // "si no, si la opcion es 2"
            if (!nombresConfigurados) {  // Si todavia no hay nombres...
                cout << "\nPrimero escriban los nombres de los jugadores." << endl;
                CambiarNombres();        // ...se piden primero
            }
            JugarPartida();          // Se juega una partida completa
        } else if (opcion == 3) {    // "si no, si la opcion es 3"
            MostrarRegistro();       // Mostrar la tabla de resultados
        }
        // Si la opcion fue 4 no entra a ningun if; vuelve al while, ve que es 4 y sale
    }

    cout << "\nGracias por jugar Scrabble. Hasta pronto!" << endl; // Mensaje de despedida
    return 0;                    // Termina el programa. 0 significa "todo salio bien"
}


//  LeerOpcion: lee un numero del teclado y no deja seguir hasta que
//  el usuario escriba uno valido (entre minimo y maximo).
//  Recibe: el rango permitido. Devuelve: el numero elegido.
//
//  Idea clave: lo que el usuario escribe primero queda en una "bandeja"
//  (buffer) y cin va sacando de ahi lo que necesita.
//Lee un numero entre minimo y maximo
int LeerOpcion(int minimo, int maximo) {
    int opcion;                  // Aqui se guarda el numero que escriba el usuario
    bool valida = false;         // Indica si ya tenemos un numero correcto. Empieza en "no"
    while (!valida) {            // Repetir mientras NO sea valida
        cin >> opcion;           // Intenta leer un numero entero del teclado
        if (cin.eof()) {         // eof = "fin de la entrada": el usuario cerro la entrada (Ctrl+D o Ctrl+Z)
            cout << "\nFin de la entrada. Se cierra el programa." << endl;
            exit(0);             // Cierra el programa completo para no quedar en un ciclo infinito
        }
        if (cin.fail()) {        // fail = la lectura fallo (por ejemplo, escribio letras en vez de un numero)
            cin.clear();         // Quita la marca de error de cin para que vuelva a funcionar
            cin.ignore(10000, '\n');  // Bota lo que quedo en la bandeja hasta el Enter ('\n')
            cout << "Debe escribir un numero entre " << minimo << " y " << maximo << ": ";
        } else {                 // La lectura salio bien: si es un numero
            cin.ignore(10000, '\n');  // Bota el Enter que quedo en la bandeja (si no, el proximo getline leeria una linea vacia)
            if (opcion < minimo || opcion > maximo) {  // || significa "o": si es menor que el minimo O mayor que el maximo
                cout << "Opcion fuera de rango. Escriba un numero entre "
                     << minimo << " y " << maximo << ": ";
            } else {             // Esta dentro del rango
                valida = true;   // Ya es valida: el while termina
            }
        }
    }
    return opcion;               // Devuelve el numero a quien llamo la funcion
}


//  NormalizarTexto: devuelve el texto en MAYUSCULAS, sin tildes y con
//  la Ñ siempre igual. Asi "canción", "Cancion" y "CANCION" quedan
//  iguales y se pueden comparar.
//
//  Ojo: para la computadora un texto es una lista de BYTES (numeros
//  de 0 a 255). Las letras normales ocupan 1 byte, pero las letras con
//  tilde y la Ñ pueden ocupar 2 bytes (UTF-8, el primero es 0xC3) o
//  1 byte (consola antigua de Windows, llamada CP850).
//  Los numeros con 0x estan escritos en hexadecimal (base 16).
//  Ejemplo: 0xC3 = 195.
// Normalizar texto de mayusculas, sin tildes ni dieresis, Ñ unificada
string NormalizarTexto(string texto) {
    string salida = "";          // Aqui se va armando el resultado, empieza vacio
    int i = 0;                   // Posicion actual dentro del texto (en bytes)
    int n = texto.length();      // length() dice cuantos bytes tiene el texto
    while (i < n) {              // Se usa while (y no for) porque a veces se avanza 1 byte y a veces 2
        unsigned char c = texto[i];      // Byte actual. "unsigned" = sin signo, para que vaya de 0 a 255
                                         // (con signo, 0xC3 se guardaria como -61 y no se reconoceria)
        unsigned char sig = 0;           // Byte siguiente. Empieza en 0 por si no hay siguiente
        if (i + 1 < n) sig = texto[i + 1];  // Si existe un byte despues, lo guarda

        // CASO A: letra especial de 2 bytes (UTF-8). Siempre empieza con 0xC3
        if (c == 0xC3 && sig != 0) {     // && significa "y": el byte es 0xC3 Y hay un byte siguiente
            if (sig == 0xA1 || sig == 0x81) salida += 'A';        // á o Á  ->  A    (+= agrega al final)
            else if (sig == 0xA9 || sig == 0x89) salida += 'E';   // é o É  ->  E
            else if (sig == 0xAD || sig == 0x8D) salida += 'I';   // í o Í  ->  I
            else if (sig == 0xB3 || sig == 0x93) salida += 'O';   // ó o Ó  ->  O
            else if (sig == 0xBA || sig == 0x9A || sig == 0xBC || sig == 0x9C) salida += 'U'; // ú, Ú, ü, Ü -> U
            else if (sig == 0xB1 || sig == 0x91) salida += "Ñ";   // ñ o Ñ  ->  Ñ (va entre comillas dobles porque son 2 bytes)
            else {                       // Otra letra especial que no nos interesa (por ejemplo ç)
                salida += texto[i];      // Se copia el primer byte sin cambios
                salida += texto[i + 1];  // Y el segundo byte sin cambios
            }
            i += 2;                      // Se usaron 2 bytes, asi que se avanza 2 posiciones
        } else {
        // CASO B: caracter de 1 byte
            if (c == 0xA0 || c == 0xB5) salida += 'A';        // á o Á en la consola antigua de Windows
            else if (c == 0x82 || c == 0x90) salida += 'E';   // é o É
            else if (c == 0xA1 || c == 0xD6) salida += 'I';   // í o Í
            else if (c == 0xA2 || c == 0xE0) salida += 'O';   // ó o Ó
            else if (c == 0xA3 || c == 0xE9 || c == 0x81 || c == 0x9A) salida += 'U'; // ú, Ú, ü, Ü
            else if (c == 0xA4 || c == 0xA5) salida += "Ñ";   // ñ o Ñ
            else if (c >= 'a' && c <= 'z') salida += (char)(c - 'a' + 'A'); // Minuscula -> mayuscula.
                                         // Truco: 'c' - 'a' da 2 (su posicion en el abecedario),
                                         // y 2 + 'A' da 'C'. (char) convierte el numero en letra.
            else salida += texto[i];     // Cualquier otra cosa (mayusculas, numeros, simbolos) se copia igual
            i++;                         // Se uso 1 byte, se avanza 1 posicion
        }
    }
    return salida;                       // Devuelve el texto normalizado
}


//  CargarRegistro: lee el archivo registro.txt y pasa los datos al
//  arreglo Registro. Cada linea del archivo tiene este formato:
//      Victor 3 1 0      (nombre, ganadas, perdidas, empatadas)
//  Devuelve false si el archivo no existe (primera vez que se juega).
// Lee registro.txt. Falso si no existe (primera ejecucion)
bool CargarRegistro() {
    ifstream archivo("registro.txt");   // ifstream = abre un archivo para LEER
    CantRegistrados = 0;                // Empieza con el historial vacio
    if (!archivo.is_open()) {           // Si NO se pudo abrir (el archivo no existe)...
        return false;                   // ...avisa que es la primera vez
    }
    string nombre;                      // Variables temporales para leer cada linea
    int g, p, e;                        // g = ganadas, p = perdidas, e = empatadas
    // archivo >> ... lee datos separados por espacios, igual que cin.
    // Si logra leer los 4 datos da "verdadero"; al llegar al final del archivo da "falso".
    while (CantRegistrados < CANTMAXREGISTRO && archivo >> nombre >> g >> p >> e) {
        Registro[CantRegistrados].nombre = nombre;   // Guarda el nombre en el siguiente espacio libre
        Registro[CantRegistrados].ganadas = g;       // Guarda las ganadas
        Registro[CantRegistrados].perdidas = p;      // Guarda las perdidas
        Registro[CantRegistrados].empatadas = e;     // Guarda las empatadas
        CantRegistrados++;                           // ++ suma 1: el siguiente espacio libre
    }
    archivo.close();                    // Cierra el archivo
    return true;                        // Avisa que si se pudo leer
}


//  GuardarRegistro: hace lo contrario de CargarRegistro. Escribe el
//  arreglo Registro completo en registro.txt.
//  Importante: al abrir con ofstream, lo que habia en el archivo SE
//  BORRA, por eso se vuelven a escribir todos los jugadores.
// Reescribe registro.txt completo
void GuardarRegistro() {
    ofstream archivo("registro.txt");   // ofstream = abre (o crea) un archivo para ESCRIBIR
    if (!archivo.is_open()) {           // Si no se pudo abrir (sin permisos, disco lleno, etc.)...
        cout << "No se pudo guardar el registro de resultados." << endl;
        return;                         // ...sale de la funcion sin hacer nada mas
    }
    for (int i = 0; i < CantRegistrados; i++) {   // Recorre todos los jugadores registrados
        // Funciona igual que cout, pero escribe en el archivo en vez de la pantalla.
        // Los datos van separados por un espacio para poder leerlos despues con >>
        archivo << Registro[i].nombre << " " << Registro[i].ganadas << " "
                << Registro[i].perdidas << " " << Registro[i].empatadas << endl;
    }
    archivo.close();                    // Cierra el archivo para que todo quede guardado
}


//  BuscarJugadorRegistro: dice en que posicion del historial esta un
//  jugador. Devuelve la posicion (0, 1, 2...) o -1 si no esta.
//  Compara los nombres normalizados: "victor" encuentra a "Victor".
// Posicion del nombre en Registro, o -1
int BuscarJugadorRegistro(string nombre) {
    string buscado = NormalizarTexto(nombre);       // Nombre buscado en mayusculas y sin tildes
    for (int i = 0; i < CantRegistrados; i++) {     // Revisa los jugadores uno por uno
        if (NormalizarTexto(Registro[i].nombre) == buscado) {  // Si coincide...
            return i;                               // ...devuelve su posicion y termina
        }
    }
    return -1;                                      // Si reviso todos y no lo encontro: -1
}


//  CambiarNombres: define quienes van a jugar.
//  Fase 1: consigue dos nombres validos (de la lista o escritos).
//  Fase 2: los registra en el historial (si son nuevos) y los asigna
//          a Jugadores[0] y Jugadores[1].
// Pide los dos nombres y los asocia al registro.
// Si ya hay jugadores en el registro, se puede elegir uno de la lista
// o escribir un nombre nuevo.
void CambiarNombres() {
    string nombres[2];                   // Nombres temporales: nombres[0] (jugador 1) y nombres[1] (jugador 2)

    // FASE 1: conseguir dos nombres validos
    for (int i = 0; i < 2; i++) {        // Se repite 2 veces: i = 0 jugador 1, i = 1 jugador 2
        bool valido = false;             // Todavia no tenemos un nombre valido
        while (!valido) {                // Se repite hasta que el nombre sea valido
            cout << "\n--- Jugador " << (i + 1) << " ---" << endl;  // i + 1 para mostrar 1 y 2 (no 0 y 1)
            int eleccion = 0;            // 0 significa "escribir un nombre nuevo"
            if (CantRegistrados > 0) {   // Si hay jugadores guardados, se muestra la lista
                cout << "Jugadores registrados:" << endl;
                for (int k = 0; k < CantRegistrados; k++) {
                    cout << "  " << (k + 1) << ". " << Registro[k].nombre << endl;  // "1. Victor", "2. Jhon"...
                }
                cout << "  0. Escribir un nombre nuevo" << endl;
                cout << "Opcion: ";
                eleccion = LeerOpcion(0, CantRegistrados);  // Acepta de 0 hasta la cantidad de jugadores
            }

            if (eleccion > 0) {          // Eligio a alguien de la lista
                nombres[i] = Registro[eleccion - 1].nombre;  // Se resta 1: eligio 1 -> Registro[0]
            } else {                     // Eligio 0, o no habia lista: hay que escribir el nombre
                cout << "Nombre del jugador " << (i + 1) << ": ";
                string leido;            // Aqui se guarda lo que escriba
                getline(cin, leido);     // getline lee la linea COMPLETA, con espacios ("Ana Maria")
                if (cin.eof()) {         // Si se cerro la entrada, se cierra el programa
                    cout << "\nFin de la entrada. Se cierra el programa." << endl;
                    exit(0);
                }
                // quitar espacios al inicio y al final, y cambiar los de adentro por _
                // Paso 1: cambiar los caracteres invisibles \r (lo deja Windows al final)
                //         y \t (tecla Tab) por espacios normales
                string limpio = "";
                for (int k = 0; k < (int)leido.length(); k++) {  // Recorre letra por letra. (int) convierte
                                                                // length() a int para compararlo con k
                    char c = leido[k];                          // La letra de la posicion k
                    if (c == '\r' || c == '\t') c = ' ';        // Si es \r o \t, se cambia por un espacio
                    limpio += c;                                // Se agrega al final de limpio
                }
                // Paso 2: ubicar donde empieza y termina el texto real (sin espacios en los extremos)
                int ini = 0;                                    // Marcador del inicio: primera posicion
                int fin = (int)limpio.length() - 1;             // Marcador del final: ultima posicion
                while (ini <= fin && limpio[ini] == ' ') ini++; // Mientras haya espacio al inicio, avanza
                while (fin >= ini && limpio[fin] == ' ') fin--; // Mientras haya espacio al final, retrocede
                // Paso 3: copiar solo de ini a fin, cambiando los espacios de adentro por _
                // (en el archivo los datos se separan por espacios; "Ana Maria" se partiria en dos)
                string recortado = "";
                for (int k = ini; k <= fin; k++) {
                    if (limpio[k] == ' ') recortado += '_';     // Espacio -> guion bajo
                    else recortado += limpio[k];                // Cualquier otra letra se copia igual
                }
                nombres[i] = recortado;                         // "  Ana Maria " queda "Ana_Maria"
            }

            // Revisiones del nombre
            if (nombres[i] == "") {                             // No puede estar vacio
                cout << "El nombre no puede estar vacio." << endl;
            } else if (i == 1 && NormalizarTexto(nombres[1]) == NormalizarTexto(nombres[0])) {
                // Solo para el jugador 2 (i == 1): no puede llamarse igual que el jugador 1
                cout << nombres[0] << " ya es el jugador 1. Elija otro jugador." << endl;
            } else {
                valido = true;                                  // Todo bien: sale del while
            }
        }
    }

    // FASE 2: registrar los nombres
    for (int i = 0; i < 2; i++) {                       // Para cada jugador
        int pos = BuscarJugadorRegistro(nombres[i]);    // ¿Ya esta en el historial?
        if (pos != -1) {                                // Si ya existia...
            nombres[i] = Registro[pos].nombre;          // ...usa el nombre tal como esta guardado
            cout << "Bienvenido de nuevo, " << nombres[i] << ". Ganadas: " << Registro[pos].ganadas
                 << "  Perdidas: " << Registro[pos].perdidas
                 << "  Empatadas: " << Registro[pos].empatadas << endl;
        } else if (CantRegistrados < CANTMAXREGISTRO) { // Si es nuevo y hay espacio en el historial...
            Registro[CantRegistrados].nombre = nombres[i];   // ...se agrega en el siguiente espacio libre
            Registro[CantRegistrados].ganadas = 0;           // Empieza con 0 ganadas
            Registro[CantRegistrados].perdidas = 0;          // 0 perdidas
            Registro[CantRegistrados].empatadas = 0;         // 0 empatadas
            CantRegistrados++;                               // Hay un jugador mas
            cout << "Jugador nuevo registrado: " << nombres[i] << endl;
        } else {                                        // Si el historial esta lleno (100 jugadores)
            cout << "El registro esta lleno; " << nombres[i]
                 << " no se podra guardar." << endl;
        }
        Jugadores[i].nombrejug = nombres[i];            // Asigna el nombre al jugador de la partida
    }
    GuardarRegistro();                                  // Guarda el historial en el archivo
    nombresConfigurados = true;                         // Marca que ya hay nombres
}


//  MostrarRegistro: imprime la tabla de resultados de todos los
//  jugadores, con las columnas alineadas.
// Tabla de resultados historicos
void MostrarRegistro() {
    if (CantRegistrados == 0) {                 // Si no hay nadie registrado...
        cout << "\nNo hay jugadores registrados." << endl;
        return;                                 // ...sale de la funcion
    }
    cout << "\n=========== REGISTRO DE RESULTADOS ===========" << endl;
    cout << "Jugador              Ganadas  Perdidas  Empatadas" << endl;  // Encabezado de la tabla
    int i = 0;                                  // Posicion del jugador que se va a mostrar
    while (i < CantRegistrados) {               // Recorre todos los jugadores
        string nombre = Registro[i].nombre;
        cout << nombre;                         // Escribe el nombre
        for (int k = nombre.length(); k < 21; k++) cout << " ";  // Rellena con espacios hasta 21
                                                // caracteres para que los numeros queden alineados
        cout << Registro[i].ganadas << "\t " << Registro[i].perdidas << "\t   "  // \t = tabulacion
             << Registro[i].empatadas << endl;
        i = i + 1;                              // Pasa al siguiente jugador
    }
}


//  ValorLetra: dice cuantos puntos vale una ficha, segun la tabla
//  oficial del Scrabble en espanol. Cuando un if hace return, la
//  funcion termina ahi mismo.
// Puntos de una ficha segun la tabla oficial
int ValorLetra(string letra) {
    if (letra == "A" || letra == "E" || letra == "I" || letra == "L" || letra == "N" ||
        letra == "O" || letra == "R" || letra == "S" || letra == "T" || letra == "U") {
        return 1;                                           // Estas letras valen 1 punto
    }
    if (letra == "D" || letra == "G") return 2;             // Valen 2
    if (letra == "B" || letra == "C" || letra == "M" || letra == "P") return 3;  // Valen 3
    if (letra == "F" || letra == "H" || letra == "V" || letra == "Y") return 4;  // Valen 4
    if (letra == "CH" || letra == "Q") return 5;            // Valen 5
    if (letra == "J" || letra == "LL" || letra == "Ñ" || letra == "RR" || letra == "X") return 8;  // Valen 8
    if (letra == "Z") return 10;                            // Vale 10
    return 0;                                               // Cualquier otra cosa vale 0
}


//  LlenarBolsa: crea las 98 fichas del juego.
//  Usa dos arreglos paralelos: letras[i] dice la letra y cantidades[i]
//  dice cuantas fichas hay de esa letra (12 A, 12 E, 6 I...).
// Crea las 98 fichas
void LlenarBolsa() {
    string letras[28] = {"A", "E", "I", "L", "N", "O", "R", "S", "T", "U",  // Las 28 letras distintas
                         "D", "G",
                         "B", "C", "M", "P",
                         "F", "H", "V", "Y",
                         "CH", "Q",
                         "J", "LL", "Ñ", "RR", "X",
                         "Z"};
    int cantidades[28] = {12, 12, 6, 4, 5, 9, 5, 6, 4, 5,   // Cuantas fichas hay de cada letra,
                          5, 2,                             // en el mismo orden que el arreglo letras
                          2, 4, 2, 2,
                          1, 2, 1, 1,
                          1, 1,
                          1, 1, 1, 1, 1,
                          1};
    FichasEnBolsa = 0;                          // La bolsa empieza vacia
    for (int i = 0; i < 28; i++) {              // Por cada tipo de letra...
        for (int j = 0; j < cantidades[i]; j++) {   // ...se repite tantas veces como fichas tenga
            Bolsa[FichasEnBolsa].letra = letras[i];             // Pone la letra en la ficha
            Bolsa[FichasEnBolsa].valor = ValorLetra(letras[i]); // Pone su valor en puntos
            FichasEnBolsa++;                                    // La bolsa tiene una ficha mas
        }
    }
    // Al terminar hay 98 fichas en la bolsa (ordenadas; se sacan al azar despues)
}


//  ReponerPortafichas: despues de jugar, quita del atril las fichas
//  que se usaron y saca fichas nuevas al azar hasta tener 7.
//  Recibe: el numero del jugador (0 o 1).
// Quita las fichas usadas y completa hasta siete
void ReponerPortafichas(int numJugador) {
    // Paso 1: quedarse solo con las fichas NO usadas, corriendolas a la izquierda
    int n = 0;                                  // Posicion donde se va a escribir la siguiente ficha que se queda
    for (int j = 0; j < Jugadores[numJugador].CantFichasAct; j++) {   // Recorre el atril
        if (!AtrilUsado[j]) {                   // Si la ficha j NO se uso...
            Jugadores[numJugador].Atril[n] = Jugadores[numJugador].Atril[j];  // ...se mueve a la posicion n
            n++;
        }
    }
    Jugadores[numJugador].CantFichasAct = n;    // Ahora tiene n fichas

    // Paso 2: sacar fichas al azar de la bolsa hasta tener 7 (o hasta que la bolsa se vacie)
    while (Jugadores[numJugador].CantFichasAct < TOTALPORTAFICHAS && FichasEnBolsa > 0) {
        int pos = rand() % FichasEnBolsa;       // Posicion al azar entre 0 y FichasEnBolsa - 1.
                                                // % es el residuo de la division
        Ficha tomada = Bolsa[pos];              // Guarda la ficha que salio
        Bolsa[pos] = Bolsa[FichasEnBolsa - 1];  // La ultima ficha de la bolsa tapa el hueco
        FichasEnBolsa--;                        // -- resta 1: la bolsa tiene una ficha menos
        Jugadores[numJugador].Atril[Jugadores[numJugador].CantFichasAct] = tomada;  // La pone en el atril
        Jugadores[numJugador].CantFichasAct++;  // El jugador tiene una ficha mas
    }
    // Paso 3: si la bolsa se acabo, deja vacias las posiciones que no se llenaron
    for (int k = Jugadores[numJugador].CantFichasAct; k < TOTALPORTAFICHAS; k++) {
        Jugadores[numJugador].Atril[k].letra = "";
        Jugadores[numJugador].Atril[k].valor = 0;
    }
    // Paso 4: borra las marcas de "usada" para la proxima jugada
    for (int k = 0; k < TOTALPORTAFICHAS; k++) {
        AtrilUsado[k] = false;
    }
}


//  MostrarTablero: dibuja el tablero con los numeros de fila y columna
//  del 1 al 15. Una casilla vacia se ve como "." y el centro como "*".
//  Cada casilla ocupa 3 espacios para que todo quede alineado.
// Imprime el tablero con coordenadas de 1 a 15
void MostrarTablero() {
    cout << endl << "    ";                   // Espacio en blanco donde iria el numero de fila
    for (int c = 0; c < TAMANOTABLERO; c++) { // Escribe los numeros de columna
        if (c + 1 < 10) cout << "  " << (c + 1);  // Numeros de 1 cifra: 2 espacios antes
        else cout << " " << (c + 1);              // Numeros de 2 cifras: 1 espacio antes
    }
    cout << endl;
    for (int r = 0; r < TAMANOTABLERO; r++) {     // Por cada fila...
        if (r + 1 < 10) cout << "  " << (r + 1) << " ";  // ...escribe su numero
        else cout << " " << (r + 1) << " ";
        for (int c = 0; c < TAMANOTABLERO; c++) { // ...y luego cada columna de esa fila
            string casilla = Tablero[r][c];       // Lo que hay en la casilla
            if (casilla == "") {                  // Si esta vacia...
                if (r == CENTRO && c == CENTRO) casilla = "*";  // ...el centro se muestra como *
                else casilla = ".";                             // ...las demas como .
            }
            int ancho = casilla.length();         // Cuantos bytes ocupa ("A" = 1, "CH" = 2)
            if (casilla == "Ñ") ancho = 1;      // 2 bytes, 1 caracter en pantalla
            for (int k = ancho; k < 3; k++) cout << " ";  // Rellena con espacios hasta 3
            cout << casilla;                      // Escribe la casilla
        }
        cout << endl;                             // Salto de linea al terminar la fila
    }
    cout << "Fichas en la bolsa: " << FichasEnBolsa << endl;
}


//  MostrarPortafichas: muestra las fichas de un jugador con su valor.
//  Ejemplo:  Portafichas de Victor: [C(3)] [A(1)] [S(1)] ...
// Fichas del jugador con su valor
void MostrarPortafichas(int numJugador) {
    cout << "Portafichas de " << Jugadores[numJugador].nombrejug << ": ";
    if (Jugadores[numJugador].CantFichasAct == 0) {   // Si no tiene fichas
        cout << "(vacio)";
    }
    for (int k = 0; k < Jugadores[numJugador].CantFichasAct; k++) {  // Recorre sus fichas
        cout << "[" << Jugadores[numJugador].Atril[k].letra << "("   // Escribe [letra(valor)]
             << Jugadores[numJugador].Atril[k].valor << ")] ";
    }
    cout << endl;
}


//  PedirPalabra: le pregunta al jugador que palabra quiere poner,
//  donde empieza (fila y columna) y en que direccion (H o V).
//  Guarda las respuestas en las variables globales PalabraJugada,
//  fila, columna y sentido.
// Lee la palabra, la fila, la columna y el sentido
void PedirPalabra() {
    PalabraJugada = "";                         // Empieza vacia
    while (PalabraJugada == "") {               // Insiste mientras la palabra este vacia
        cout << "Escriba la palabra: ";
        string leida;
        getline(cin, leida);                    // Lee la linea completa
        if (cin.eof()) {                        // Si se cerro la entrada, cierra el programa
            cout << "\nFin de la entrada. Se cierra el programa." << endl;
            exit(0);
        }
        string sinEspacios = "";                // Copia de la palabra sin espacios
        for (int k = 0; k < (int)leida.length(); k++) {
            if (leida[k] != ' ' && leida[k] != '\r' && leida[k] != '\t') {  // Si no es espacio, \r ni Tab...
                sinEspacios += leida[k];        // ...se copia
            }
        }
        PalabraJugada = NormalizarTexto(sinEspacios);  // " casa " -> "CASA"
    }
    if (PrimeraJugada) {                        // En la primera jugada de la partida...
        fila = CENTRO;                          // ...la palabra empieza obligatoriamente en el centro
        columna = CENTRO;
        cout << "La primera letra se ubica en la casilla central (*)." << endl;
    } else {                                    // En las demas jugadas se pregunta la posicion
        cout << "Fila de la primera letra (1-15): ";
        fila = LeerOpcion(1, 15) - 1;           // Se resta 1: el usuario dice 8 -> posicion 7 del arreglo
        cout << "Columna de la primera letra (1-15): ";
        columna = LeerOpcion(1, 15) - 1;
    }

    sentido = ' ';                              // Valor no valido para que entre al while
    while (sentido != 'H' && sentido != 'V') {  // Repite hasta que sea H o V
        cout << "Sentido (H = horizontal, V = vertical): ";
        string s;
        getline(cin, s);                        // Lee la respuesta
        if (cin.eof()) {
            cout << "\nFin de la entrada. Se cierra el programa." << endl;
            exit(0);
        }
        if (s.length() > 0) {                   // Si escribio algo...
            sentido = s[0];                     // ...toma solo la primera letra
            if (sentido == 'h') sentido = 'H';  // Acepta minuscula
            if (sentido == 'v') sentido = 'V';
        }
        if (sentido != 'H' && sentido != 'V') { // Si no es ni H ni V, avisa
            cout << "Sentido no valido." << endl;
        }
    }
}


//  PartirPalabra: separa la palabra en FICHAS.
//  En el Scrabble en espanol CH, LL, RR y Ñ son una sola ficha.
//  Ejemplo: "CHURRO" -> CH, U, RR, O  (4 fichas)
//  El resultado queda en palabraFichas y la cantidad en CantFichasPalabra.
// Separa la palabra en fichas (CH, LL, RR y Ñ = una ficha)
void PartirPalabra(string palabra) {
    CantFichasPalabra = 0;                      // Empieza con 0 fichas
    int i = 0;                                  // Posicion actual en el texto
    int n = palabra.length();                   // Cuantos bytes tiene la palabra
    while (i < n) {                             // Recorre la palabra
        string ficha;                           // Aqui va la ficha que se encuentre
        unsigned char c = palabra[i];           // Byte actual
        unsigned char sig = 0;                  // Byte siguiente (0 si no hay)
        if (i + 1 < n) sig = palabra[i + 1];

        if (c == 0xC3 && sig == 0x91) {         // Los 2 bytes de la Ñ en UTF-8
            ficha = "Ñ";
            i += 2;                             // Avanza 2
        } else if ((c == 'C' && sig == 'H') || (c == 'L' && sig == 'L') ||
                   (c == 'R' && sig == 'R')) {  // Si son CH, LL o RR...
            ficha = palabra.substr(i, 2);       // ...toma las 2 letras como una ficha. substr(desde, cuantas)
            i += 2;                             // Avanza 2
        } else {                                // Cualquier otra letra
            ficha = palabra.substr(i, 1);       // Toma 1 letra
            i++;                                // Avanza 1
        }
        if (CantFichasPalabra < MAXCANTPALABRA) {   // Solo guarda si hay espacio en el arreglo (15)
            palabraFichas[CantFichasPalabra] = ficha;
        }
        CantFichasPalabra++;                    // Cuenta siempre: si pasa de 15, despues se rechaza por no caber
    }
}


//  ValidarJugadaTablero: revisa las reglas que se pueden comprobar
//  sin preguntarle al rival. Devuelve true si todo esta bien.
//   1. Minimo 2 fichas
//   2. Que quepa en el tablero
//   3. La primera palabra pasa por el centro
//   4. Las demas tocan alguna letra que ya este en el tablero
//   5. Las letras coinciden y el jugador tiene las fichas
//   6. Pone al menos una ficha propia
// Revisa la jugada en el tablero, en seis pasos
bool ValidarJugadaTablero() {
    for (int k = 0; k < TOTALPORTAFICHAS; k++) {  // Borra las marcas de "usada" para empezar limpio
        AtrilUsado[k] = false;
    }

    // Regla 1: minimo 2 fichas
    if (CantFichasPalabra < 2) {
        cout << "La palabra debe tener al menos dos fichas." << endl;
        return false;                           // return false = la jugada no vale
    }
    // Regla 2: que la palabra quepa en el tablero
    if (fila < 0 || fila >= TAMANOTABLERO || columna < 0 || columna >= TAMANOTABLERO ||
        (sentido == 'H' && columna + CantFichasPalabra > TAMANOTABLERO) ||   // En horizontal se sale por la derecha
        (sentido == 'V' && fila + CantFichasPalabra > TAMANOTABLERO)) {      // En vertical se sale por abajo
        cout << "La palabra no cabe en el tablero." << endl;
        return false;
    }

    // Direccion de avance: dr = cuanto cambia la fila, dc = cuanto cambia la columna en cada paso.
    // Horizontal: la columna aumenta (dc = 1). Vertical: la fila aumenta (dr = 1).
    // Asi la ficha numero k esta en (fila + dr*k, columna + dc*k) en los dos sentidos.
    int dr = 0, dc = 0;
    if (sentido == 'H') dc = 1;
    else dr = 1;

    // Reglas 3 y 4: revisar si pasa por el centro y si toca alguna letra existente
    bool pasaCentro = false;
    bool conecta = false;
    for (int k = 0; k < CantFichasPalabra; k++) {   // Por cada ficha de la palabra
        int r = fila + dr * k;                      // Fila de la ficha k
        int c = columna + dc * k;                   // Columna de la ficha k
        if (r == CENTRO && c == CENTRO) pasaCentro = true;            // ¿Es la casilla central?
        if (Tablero[r][c] != "") conecta = true;                      // ¿Usa una letra que ya estaba?
        if (r > 0 && Tablero[r - 1][c] != "") conecta = true;         // ¿Hay letra arriba? (r > 0 evita salirse)
        if (r < TAMANOTABLERO - 1 && Tablero[r + 1][c] != "") conecta = true;  // ¿Hay letra abajo?
        if (c > 0 && Tablero[r][c - 1] != "") conecta = true;         // ¿Hay letra a la izquierda?
        if (c < TAMANOTABLERO - 1 && Tablero[r][c + 1] != "") conecta = true;  // ¿Hay letra a la derecha?
    }
    if (PrimeraJugada && !pasaCentro) {             // Regla 3
        cout << "La primera palabra debe pasar por la casilla central (fila 8, columna 8)." << endl;
        return false;
    }
    if (!PrimeraJugada && !conecta) {               // Regla 4
        cout << "La palabra debe usar o tocar al menos una letra del tablero." << endl;
        return false;
    }

    // Regla 5: revisar casilla por casilla
    int numJ = TurnoActual;                         // Numero del jugador que esta jugando
    int nuevas = 0;                                 // Cuantas fichas propias pone
    for (int k = 0; k < CantFichasPalabra; k++) {
        int r = fila + dr * k;
        int c = columna + dc * k;
        if (Tablero[r][c] != "") {                  // Si la casilla ya tiene una letra...
            if (Tablero[r][c] != palabraFichas[k]) {    // ...debe ser la misma que pide la palabra
                cout << "En la fila " << (r + 1) << ", columna " << (c + 1)
                     << " ya hay una " << Tablero[r][c] << " y la palabra pide "
                     << palabraFichas[k] << "." << endl;
                return false;
            }
            // Si coincide, se reutiliza esa letra y no se gasta ficha del atril
        } else {                                    // Si la casilla esta vacia, el jugador debe tener la ficha
            bool hallada = false;
            for (int j = 0; j < Jugadores[numJ].CantFichasAct && !hallada; j++) {  // Busca en su atril
                if (!AtrilUsado[j] && Jugadores[numJ].Atril[j].letra == palabraFichas[k]) {
                    // Si la ficha j no se ha usado y es la letra que se necesita...
                    AtrilUsado[j] = true;           // ...se marca como usada (para no usarla dos veces)
                    hallada = true;                 // ...y se deja de buscar
                }
            }
            if (!hallada) {                         // No la tiene
                cout << "No tiene la ficha " << palabraFichas[k]
                     << " en su portafichas." << endl;
                return false;
            }
            nuevas++;                               // Una ficha propia mas
        }
    }
    // Regla 6: tiene que poner al menos una ficha propia
    if (nuevas == 0) {
        cout << "Debe poner al menos una ficha de su portafichas." << endl;
        return false;
    }
    return true;                                    // Paso todas las reglas
}


//  ValidarConRival: como el juego no tiene diccionario, el OTRO
//  jugador decide si la palabra existe. Devuelve true si dice que si.
// El jugador que no tiene el turno dice si la palabra vale
bool ValidarConRival(string palabra) {
    int rival = 1 - TurnoActual;                    // Si juega el 0, el rival es 1; si juega el 1, el rival es 0
    cout << Jugadores[rival].nombrejug << ", la palabra \"" << palabra   // \" escribe comillas en pantalla
         << "\" es valida?  1. Si   2. No" << endl;
    cout << "Opcion: ";
    int respuesta = LeerOpcion(1, 2);               // Solo acepta 1 o 2
    return respuesta == 1;                          // Devuelve true si respondio 1, false si respondio 2
}


//  ArmarYValidarPalabras: averigua TODAS las palabras que se formaron
//  con la jugada (la principal y las cruzadas) y le pide al rival que
//  apruebe cada una. Cuando se llama, las fichas YA estan en Tablero
//  y CopiaTablero tiene el tablero de antes de la jugada.
// ArmarYValidarPalabras: arma la principal y las cruzadas, revisa las fichas
// dobles sueltas y le pide al rival que apruebe cada palabra
bool ArmarYValidarPalabras() {
    CantPalabrasFormadas = 0;                       // Empieza sin palabras
    int dr = 0, dc = 0;                             // Direccion de la palabra (igual que antes)
    if (sentido == 'H') dc = 1;
    else dr = 1;

    // palabra principal
    // Paso A: RETROCEDER desde la primera letra mientras haya letras antes.
    // Si alguien escribe "AS" pegado a "CAS", la palabra real es "CASAS".
    int r = fila, c = columna;                      // Empieza en la primera letra que puso el jugador
    while (r - dr >= 0 && c - dc >= 0 && Tablero[r - dr][c - dc] != "") {  // Si la casilla anterior existe y tiene letra...
        r -= dr;                                    // ...retrocede una casilla (-= resta)
        c -= dc;
    }
    // Paso B: AVANZAR desde el inicio leyendo letras hasta encontrar una casilla vacia
    int r0 = r, c0 = c;                             // Guarda donde empieza la palabra (r y c van a cambiar)
    string texto = "";                              // Aqui se arma la palabra
    int largo = 0;                                  // Cuantas fichas tiene
    while (r < TAMANOTABLERO && c < TAMANOTABLERO && Tablero[r][c] != "") {  // Mientras la casilla exista y tenga letra
        texto += Tablero[r][c];                     // Pega la letra
        largo++;                                    // Una ficha mas
        r += dr;                                    // Avanza una casilla (+= suma)
        c += dc;
    }
    PalabrasFormadas[0] = texto;                    // La palabra principal siempre va en la posicion 0
    FilaPalabra[0] = r0;                            // Donde empieza
    ColumnaPalabra[0] = c0;
    SentidoPalabra[0] = sentido;                    // Su sentido
    LargoPalabra[0] = largo;                        // Cuantas fichas tiene
    CantPalabrasFormadas = 1;                       // Por ahora hay 1 palabra

    // Palabras cruzadas: las que se forman en la direccion PERPENDICULAR
    int pr = dc, pc = dr;                           // Perpendicular = intercambiar dr y dc
    char sentidoCruz = 'H';                         // Sentido de las cruzadas
    if (sentido == 'H') sentidoCruz = 'V';          // Si la principal es horizontal, las cruzadas son verticales
    for (int k = 0; k < CantFichasPalabra; k++) {   // Por cada ficha de la palabra
        int fr = fila + dr * k;                     // Fila de la ficha k
        int fc = columna + dc * k;                  // Columna de la ficha k
        if (CopiaTablero[fr][fc] == "") {           // Solo si la ficha es NUEVA (antes la casilla estaba vacia).
                                                    // Las letras que ya estaban ya formaron sus palabras antes.
            int rr = fr, cc = fc;
            while (rr - pr >= 0 && cc - pc >= 0 && Tablero[rr - pr][cc - pc] != "") {  // Retrocede en perpendicular
                rr -= pr;
                cc -= pc;
            }
            int ir = rr, ic = cc;                   // Donde empieza la cruzada
            string cruzada = "";
            int largoCruz = 0;
            while (rr < TAMANOTABLERO && cc < TAMANOTABLERO && Tablero[rr][cc] != "") {  // Avanza leyendo letras
                cruzada += Tablero[rr][cc];
                largoCruz++;
                rr += pr;
                cc += pc;
            }
            if (largoCruz >= 2 && CantPalabrasFormadas < MAXPALABRASFORMADAS) {  // Una sola letra no es palabra
                PalabrasFormadas[CantPalabrasFormadas] = cruzada;     // Se guarda en la siguiente posicion libre
                FilaPalabra[CantPalabrasFormadas] = ir;
                ColumnaPalabra[CantPalabrasFormadas] = ic;
                SentidoPalabra[CantPalabrasFormadas] = sentidoCruz;
                LargoPalabra[CantPalabrasFormadas] = largoCruz;
                CantPalabrasFormadas++;                               // Una palabra mas
            }
        }
    }

    // Revisar que no haya CH, LL o RR armados con DOS fichas sueltas
    // (por ejemplo, una L que ya estaba y otra L nueva al lado: no se permite)
    for (int i = 0; i < CantPalabrasFormadas; i++) {    // Por cada palabra formada
        int wr = 0, wc = 0;                             // Direccion de ESA palabra
        if (SentidoPalabra[i] == 'H') wc = 1;
        else wr = 1;
        for (int k = 0; k + 1 < LargoPalabra[i]; k++) { // Compara cada casilla con la siguiente
            string a = Tablero[FilaPalabra[i] + wr * k][ColumnaPalabra[i] + wc * k];              // Casilla k
            string b = Tablero[FilaPalabra[i] + wr * (k + 1)][ColumnaPalabra[i] + wc * (k + 1)];  // Casilla k + 1
            if ((a == "C" && b == "H") || (a == "L" && b == "L") || (a == "R" && b == "R")) {
                cout << "La palabra " << PalabrasFormadas[i] << " forma " << a << b
                     << " con dos fichas sueltas; " << a << b
                     << " debe ser una sola ficha." << endl;
                return false;                           // La jugada no vale
            }
        }
    }

    // El rival aprueba cada palabra
    cout << "\n----- VALIDACION DEL RIVAL -----" << endl;
    cout << Jugadores[TurnoActual].nombrejug << " formo " << CantPalabrasFormadas
         << " palabra(s): ";
    for (int i = 0; i < CantPalabrasFormadas; i++) {    // Muestra la lista de palabras
        cout << PalabrasFormadas[i];
        if (i < CantPalabrasFormadas - 1) cout << ", "; // Coma entre palabras, pero no despues de la ultima
    }
    cout << endl;
    for (int i = 0; i < CantPalabrasFormadas; i++) {    // Pregunta por cada palabra
        if (!ValidarConRival(PalabrasFormadas[i])) {    // Si el rival rechaza alguna...
            cout << Jugadores[1 - TurnoActual].nombrejug << " rechazo la palabra "
                 << PalabrasFormadas[i] << "." << endl;
            return false;                               // ...toda la jugada se cae
        }
    }
    cout << "Todas las palabras fueron aceptadas." << endl;
    return true;                                        // Todas aceptadas
}


//  CalcularPuntos: suma el valor de las letras de TODAS las palabras
//  formadas, leyendolas del tablero (tambien cuentan las letras que
//  ya estaban). No hay casillas de puntaje doble ni triple.
// Suma las fichas de cada palabra leyendolas del tablero
int CalcularPuntos() {
    int total = 0;                                      // Empieza en 0 puntos
    for (int i = 0; i < CantPalabrasFormadas; i++) {    // Por cada palabra formada
        int wr = 0, wc = 0;                             // Su direccion
        if (SentidoPalabra[i] == 'H') wc = 1;
        else wr = 1;
        for (int k = 0; k < LargoPalabra[i]; k++) {     // Por cada ficha de esa palabra
            total += ValorLetra(Tablero[FilaPalabra[i] + wr * k][ColumnaPalabra[i] + wc * k]);  // Suma su valor
        }
    }
    return total;                                       // Devuelve el total
}


//  CambiarFichas: en vez de jugar, el jugador cambia TODAS sus fichas.
//  Devuelve false si en la bolsa no hay suficientes fichas.
// Cambia todas las fichas del jugador en turno
bool CambiarFichas() {
    int numJ = TurnoActual;                     // El jugador que esta jugando
    int n = Jugadores[numJ].CantFichasAct;      // Cuantas fichas tiene
    if (n == 0 || FichasEnBolsa < n) {          // Si no tiene fichas o la bolsa no alcanza...
        return false;                           // ...no se puede cambiar
    }
    Ficha temporal[TOTALPORTAFICHAS];           // Lugar para guardar las fichas viejas
    for (int k = 0; k < n; k++) {
        temporal[k] = Jugadores[numJ].Atril[k]; // 1. Guarda las fichas viejas aparte
    }
    for (int k = 0; k < n; k++) {               // 2. Saca n fichas nuevas al azar
        int pos = rand() % FichasEnBolsa;
        Jugadores[numJ].Atril[k] = Bolsa[pos];
        Bolsa[pos] = Bolsa[FichasEnBolsa - 1];  // La ultima ficha tapa el hueco
        FichasEnBolsa--;
    }
    for (int k = 0; k < n; k++) {               // 3. Mete las viejas en la bolsa.
        Bolsa[FichasEnBolsa] = temporal[k];     //    Se hace DESPUES de sacar las nuevas para que
        FichasEnBolsa++;                        //    no le vuelvan a salir las mismas.
    }
    return true;                                // El cambio se hizo
}


//  ProbarJugada: maneja un intento completo de poner una palabra.
//  Si algo falla, deja intentar otra palabra o saltar el turno.
//  Devuelve true si la palabra quedo en el tablero.
// Pide, valida y coloca una palabra, con reintento
bool ProbarJugada() {
    while (true) {                              // Ciclo "infinito": solo se sale con return
        PedirPalabra();                         // Pide palabra, fila, columna y sentido
        PartirPalabra(PalabraJugada);           // La separa en fichas

        bool valida = ValidarJugadaTablero();   // Revisa las reglas del tablero
        if (valida) {
            // Guarda una copia del tablero (respaldo) por si hay que deshacer
            for (int r = 0; r < TAMANOTABLERO; r++)
                for (int c = 0; c < TAMANOTABLERO; c++)
                    CopiaTablero[r][c] = Tablero[r][c];

            // Pone las fichas en el tablero
            for (int k = 0; k < CantFichasPalabra; k++) {
                if (sentido == 'H') Tablero[fila][columna + k] = palabraFichas[k];  // Hacia la derecha
                else Tablero[fila + k][columna] = palabraFichas[k];                 // Hacia abajo
            }
            MostrarTablero();                   // Muestra como quedo para que el rival lo vea

            valida = ArmarYValidarPalabras();   // Busca las palabras formadas y el rival las aprueba
            if (!valida) {                      // Si se rechazo...
                for (int r = 0; r < TAMANOTABLERO; r++)
                    for (int c = 0; c < TAMANOTABLERO; c++)
                        Tablero[r][c] = CopiaTablero[r][c];   // ...restaura el tablero desde la copia
                cout << "La palabra se quito del tablero." << endl;
            }
        }

        if (valida) {                           // Si todo salio bien
            PuntosTurno = CalcularPuntos();     // Calcula los puntos
            Jugadores[TurnoActual].puntajejug += PuntosTurno;  // Se los suma al jugador
            ReponerPortafichas(TurnoActual);    // Repone sus fichas hasta 7
            PrimeraJugada = false;              // Ya no es la primera jugada
            return true;                        // Termina: la jugada se hizo
        }

        // Si llego aqui, la jugada fallo
        for (int k = 0; k < TOTALPORTAFICHAS; k++) AtrilUsado[k] = false;  // Borra las marcas
        cout << "1. Intentar otra palabra   2. Saltar el turno" << endl;
        cout << "Opcion: ";
        if (LeerOpcion(1, 2) == 2) {            // Si elige saltar...
            return false;                       // ...termina sin jugar
        }
        MostrarPortafichas(TurnoActual);        // Si elige intentar otra, le recuerda sus fichas y repite
    }
}


//  JugarTurno: controla el turno completo de un jugador:
//  muestra tablero y fichas, y le deja elegir que hacer.
// Controla el turno completo de un jugador
void JugarTurno() {
    int numJ = TurnoActual;                     // El jugador que tiene el turno
    PuntosTurno = 0;                            // Empieza con 0 puntos en este turno

    cout << "\n=========================================" << endl;
    cout << "Turno de " << Jugadores[numJ].nombrejug << endl;
    cout << "=========================================" << endl;
    MostrarTablero();                           // Muestra el tablero
    MostrarPortafichas(numJ);                   // Muestra sus fichas

    bool terminado = false;                     // El turno aun no termina
    while (!terminado) {                        // Repite el menu hasta que el turno termine
        cout << "1. Colocar palabra   2. Cambiar todas las fichas   3. Saltar turno" << endl;
        cout << "Opcion: ";
        int opcion = LeerOpcion(1, 3);
        if (opcion == 1) {                      // Colocar palabra
            if (ProbarJugada()) Jugadores[numJ].TurnosSaltados = 0;  // Jugo: el contador vuelve a 0
            else Jugadores[numJ].TurnosSaltados++;                   // Termino saltando: cuenta como salto
            terminado = true;
        } else if (opcion == 2) {               // Cambiar fichas
            if (CambiarFichas()) {
                cout << "Se cambiaron todas las fichas." << endl;
                Jugadores[numJ].TurnosSaltados = 0;  // Cambiar NO cuenta como saltar
                terminado = true;
            } else {                            // No se pudo: vuelve a mostrar el menu
                cout << "No hay suficientes fichas en la bolsa para cambiar. Elija otra accion." << endl;
            }
        } else {                                // Opcion 3: saltar el turno
            Jugadores[numJ].TurnosSaltados++;
            terminado = true;
        }
    }

    cout << "\nPuntos de esta jugada: " << PuntosTurno << endl;          // Resumen del turno
    cout << "Puntaje total -> " << Jugadores[0].nombrejug << ": " << Jugadores[0].puntajejug
         << "   " << Jugadores[1].nombrejug << ": " << Jugadores[1].puntajejug << endl;
    MostrarPortafichas(numJ);                   // Muestra sus fichas ya repuestas
}


//  ActualizarResultados: al terminar la partida dice quien gano y lo
//  suma al historial (ganadas, perdidas o empatadas).
// Ganador o empate y registro
void ActualizarResultados() {
    int p0 = Jugadores[0].puntajejug;           // Puntos del jugador 1
    int p1 = Jugadores[1].puntajejug;           // Puntos del jugador 2
    int pos0 = BuscarJugadorRegistro(Jugadores[0].nombrejug);   // Donde esta cada uno en el historial
    int pos1 = BuscarJugadorRegistro(Jugadores[1].nombrejug);

    cout << "\n=============== FIN DEL JUEGO ===============" << endl;
    cout << Jugadores[0].nombrejug << ": " << p0 << " puntos" << endl;
    cout << Jugadores[1].nombrejug << ": " << p1 << " puntos" << endl;

    if (p0 > p1) {                              // Gano el jugador 1
        cout << "Gana " << Jugadores[0].nombrejug << "!" << endl;
        if (pos0 != -1) Registro[pos0].ganadas++;   // != -1 protege por si no se pudo registrar
        if (pos1 != -1) Registro[pos1].perdidas++;
    } else if (p1 > p0) {                       // Gano el jugador 2
        cout << "Gana " << Jugadores[1].nombrejug << "!" << endl;
        if (pos1 != -1) Registro[pos1].ganadas++;
        if (pos0 != -1) Registro[pos0].perdidas++;
    } else {                                    // Empataron
        cout << "Empate!" << endl;
        if (pos0 != -1) Registro[pos0].empatadas++;
        if (pos1 != -1) Registro[pos1].empatadas++;
    }
    GuardarRegistro();                          // Guarda el historial actualizado en el archivo
}


//  JugarPartida: dirige toda la partida.
//  Prepara el tablero, la bolsa y los jugadores; repite turnos hasta
//  que la partida termine, y al final guarda los resultados.
// Prepara la partida, repite los turnos y cierra
void JugarPartida() {
    // Deja las 225 casillas del tablero vacias (dos for: uno para filas y otro para columnas)
    for (int r = 0; r < TAMANOTABLERO; r++)
        for (int c = 0; c < TAMANOTABLERO; c++)
            Tablero[r][c] = "";

    LlenarBolsa();                              // Pone las 98 fichas en la bolsa

    for (int i = 0; i < 2; i++) {               // Prepara a cada jugador
        Jugadores[i].puntajejug = 0;            // 0 puntos
        Jugadores[i].CantFichasAct = 0;         // 0 fichas
        Jugadores[i].TurnosSaltados = 0;        // 0 turnos saltados
        ReponerPortafichas(i);                  // Le reparte 7 fichas al azar
    }

    PrimeraJugada = true;                       // La primera palabra debe ir en el centro
    PartidaTerminada = false;                   // La partida empieza
    TurnoActual = 0;                            // Empieza el jugador 1

    while (!PartidaTerminada) {                 // Repite turnos hasta que termine la partida
        JugarTurno();                           // Juega un turno
        // Termina si la bolsa esta vacia Y el jugador que acaba de jugar se quedo sin fichas
        bool sinFichas = FichasEnBolsa == 0 && Jugadores[TurnoActual].CantFichasAct == 0;
        // o si los dos jugadores llevan 2 o mas turnos seguidos sin jugar
        bool ambosSaltaron = Jugadores[0].TurnosSaltados >= 2 && Jugadores[1].TurnosSaltados >= 2;
        if (sinFichas || ambosSaltaron) {
            PartidaTerminada = true;            // Termina la partida
        } else {
            TurnoActual = 1 - TurnoActual;      // Cambia de jugador: 0 -> 1, 1 -> 0
        }
    }
    ActualizarResultados();                     // Dice quien gano y lo guarda
}
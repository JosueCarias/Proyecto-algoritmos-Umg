#include <iostream>
#include <string>
#include <fstream>
#include <cstdio> 
#include <sstream>
#include <iomanip>
#include <ctime>
#include <stdexcept>

using namespace std;

// Variables para las rutas de los archivos (uso de rutas relativas)
string CTASMONETARIAS = "CTASMONETARIAS.txt";
string TRANSACCIONES = "TRANSACCIONES.txt";
string TEMP = "TEMP.txt";

// Códigos ANSI para colores del menu
#define RESET   "\033[0m"
#define RED     "\033[31m"      // Rojo
#define GREEN   "\033[32m"      // Verde
#define YELLOW  "\033[33m"      // Amarillo
#define BLUE    "\033[34m"      // Azul
#define MAGENTA "\033[35m"      // Magenta
#define CYAN    "\033[36m"      // Cian
#define WHITE   "\033[37m"      // Blanco

// Función para centrar el texto
void MensajeCentrado(const string &text) {
    int width = 80; 
    int padding = (width - text.length()) / 2;
    for (int i = 0; i < padding; ++i) cout << " ";
    cout << text << endl;
}


// Función para mostrar el menú
char menu() {
    char opcion = 0;
    MensajeCentrado(BLUE "*********************************************" RESET);
    MensajeCentrado(BLUE "*                                            *" RESET);
    MensajeCentrado(YELLOW "*           BIENVENIDO AL SISTEMA            *" RESET);
    MensajeCentrado(BLUE "*                                            *" RESET);
    MensajeCentrado(BLUE "**********************************************" RESET);
    MensajeCentrado(BLUE "*                                            *" RESET);
    MensajeCentrado(CYAN "*          Por favor, ingrese la opción      *" RESET);
    MensajeCentrado(BLUE "*                                            *" RESET);
    MensajeCentrado(BLUE "**********************************************" RESET);
    MensajeCentrado(BLUE "*                                            *" RESET);
    MensajeCentrado(GREEN "* 1). Crear cuentas monetarias               *" RESET);
    MensajeCentrado(GREEN "* 2). Operar depositos                       *" RESET);
    MensajeCentrado(GREEN "* 3). Operar retiros                         *" RESET);
    MensajeCentrado(GREEN "* 4). Transferencias                         *" RESET);
    MensajeCentrado(GREEN "* 5). Consulta saldo de cuenta               *" RESET);
    MensajeCentrado(RED   "* 6). Salir                                  *" RESET);
    MensajeCentrado(BLUE "*                                            *" RESET);
    MensajeCentrado(BLUE "**********************************************" RESET);
    cin >> opcion;
    return opcion;
}


// Función para obtener la fecha y hora actuales
string obtenerFechaHoraActual() {
    time_t ahora = time(0);
    tm *tiempoLocal = localtime(&ahora);
    stringstream fechaHora;
    fechaHora << setfill('0') << setw(2) << tiempoLocal->tm_mday << "/"
              << setw(2) << tiempoLocal->tm_mon + 1 << "/"  
              << tiempoLocal->tm_year + 1900 << " "  
              << setw(2) << tiempoLocal->tm_hour << ":"
              << setw(2) << tiempoLocal->tm_min << ":"
              << setw(2) << tiempoLocal->tm_sec;
    return fechaHora.str();
}

// opcion 1 del menu 
// funcion para crear cuentas
string crearCuentaMonetaria() {
    ofstream cuentas(CTASMONETARIAS, ios::app);  
    if (!cuentas.is_open()) {
        return "Error: No se pudo abrir el archivo de cuentas.";
    }

    string numeroCuenta, nombre, telefono, correo, saldoInicial;

    cout << "Ingrese el numero de cuenta: ";
    cin >> numeroCuenta;
    cout << "Ingrese el nombre del titular: ";
    cin.ignore(); 
    getline(cin, nombre);
    cout << "Ingrese el telefono del titular: ";
    getline(cin, telefono);
    cout << "Ingrese el correo del titular: ";
    getline(cin, correo);
    cout << "Ingrese el saldo inicial: ";
    cin >> saldoInicial;

    cuentas << numeroCuenta << ";" << nombre << ";" << telefono << ";" << correo << ";" << saldoInicial << endl;
    cuentas.close();

    return "Cuenta creada exitosamente.";
}

// opcion 2 del menu 
// Función para operar depósitos
string depositos() {
    string numeroCuenta, linea, monto, nuevoSaldo, fecha;
    bool cuentaEncontrada = false;

    ifstream cuentas(CTASMONETARIAS);
    if (!cuentas.is_open()) {
        return "Error: No se pudo abrir el archivo de cuentas.";
    }

    ofstream temp(TEMP);
    if (!temp.is_open()) {
        cuentas.close();
        return "Error: No se pudo crear el archivo temporal.";
    }

    cout << "Ingrese el numero de cuenta: ";
    cin >> numeroCuenta;

    while (getline(cuentas, linea)) {
        stringstream ss(linea);
        string cuenta, nombre, telefono, correo, saldo;

        getline(ss, cuenta, ';');
        getline(ss, nombre, ';');
        getline(ss, telefono, ';');
        getline(ss, correo, ';');
        getline(ss, saldo, ';');

        if (cuenta == numeroCuenta) {
            cuentaEncontrada = true;
            cout << "Cuenta encontrada: " << nombre << endl;
            cout << "Saldo actual: " << saldo << endl;

            cout << "Ingrese la cantidad a depositar: ";
            cin >> monto;

            try {
                nuevoSaldo = to_string(stoi(saldo) + stoi(monto));
                cout << "Depósito exitoso. Nuevo saldo: " << nuevoSaldo << endl;
                temp << cuenta << ";" << nombre << ";" << telefono << ";" << correo << ";" << nuevoSaldo << endl;
            } catch (invalid_argument &e) {
                cout << "Error: Datos no válidos en el archivo de cuentas." << endl;
                temp << linea << endl;
            }
        } else {
            temp << linea << endl;
        }
    }

    cuentas.close();
    temp.close();

    if (cuentaEncontrada) {
        if (remove(CTASMONETARIAS.c_str()) != 0) {
            return "Error al eliminar el archivo original.";
        }

        if (rename(TEMP.c_str(), CTASMONETARIAS.c_str()) != 0) {
            return "Error al renombrar archivo temporal.";
        }

        ofstream transacciones(TRANSACCIONES, ios::app);
        if (!transacciones.is_open()) {
            return "Error al registrar transaccion.";
        }

        fecha = obtenerFechaHoraActual();
        transacciones << numeroCuenta << ";" << fecha << ";" << monto << ";D" << endl;
        transacciones.close();

        return "Depósito registrado correctamente.";
    } else {
        remove(TEMP.c_str());
        return "Cuenta no encontrada.";
    }
}

//Opcion 3 del menu
// Función para operar retiros
string retiros() {
    string numeroCuenta, linea, monto, nuevoSaldo, fecha;
    bool cuentaEncontrada = false;

    ifstream cuentas(CTASMONETARIAS);
    if (!cuentas.is_open()) {
        return "Error: No se pudo abrir el archivo de cuentas.";
    }

    ofstream temp(TEMP);
    if (!temp.is_open()) {
        cuentas.close();
        return "Error: No se pudo crear el archivo temporal.";
    }

    cout << "Ingrese el numero de cuenta: ";
    cin >> numeroCuenta;

    while (getline(cuentas, linea)) {
        stringstream ss(linea);
        string cuenta, nombre, telefono, correo, saldo;

        getline(ss, cuenta, ';');
        getline(ss, nombre, ';');
        getline(ss, telefono, ';');
        getline(ss, correo, ';');
        getline(ss, saldo, ';');

        if (cuenta == numeroCuenta) {
            cuentaEncontrada = true;
            cout << "Cuenta encontrada: " << nombre << endl;
            cout << "Saldo actual: " << saldo << endl;

            cout << "Ingrese la cantidad del retiro: ";
            cin >> monto;

            try {
                if (stoi(saldo) >= stoi(monto)) {
                    nuevoSaldo = to_string(stoi(saldo) - stoi(monto));
                    cout << "Retiro exitoso. Nuevo saldo: " << nuevoSaldo << endl;
                    temp << cuenta << ";" << nombre << ";" << telefono << ";" << correo << ";" << nuevoSaldo << endl;
                } else {
                    cout << "Fondos insuficientes." << endl;
                    temp << linea << endl;
                }
            } catch (invalid_argument &e) {
                cout << "Error: Datos no válidos en el archivo de cuentas." << endl;
                temp << linea << endl;
            }
        } else {
            temp << linea << endl;
        }
    }

    cuentas.close();
    temp.close();

    if (cuentaEncontrada) {
        if (remove(CTASMONETARIAS.c_str()) != 0) {
            return "Error al eliminar el archivo original.";
        }

        if (rename(TEMP.c_str(), CTASMONETARIAS.c_str()) != 0) {
            return "Error al renombrar archivo temporal.";
        }

        ofstream transacciones(TRANSACCIONES, ios::app);
        if (!transacciones.is_open()) {
            return "Error al registrar transaccion.";
        }

        fecha = obtenerFechaHoraActual();
        transacciones << numeroCuenta << ";" << fecha << ";" << monto << ";R" << endl;
        transacciones.close();

        return "Retiro registrado correctamente.";
    } else {
        remove(TEMP.c_str());
        return "Cuenta no encontrada.";
    }
}

//Opcion 4 del menu
// Función para operar transferencias
string transferencias() {
    string cuentaOrigen, cuentaDestino, linea, monto, nuevoSaldoOrigen, nuevoSaldoDestino;
    bool cuentaOrigenEncontrada = false, cuentaDestinoEncontrada = false;
    string saldoOrigen, saldoDestino;

    ifstream cuentas(CTASMONETARIAS);
    if (!cuentas.is_open()) {
        return "Error: No se pudo abrir el archivo de cuentas.";
    }

    ofstream temp(TEMP);
    if (!temp.is_open()) {
        cuentas.close();
        return "Error: No se pudo crear el archivo temporal.";
    }

    cout << "Ingrese el numero de cuenta origen: ";
    cin >> cuentaOrigen;
    cout << "Ingrese el numero de cuenta destino: ";
    cin >> cuentaDestino;
    cout << "Ingrese el monto a transferir: ";
    cin >> monto;

    while (getline(cuentas, linea)) {
        stringstream ss(linea);
        string cuenta, nombre, telefono, correo, saldo;

        getline(ss, cuenta, ';');
        getline(ss, nombre, ';');
        getline(ss, telefono, ';');
        getline(ss, correo, ';');
        getline(ss, saldo, ';');

        // Actualizar la cuenta de origen
        if (cuenta == cuentaOrigen) {
            cuentaOrigenEncontrada = true;
            saldoOrigen = saldo;
            if (stoi(saldoOrigen) >= stoi(monto)) {
                nuevoSaldoOrigen = to_string(stoi(saldoOrigen) - stoi(monto));
                temp << cuenta << ";" << nombre << ";" << telefono << ";" << correo << ";" << nuevoSaldoOrigen << endl;
            } else {
                cout << "Fondos insuficientes en la cuenta de origen." << endl;
                temp << linea << endl;
            }
        } 
        // Actualizar la cuenta de destino
        else if (cuenta == cuentaDestino) {
            cuentaDestinoEncontrada = true;
            saldoDestino = saldo;
            nuevoSaldoDestino = to_string(stoi(saldoDestino) + stoi(monto));
            temp << cuenta << ";" << nombre << ";" << telefono << ";" << correo << ";" << nuevoSaldoDestino << endl;
        } 
        else {
            temp << linea << endl;
        }
    }

    cuentas.close();
    temp.close();

    if (cuentaOrigenEncontrada && cuentaDestinoEncontrada) {
        if (remove(CTASMONETARIAS.c_str()) != 0) {
            return "Error al eliminar el archivo original.";
        }

        if (rename(TEMP.c_str(), CTASMONETARIAS.c_str()) != 0) {
            return "Error al renombrar archivo temporal.";
        }

        ofstream transacciones(TRANSACCIONES, ios::app);
        if (!transacciones.is_open()) {
            return "Error al registrar transacción.";
        }

        string fecha = obtenerFechaHoraActual();
        transacciones << cuentaOrigen << ";" << fecha << ";" << monto << ";T-D" << endl;  
        transacciones << cuentaDestino << ";" << fecha << ";" << monto << ";T-C" << endl; 
        transacciones.close();

        return "Transferencia registrada correctamente.";
    } else {
        remove(TEMP.c_str());
        return "Error: Cuenta origen o destino no encontrada.";
    }
}

//Opcion 5 del menu
// Función para consultar el saldo de una cuenta
string consultaSaldo() {
    string numeroCuenta, linea;
    bool cuentaEncontrada = false;

    ifstream cuentas(CTASMONETARIAS);
    if (!cuentas.is_open()) {
        return "Error: No se pudo abrir el archivo de cuentas.";
    }

    cout << "Ingrese el numero de cuenta: ";
    cin >> numeroCuenta;

    while (getline(cuentas, linea)) {
        stringstream ss(linea);
        string cuenta, nombre, telefono, correo, saldo;

        getline(ss, cuenta, ';');
        getline(ss, nombre, ';');
        getline(ss, telefono, ';');
        getline(ss, correo, ';');
        getline(ss, saldo, ';');

        if (cuenta == numeroCuenta) {
            cuentaEncontrada = true;
            cout << "Cuenta encontrada: " << nombre << endl;
            cout << "Saldo actual: " << saldo << endl;
            break;
        }
    }

    cuentas.close();

    if (!cuentaEncontrada) {
        return "Error: Cuenta no encontrada.";
    }
    return "Consulta de saldo realizada exitosamente.";
}


// Función principal con ciclo para mostrar el menú
int main() {
    char opcion;      // Variable para entrar a los apartados del menu o switch
    string CCMD;     // Variable para repetir la funcion de crear cuenta monetaria en el menu o switch
    string OPD;     // Variable para repetir la funcion de Operar depositos en el menu o switch
    string ORD;    // Variable para repetir la funcion de Operar retiros en el menu o switch
    string TD;    // Variable para repetir la funcion de Transferencias en el menu o switch
    string CSCD; // Variable para repetir la funcion de Consultar saldo de cuenta en el menu o switch 
    
    do {
        opcion = menu();
        switch (opcion) {
            case '1':
                do {
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*   Ha Seleccionado Crear Cuenta Monetaria   *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    cout << crearCuentaMonetaria() << endl;
                    MensajeCentrado(RED "----------------------------------------------" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*     ¿Desea hacer otra cuenta monetaria?    *" RESET);
                    MensajeCentrado(YELLOW "*                    Si/No:                  *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    cin >> CCMD;
                } while (CCMD == "Si" || CCMD == "si");
                break;
            case '2':
                do {
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*       Ha Seleccionado Operar depositos     *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    cout << depositos() << endl;
                    MensajeCentrado(RED "----------------------------------------------" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*          ¿Desea Operar otro deposito?      *" RESET);
                    MensajeCentrado(YELLOW "*                    Si/No:                  *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    cin >> OPD;
                } while (OPD == "Si" || OPD == "si");
                break;
            case '3':
                do {
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*       Ha Seleccionado Operar retiros       *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    cout << retiros() << endl;
                    MensajeCentrado(RED "----------------------------------------------" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*           ¿Desea Operar otro retiro?       *" RESET);
                    MensajeCentrado(YELLOW "*                    Si/No:                  *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    cin >> ORD;
                } while (ORD == "Si" || ORD == "si");
                break;
            case '4':
                do {
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*       Ha Seleccionado Transferencias       *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    cout << transferencias() << endl;
                    MensajeCentrado(RED "----------------------------------------------" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*        ¿Desea hacer otra Transferencia?    *" RESET);
                    MensajeCentrado(YELLOW "*                    Si/No:                  *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    cin >> TD;
                } while (TD == "Si" || TD == "si");
                break;
            case '5':
                do {
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                 Ha seleccionado            *" RESET);
                    MensajeCentrado(GREEN "*           Consulta Saldo de Cuenta         *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "*                                            *" RESET);
                    MensajeCentrado(GREEN "**********************************************" RESET);
                    cout << consultaSaldo() << endl;
                    MensajeCentrado(RED "----------------------------------------------" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "*        ¿Desea Consultar saldo de nuevo?    *" RESET);
                    MensajeCentrado(YELLOW "*                    Si/No:                  *" RESET);
                    MensajeCentrado(YELLOW "*                                            *" RESET);
                    MensajeCentrado(YELLOW "**********************************************" RESET);
                    cin >> CSCD;
                } while (CSCD == "Si" || CSCD == "si");
                break;
            case '6':
                MensajeCentrado(GREEN "**********************************************" RESET);
                MensajeCentrado(GREEN "*                                            *" RESET);
                MensajeCentrado(GREEN "*                                            *" RESET);
                MensajeCentrado(GREEN "*             Saliendo del programa          *" RESET);
                MensajeCentrado(GREEN "*--------------------------------------------*" RESET);
                MensajeCentrado(GREEN "*                                            *" RESET);
                MensajeCentrado(GREEN "**********************************************" RESET);
                break;
            default:
                MensajeCentrado(RED "**********************************************" RESET);
                MensajeCentrado(RED "*                                            *" RESET);
                MensajeCentrado(RED "*                                            *" RESET);
                MensajeCentrado(RED "*               Opcion no valida             *" RESET);
                MensajeCentrado(RED "*             !Intente Nuevamente!           *" RESET);
                MensajeCentrado(RED "*                                            *" RESET);
                MensajeCentrado(RED "**********************************************" RESET);
        }
    } while (opcion != '6');
    return 0;
}

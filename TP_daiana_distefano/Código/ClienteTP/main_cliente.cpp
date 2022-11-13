#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    int socket_estado;
    Client(){}

    int _init(string ip, int port){
        socket_estado = WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());// IP al servidor -> cmd -> ipconfig -> IPv4
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        socket_estado = connect(server, (SOCKADDR *)&addr, sizeof(addr));
        if(socket_estado == SOCKET_ERROR){
            cout<<"Conexion fallo - ERROR: "<<WSAGetLastError()<<endl; // valida si fallo la conexion
            socket_estado = closesocket(server);
            if (socket_estado == SOCKET_ERROR){ // valida si fallo la funcion de cerrar socket
                cout<<"Funcion 'closesocket' fallo - ERROR: "<<WSAGetLastError()<<endl;
            }
            WSACleanup();
            system("pause");
            system("cls");
            return -1;
        }else{
            system("cls");
            cout << "\n¡¡Conectado al servidor!!" << endl << endl;
            return 0;
        }

    }

    /************************************************FINALIZA CLIENTE************************************************/

    int ValidarConectividad(){ //Sirve para comprobar si el servidor esta apto para recibir y enviar mensajes
        int isConnected = 0;
        u_long iMode= 1;
        ioctlsocket(server,FIONBIO,&iMode);
        memset(buffer, 0, sizeof(buffer));
        recv(server, buffer, sizeof(buffer), 0);
        if(strcmp(buffer,"cerrarSesion")==0){
            isConnected = -1;
        }iMode = 0;
        ioctlsocket(server,FIONBIO,&iMode); //Y viceversa
        return isConnected;
    }

    void Enviar(){
        //send(server, buffer, sizeof(buffer), 0);
        send(server, buffer, strlen(buffer), 0);
        fill_n(this->buffer, sizeof(buffer),0);
    }

    void Recibir(){

        recv(server, buffer, sizeof(buffer), 0);
        cout << "\nResultado: " << buffer << endl;
        fill_n(this->buffer, sizeof(buffer),0);
    }

    void CerrarSesion(){

        closesocket(server);
        WSACleanup();
        cout << "Socket cerrado" << endl;

    }

    void VerRegistro(){
        fill_n(this->buffer, sizeof(buffer),0);
        char registrosCompletos[1000]="";
        do{
            recv(server, buffer, sizeof(buffer), 0);
            strcpy(registrosCompletos,buffer);
            cout<<registrosCompletos<<"\n";
        }while(!(strcmp(registrosCompletos,"TERMINO")==0)); // Si recibe la bandera, corta el bucle
        cout<<"Registro de actividades: "<<registrosCompletos<<"\n";

        system("pause");system("cls");
    }


    int MenuOperacion(){

        int code = 1;
        system("cls");
        char opcionOp[21] = {};
        int largo_opcion = 0;
        string opcion;

        fill_n(this->buffer, sizeof(buffer),0);

        cout << "|-------MENU DE OPERACIONES-------|\n";
        cout << "- Suma: + \n- Resta: -\n- Multiplicacion: *\n- Division: /\n- Potenciacion: ^\n- Factorial: !\n- VOLVER" << endl;
        cout << "Escribir los operandos y el operador que quiera usar\nO puede escribir VOLVER para regresar al menu principal: ";

        fflush(stdin);
        getline(cin, opcion);

        largo_opcion = opcion.length();
        //paso el string a la cadena del char, ya que usar directamente el char me generaba problemas
        for(int i = 0; i < largo_opcion; i++){

            opcionOp[i] = opcion[i];

        }

        strupr(opcionOp); // lo paso a mayusculas para que indistintamente como ponga volver, siempre va a estar en mayusculas

        while(strlen(opcionOp) > 20 || strlen(opcionOp) == 0){
            cout << "La operacion debe tener entre 1 y 20 caracteres: ";
            cin >> opcionOp;
        }


        if(strcmp(opcionOp, "VOLVER") ==0){
            cout << "Se selecciono lo opcion volver, se volvera al menu anterior" << endl;
            fill_n(this->buffer, sizeof(buffer),0);
            code = 0;
        }else{
            strcpy(this->buffer, opcionOp);
            Enviar();
        }

        return code; // retorno para validar en el main
    }

    void MenuPrincipal(){

        int opcion = 0;

        while(opcion != 3){
            system("cls");
            int code;
            cout<< "\n|----------BIENVENIDO AL MENU DEL SERVIDOR----------|\n\n" <<endl;
            cout<< "1) Realizar calculo\n" <<endl;
            cout<< "2) Ver registro de actividades\n" <<endl;
            cout<< "3) Cerrar sesion\n" <<endl;
            cout<< "INGRESE UNA OPCION: ";
            cin>>opcion;   //valido inactividad
            if(ValidarConectividad()== -1){
                cout<<"Cliente desconectado por inactividad\n";
                system("pause");
                opcion = 3;
            }
            fill_n(this->buffer, sizeof(this->buffer),0);
            switch(opcion){

            case 1:
                system("cls");
                code = MenuOperacion();
                if(code != 0){ //Compruebo si el cliente decidio la opcion volver
                    Recibir();
                }
                system("pause");
                break;
            case 2:
                system("cls");
                cout <<"\nSe solicita ver las actividades" <<endl;
                strcpy(this->buffer, "verActividades");
                Enviar();
                VerRegistro();
                break;

            case 3:
                system("cls");
                strcpy(this->buffer, "cerrarSesion");
                Enviar();
                system("pause");
                CerrarSesion();
                break;

            default:
                system("cls");
                cout<<"\nSe ingreso el valor: " << opcion << endl;
                cout<<"\nNo es una opcion valida - SELECCIONE UN OPCION DE LAS BRINDADAS\n" <<endl;
                break;
            }
        }


    }

};

int main()
{
    ciclar: // Un goto para volver a ciclar todo
    int port;
    string ip="";
    Client *Cliente = new Client();

    do{
        cout << "-------CONECTANDOSE AL SERVIDOR . . .-------" << endl;
        cout<<"Ingrese la IP del servidor para conectarse: ";
        fflush(stdin);
        getline(cin, ip);
        cout<<"\nIngrese un puerto: ";
        cin>>port;
    }while(Cliente->_init(ip, port)!=0); /** Creo el cliente en _init con la ip y el puerto**/

    Cliente->MenuPrincipal();
    goto ciclar;

    return 0;

}

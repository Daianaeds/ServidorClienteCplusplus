#include <iostream>
#include <winsock2.h>
#include <stdio.h>

using namespace std;

class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    int socket_estado;
    Client(){}

    int _init(int port){
        socket_estado = WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_addr.s_addr = inet_addr("192.168.1.33"); // IP al servidor -> cmd -> ipconfig -> IPv4
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

    void Enviar(){

        cout << "Escribe la operacion a realizar: ";
        cin >>this->buffer;
        if(buffer[0] == '\0'){// si da valor 0 es por que son iguales, si da 1 es por que son distintos
            cout << "No se pueden ingresar datos vacios";
            Menu();
        }
        else if(sizeof(buffer) <= 20){
            send(server,buffer, sizeof(buffer), 0);
            cout << "Largo del buffer: " << sizeof(buffer) << endl;
            memset(buffer, 0, sizeof(buffer)); //reseteamos la variable
            cout << "Mensaje menor 20 caracteres enviado" << endl;
        }
        else if(sizeof(buffer) > 20){
            send(server,buffer, sizeof(buffer), 0);
            cout << "Largo del buffer: " << sizeof(buffer) << endl;
            memset(buffer, 0, sizeof(buffer)); //reseteamos la variable
            cout << "Mensaje mayor 20 caracteres enviado" << endl;
        }

    }

    void Recibir(){

        recv(server, buffer, sizeof(buffer), 0);
        cout << "Servidor dice: " << buffer << endl;
        memset(buffer, 0, sizeof(buffer)); //resetea el arreglo buffer a 0 (qué variable manipulamos, que se pone, que numero del arreglo(tamaño del buffer se le pone 0))

    }

    void CerrarSesion(){

        closesocket(server);
        WSACleanup();
        cout << "Socket cerrado" << endl;

    }


    void Menu(){
        int opcion = 0;

        while(opcion != 3){

            cout<< "|----------BIENVENIDO AL MENU DEL SERVIDOR----------|\n\n" <<endl;
            cout<< "1) Realizar calculo\n" <<endl;
            cout<< "2) Ver registro de actividades\n" <<endl;
            cout<< "3) Cerrar sesion\n" <<endl;
            cout<< "SELECCIONE UNA OPCION:\n" <<endl;
            cin>>opcion;
            switch(opcion){

                case 1:
                    system("cls");
                    cout <<"\nSe usara funcion enviarCalular al servidor" <<endl;
                    Enviar();
                    break;

                case 2:
                    system("cls");
                    cout <<"\nSe usara funcion verActividades al servidor" <<endl;
                    break;

                case 3:
                    system("cls");
                    cout <<"\nSe usara funcion cerrarSesion al servidor" <<endl;
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

    int port;
    Client *Cliente = new Client();
    /*while(true){

        Cliente->_init();
        Cliente->Menu();
        //Cliente->Enviar();
        //Cliente->Recibir();

    }*/

    do{
        cout << "-------CONECTANDOSE AL SERVIDOR . . .-------" << endl;
        cout << "IP: 192.168.1.33";
        fflush(stdin);
        cout<<"\nIngrese un puerto: ";
        cin>>port;
    }while(Cliente->_init(port)!=0);

    Cliente->Menu();

    return 0;

}

#include <iostream>
#include <winsock2.h>

using namespace std;

class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];

    Client(){
        u_short port = 5555;
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        addr.sin_addr.s_addr = inet_addr("192.168.1.33"); // IP al servidor -> cmd -> ipconfig -> IPv4
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        connect(server, (SOCKADDR *)&addr, sizeof(addr));
        cout << "Conectado al servidor!!" << endl << endl;
    }

    void Enviar(){

        cout << "Escribe el mensaje a enviar: ";
        cin >>this->buffer;
        send(server,buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer)); //reseteamos la variable
        cout << "Mensaje enviado al servidor!!" << endl;
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

            cout<< "SELECCIONE UNA OPCION:\n" <<endl;
            cout<< "1- Realizar calculo\n" <<endl;
            cout<< "2- Ver registro de actividades\n" <<endl;
            cout<< "3- Cerrar sesion\n" <<endl;
            cin>>opcion;
            switch(opcion){

                case 1:
                    system("cls");
                    cout <<"\nSe usara funcion enviarCalular al servidor" <<endl;
                    //Cliente->Enviar();
                    break;

                case 2:
                    system("cls");
                    cout <<"\nSe usara funcion verActividades al servidor" <<endl;
                    break;

                case 3:
                    system("cls");
                    cout <<"\nSe usara funcion cerrarSesion al servidor" <<endl;
                    //Cliente->CerrarSesion()
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

    Client *Cliente = new Client();
    while(true){

        Cliente->Enviar();
        Cliente->Recibir();

    }

}

#include <iostream>
#include <winsock2.h>

using namespace std;

class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];

    Server(){
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5555);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout<<"Servidor esperando para conexion. . ." << endl;

        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET){
            cout << "El cliente se ha conectado!" << endl;
        }
    }

    void Enviar(){ //para enviar al cliente, esto lo voy a usar cuando le tengamos que enviar la respuesta al cliente

        cout << "Escribe el mensaje a enviar: ";
        cin >>this->buffer;
        send(client,buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer)); //reseteamos la variable
        cout << "Mensaje enviado al cliente!!" << endl;

    }

    void Recibir(){

        recv(client, buffer, sizeof(buffer), 0);  //para recibir datos del cliente
        cout << "Cliente dice: " << buffer << endl;
        memset(buffer, 0, sizeof(buffer)); //resetea el arreglo buffer a 0 (qué variable manipulamos, que se pone, que numero del arreglo(tamaño del buffer se le pone 0))

    }

    void CerrarSocket(){ //se cierra el socket

        closesocket(client);
        cout << "El cliente se ha desconectado. Se ha liberado el socket" << endl;

    }
};


int main()
{

    Server *Servidor = new Server();
    while(true){

        Servidor->Recibir();
        Servidor->Enviar();

    }

}

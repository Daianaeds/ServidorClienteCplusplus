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

    int _init(string ip, int port){ /** ACA LE AGREGUE UN PARAMETRO MAS, PARA QUE RECIBA LA IP QUE SE INGRESO **/
        socket_estado = WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_addr.s_addr = inet_addr(ip.c_str()); /** BORRO LA CADENA DE STRING Y LA REEMPLAZO POR EL PARAMETRO **/// IP al servidor -> cmd -> ipconfig -> IPv4
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

    void Enviar(){
        //send(server, buffer, sizeof(buffer), 0);
        send(server, buffer, strlen(buffer), 0);
        fill_n(this->buffer, sizeof(buffer),0);
    }

    /*void Recibir(){

        recv(server, buffer, sizeof(buffer), 0);
        cout << "Servidor dice: " << buffer << endl;
        memset(buffer, 0, sizeof(buffer)); //resetea el arreglo buffer a 0 (qué variable manipulamos, que se pone, que numero del arreglo(tamaño del buffer se le pone 0))

    }*/

    void Recibir(){

        recv(server, buffer, sizeof(buffer), 0);
        cout << "Respuesta: " << buffer << endl;
        fill_n(this->buffer, sizeof(buffer),0);
    }

    void CerrarSesion(){

        closesocket(server);
        WSACleanup();
        cout << "Socket cerrado" << endl;

    }

    int MenuOperacion(){ /** CAMBIO EL TIPO DE RETORNO DE LA FUNCION DE VOID A INT **/
                        /** PARA QUE ME DEVUELVA UN CODIGO PARA SABER SI SE QUISO VOLVER AL MENU **/
        int code = 1;   /** Y PODER VALIDARLO DESDE EL MAIN **/
        system("cls");
        char opcionOp[21] = {};
        string opcion;
        int largo_opcion = 0;

        fill_n(this->buffer, sizeof(buffer),0);

        cout << "|-------MENU DE OPERACIONES-------|\n";
        cout << "- Suma: + \n- Resta: -\n- Multiplicacion: *\n- Division: /\n- Potenciacion: ^\n- Factorial: !\n- VOLVER" << endl;
        cout << "Escribir los operandos y el operador que quiera usar, o escriba VOLVER para regresar al menu principal: ";

        fflush(stdin);
        getline(cin, opcion);

        largo_opcion = opcion.length();
        //paso el string a la cadena del char, ya que usar directamente el char me generaba problemas
        for(int i = 0; i < largo_opcion; i++){

            opcionOp[i] = opcion[i];

        }

        strupr(opcionOp);
        while(strlen(opcionOp) > 20){
            cout << "Error ingrese una operacion valida: ";
            fflush(stdin);
            getline(cin, opcion);
        }

        if(strcmp(opcionOp, "VOLVER") ==0){
            cout << "Se selecciono lo opcion volver";
            fill_n(this->buffer, sizeof(buffer),0);
            code = 0; /** SE DECIDIO VOLVER, CAMBIO EL VALOR DEL CODIGO **/
        }else{
            strcpy(this->buffer, opcionOp);
            Enviar();
        }



        /*strupr(opcion);
        while (opcion.length() > 20 || opcion != "VOLVER"){

            switch(opcion){

            case "VOLVER":
                cout << "Se selecciono la opcion volver al menu\n";
                break;

            }


        }*/
        return code; /** Y LO RETORNO **/
    }

    void VerRegistro(){
        fill_n(this->buffer, sizeof(buffer),0);
        char registrosCompletos[1000]="";
        do{ /** AGREGO ESTE DO-WHILE PARA RECIBIR TODAS LAS LINEAS DEL ARCHIVO QUE EL SERVIDOR VA LEYENDO Y ME ENVIA A LA VEZ **/
            recv(server, buffer, sizeof(buffer), 0);
            strcpy(registrosCompletos,buffer);
            cout<<registrosCompletos<<"\n";
        }while(!(strcmp(registrosCompletos,"TERMINO")==0)); /** CUANDO SE RECIBA ESTE FLAG DEL SERVIDOR, ROMPO EL BUCLE Y NO RECIBO MAS **/
        cout<<"MENSAJE RECIBIDO: "<<registrosCompletos<<"\n";

        //Recibir();
        //fill_n(this->buffer, sizeof(buffer),0);
        system("pause");system("cls");
    }

    int checkConnectivity(){ /** Sirve para comprobar si el servidor esta apto para recibir y enviar mensajes**/
        int isConnected = 0; /** LA FUNCION QUE HABIA HECHO EN EL TP PARA COMPROBAR SI EL SERVIDOR ME MANDO ALGO INDICANDO SI SIGUE VIVO **/
        u_long iMode= 1;    /** ESTA FUNCION ES NECESARIO LLAMARLA DESPUES DE CADA cin (DESPUES DE CADA ENTRADA DEL USUARIO) **/
        ioctlsocket(server,FIONBIO,&iMode); //Funcion que convierte la conexion en blocking-sockets a non-blocking sockets
        memset(buffer, 0, sizeof(buffer));
        recv(server, buffer, sizeof(buffer), 0);
        if(strcmp(buffer,"cerrarSesion")==0){
            isConnected = -1;
        }iMode = 0;
        ioctlsocket(server,FIONBIO,&iMode); //Y viceversa
        return isConnected;
    }

    /*void Menu(){
        int opcion = 0;

        while(opcion != 3){
            fflush(stdin);
            cout<< "\n|----------BIENVENIDO AL MENU DEL SERVIDOR----------|\n\n" <<endl;
            cout<< "1) Realizar calculo\n" <<endl;
            cout<< "2) Ver registro de actividades\n" <<endl;
            cout<< "3) Cerrar sesion\n" <<endl;
            cout<< "INGRESE UNA OPCION: ";
            cin>>opcion;
            switch(opcion){

            case 1:
                system("cls");
                fill_n(this->buffer, sizeof(buffer),0);
                MenuOperacion();
                break;

            case 2:
                system("cls");
                cout <<"\nSe usara funcion verActividades al servidor" <<endl;
                strcpy(this->buffer, "verActividades");
                Enviar();
                VerRegistro();
                break;

            case 3:
                system("cls");
                cout <<"\nSe usara funcion cerrarSesion al servidor" <<endl;
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
    }*/



};

int main()
{
    ciclar: /** UN GOTO PARA CICLAR TODO DE NUEVO **/
    int port; /**DECLARO VARIABLE STRING (no char*) Y UN ENTERO PARA EL PUERTO **/
    //const char ip[12]={}; /** HABIA PROBADO CON UN ARRGLO DE CARACTERES PERO TERMINE OPTANDO POR UN string, EN ESTE CASO POR SU RARO COMPORTAMIENTO **/
    string ip="";
    Client *Cliente = new Client();

    do{
        cout << "-------CONECTANDOSE AL SERVIDOR . . .-------" << endl;
        cout<<"Ingrese la IP del servidor para conectarse: ";
        fflush(stdin);
        getline(cin, ip); /** OBTENGO LA DIRECCION IP **/
        //getline(cin, ipp);
        //strcpy(ip, ipp.c_str());
        cout<<"\nIngrese un puerto: ";
        cin>>port; /** OBTENGO EL PUERTO **/
    }while(Cliente->_init(ip, port)!=0); /** MANDO LOS PARAMENTROS AL _init **/

    int opcion = 0;

    while(opcion != 3){
        system("cls");
        int code;
        cout<< "\n|----------BIENVENIDO AL MENU DEL SERVIDOR----------|\n\n" <<endl;
        cout<< "1) Realizar calculo\n" <<endl;
        cout<< "2) Ver registro de actividades\n" <<endl;
        cout<< "3) Cerrar sesion\n" <<endl;
        cout<< "INGRESE UNA OPCION: ";
        cin>>opcion;    /** DESPUES DE ESTA ULTIMA ENTRADA DEL USUARIO LLAMO AL SERVIDOR PARA PREGUNTARLE SI PASARON LOS 2 MINUTOS Y NO SE DESCONECTO POR INACTIVIDAD **/
        if(Cliente->checkConnectivity()== -1){cout<<"Cliente desconectado por inactividad\n";system("pause");opcion = 3;}
        fill_n(Cliente->buffer, sizeof(Cliente->buffer),0);
        switch(opcion){

        case 1:
            system("cls");
            code = Cliente->MenuOperacion();
            if(code != 0){ /** COMPRUEBO SI EL CLIENTE DECIDIO LA OPCION VOLVER AL MENU **/
                Cliente->Recibir();
            }
            system("pause");
            break;
        case 2:
            system("cls");
            cout <<"\nSe usara funcion verActividades al servidor" <<endl;
            strcpy(Cliente->buffer, "verActividades");
            Cliente->Enviar();
            Cliente->VerRegistro();
            break;

        case 3:
            system("cls");
            cout <<"\nSe usara funcion cerrarSesion al servidor" <<endl;
            strcpy(Cliente->buffer, "cerrarSesion");
            Cliente->Enviar();
            system("pause");
            Cliente->CerrarSesion();
            break;

        default:
            system("cls");
            cout<<"\nSe ingreso el valor: " << opcion << endl;
            cout<<"\nNo es una opcion valida - SELECCIONE UN OPCION DE LAS BRINDADAS\n" <<endl;
            break;
        }
    }
    goto ciclar; /** COMENTO EL WHILE QUE ME ROMPIA CADA VEZ QUE EL CLIENTE SE DESCONECTABA Y LO REEMPLAZO POR UN GOTO **/
    /*while(true){
        Cliente->Enviar();
        Cliente->Recibir();
    }*/

    return 0;

}

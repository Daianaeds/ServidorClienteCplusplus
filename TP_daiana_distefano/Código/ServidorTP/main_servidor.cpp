#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <ctime>
#include <string.h>
#include <math.h>

using namespace std;

class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024]="", date[32];

    Server(){
        u_short port;
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        cout<<"Ingrese el puerto de escucha: "; //inicializa el server
        cin>>port;

        serverAddr.sin_port = htons(port);

        DWORD timeout = 120 * 1000; //configuracion del time out
        setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof timeout);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout<<"Servidor esperando para conexion. . ." << endl;

        ofstream archivo;//se crea el archivo de server.log con los datos de horaActual y el puerto
        archivo.open("server.log.txt", ios::app); //out para crear
        if(archivo.fail()){
            cout<<"No se pudo abrir el archivo";
            exit(1);
        }

        archivo<<horaActual() << ": =============================\n";
        archivo<<horaActual() << ": =======Inicia Servidor=======\n";
        archivo<<horaActual() << ": =============================\n";
        archivo<<horaActual() << ": Socket creado. Puerto de escucha: " << port << "\n";

        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET){

            cout << "\n----------El cliente se ha conectado!----------\n\n" << endl;
            archivo<<horaActual() << ": Conexion Aceptada\n";

        }

        archivo.close();
    }

    /************************************************FINALIZA SERVIDOR************************************************/

    char* horaActual(){ //funcion donde trae la hora actual
        time_t now = time(0);
        tm* localtm = localtime(&now);
        strftime(date,sizeof(date),"%d-%m-%y_%H:%M:%S",localtm);
        return date;
    }

    void Enviar(){ //para enviar al cliente, esto lo voy a usar cuando le tengamos que enviar la respuesta al cliente

        send(client,buffer, sizeof(buffer), 0);
        fill_n(this->buffer, sizeof(buffer),0); //resetea el arreglo buffer a 0 (qué variable manipulamos, que se pone, que numero del arreglo(tamaño del buffer se le pone 0))

    }

    void Recibir(){
        strcpy(buffer, " ");
        int code = recv(client, buffer, sizeof(buffer), 0);

        if(code == SOCKET_ERROR){   //si recibe socket_error -1 es por que hubo timeout, si recibe 0 es por que recibio algo del cliente
            strcpy(buffer, "cerrarSesion"); //copia al buffer el cerrarSesion y lo usa como bandera
            send(client,buffer, sizeof(buffer), 0); // y se lo manda al cliente y sale de la funcion por que el server ya no se encuentra operativo
            return;
        }

        else if(strlen(this->buffer)>=1 || strlen(this->buffer) <= 20 ){

            if(strcmp(buffer, "verActividades") == 0){ //comprueba si el cliente solicito ver actividades
                cout << "\n----------El cliente solicito ver actividades----------\n\n";
                LeerArchivo();
            }else if(strcmp(buffer, "cerrarSesion") == 0){ //comprueba si el cliente solicito cerrar sesion
                cout << "El cliente cerro sesion\n";
            }else{
                strlwr(buffer);
                cout << "El cliente solicito realizar calculo: " << buffer << endl; //si no es ninguno de los anteriores, ingresa directamente a realizar calculo
                realizarCalculo();
            }

        }
        if(strcmp(buffer, "cerrarSesion") != 0){
            //valida que no borre el buffer cuando el cliente envie cerrar sesion para usarlo en el main si cliente solicito cerrar sesion o no
           fill_n(this->buffer, sizeof(buffer),0);
        }


    }

    void realizarCalculo (){

        int posicion_signo = 0, contador_caracteres = 0, cont_signo = 0;
        char caracter_error = ' ', valor_total[30] = {}, signo, valor_a[20] = {}, valor_b[20] = {};
        char operadores[] = {'+','-','*','/','!','^'};
        /** posicion_signo almacena donde esta el signo, contador caracteres valida que ingresen la misma cantidad y cant_signo valida que haya 1 solo signo en la operacion
        operadores[] es una lista con todos los operadores para despues validar que operador tiene
        caracter_error almacena el caracter equivocado, valor_total donde se guardara el resultado cuando se pase a char, signo guarda el signo que se ingreso
        valor_a y valor_b guardaran los operando de las operaciones **/


        // recorre el buffer y valida si tiene signos
        for(int i=0; i<strlen(buffer); i++){
            if(buffer[i] == '+' || buffer[i] == '-' || buffer[i] == '*' || buffer[i] == '/' || buffer[i] == '!' || buffer[i] == '^' || isdigit(buffer[i])){
                contador_caracteres++; // contador de caracteres de la operacion
            }
            else{
                caracter_error = buffer[i];
            }
        }

        //obtengo la posicion del signo y su valor
        for (int j=0; j<strlen(operadores); j++){
            for (int i=0; i<strlen(buffer); i++ ){
                if(buffer[i] == operadores[j]){
                    cont_signo++;
                    signo = buffer[i];
                    posicion_signo = i;
                }
            }
        }

        if(contador_caracteres == strlen(buffer) && signo == NULL ){
            Enviar();
        }
        else{

            /******************************* VALIDACIONES *******************************/

            // valido si hay mas de 1 signo
            if(cont_signo > 1){

                char error[20] = {};

                //guardo en error solo la parte que contiene los dos signos
                for(int i = posicion_signo; i < strlen(buffer); i++){
                    error[i-(posicion_signo)] = buffer[i];
                }

                char mensaje[] = "No se pudo realizar la operacion, la operacion está mal formada: ";
                strcat(mensaje,error);
                strcpy(this->buffer, mensaje); //falta agregar en donde esta mal formada
                Enviar();

            }

            //valida si factorial comienza con ! o si hay valores despues del !
            else if(signo=='!' && buffer[strlen(buffer)-1] != '!'){

                char error[20] = {};

                //guardo en error solo la parte que contiene los dos signos
                for(int i = posicion_signo; i < strlen(buffer); i++){
                    error[i-(posicion_signo)] = buffer[i];
                }

                char mensaje[] = "No se pudo realizar la operacion, la operacion está mal formada: ";
                strcat(mensaje, error);
                strcpy(this->buffer, mensaje); //falta agregar en donde esta mal formada
                Enviar();
            }

            //valida que la primera posicion no posea un signo y que sea != a '!'

            else if(!isdigit(buffer[0])){

                if(buffer[0] != '!'){
                    cout << buffer[0];
                    char mensaje[] = "No se pudo realizar la operacion, la operacion está mal formada: ";
                    strcat(mensaje, buffer);
                    strcpy(this->buffer, mensaje); //falta agregar en donde esta mal formada
                    Enviar();
                }
            }


            else if(signo=='!' && posicion_signo==0 || signo != '!' && posicion_signo==strlen(buffer)-1 ){
                strcpy(this->buffer, "No se pudo realizar la operacion, la operacion está mal formada"); //falta agregar en donde esta mal formada
                Enviar();
            }

            //valida si posee una letra en la operacion
            else if(strlen(buffer) != contador_caracteres){

                char mensaje[] = "No se pudo realizar la operacion, la operacion está mal formada: ";
                strcat(mensaje, &caracter_error);
                strcat(mensaje, "\n");
                strcpy(this->buffer, mensaje);
                cout << "Mensaje: " << this->buffer;
                Enviar();
            }

            else if(isdigit(buffer[0]== false)){
                strcpy(this->buffer, "Error, la operacion no es correcta"); //falta agregar en donde esta mal formada
                Enviar();
            }

            /******************************* REALIZO OPERACIONES *******************************/
            else{

                //separo el valor a
                for (int i = 0; i < posicion_signo ; i++){

                    if(isdigit(buffer[i])){
                    valor_a[i] = buffer[i];
                    }

                }

                //separo el valor b
                for (int i = posicion_signo+1; i < strlen(buffer) ; i++){

                    if(isdigit(buffer[i])){
                        valor_b[i-(posicion_signo+1)] = buffer[i];
                    }

                }

                int operadorA = atoi(valor_a);
                int operadorB = atoi(valor_b);
                int resultado = 0;
                switch(signo){

                case '+':
                    resultado =  calcularSuma(operadorA, operadorB);
                    break;

                case '-':
                    resultado = calcularResta(operadorA, operadorB);
                    break;

                case '*':
                    resultado = calcularMultiplicacion(operadorA, operadorB);
                    break;

                case '/':
                    resultado = calcularDivision(operadorA,operadorB);
                    break;

                case '!':
                    resultado = calcularFactorial(operadorA);
                    break;

                case '^':
                    resultado = calcularPotencia(operadorA, operadorB);
                    break;

                default:
                    break;

                }
                itoa(resultado, valor_total, 10);

                strcpy(this->buffer, valor_total); //falta agregar en donde esta mal formada
                Enviar();


            }

        }

    }

    int calcularSuma(int operador1, int operador2){
        return operador1 + operador2;
    }

    int calcularResta(int operador1, int operador2){
        return operador1 - operador2;
    }

    int calcularMultiplicacion(int operador1, int operador2){
        return operador1 * operador2;
    }

    int calcularDivision(int operador1, int operador2){
        if(operador2 == 0){
            strcpy(this->buffer, "No se puede dividir por 0");
            Enviar();
        }
        return operador1 / operador2;
    }


    int calcularFactorial(int operador1){

        cout << "Ingresa a factorial";
        int factorial = 1;

        for (int i =1; i<=operador1; i++){
            factorial = factorial * i;
        }

        return factorial;

    }

    int calcularPotencia(int operador1, int potencia){
        return pow(operador1, potencia);
    }

    void EsperarNuevoCliente(){
        int clientAddrSize = sizeof(clientAddr);

        cout << "Esperando nuevos clientes..."<<endl;

        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET){
            cout << "Cliente conectado!" << endl;
        }

    }

    void CerrarSocket(){ //se cierra el socket

        int clientAddrSize = sizeof(clientAddr);
        if(closesocket(client) == 0){

            cout<<"\n----------El cliente cerro sesion----------\n\n";
            cout << "El cliente se ha desconectado. Se ha liberado el socket\n";

            ofstream archivo;
            archivo.open("server.log.txt",ios::app);
            if(archivo.fail()){
            cout<<"No se pudo abrir el archivo";
            exit(1);
            }

            //escribo en el archivo la hora que se desconecto el cliente
            archivo << horaActual() << ": Conexion Cerrada - cliente cerro sesion\n";
            archivo.close();
        }

    }

    void LeerArchivo(){

        fill_n(this->buffer, sizeof(buffer),0);
        ifstream archivo;

        string linea;
        string texto;
        archivo.open("server.log.txt", ios::in); //archivo de solo lectura

        if(archivo.fail()){
            cout<<"No se pudo abrir el archivo";
            exit(1);
        }
        while(!archivo.eof() && getline(archivo, linea )){
            cout<< linea << "\n";
            strcpy(buffer, linea.c_str());
            Enviar();

        }
        strcpy(buffer, "TERMINO");//bandera que informa el final del archivo
        Enviar();
        archivo.close();
    }


};

int main()
{

    Server *Servidor = new Server();
    while(true){

        Servidor->Recibir();
        if(strcmp(Servidor->buffer, "cerrarSesion") == 0){
            fill_n(Servidor->buffer, sizeof(Servidor->buffer),0);
            Servidor->CerrarSocket();
            Servidor->EsperarNuevoCliente();
        }
        else{
            Servidor->Enviar();
        }

    }

}

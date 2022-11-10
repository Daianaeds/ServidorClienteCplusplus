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
    char buffer[1024]=""/** INICIALICE EL BUFFER **/, date[32];

    Server(){
        u_short port; /** BORRE EL PUERTO INICIALIZADO **/
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        cout<<"Ingrese el puerto de escucha: "; /** INGRESA DESDE ACA **/
        cin>>port;

        serverAddr.sin_port = htons(port);

        DWORD timeout = 120 * 1000; /** CONFIG. PARA INDICAR EL TIME OUT**/
        setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof timeout);/****/

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

            cout << "El cliente se ha conectado!" << endl;
            archivo<<horaActual() << ": Conexion Aceptada\n";

        }

        archivo.close();
    }

    /************************************************FINALIZA SERVIDOR************************************************/

    void Enviar(){ //para enviar al cliente, esto lo voy a usar cuando le tengamos que enviar la respuesta al cliente

        send(client,buffer, sizeof(buffer), 0);
        fill_n(this->buffer, sizeof(buffer),0); //reseteamos la variable

    }

    bool CadenaVacia(){ //retorna verdadero si no es una cadena vacia
        return strlen(this->buffer) == 0;
    }

    void Recibir(){
        strcpy(buffer, " ");
        int code = recv(client, buffer, sizeof(buffer), 0); /** NO LLAMO A LA FUNCION RECIBIR PORQUE AUN NO QUIERO QUE BORRE EL BUFFER**/

        if(code == SOCKET_ERROR){   /** SI RECIBE SOCKET_ERROR (-1) QUIERE DECIR QUE RECIBIO UN TIMEOUT, DE LO CONTRARIO RECIBE (0) SI RECIBE ALGUN DATO DEL CLIENTE **/
            strcpy(buffer, "cerrarSesion"); /** COPIA UN FLAG AL BUFFER (APROVECHANDO EL STRING cerrarSesion) **/
            send(client,buffer, sizeof(buffer), 0); /** Y SE LO MANDA AL CLIENTE Y SALGO DE LA FUNCION PORQUE EL SERVIDOR YA NO SE ENCUENTRA MAS OPERATIVO **/
            return;
        }
        cout << "Estado del buffer "<< buffer << endl;
        cout << "Largo del buffer " << strlen(buffer) << endl;

        if(CadenaVacia()){
            cout << "Entro a cadena vacia" << endl;
            fill_n(this->buffer, sizeof(buffer),0); // reemplazar a los memset
            strcpy(this->buffer, "No se pueden ingresar datos vacios");
            Enviar();
        }

        else if(strlen(this->buffer)>=1 || strlen(this->buffer) <= 20 ){

            if(strcmp(buffer, "verActividades") == 0){

                cout << "El cliente solicito ver actividades\n";
                LeerArchivo();

            }
            else if(strcmp(buffer, "cerrarSesion") == 0){
                cout << "El cliente cerro sesion\n";
            }
            /*else if(ValidarOperacion(this->buffer) == 1){


            }*/

            else{

                bool operacionOK; //bandera que indica que encontro un o varios operadores
                char caracter_error; // va a almacenarse el caracter que no corresponde, considerar hacerlo un char[]
                char valor_total[30]; // se usará un char para poder enviarlo al buffer
                char signo; // almacena el operando dentro de la variable, considerar hacerlo un char []
                char operadores[] = {'+','-','*','/','!','^'}; // lista con todos los operadores para despues validar que operador tiene
                char valor_a[20], valor_b[20]; // variables que guardaran los operadores
                int posicion_signo = 0; // almacenara la posicion del operando
                int contador_caracteres = 0;
                int cont_signo = 0; // contador de operandos, indica si encontro mas de 1 operando. Si hay mas de 1, se validará con un error

                // recorre el buffer y valida si tiene operandos
                for(int i=0; i<strlen(buffer); i++){
                    if(buffer[i] == '+' || buffer[i] == '-' || buffer[i] == '*' || buffer[i] == '/' || buffer[i] == '!' || buffer[i] == '^' || isdigit(buffer[i])){
                        contador_caracteres++; // contador de operandos
                    }
                    else{
                        caracter_error = buffer[i];
                        break;
                    }

                }

                for (int j=0; j<strlen(operadores); j++){
                    for (int i=0; i<strlen(buffer); i++ ){
                        if(buffer[i] == operadores[j]){
                            cont_signo++;
                            signo = buffer[i];
                            posicion_signo = i;
                        }
                    }
                }

                // valido si hay mas de 1 signo
                if(cont_signo > 1){
                    strcpy(this->buffer, "No se pudo realizar la operación, la operación está mal formada"); //falta agregar en donde esta mal formada
                    Enviar();

                }

                else if(signo=='!' && buffer[strlen(buffer)-1] != '!'){
                    strcpy(this->buffer, "No se pudo realizar la operación, la operación está mal formada"); //falta agregar en donde esta mal formada
                    Enviar();
                }

                else if(signo=='!' && posicion_signo==0 || signo != '!' && posicion_signo==strlen(buffer)-1 ){
                    strcpy(this->buffer, "No se pudo realizar la operación, la operación está mal formada"); //falta agregar en donde esta mal formada
                    Enviar();
                }

                else if(strlen(buffer) != contador_caracteres){
                    strcpy(this->buffer, "No se pudo realizar la operación, la operación está mal formada"); //falta agregar en donde esta mal formada
                    Enviar();
                }

                else if(isdigit(buffer[0]== false)){
                    strcpy(this->buffer, "Error, la operacion no es correcta"); //falta agregar en donde esta mal formada
                    Enviar();
                }

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
                        resultado = operadorA + operadorB;
                        break;

                    case '-':
                        resultado = operadorA - operadorB;
                        break;

                    case '*':
                        resultado = operadorA * operadorB;
                        break;

                    case '/':
                        if(operadorB == 0){
                            cout << "No se puede dividir por 0";
                        }
                        else{
                            resultado = operadorA / operadorB;
                        }
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
                    Enviar();;


                }



            }

        }
        else{
            cout << "La operacion debe tener entre 1 y 20 caracteres\n"; //validar en cliente
        }
        if(strcmp(buffer, "cerrarSesion") != 0){ /** AGREGO ESTA VALIDACION PARA QUE NO ME BORRE EL BUFFER CUANDO SEA cerrarSesion YA QUE LO VOY A NECESITAR
                PARA CUANDO VUELVA AL MAIN Y PUEDA VALIDAR SI EL CLIENTE SOLICITO CERRAR SESION **/
           fill_n(this->buffer, sizeof(buffer),0); //resetea el arreglo buffer a 0 (qué variable manipulamos, que se pone, que numero del arreglo(tamaño del buffer se le pone 0))
        }


    }

    /*int ValidarOperacion(char buffer){
        int i=0;
        bool validarUltDigInt = false;
        bool operando = false;
        char operando;
        char operador1[20], operador2[20];

        for(i = 0; i < strlen(buffer); i++){

            char operando = buffer[i];
            if( operando == '+' || operando == '-' || operando == '*' || operando == '/' || operando == '!' || operando == '^'){

                strcat(operador1[i],atoi(buffer[i]); //almaceno en el operador1 hasta que llegue a algun operando
                switch(operando){

                case '+':
                    //calcularSuma();
                    break;

                case '-':
                    //calcularResta();
                    break;

                case '*':
                    //calcularMultiplicacion();
                    break;

                case '/':
                    //calcularDivision();
                    break;

                case '!':
                    //calcularFactorial(char operador1);
                    break;

                case '^':
                    //calcularPotencia(char operador1);
                    break;
                }

            }

        }

        return 1;
    }*/

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
            cout << "No se puede dividir por 0";
        }
        return operador1 / operador2;
    }

    int calcularFactorial(int operador1){

        int factorial = 1;

        for (int i =1; i<operador1; i++){
            factorial = factorial * i;
        }

        return factorial;

    }

    int calcularPotencia(int operador1, int potencia){
        return pow(operador1, potencia);
    }

    void EsperarNuevoCliente(){
        /*bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Esperando a un cliente" << endl << endl;
        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente Conectado" << endl<< endl;
        }*/
        int clientAddrSize = sizeof(clientAddr);
        cout << "Esperando nuevos clientes..."<<endl;
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;
        }

    }

    void CerrarSocket(){ //se cierra el socket

        /*ofstream archivo;
        archivo.open("server.log.txt",ios::app);
        if(archivo.fail()){
            cout<<"No se pudo abrir el archivo";
            exit(1);
        }

        archivo << horaActual() << ": Conexion Cerrada - cliente cerro sesion\n";
        archivo.close();
        closesocket(client);
        cout << "El cliente se ha desconectado. Se ha liberado el socket\n" << endl;*/
        int clientAddrSize = sizeof(clientAddr);
        if(closesocket(client) == 0){

            cout<<"\n----------El cliente cerro sesion----------\n\n";
            cout << "El cliente se ha desconectado. Se ha liberado el socket\n";

            /** ACA TE BORRE UN COUT DE "Esperando nuevos clientes.." YA QUE SE REPETIA EN LA FUNCION EsperarNuevoCliente**/
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

        /*if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;
        }*/

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
        while(!archivo.eof() && getline(archivo, linea )){ /** ACA REESTRUCTURE EL WHILE PARA QUE SOLAMENTE LEA UNA LINEA DEL ARCHIVO **/
            cout<< linea << "\n";       /** Y LO MANDE AL CLIENTE HASTA QUE SEA EL FINAL DEL ARCHIVO, Y ASI EVITAR MANDAR TODO EL STRING DEL ARCHIVO Y QUE NO SE SATURE EL BUFFER **/
            strcpy(buffer, linea.c_str());
            Enviar();

        }
        strcpy(buffer, "TERMINO");  /** CUANDO YA FINALIZA DE LEER TODO, LE MANDO UN FLAG PARA INDICARLE AL CLIENTE QUE NO VA RECIBIR MAS NADA DEL ARCHIVO **/
        Enviar();
        /*while(!archivo.eof() && getline(archivo, linea)){ //valida que no sea el final del archivo

            texto = texto + linea + "\n";

        }
        std::string cadena = texto;
        strcpy(buffer, cadena.c_str());
        Enviar();*/
        archivo.close();
    }

    char* horaActual(){
        time_t now = time(0);
        tm* localtm = localtime(&now);
        strftime(date,sizeof(date),"%d-%m-%y_%H:%M:%S",localtm);
        return date;
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

/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Semestar:       Zimski (V)
    
    Ime fajla:      server.c
    Opis:           TCP serverAddress
    ********************************************************************
*/
#define IP_ADDRESS "127.0.0.1"
#define PORT 12345
#define DEFAULT_BUFLEN 512
#define MAX_LENGHT 33
#define DEFAULT_NUMBER_OF_MODULES 10
#define MAX_MODULES 100

#include<stdio.h>
#include<string.h>    //strlen
#include <stdlib.h>      //exit codes
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <stdbool.h>
#include "global_variables.h"
#include <time.h>

struct AnalogModule{
    char name[MAX_LENGHT];
    unsigned int value; 
};


struct DigitalModule{
    char name[MAX_LENGHT];
    bool value; 
};


void checkArg(int * argc, char *argv[]){  // nije dobro za slucaj ./server 55a  5
    if(*argc != 3 ){
        analogNum = digitalNum = DEFAULT_NUMBER_OF_MODULES;
    }
    else{
        analogNum = atoi(argv[1]);
        digitalNum = atoi(argv[2]);
        
        if(analogNum<1 || analogNum>MAX_MODULES){
            analogNum = DEFAULT_NUMBER_OF_MODULES;
        }
        if(digitalNum<1 || digitalNum>MAX_MODULES){
            digitalNum = DEFAULT_NUMBER_OF_MODULES;
        }
        
    }
}

struct AnalogModule analogModule[MAX_MODULES];
struct DigitalModule digitalModule[MAX_MODULES];

void fillStructure(){
    for(int i=0;i<analogNum;i++){
        sprintf(analogModule[i].name, "A%d", i+1);
        analogModule[i].value = rand()%1001;
    }

    for(int i=0;i<digitalNum;i++){
        sprintf(digitalModule[i].name, "D%d", i+1);
        digitalModule[i].value = rand()%2;
    }
}

void printStructure(){
    printf("Analogni moduli: \n");
    for(int i=0;i<analogNum;i++){
        printf("%s, %u\n",analogModule[i].name, (unsigned int)analogModule[i].value);
    }

    printf("\nDigitalni moduli: \n");

    for(int i=0;i<digitalNum;i++){
        printf("%s, %d\n",digitalModule[i].name, digitalModule[i].value);
    }
}

void readMessage(int* readSize, int* clientSocketFd, char* clientMessage){
    *readSize = recv(*clientSocketFd, clientMessage, DEFAULT_BUFLEN, 0);
    
    clientMessage[*readSize] = '\0';
    if(*readSize>0){
        printf("Primljena poruka od klijenta: %s\n", clientMessage);
    }

    if(*readSize == 0)
    {
        printf("Client disconnected\n\n");

        //Close client socket
        close(*clientSocketFd);
        printf("Client socket closed\n");
        return;

    }
    else if(*readSize == -1)
    {
        perror("Recvive client message failed failed");
        while(1){}

    }
}

void sendMessage(int* clientSocketFd, char* serverMessage){ // send message
    if(send(*clientSocketFd , serverMessage , strlen(serverMessage), 0) < 0)
    {
        printf("Message sending failed");
    }
    else
    {
        printf("Poruka je spremna za slanje klijentu:\n\"%s\"\n", serverMessage);
    }
}

void listAnalog(char* clientMessage, char* serverMessage, char* tmp, size_t sizeOfTmp){
        strcat(serverMessage,"\n");
        for(int i=0;i<analogNum;i++){
            strcat(serverMessage, analogModule[i].name);
            snprintf(tmp, sizeOfTmp, " %u", analogModule[i].value);
            strcat(serverMessage, tmp);
            strcat(serverMessage, "\n");

        }
        strcat(serverMessage,"\0");
}

void listDigital(char* clientMessage, char* serverMessage, char* tmp, size_t sizeOfTmp){
    {
        strcat(serverMessage,"\n");
        for(int i=0;i<digitalNum;i++){
            strcat(serverMessage,digitalModule[i].name);
            snprintf(tmp, sizeOfTmp, " %u", digitalModule[i].value);
            strcat(serverMessage, tmp);
            strcat(serverMessage, "\n");
        }
        strcat(serverMessage,"\0");
    }
}

void parseBuffer(char* buffer,int* command, char* name, unsigned int* value){
    sscanf(buffer, "%u %s %u", command, name, value);
}

bool compareNameAnalog(char* name,unsigned int* value){
    for(int i=0;i<analogNum;i++){
        if(strcmp(name, analogModule[i].name)==0){
            analogModule[i].value = *value;
            return 1;
        }
        
    }
    return 0;
}
bool compareNameDigital(char* name,unsigned int* value){
    for(int i=0;i<digitalNum;i++){
        if(strcmp(name, digitalModule[i].name)==0){
            digitalModule[i].value = *value;
            return 1;
        }
    }
    return 0;
}

int main(int argc , char *argv[])
{
    int serverSocketFd;
    int clientSocketFd;
    int readSize;
    socklen_t len;

    checkArg(&argc, argv);

    srand(time(NULL));

    fillStructure();
    printStructure();

    //printf("Analogni: %u, digitalni: %u\n", analogNum, digitalNum);

    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    char clientMessage[DEFAULT_BUFLEN];
    char serverMessage[DEFAULT_BUFLEN];

    clientMessage[0] = '\0';
    serverMessage[0] = '\0';
   
    //Create socket
    serverSocketFd = socket(AF_INET , SOCK_STREAM , 0);
    if (serverSocketFd == -1)
    {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Socket successfully created\n\n");
    }

    //Prepare the sockaddr_in structure
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    //Bind
    if(bind(serverSocketFd,(struct sockaddr *)&serverAddress , sizeof(serverAddress)) < 0)
    {
        //print the error message
        perror("Bind failed. Error");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Socket bound sccessfully to [%s:%hu]\n\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
    }

    //Listen
    listen(serverSocketFd , 3);

    //Accept and incoming connection
    printf("Waiting for incoming connections...\n\n");
    len = sizeof(struct sockaddr_in);

    //accept connection from an incoming clientAddress
    konekcija: 
    clientSocketFd = accept(serverSocketFd, (struct sockaddr *)&clientAddress, (socklen_t*)&len);
    if(clientSocketFd < 0)
    {
        perror("Accept client failed");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Client [%s:%hu] connection successfully accepted\n\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
    }

    //Receive a message from client
    char tmp[6]; // maximum ' ' + four-digit number + '\0'
    int command;
    struct AnalogModule test;
   // struct DigitalModule test2;
    while(1){

        readMessage(&readSize, &clientSocketFd, clientMessage); // read message
        if(readSize == 0){
            //printf("tu sam");
            goto konekcija;
        }
        if(readSize == -1){
            break;
        }
    
        serverMessage[0] = '\0'; // clear serverMessage

        if(clientMessage[0]-'0' == 1){listAnalog(clientMessage,serverMessage,tmp, sizeof(tmp));}
        

        if(clientMessage[0]-'0' == 2){listDigital(clientMessage,serverMessage,tmp, sizeof(tmp));}
        
        
        if(clientMessage[0]-'0' == 3){
            parseBuffer(clientMessage,&command, test.name, &test.value);
            printf("Server primio poruku: %u %s %u\n",command, test.name, test.value);

            if(!compareNameAnalog(test.name, &(test.value)))
            {
                strcpy(serverMessage,"Modul nije pronađen!\0");
            }
            else
            {
                strcpy(serverMessage,"Uspesno ste promenili vrednost analognog modula!\0");
            }
        }
        if(clientMessage[0]-'0' == 4){
            parseBuffer(clientMessage,&command, test.name, &test.value);
            printf("Server primio poruku: %u %s %u\n",command, test.name, test.value);

            if(!compareNameDigital(test.name, &(test.value)))
            {
                strcpy(serverMessage,"Modul nije pronađen!\0");
            }
            else
            {
                strcpy(serverMessage,"Uspesno ste promenili vrednost digitalnog modula!\0");
            }
            //compareNameDigital(test.name, &(test.value));
            //listDigital(clientMessage,serverMessage,tmp, sizeof(tmp));
        }
        if(clientMessage[0]-'0' == 5){
            close(serverSocketFd);
            printf("Server socket closed\n\n");
        }
 
        sendMessage(&clientSocketFd, serverMessage);
    }
    

    //Close server socket
    close(serverSocketFd);
    printf("Server socket closed\n\n");

    return 0;
}


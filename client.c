/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza
    Godina studija: Treca (III)
    Semestar:       Zimski (V)
    
    Ime fajla:      client.c
    Opis:           TCP Client
    ********************************************************************
*/

#include <stdio.h>      //printf
#include <stdlib.h>      //exit codes
#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close
#include <stdbool.h>


#define IP_ADDRESS "127.0.0.1"
#define PORT 12345
#define DEFAULT_BUFLEN 512
#define MAX_BUFFER 101

void menu(){
    printf("-------------------------\n");
    printf("Meni:\n");
    printf("1) Izlistaj analogne module\n");
    printf("2) Izlistaj digitalne module\n");
    printf("3) Promeni analogni modul\n");
    printf("4) Promeni digitalne modul\n");
    printf("5) Prekini komunikaciju!\n");
    printf("-------------------------\n\n");

    printf("Unesite komandu: ");
}

bool isValidCommand(char command){ //checks if the character is a number between 1 and 5
    if(command>48 && command<54){  
        return true;
    }
    else {
        return false;
    }
}

bool isLetter(char c){ //checks if the character is a letter 
    if((c>64 && c<91) || (c>96 && c<123))return true; //uppercase and lowercase letters
    return false;
}

void isEndOfBuffer(char* inputBuffer, int* counter, int* error){
    if(inputBuffer[*counter]!='\n'){ //end
        *error = 1;
        return;
    } 
}

void inputCommand(char* inputBuffer, char* command, int* counter){ // loading COMMAND from input buffer
    if(isValidCommand(inputBuffer[*counter])){
        command[0]=inputBuffer[*counter];
        command[1]='\0';
        (*counter)++;
        return;
    }
    command[0]='\0';
    
}

bool isNumber(char c){ //checks if the character is a number 0-9
    if(c>47 && c<58) return true;
    return false;
}

void inputName(char* inputBuffer, char* name,int* counter){
    int i;
    for(i=0; isLetter(inputBuffer[*counter]) || isNumber(inputBuffer[*counter]) ;i++,(*counter)++){ // loading NAME from the input buffer
        name[i]=inputBuffer[*counter];
    }
    name[i] = '\0';
}

void inputValue(char* inputBuffer, char* value, int* counter){ // loading VALUE from the input buffer
    int i;
    for(i=0;inputBuffer[*counter]>47 && inputBuffer[*counter]<58;i++,(*counter)++){ //value
        value[i]=inputBuffer[*counter];
    }
    value[i]='\0';
}

void isValidInputData(char* command, char* name, char* value, int* error){  // checking if the input is good
    if(command[0]=='\0'){                                                   // set error flag on 1 if isn't
        *error=1;
        return;
    }
    if(command[0]=='3' || command[0]=='4'){
        if(name[0]=='\0') {
            *error=1;
            return;
        }
        if(value[0]=='\0') {
            *error=1;
            return;
        }
    }
    else{
        if(name[0]!='\0'){
            *error=1;
            return;
        }
        if(value[0]!='\0'){
            *error=1;
            return;
        }
    }

    if(command[0]=='4' && value[0]>49) {
        *error=1;
        return;
    }
    int i=0;
    while(value[i]!='\0') i++;
    if(i>3) *error=1;
}

void readSpace(char* inputBuffer, int* counter){  // reading space
    while(inputBuffer[*counter]==' ') (*counter)++;
}

void readFromInputBuffer(char* inputBuffer, char* command, char* name, char* value, int* counter, int* error){  // reading input data from input buffer
    readSpace(inputBuffer, counter); //space                                                                    // set error flag on 1 if isn't good input

    inputCommand(inputBuffer, command, counter);  // loading command

    readSpace(inputBuffer, counter); //space

    inputName(inputBuffer, name, counter);  // loading name

    readSpace(inputBuffer, counter);// space

    inputValue(inputBuffer, value, counter); // loading value

    readSpace(inputBuffer, counter); // space

    *error = 0;
    isEndOfBuffer(inputBuffer, counter, error);
    
    isValidInputData(command, name, value, error);

}

void parseInputBuffer(char* inputBuffer, char* command, char* name, char* value, int* error){  // parsing and validating input 

    int counter=0;

    readFromInputBuffer(inputBuffer, command, name, value, &counter, error);

}

void readBuffer(char* inputBuffer){  // reading line in input buffer
    fgets(inputBuffer, MAX_BUFFER, stdin);
    //fpurge(stdin);
}

void fillMessage(char* clientMessage,char* command,char* name, char* value){ // filling out the message to send to the server
    if(command[0]=='3' || command[0]=='4'){
        snprintf(clientMessage, MAX_BUFFER, "%s %s %s", command, name, value);
    }
    else{
        snprintf(clientMessage, MAX_BUFFER, "%s", command);
    }
    
}

void sendMessageToServer(int* clientSocketFd, char* clientMessage, size_t lenghtM){ // sending message to server
    if(send(*clientSocketFd , clientMessage , lenghtM, 0) < 0)
        {
            printf("Message sending failed");
        }
        else
        {
            printf("Komanda \"%s\" uspesno poslata serveru\n\n", clientMessage);
        }
}

void readMessageFromServer(int* readSize, int* clientSocketFd, char* serverMessage){ // reading message from server
    // read message
    *readSize = recv(*clientSocketFd, serverMessage, DEFAULT_BUFLEN, 0);
    
    serverMessage[*readSize] = '\0';
    printf("Primljena poruka: %s\n", serverMessage);
    

    if(*readSize == 0)
    {
        printf("Client disconnected\n\n");

        //Close client socket
        close(*clientSocketFd);
        printf("Client socket closed\n");

    }
    else if(*readSize == -1)
    {
        perror("Recvive clien message failed failed");
    }
}

int main(int argc , char *argv[])
{
    int clientSocketFd;
    struct sockaddr_in serverAddress;

    char inputBuffer[MAX_BUFFER];
    char command[MAX_BUFFER];
    char name[MAX_BUFFER];
    char value[MAX_BUFFER];
    int readSize;
    int error=0;
   
    char clientMessage[DEFAULT_BUFLEN];
    char serverMessage[DEFAULT_BUFLEN];

    //Create socket
    clientSocketFd = socket(AF_INET , SOCK_STREAM , 0);
    if (clientSocketFd == -1)
    {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Socket successfully created\n\n");
    }

    //Set server IP address and port
    serverAddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    //Connect to server
    if (connect(clientSocketFd, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) < 0)
    {
        perror("Failed to connect");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Successfully connected to server [%s:%hu]\n\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
    }

    while(1){
        //Send some data
        menu();
        error=0;

        readBuffer(inputBuffer); // read line in input buffer

        parseInputBuffer(inputBuffer, command, name, value, &error);

        if(error==1){
            printf("\nNeispravan format poruke! Pokusajte opet!\n");
            printf("Unesite poruku u formatu:\n");
            printf("<komanda[1][2][5]>\n");
            printf("<komanda[3][4]><naziv modula><vrednost modula (0-999 za analogni | 0-1 za digitalni)>\n\n");
            continue;
        }

        if(command[0] == '5'){ // end of program
            break;
        }

        fillMessage(clientMessage,command, name, value);

        sendMessageToServer(&clientSocketFd, clientMessage, strlen(clientMessage));
        readMessageFromServer(&readSize, &clientSocketFd, serverMessage);
    }


    //Close socket
    printf("\nUspesno ste izasli iz programa!\n\n");
    close(clientSocketFd);
    printf("Socket closed\n\n");

    return 0;
}


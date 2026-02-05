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

void menu(){
    printf("-------------------------\n");
    printf("Menu\n");
    printf("1) Izlistaj analogne module\n");
    printf("2) Izlistaj digitalne module\n");
    printf("3) Promeni analogni modul\n");
    printf("4) Promeni digitalne modul\n");
    printf("5) Prekini komunikaciju!\n");
    printf("-------------------------\n\n");
}

bool input_command(int* command, int* client_socket_fd){ //ne radi za unos slova kao komande 
    printf("Unesite komandu: ");
    scanf("%d",command);
    if(*command<1 || *command>5){
        printf("Unesite ponovo komandu\n");
        return 0;
    }
    //printf("Comand: %d\n",*command); 
    return 1;
}

void reset_message(char* message){
    message[0]= '\0';
}

/*void parse_command(int* command, char* message, int size_of_messgae){

}**/
void fill_message(int* command, char* message){
    message[0] = (char)(*command + '0');
    message[1] = '\0';
}

int main(int argc , char *argv[])
{
    int client_socket_fd;
    struct sockaddr_in server_address;
    char client_message[DEFAULT_BUFLEN];


    int command;
    int read_size;
    int value;
    char name[5];
    char tmp[6];
   
   //char client_message[DEFAULT_BUFLEN];
    char server_message[DEFAULT_BUFLEN];

    //Create socket
    client_socket_fd = socket(AF_INET , SOCK_STREAM , 0);
    if (client_socket_fd == -1)
    {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Socket successfully created\n\n");
    }

    //Set server IP address and port
    server_address.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    //Connect to server
    if (connect(client_socket_fd, (struct sockaddr *)&server_address , sizeof(server_address)) < 0)
    {
        perror("Failed to connect");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Successfully connected to server [%s:%hu]\n\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
    }

    while(1){
        //Send some data
        menu();
        reset_message(client_message);
        if(!input_command(&command, &client_socket_fd)){
            continue;
        }
        if(command == 5){
         break;
        }
        
        fill_message(&command,client_message);
        
        if(command == 3 || command == 4){
            name[0] = '\0';
            scanf("%s %u",name, &value);
            //while(getchar()!='\n');
            strcat(client_message," ");
            strcat(client_message, name);
            snprintf(tmp, sizeof(tmp), " %u",value);
            strcat(client_message, tmp);
            strcat(client_message, "\0");
        }

        if(send(client_socket_fd , client_message , strlen(client_message), 0) < 0)
        {
            printf("Message sending failed");
        }
        else
        {
            printf("Komanda \"%s\" uspesno poslata serveru\n\n", client_message);
        }
        { // read message
            read_size = recv(client_socket_fd, server_message, DEFAULT_BUFLEN, 0);
            
            server_message[read_size] = '\0';
            printf("Primljena poruka: %s\n", server_message);
            

            if(read_size == 0)
            {
                printf("Client disconnected\n\n");

                //Close client socket
                close(client_socket_fd);
                printf("Client socket closed\n");

            }
            else if(read_size == -1)
            {
                perror("Recvive clien message failed failed");
            }
        }
    }


    //Close socket
    close(client_socket_fd);
    printf("Socket closed\n\n");

    return 0;
}


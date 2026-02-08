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

bool is_valid_command(char command){
    if(command>48 && command<54){  // conversion command to int
        return true;
    }
    else {
        return false;
    }
}

void parse_command(unsigned int* c, char* command){
    *c = command[0] - '0';
}

bool is_letter(char c){
    if((c>64 && c<91) || (c>96 && c<123))return true;
    return false;
}

void read_counter(char* input_buffer, int* counter){
    while(input_buffer[*counter]==' ') (*counter)++; // command
}

void is_end_of_buffer(char* input_buffer, int* counter, int* error){
    if(input_buffer[*counter]!='\n'){ //end
        *error = 1;
        return;
    } 
}

void input_command(char* input_buffer, char* command, int* counter){
    if(is_valid_command(input_buffer[*counter])){
        command[0]=input_buffer[*counter];
        command[1]='\0';
        (*counter)++;
        return;
    }
    command[0]='\0';
    
}

bool is_number(char c){
    if(c>47 && c<58) return true;
    return false;
}

void input_name(char* input_buffer, char* name,int* counter){
    int i;
    /*if(is_letter(input_buffer[*counter])){ // only first character must be a letter
        name[0]=input_buffer[*counter];
        (*counter)++;
    }*/
    for(i=0; is_letter(input_buffer[*counter]) || is_number(input_buffer[*counter]) ;i++,(*counter)++){ // name
        name[i]=input_buffer[*counter];
    }
    name[i] = '\0';
}

void input_value(char* input_buffer, char* value, int* counter){
    int i;
    for(i=0;input_buffer[*counter]>47 && input_buffer[*counter]<58;i++,(*counter)++){ //value
        value[i]=input_buffer[*counter];
    }
    value[i]='\0';
}

void is_valid_input_data(char* command, char* name, char* value, int* error){
    if(command[0]=='\0'){
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

void read_space(char* input_buffer, int* counter){
    while(input_buffer[*counter]==' ') (*counter)++;
}

void read_from_input_buffer(char* input_buffer, char* command, char* name, char* value, int* counter, int* error){
    read_space(input_buffer, counter); //space

    input_command(input_buffer, command, counter);

    read_space(input_buffer, counter); //space

    input_name(input_buffer, name, counter);

    read_space(input_buffer, counter);// space

    input_value(input_buffer, value, counter);

    read_space(input_buffer, counter); // space

    *error = 0;
    is_end_of_buffer(input_buffer, counter, error);
    
    is_valid_input_data(command, name, value, error);

}

void parse_input_buffer(char* input_buffer, char* command, char* name, char* value, int* error){

    int counter=0;

    read_from_input_buffer(input_buffer, command, name, value, &counter, error);

}

void read_buffer(char* input_baffer){
    fgets(input_baffer, MAX_BUFFER, stdin);
    //fpurge(stdin);
}

void fill_message(char* client_message,char* command,char* name, char* value){
    if(command[0]=='3' || command[0]=='4'){
        snprintf(client_message, MAX_BUFFER, "%s %s %s", command, name, value);
    }
    else{
        snprintf(client_message, MAX_BUFFER, "%s", command);
    }
    
}

void send_message_to_server(int* client_socket_fd, char* client_message, size_t lenght_m){
    if(send(*client_socket_fd , client_message , lenght_m, 0) < 0)
        {
            printf("Message sending failed");
        }
        else
        {
            printf("Komanda \"%s\" uspesno poslata serveru\n\n", client_message);
        }
}

void read_message_from_server(int* read_size, int* client_socket_fd, char* server_message){
    // read message
    *read_size = recv(*client_socket_fd, server_message, DEFAULT_BUFLEN, 0);
    
    server_message[*read_size] = '\0';
    printf("Primljena poruka: %s\n", server_message);
    

    if(*read_size == 0)
    {
        printf("Client disconnected\n\n");

        //Close client socket
        close(*client_socket_fd);
        printf("Client socket closed\n");

    }
    else if(*read_size == -1)
    {
        perror("Recvive clien message failed failed");
    }
}

int main(int argc , char *argv[])
{
    int client_socket_fd;
    struct sockaddr_in server_address;
    char client_message[DEFAULT_BUFLEN];


    char input_buffer[MAX_BUFFER];
    char command[MAX_BUFFER];
    char name[MAX_BUFFER];
    char value[MAX_BUFFER];
    int read_size;
    int error=0;
   
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
        error=0;

        read_buffer(input_buffer); // read line in input buffer

        parse_input_buffer(input_buffer, command, name, value, &error);

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

        fill_message(client_message,command, name, value);

        send_message_to_server(&client_socket_fd, client_message, strlen(client_message));
        read_message_from_server(&read_size, &client_socket_fd, server_message);
    }


    //Close socket
    printf("\nUspesno ste izasli iz programa!\n\n");
    close(client_socket_fd);
    printf("Socket closed\n\n");

    return 0;
}


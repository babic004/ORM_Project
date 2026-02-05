/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Semestar:       Zimski (V)
    
    Ime fajla:      server_address.len
    Opis:           TCP server_address
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


void check_arg(int * argc, char *argv[]){  // nije dobro za slucaj ./server 55a  5
    if(*argc != 3 ){
        analog_num = digital_num = DEFAULT_NUMBER_OF_MODULES;
    }
    else{
        analog_num = atoi(argv[1]);
        digital_num = atoi(argv[2]);
        
        if(analog_num<1 || analog_num>MAX_MODULES){
            analog_num = DEFAULT_NUMBER_OF_MODULES;
        }
        if(digital_num<1 || digital_num>MAX_MODULES){
            digital_num = DEFAULT_NUMBER_OF_MODULES;
        }
        
    }
}

struct AnalogModule analog_module[MAX_MODULES];
struct DigitalModule digital_module[MAX_MODULES];

void fill_structure(){
    for(int i=0;i<analog_num;i++){
        sprintf(analog_module[i].name, "A%d", i+1);
        analog_module[i].value = rand()%1001;
    }

    for(int i=0;i<digital_num;i++){
        sprintf(digital_module[i].name, "D%d", i+1);
        digital_module[i].value = rand()%2;
    }
}

void print_structure(){
    printf("Analogni moduli: \n");
    for(int i=0;i<analog_num;i++){
        printf("%s, %u\n",analog_module[i].name, (unsigned int)analog_module[i].value);
    }

    printf("\nDigitalni moduli: \n");

    for(int i=0;i<digital_num;i++){
        printf("%s, %d\n",digital_module[i].name, digital_module[i].value);
    }
}

void read_message(int* read_size, int* client_socket_fd, char* client_message){
    *read_size = recv(*client_socket_fd, client_message, DEFAULT_BUFLEN, 0);
    
    client_message[*read_size] = '\0';
    printf("Primljena poruka od klijenta: %s\n", client_message);
    

    if(*read_size == 0)
    {
        printf("Client disconnected\n\n");

        //Close client socket
        close(*client_socket_fd);
        printf("Client socket closed\n");
        return;

    }
    else if(*read_size == -1)
    {
        perror("Recvive client message failed failed");
        while(1){}

    }
}

void send_message(int* client_socket_fd, char* server_message){ // send message
    if(send(*client_socket_fd , server_message , strlen(server_message), 0) < 0)
    {
        printf("Message sending failed");
    }
    else
    {
        printf("Poruka je spremna za slanje klijentu:\n %s\n", server_message);
    }
}

void list_analog(char* client_message, char* server_message, char* tmp, size_t size_of_tmp){
        strcat(server_message,"\n");
        for(int i=0;i<analog_num;i++){
            strcat(server_message, analog_module[i].name);
            snprintf(tmp, size_of_tmp, " %u", analog_module[i].value);
            strcat(server_message, tmp);
            strcat(server_message, "\n");

        }
        strcat(server_message,"\0");
}

void list_digital(char* client_message, char* server_message, char* tmp, size_t size_of_tmp){
    {
        strcat(server_message,"\n");
        for(int i=0;i<digital_num;i++){
            strcat(server_message,digital_module[i].name);
            snprintf(tmp, size_of_tmp, " %u", digital_module[i].value);
            strcat(server_message, tmp);
            strcat(server_message, "\n");
        }
        strcat(server_message,"\0");
    }
}

void parse_buffer(char* buffer,int* command, char* name, unsigned int* value){
    sscanf(buffer, "%u %s %u", command, name, value);
}

bool compare_name_analog(char* name,unsigned int* value){
    for(int i=0;i<analog_num;i++){
        if(strcmp(name, analog_module[i].name)==0){
            analog_module[i].value = *value;
            return 1;
        }
        
    }
    return 0;
}
bool compare_name_digital(char* name,unsigned int* value){
    for(int i=0;i<digital_num;i++){
        if(strcmp(name, digital_module[i].name)==0){
            digital_module[i].value = *value;
            return 1;
        }
    }
    return 0;
}

int main(int argc , char *argv[])
{
    int server_socket_fd;
    int client_socket_fd;
    int read_size;
    socklen_t len;

    check_arg(&argc, argv);

    srand(time(NULL));

    fill_structure();
    print_structure();

    //printf("Analogni: %u, digitalni: %u\n", analog_num, digital_num);

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    char client_message[DEFAULT_BUFLEN];
    char server_message[DEFAULT_BUFLEN];

    client_message[0] = '\0';
    server_message[0] = '\0';
   
    //Create socket
    server_socket_fd = socket(AF_INET , SOCK_STREAM , 0);
    if (server_socket_fd == -1)
    {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Socket successfully created\n\n");
    }

    //Prepare the sockaddr_in structure
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    //Bind
    if(bind(server_socket_fd,(struct sockaddr *)&server_address , sizeof(server_address)) < 0)
    {
        //print the error message
        perror("Bind failed. Error");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Socket bound sccessfully to [%s:%hu]\n\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
    }

    //Listen
    listen(server_socket_fd , 3);

    //Accept and incoming connection
    printf("Waiting for incoming connections...\n\n");
    len = sizeof(struct sockaddr_in);

    //accept connection from an incoming client_address
    konekcija: 
    client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_address, (socklen_t*)&len);
    if(client_socket_fd < 0)
    {
        perror("Accept client failed");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Client [%s:%hu] connection successfully accepted\n\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    }

    //Receive a message from client
    char tmp[6]; // maximum ' ' + four-digit number + '\0'
    int command;
    struct AnalogModule test;
   // struct DigitalModule test2;
    while(1){

        read_message(&read_size, &client_socket_fd, client_message); // read message
        if(read_size == 0){
            //printf("tu sam");
            goto konekcija;
        }
        if(read_size == -1){
            break;
        }
    
        server_message[0] = '\0'; // clear server_message

        if(client_message[0]-'0' == 1){list_analog(client_message,server_message,tmp, sizeof(tmp));}
        

        if(client_message[0]-'0' == 2){list_digital(client_message,server_message,tmp, sizeof(tmp));}
        
        
        if(client_message[0]-'0' == 3){
            parse_buffer(client_message,&command, test.name, &test.value);
            printf("Server primio poruku: %u %s %u\n",command, test.name, test.value);

            if(!compare_name_analog(test.name, &(test.value)))
            {
                strcpy(server_message,"Pogresna komanda\0");
            }
            else
            {
                list_analog(client_message,server_message,tmp, sizeof(tmp));
            }
        }
        if(client_message[0]-'0' == 4){
            parse_buffer(client_message,&command, test.name, &test.value);
            printf("Server primio poruku: %u %s %u\n",command, test.name, test.value);

            if(!compare_name_digital(test.name, &(test.value)))
            {
                strcpy(server_message,"Pogresna komanda\0");
            }
            else
            {
                list_digital(client_message,server_message,tmp, sizeof(tmp));
            }
            compare_name_digital(test.name, &(test.value));
            list_digital(client_message,server_message,tmp, sizeof(tmp));
        }
        if(client_message[0]-'0' == 5){
            close(server_socket_fd);
            printf("Server socket closed\n\n");
        }
 
        send_message(&client_socket_fd, server_message);
    }
    

    //Close server socket
    close(server_socket_fd);
    printf("Server socket closed\n\n");

    return 0;
}


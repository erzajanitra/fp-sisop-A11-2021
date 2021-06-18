#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
struct sockaddr_in address;
int sock = 0, valread;
struct sockaddr_in serv_addr;
char buffer[1024] = {0};
int flag=0;

void authent(){
    char input_client[100];
    read(sock, buffer, 1024); 
    printf("%s\n", buffer);
    printf("Input Field : ");
    gets(input_client);
    send(sock, input_client, strlen(input_client), 0);
    bzero(buffer, 1024);
    read(sock, buffer, 1024);
    
    char inp_user[100];
    strcpy(inp_user,buffer);
    bzero(buffer, 1024);

    if (strstr(inp_user, "=== Root Authority ===") != 0)
    {
        printf("%s\n", inp_user);
        char command[200];
        gets(command);
        send(sock, command, strlen(command), 0);

        if(strstr(command,"quit")!=0){
            sock = 0;
        }
    }
    else if (strstr(inp_user, "=== Checking For Password ===") != 0)
    {
        printf("%s\n", inp_user);
        read(sock, buffer, 1024);
        //printf("%s\n", buffer); ada/nggak ada.
        if(strstr(buffer,"ada")!=0){ 
            flag = 1;
        }else{
            flag = 0;
        }

        while(flag == 1){
            printf("mySQL >");
            send(sock,"User Enter!",11,0);
            char usr[100];
            gets(usr);
            send(sock,usr,strlen(usr),0);
            bzero(buffer,1024);
            read(sock,buffer,1024);

            if(strstr(buffer,"quit")!=0){
                printf("%s\n",buffer);
                flag = 0;
            }else{
                printf("%s\n",buffer);
                continue;
            }


        }
    }else{
        sock = 0;
    }
}

int main(int argc, char const *argv[]) {
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(sock >= 0){
        authent();
    }
    return 0;
}

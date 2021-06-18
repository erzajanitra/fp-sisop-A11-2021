#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
FILE *fp, *fw, *p;
char *userdb = "/home/tsania/Documents/FP/userdb.txt";
char *usersdb = "/home/tsania/Documents/FP/usersdb.txt";
char *p_file = "/home/tsania/Documents/FP/permissiondb.txt";
int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[1024] = {0};
int flag =0;

char *success = "success";
char *failed = "failed";

struct authent_data{
  char username[100];
  char password[100];
  char permission[100];
};
char *fplog = "/home/tsania/Documents/FP/FP.log";

struct buatlog{
    char username[100];
    char desc[1024];
};

void logging(struct buatlog data){
    FILE * LOGFILE = fopen(fplog, "a");
	  time_t now;
	  time ( &now );
	  struct tm * timeinfo = localtime (&now);

    fprintf(LOGFILE,"%02d-%02d-%02d %02d:%02d:%02d:%s:%s\n",
        timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
        data.username,data.desc);

    fclose(LOGFILE);
}


void create_db(char arr[]){
    char filepath[100];
    strcpy(filepath,"/home/tsania/Documents/FP/");
    strcat(filepath,arr);
    mkdir(filepath,0777);

   
}

bool cari_akun(char *login){
  
  fp = fopen(userdb, "r");
  char data2[100];
  while (fgets(data2, sizeof(data2), fp) != NULL)
  {
    if (strstr(data2, login) != 0)
    {
      flag = 1;
      break;
    }
  }

  fclose(fp);

  if(flag == 1) return 1;
  else return 0;
}

void authent(){
    char inp_cmd[100];
    struct authent_data new[100];
    char *welcome = "=== Please Sign In to Continue ===\nFormat : ./mySQL -u username -p password\n";
    send(new_socket , welcome , strlen(welcome) , 0 );
    read( new_socket , buffer, 1024);//user command : ./mySQL blablabla
    
    strcpy(inp_cmd,buffer);
    if(strncmp(inp_cmd,"sudo",4)==0){
        flag = 2; 
        int i=0;
      while(flag == 2){
        send(new_socket, "=== Root Authority ===", 25, 0);
        bzero(buffer,1024);
        read(new_socket, buffer, 1024);//baca input
        printf("%s\n",buffer );
      
        if (strstr(buffer, "CREATE USER") != 0)
        {
          printf("CREATE USER!\n");
          
          //char misal[100] = "CREATE USER jack IDENTIFIED BY admin123";
          char *pwd = strrchr(buffer, ' ') + 1;
          char *uname = strtok(buffer, " ");
          int n = 0;
          char user[100];
          while (uname != NULL && n < 2)
          {
            uname = strtok(NULL, " ");
            if (n == 1)
              strcat(user, uname);
            n++;
          }

          strcpy(new[i].username, user);
          strcpy(new[i].password, pwd);
          strcpy(new[i].permission,": ");

          char inp_str[100];
          sprintf(inp_str, "-u %s -p %s", new[i].username, new[i].password);

          fw = fopen(userdb, "a");
          fprintf(fw,"%s\n",inp_str);
          fclose(fw);
          i++;
          printf("New Data Added!\n");
        }else if(strstr(buffer,"GRANT PERMISSION")!=0){
          char rootcmd[100];
          strncpy(rootcmd,buffer,strlen(buffer)-1);//GRANT PERMISSION database1 INTO tsania;
          char *username = strrchr(rootcmd,' ')+1;//hilangin titik koma
          char uname[100];
          strcpy(uname,username);
          char first_cmd[100];
          strncpy(first_cmd,rootcmd,strlen(rootcmd)-strlen(uname)-5);
          char *dbnama = strrchr(first_cmd,' ')+1;
          char db[100];
          strcpy(db,dbnama);
          
          int iter=0;

          while(iter < i){
            
            if(strcmp(username,new[iter].username)==0){
                strcat(new[iter].permission,db);
                p = fopen(p_file, "a");
                fprintf(p,"%s %s %s\n",new[iter].username,new[iter].password,new[iter].permission);
                fclose(p);
                break;
            }
            iter++;
          }

            printf("Grant Access Success!\n");
        }else{
          flag = 0;
        }
      }
    }else if(strncmp(inp_cmd,"./mySQL",7)==0){
      send(new_socket, "=== Checking For Password ===", 30, 0);

      char login[100];
      sprintf(login, "%s", strchr(inp_cmd, '-'));
      printf("Login String : %s\n",login);
      struct authent_data attempt;

      strncpy(attempt.username, login, strlen(login) - strlen(strrchr(login, '-')));
      strcpy(attempt.password, strrchr(login, '-'));

      if(cari_akun(login)==1){
        send(new_socket,"ada",3,0);
        flag = 1;
      }else{
        send(new_socket,"gaada",5,0);
        flag = 0;
      }

      while(flag == 1){
        bzero(buffer,1024);
        read(new_socket,buffer,1024);
        printf("%s\n",buffer);
        struct buatlog logs;
        char *asli = strchr(attempt.username,' ')+1;
        char uname[100];
        strcpy(uname,asli);
        strcpy(logs.username,uname);
        bzero(buffer,1024);
        read(new_socket,buffer,1024);

        if(strcmp(buffer,"quit")==0){
          send(new_socket,"quit",4,0);
          flag = 0;
        }else if(strstr(buffer,"CREATE DATABASE")!=0){
          char cmd_input[100];
          strncpy(cmd_input,buffer,strlen(buffer)-1);
          char *db = strtok(cmd_input, " ");
          int n = 0;
          char nama_db[100];
          while (db != NULL && n < 1 )
          {
            db = strtok(NULL, " ");
            if (n == 1)
              strcpy(nama_db,db);
            n++;
          }

          int it=0;

          while(it < 100){
            
            if(strstr(nama_db,new[it].permission)!=0){
                create_db(nama_db);
                break;
            }
            it++;
          }
          fw = fopen(usersdb, "a");
          fprintf(fw,"%s\n",nama_db);
          fclose(fw);
          //printf("DB Create Success!\n");
          send(new_socket,"create db success!",18,0);
          strcpy(logs.desc,buffer);
          logging(logs);
        }else if(strstr(buffer,"INSERT INTO")!=0){
          send(new_socket,"insert db success!",18,0);
          strcpy(logs.desc,buffer);
          logging(logs);
        }else if(strstr(buffer,"DELETE DATABASE")!=0){
          send(new_socket,"delete db success!",18,0); 
          strcpy(logs.desc,buffer);
          logging(logs); 
        }else{
          send(new_socket,"Invalid Command!",16,0);
          strcpy(logs.desc,buffer);
          logging(logs);
        }
      }
    }else{
      new_socket = 0;
    }

  bzero(buffer,1024);
  
}


int main(int argc, char const *argv[]) {
    
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }


    while(new_socket >= 0){
      authent();
    }
    
    return 0;
}

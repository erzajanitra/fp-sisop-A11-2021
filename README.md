# Final Project Sisop 2021-A11

## Authentication
  Pertama-tama struct diinisialisasi terlebih dahulu guna menyimpan data user yang diinputkan pada general user. Superuser tidak membutuhkan struct berikut dikarenakan sudah memiliki built-in command yang sudah pasti memperbolehkan akses database secara langsung.
  PADA SERVER :
```
struct authent_data{
  char username[100];
  char password[100];
  char permission[100];
};
```
### Root/Superuser
  Command yang digunakan untuk mengakses database sebagai superuser adalah dengan menginputkan command : `sudo ./mySQL.` Dan ketika superuser melakukan login attempt tidak ada pengecekan password seperti halnya general user.
### General User
  Command yang digunakan untuk mengakses database sebagai general user adalah dengan menginputkan command : `./mySQL -u username -p password`. Dan ketika general user melakukan login attempt terdapat pengecekan password sehingga ketika berhasil akan dapat masuk ke console teritori user dan apabila gagal maka mengulangi login attempt.
  PADA SERVER :
```
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
```
  Potongan Source Code diatas berfungsi untuk mencari data yang terdaftar pada list user database guna mengecek apakah data user yang diinputkan sudah terdaftar pada sistem. jika tidak ditemukan maka kembali ke laman login attempt dan jika ditemukan maka program berjalan seperti :
  PADA SERVER :
```
if(cari_akun(login)==1){
        send(new_socket,"ada",3,0);
        flag = 1;
}
while (flag == 1){
    //seluruh query command yang berhak dijalankan oleh user
    //dapat dilihat pada source code.
}

```
  PADA CLIENT :
```
if(strstr(buffer,"ada")!=0){ 
            flag = 1;
        }else{
            flag = 0;
        }

while(flag == 1){
    printf("mySQL >");

    //seluruh hasil komunikasi antara server-client
    //dapat dilihat pada source code.
}
```

## Authorization
### Root/Superuser
### General User
## DDL (Data Definition Language)
### CREATE DATABASE
### CREATE TABLE
### DROP DATABASE/TABLE/COLUMN
## DML (Data Manipulation Language)
### INSERT
### DELETE
### UPDATE
### SELECT
## Logging
  Pada pembuatan logfile dari program diterapkan insersi log baru pada logfile setiap kali terjadi operasi pada clientside. untuk membuat komponen pada log perlu dibuat struct yang akan menyimpan string informasi dan username dan fungsi yang mengoperasikan suatu file dengan 'append' data baru setiap pemanggilannya.
  PADA SERVER :
```
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
```
OUTPUT LOG :


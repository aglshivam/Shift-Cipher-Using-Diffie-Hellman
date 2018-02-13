/***
*Server side Ceaser Cipher using Diffie-Hillman key exchange
*-Recieve prime number p and primitive root a from client 
*-Randomly generate private key xb
*-Generate public key using yb = a^xb mod p
*-Recieve client public key ya
*-Generate shared key by kba = ya^xb mod p
*-Find Ceaser key k = kba mod 67
*-Recieve encrypted message from the client
*-Decrpyt the encrypted message and write the result to the output.txt
***/

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

int p, a, ya, yb, xb, kba, k;

/*Return corresponding integer for the given char*/
int charToInt(char ch)
{
    if(ch == ' ')
        return 0;
    else if(ch >= 'A' && ch <= 'Z')
        return (ch -'A' +1);
    else if(ch >= 'a' && ch <= 'z')
        return (ch - 'a' +40);
    else if(ch >= '0' && ch <= '9')
        return (ch - '0' + 30);
    else if(ch == ',')
        return 27;
    else if(ch == '.')
        return 28;
    else if(ch == '?')
        return 29;
    else if(ch == '!')
        return 66;
    else return -1;
}

/*Return corresponding char for the given int*/
char intToChar(int ch)
{
    if(ch == 0)
        return ' ';
    else if(ch >=1 && ch <= 26)
        return ('A' + ch -1);
    else if(ch >= 30 && ch <= 39)
        return ('0' + ch -30);
    else if(ch >= 40 && ch <= 65)
        return ('a' + ch -40);
    else if(ch == 27)
        return ',';
    else if(ch == 28)
        return '.';
    else if(ch == 29)
        return '?';
    else if(ch == 66)
        return '!';
    else return '$';
}

/*return x^y mod p*/
int power(int x, unsigned int y, int p)
{
    int res = 1;
    x = x % p; 
               
    while (y > 0)
    {
        if (y & 1)
            res = (res*x) % p;
 
        y = y>>1;
        x = (x*x) % p;
    }
    return res;
}


int connectToClient()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    return new_socket;
}

void decryptionAlgo(int new_socket)
{
    char buffer [1024];
    int i, j, r, valread;
    FILE *fp = fopen("output.txt", "w");
    while(1)
    {
        i = 0;
        memset(buffer, '\0', sizeof(buffer));
        fflush(stdin);
        fflush(stdout);
        valread = read( new_socket , buffer, 1024);
        if(valread == 0)
            break;
        printf("%s\n",buffer);
        while(i < valread)
        {
            j = charToInt(buffer[i]);
            if(j != -1)
            {
                r = (j - k)%67;
                r = r < 0 ? r + 67 : r;
                buffer[i] = intToChar(r); 
            }
            i++;
        }
        fwrite(buffer , 1 , i , fp );
    }
    fclose(fp);
}

void msgTransfer(int new_socket)
{
    int valread;
    char buffer[1024] = {0};

    //recieved p
    valread = read( new_socket , buffer, 1024);
    p = atoi(buffer);
    //printf("p is %d\n",p);
    send(new_socket , "1", strlen("1") , 0 ); 

    memset(buffer, '\0', sizeof(buffer));
    fflush(stdin);
    fflush(stdout);
    //recieved a
    valread = read( new_socket , buffer, 1024);
    a = atoi(buffer);
    //printf("a is %d\n",a);
    send(new_socket , "1", strlen("1") , 0 ); 
    
    memset(buffer, '\0', sizeof(buffer));
    fflush(stdin);
    fflush(stdout);

    //recieved ya
    valread = read( new_socket , buffer, 1024);
    ya = atoi(buffer);
    //printf("ya is %d\n",ya);
    send(new_socket , "1", strlen("1") , 0 ); 
    fflush(stdin);

    xb = rand()%p;
    yb = power(a, xb, p);
    //printf("yb is %d\n",yb);
    fflush(stdout);
    char snum[10];
    sprintf(snum, "%d", yb);
    send(new_socket , snum, strlen(snum) , 0 );   
    fflush(stdout);

    //printf("message sent\n");
    kba = power(ya, xb, p);
    k = kba %67;
    //printf("kba is %d\n", kba);
    //printf("key is %d\n", k);
    decryptionAlgo(new_socket);
    printf("Message succesfully decrypted!!!\n");
    close(new_socket);
}

int main(int argc, char const *argv[])
{
    int new_socket = connectToClient();
    msgTransfer(new_socket);
    return 0;
}
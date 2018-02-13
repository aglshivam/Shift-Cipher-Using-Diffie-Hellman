/***
*Client Side Ceaser Cipher using Diffie-Hillman key exchange
*-
***/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080


int xa, ya, yb, kab, k, p, a;
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


int calc_primitive(int num)
{
	int f=num-1,i,j,check[num];
	int flag=1;
	for(i=2;i<num;i++)
	{
	    for(j=0;j<num;j++)
	        check[j]=0;
	    flag=0;
	    for(j=0;j<f;j++)
	    {
	        int res=power(i,j,num);
	        if(check[res]!=0)
	        {
	            flag=0;
	            break;
	        }
	        check[res]=1;
	    }
	    if(!flag)
	        return i;
	}
}

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

int miillerTest(int d, int n)
{
    int a = 2 + rand() % (n - 4);
 
    int x = power(a, d, n);
 
    if (x == 1  || x == n-1)
       return 1;
 
    while (d != n-1)
    {
        x = (x * x) % n;
        d *= 2;
 
        if (x == 1)      return 0;
        if (x == n-1)    return 1;
    }
 
    // Return composite
    return 0;
}

int isPrime(int n, int k)
{
    // Corner cases
    if (n <= 1 || n == 4)  return 0;
    if (n <= 3) return 1;
 
    // Find r such that n = 2^d * r + 1 for some r >= 1
    int d = n - 1;
    while (d % 2 == 0)
        d /= 2;
 
    // Iterate given nber of 'k' times
    for (int i = 0; i < k; i++)
         if (miillerTest(d, n) == 0)
              return 0;
 
    return 1;
}


int  connectTOServer()
{
	struct sockaddr_in address;
    int sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
  	return sock;
    
}

void encryptionAlgo(int sock)
{
	FILE *fp = fopen("input.txt", "r");
	int len;
	char buf [1024];
	char send_buf [1024];
	int i,j;
	while(1)
	{
		i = 0;
		memset(buf, '\0', sizeof(buf));
		memset(send_buf, '\0', sizeof(send_buf));
		len = fread (buf, 1, sizeof (buf), fp);
        if (len == 0) {
            break;
        }
        //printf("%s\n", buf);
        while(i < len)
        {
        	j = charToInt(buf[i]);
        	if(j == -1)
        		send_buf[i] = buf[i];
        	else send_buf[i] = intToChar((j+k)%67);
        	i++;
        }
    	send(sock , send_buf, strlen(send_buf) , 0 );
        
	}
	fclose(fp);
	printf("Message successfully encrypted!!!\n");
}

void msgTransfer(int sock, int p, int a)
{
	int valread;
    char buffer[1024] = {0};
    char snum[10];
    //printf("p sent %d\n",p);
    sprintf(snum, "%d", p);
    send(sock , snum, strlen(snum) , 0 );
    valread = read( sock , buffer, 1024);
    //printf("Respoce %s\n",buffer);

    //printf("Hello message sent\n");
    fflush(stdout);
    fflush(stdin);

    memset(buffer, '\0', sizeof(buffer));
    memset(snum, '\0', sizeof(snum));
    //printf("a sent %d\n",a);
    sprintf(snum, "%d", a);
    send(sock , snum, strlen(snum) , 0 );
    valread = read( sock , buffer, 1024);
    //printf("Respoce %s\n",buffer);

    fflush(stdout);
 	fflush(stdin);
 	xa = rand()%p;
 	ya = power(a, xa, p);
 	memset(buffer, '\0', sizeof(buffer));
    memset(snum, '\0', sizeof(snum));
    //printf("ya sent %d\n",ya);
	sprintf(snum, "%d", ya);
    send(sock , snum, strlen(snum) , 0 ); 
    valread = read( sock , buffer, 1024);
    //printf("Respoce %s\n",buffer);	

 	fflush(stdout);
    fflush(stdin);
 	memset(buffer, '\0', sizeof(buffer));
    memset(snum, '\0', sizeof(snum));
    valread = read( sock , buffer, 1024);
    yb = atoi(buffer);
    //printf("yb is %d\n",yb);

    fflush(stdout);
    fflush(stdin);
 	memset(buffer, '\0', sizeof(buffer));
    kab = power(yb, xa, p);
    //printf("kab is %d\n",kab);
    k = kab %67;
    printf("key is %d\n",k);
    encryptionAlgo(sock);
    close(sock);
}
  
int main(int argc, char const *argv[])
{

	while(1)
	{
		p = rand();
		if(p < 10)
			continue;
		if(isPrime(p, 2))
			break;
	}

	printf("prime number is %d\n",p);
	//a = primitiveRoot(p);
	a = calc_primitive(p);
	printf("%d\n",a);

	int sock = connectTOServer();
	msgTransfer(sock, p, a);
	return 0;
}
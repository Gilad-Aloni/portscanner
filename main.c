#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void help_menu()
{
    FILE *menu = fopen("helpmenu.txt", "rt");
    if(menu == NULL)
    {
        printf("cant find menu file");
        return;
    }

    char buffer[64];
    while(fgets(buffer, 64, menu) != NULL)
    {
        if(buffer[strlen(buffer)-1] == '\n') buffer[strlen(buffer)-1] = '\0';
        puts(buffer);
    }
}

// function returns:
// 1 if it configuares by dns(-d)
// 0 if by IP(-i)
// -1 if there are too much or not enough arguments
// 2 if the flag is help
int flag_management(int argc, const char *flag);

int main(int argc, char *argv[])
{
    int flagState = flag_management(argc, argv[1]);
    if(flagState == 2)
    {
        help_menu();
        return 0;
    }
    else if(flagState == -1)
    {
        printf("unmatching number of arguments");
        return -1;
    }

    struct hostent *h;
    struct sockaddr_in rem;
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0) // socket function returns -1 when fails
    {
        perror("sfd: ");
        return -1;
    }

    memset(&rem, sizeof(rem), 0);
    rem.sin_family = AF_INET;
    rem.sin_port = htons(atoi(argv[3])); 
    if(flagState == 0)
    {
        
        if(!inet_aton(argv[2], &rem.sin_addr)) // inet_aton returns zero if the address is valid
        {
            perror("inet_aton: ");
            return -1;    
        }
    }
    else
    {
        h = gethostbyname(argv[2]);
        if(h == NULL) // gethostbyname return NULL if an error occurs
        {
            perror("gethostbyname: ");
            return -1;
        }

        rem.sin_addr.s_addr = *(unsigned long *) h->h_addr;
    }
    

    int ret = connect(sfd, (struct sockaddr *) &rem, sizeof(struct sockaddr_in));
    if(ret < 0)
    {
        perror("connection: ");
        printf("\nthe port given is not open\n");
        return -1;
    }

    printf("Port open\n");
    
    return 0;
}

int flag_management(int argc, const char *flag)
{
    if(strcmp(flag, "-h") == 0) return 2;
    if(argc != 4) return -1;
    if(strcmp(flag, "-i") == 0) return 0;
    else if(strcmp(flag, "-d") == 0) return 1;
}

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#define BACKLOG 10
#define BUFSZ 100
#define NAMELN 20
int main(int argc, char const *argv[])
{
	int status;
	if(argc != 2)
	{
		printf("Usage: ./a port\n");
		return 0;
	}
	struct addrinfo hints, *res, *p;  // will point to the results
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	if((status = getaddrinfo(NULL, argv[1], &hints, &res)) != 0)
	{
		printf("getaddrinfo() failed: %s\n", gai_strerror(status));
    	return 1;
	}
	int ssid = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(ssid == -1)
    {
    	perror("socket() failed");
    	return 1;
    }
	int yes = 1;
    if (setsockopt(ssid, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) //forceful binding
    {
	    perror("setsockopt() failed");
	    return 1;
	} 
	if(bind(ssid, res->ai_addr, res->ai_addrlen) == -1)
    {
    	perror("bind() failed");
    	return 1;
    }
    if(listen(ssid, BACKLOG) == -1)
    {
        perror("listen() failed");
        return 1;
    }
    struct sockaddr_storage client;
    socklen_t addr_size;
    addr_size = sizeof client;
    int cid = accept(ssid, (struct sockaddr *)&client, &addr_size), len, bytes, tmp, i;
	char s[BUFSZ], c[BUFSZ], cname[NAMELN], *sname = "Server", cport[NAMELN], *cmd;
	if((status = getnameinfo((struct sockaddr *)&client, addr_size, cname, sizeof cname, cport, sizeof cport, NI_NOFQDN)) != 0)
	{
		printf("getnameinfo() failed %s\n", gai_strerror(status));
		return 1;
	}
	printf("** Connected to %s:%s **\n", cname, cport);
	while(1)
	{
		if((bytes = recv(cid, c, BUFSZ - 1, 0)) == -1)
		{
	        perror("recv");
	        return 1;
	    }
	    else if(bytes == 0)
	    {
	    	printf("** Connection Terminated **\n");
	    	break;
	    }
	    c[bytes] = '\0';
	    cmd = strtok(c, " ");
	    if(strcmp(cmd, "get") == 0)
	    {
	    	cmd = strtok(NULL, " ");
	    	if( access(cmd, F_OK) != -1 ) 
	    	{
			    // file exists
			    FILE *file;
	    		file = fopen(cmd, "r");
	    		if(file == NULL)
	    		{
	    			strcpy(s, "failure: File Cannot Be Opened");
	    			len = strlen(s);
	    			bytes = send(cid, s, len, 0);
	    		}
	    		else
	    		{
	    			strcpy(s, "success:");
	    			len = strlen(s);
	    			bytes = send(cid, s, len, 0);
	    			fseek(file, 0, SEEK_END);
					int byteCount = ftell(file);
					fseek(file, 0);
	    			sprintf(s, "%d", byteCount);
	    			len = (int)((ceil(log10(byteCount)) + 1) * sizeof(char));
	    			bytes = send(cid, s, len, 0);
	    			while(fgets(s, BUFSZ, file) != NULL) 
			        { 
			          	len = strlen(s);
    					bytes = send(cid, s, len, 0);
			        }
	    		}
	    		fclose(file);
			} 
			else 
			{
				strcpy(s, "failure: File Does Not Exits");
    			len = strlen(s);
    			bytes = send(cid, s, len, 0);
			}
	    }
	    /*else if(strcmp(cmd, "put") == 0)
	    {
	    	cmd = strtok(NULL, " ");
	    	if(access(cmd, F_OK) != -1) 
	    	{
	    		strcpy(s, "failure: File Already Exists. Owerwrite? Y/N");
	    		if((bytes = recv(cid, c, BUFSZ - 1, 0)) == -1)
				{
			        perror("recv");
			        return 1;
			    }
			    else if(bytes == 0)
			    {
			    	printf("** Connection Terminated **\n");
			    	break;
			    }
			    c[bytes] = '\0';
			    if(c[0] == 'Y')
			    {
			    	FILE *file;
			    	fopen(cmd, "w");
			    	if(file == NULL)
		    		{
		    			strcpy(s, "failure: File Cannot Be Opened");
		    			len = strlen(s);
		    			bytes = send(cid, s, len, 0);
		    		}
			    }			
			}
	    }*/
	    else if(strcmp(cmd, "ls") == 0 || strcmp(cmd, "pwd") == 0)
	    {
	    	strcat(c, " 1> log");
	    	System(c);
	    	FILE *file;
    		file = fopen("log", "r");
    		if(file == NULL)
    		{
    			strcpy(s, "failure: File Cannot Be Opened");
    			len = strlen(s);
    			bytes = send(cid, s, len, 0);
    		}
    		else
    		{
    			strcpy(s, "success:");
    			len = strlen(s);
    			bytes = send(cid, s, len, 0);
    			fseek(file, 0, SEEK_END);
				int byteCount = ftell(file);
				fseek(file, 0);
    			sprintf(s, "%d", byteCount);
    			len = (int)((ceil(log10(byteCount)) + 1) * sizeof(char));
    			bytes = send(cid, s, len, 0);
    			while(fgets(s, BUFSZ, file) != NULL) 
		        { 
		          	len = strlen(s);
					bytes = send(cid, s, len, 0);
		        }
    		}
    		fclose(file);
	    }
		else if(strcmp(cmd, "cd") == 0)
		{
			strcat(c, " 1> log");
			System(c);
	    	FILE *file;
    		file = fopen("log", "r");
    		if(file == NULL)
    		{
    			strcpy(s, "failure: File Cannot Be Opened");
    			len = strlen(s);
    			bytes = send(cid, s, len, 0);
    		}
    		else
    		{
    			fseek(file, 0, SEEK_END);
				int byteCount = ftell(file);
				if(byteCount == 0)
				{
					strcpy(s, "failure:");
					len = strlen(s);
					bytes = send(cid, s, len, 0);
					fseek(file, 0);
	    			sprintf(s, "%d", byteCount);
	    			len = (int)((ceil(log10(byteCount)) + 1) * sizeof(char));
	    			bytes = send(cid, s, len, 0);
	    			while(fgets(s, BUFSZ, file) != NULL) 
			        { 
			          	len = strlen(s);
						bytes = send(cid, s, len, 0);
			        }
				}
				else
				{	
					strcpy(s, "success:");
					len = strlen(s);
					bytes = send(cid, s, len, 0);
				
				}
    			
    		}
    		fclose(file);
		}
	}
	close(cid);
    freeaddrinfo(res);
	return 0;
}
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#define BACKLOG 10
#define BUFSZ 100
#define NAMELN 20
int main(int argc, char const *argv[])
{
	int status;
	char const *addr, *port, *type;
	if(argc != 4)
	{
		printf("Usage: ./a [s/c] address port\n");
		return 0;
	}
	type = argv[1], addr = argv[2], port = argv[3];
	struct addrinfo hints, *res, *p;  // will point to the results
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	if((status = getaddrinfo(addr, port, &hints, &res)) != 0)
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
    if(strcmp(type, "s") == 0)
    {
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
	    int cid = accept(ssid, (struct sockaddr *)&client, &addr_size), len, bytes;
		char s[BUFSZ], c[BUFSZ], cname[NAMELN], *sname = "You", cport[NAMELN];
		if((status = getnameinfo((struct sockaddr *)&client, addr_size, cname, sizeof cname, cport, sizeof cport, NI_NOFQDN)) != 0)
		{
			printf("getnameinfo() failed %s\n", gai_strerror(status));
			return 1;
		}
		printf("** Connected to %s:%s **\n", cname, cport);
		while(1)
		{
			printf("%s: ", sname);
			fgets(s, BUFSZ, stdin);
			if(strcmp(s, "close\n") == 0)
				break;
			len = strlen(s);
			bytes = send(cid, s, len, 0);
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
		    printf("%s: %s\n", cname, c);
		}
	    close(cid);
    }
    else
    {
    	if(connect(ssid, res->ai_addr, res->ai_addrlen) == -1)
	    {
	        perror("connect() failed");
	        return 1;
	    }
	    char s[BUFSZ], c[BUFSZ], sname[NAMELN], *cname = "You", sport[NAMELN];
	    socklen_t addr_size;
	    addr_size = sizeof (*(res->ai_addr));
	    int len, bytes;
	    if((status = getnameinfo((struct sockaddr *)res->ai_addr, addr_size, sname, sizeof sname, sport, sizeof sport, NI_NOFQDN)) != 0)
		{
			printf("getnameinfo() failed: %s\n", gai_strerror(status));
			return 1;
		}
		printf("** Connected to %s:%s **\n", sname, sport);
	    while(1)
		{
			if((bytes = recv(ssid, s, BUFSZ - 1, 0)) == -1)
			{
		        perror("recv");
		        return 1;
		    }
		    else if(bytes == 0)
		    {
		    	printf("** Connection Terminated **\n");
		    	break;
		    }
		    s[bytes] = '\0';
		    printf("%s: %s\n", sname, s);
		    printf("%s: ", cname);
			fgets(c, BUFSZ, stdin);
			if(strcmp(c, "close\n") == 0)
			{
				printf("** Connection Terminated **\n");
				break;
			}
			len = strlen(c);
			bytes = send(ssid, c, len, 0);
		}
	    close(ssid);
    }
    freeaddrinfo(res);
	return 0;
}
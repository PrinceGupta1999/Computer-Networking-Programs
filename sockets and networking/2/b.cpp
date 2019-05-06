#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;
int main()
{
	ifstream f("tcp.pcap",ios::binary|ios::in) ;
	char c;	
	int offset = 0, base = 40; // Size of pcap header 
	unsigned int u;
	while(offset < base && !f.eof())
	{
		f.get(c);
		offset++;
	}
	printf("--Information from Data Link Layer--\n");
	printf("Destination MAC Addr: ");
	while(offset < base + 6 && !f.eof()){
		f.get(c);
		printf("%02x",(u_char)c);
		offset++;
		if(offset != base + 6)
			printf(":");
	}
	printf("\n");
	printf("Source MAC Addr: ");
	while(offset < base + 12 && !f.eof())
	{
		f.get(c);
		printf("%02x",(u_char)c);
		offset++;
		if(offset != base + 12)
			printf(":");
	}
	printf("\n");
	base += 14; // Size of Ethernet Header
	printf("--Information from Network Layer--");
	while(offset < base + 10 && !f.eof())
	{
		f.get(c);
		offset++;
	}
	printf("\n");
	printf("CheckSum: ");
	while(offset < base + 12 && !f.eof())
	{
		f.get(c);
		printf("%02x",(u_char)c);
		offset++;
		if(offset != base + 12)
			printf(":");
	}
	printf("\n");
	printf("Source IP Address: ");
	while(offset < base + 16 && !f.eof())
	{
		f.get(c);
		printf("%u",(u_char)c);
		offset++;
		if(offset != base + 16)
			printf(".");
	}
	printf("\n");
	printf("Destination IP Address: ");
	while(offset < base + 20 && !f.eof()){
		f.get(c);
		printf("%u",(u_char)c);
		offset++;
		if(offset != base + 20)
			printf(".");
	}
	base += 20; // Size of IP Header
	printf("\n");
	printf("--Information from TCP Layer--\n");
	printf("Source Port Number: ");
	f.get(c);
	u = (u_char)c;
	u *= 256;
	f.get(c);
	u += (u_char)c;
	printf("%u\n",u);
	printf("Destination Port Number: ");
	f.get(c);
	u = (u_char)c;
	u *= 256;
	f.get(c);
	u += (u_char)c;
	printf("%u\n",u);
	// offset += 4;
	// base += 32; // Size of TCP Header
	// printf("--Information from HTTP Layer--\n");
	// while(offset < base)
	// {
	// 	f.get(c);
	// 	offset++;
	// }
	// while(offset < base + 17)
	// {
	// 	f.get(c);
	// 	printf("%c", c);
	// 	offset++;
	// }
	f.close();
	return 0;
}
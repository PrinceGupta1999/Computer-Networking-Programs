#include <iostream>
#include <fstream>
#include <stdio.h>
bool bit(unsigned short int i, unsigned short int num)
{
	if(((1 << i) & num) == 0)
		return false;
	return true;
}
void ocadd(unsigned short int &res, unsigned short int &a)
{
	bool s, c = false, ab, rb;
	unsigned short int i, v, tr = 0;
	for(i = 0; i < 16; i++)
	{
		rb = bit(i, res), ab = bit(i, a);
		s = rb ^ ab ^ c;
		c = (rb & ab) | (ab & c) | (c & rb);
		v = s;
		tr |= (v << i); 
	}
	if(c)
		a = 1, ocadd(tr, a);
	res = tr;
}
using namespace std;
int main()
{
	ifstream f("icmp.pcap",ios::binary|ios::in) ;
	char c;
	unsigned short int res = 0, a, cs, i;	
	int offset = 0, base = 54; // Size of pcap header and ethernet header 
	while(offset < base && !f.eof())
	{
		f.get(c);
		offset++;
	}
	while(offset < base + 20 && !f.eof()){
		f.get(c);
		if(offset % 2 == 0)
			a = ((u_char)c), a *= 256;
		else 
		{
			a += ((u_char)c);
			if(offset == base + 11)
				cs = a;
			else
				ocadd(res, a);
		}
		offset++;
	}
	printf("Checksum Evaluated at Source: %02x\n", cs);
	printf("Checksum Evaluated Before 1's Complement: %02x\n", res);
	res ^= 0xffff;
	printf("Checksum Evaluated at Destination: %02x\n", res);
	if(res == cs)
		printf("IP Header's Integrity Not Lost\n");
	else
		printf("IP Header's Integrity has been Lost\n");
	f.close();
	return 0;
}
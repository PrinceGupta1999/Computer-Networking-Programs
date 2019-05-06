#include <bits/stdc++.h>
using namespace std;
int main(int argc, char const *argv[])
{
	ifstream file;
	int i;
	size_t pos;
	string line, search = "inet";
	system("ifconfig wlo1 1> log");
	file.open("log");
	while(getline(file, line))
	{
		pos = line.find(search);
		if(pos != string::npos)
		{
			cout<<"IPv4 Address: ";
			for(i = pos + search.size() + 1; line[i] != ' '; i++)
				cout<<line[i];
			cout<<"\n";
			search = " netmask ";
			i += search.size() + 1;
			cout<<"Netmask: ";
			for(; line[i] != ' '; i++)
				cout<<line[i];
			cout<<"\n";
			search = "inet6";
			getline(file, line);
			pos = line.find(search);
			if(pos != string::npos)
			{
				cout<<"IPv6 Address: ";
				for(i = pos + search.size() + 1; line[i] != ' '; i++)
					cout<<line[i];
				cout<<"\n";
			}
			search = "ether";
			getline(file, line);
			pos = line.find(search);
			if(pos != string::npos)
			{
				cout<<"mac Address: ";
				for(i = pos + search.size() + 1; line[i] != ' '; i++)
					cout<<line[i];
				cout<<"\n";
			}
			break;
		}
	}
	file.close();
	return 0;
}
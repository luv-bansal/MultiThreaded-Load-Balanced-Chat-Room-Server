#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
using namespace std;

#define MAX_LEN 256
#define BACKLOG 10
#define SERVER_NAME_LEN_MAX 255
#define PORT 6000
vector<int> SERVERPORTS;
map<string,int> roomServerDict;
int clientNumber = 0;

struct socket_client_thread {
	int server_socket;
	struct sockaddr_in client_address;
};

void *balance_load(void *arg);
void signal_handler(int signal_number);

int main()
{
	int socket_id, server_socket, totalServers, serverport;
	cout<<"\n\t************Load Balancer************\n";
    cout<<"Enter the Starting Server port: ";
    cin>>serverport;
	cout<<"Enter total number of Servers: ";
    cin>>totalServers;
	cout<<"\n";
    while(totalServers--)
    {
        SERVERPORTS.push_back(serverport++);
    }
	struct sockaddr_in address;
	pthread_attr_t pthread_attr;
	socket_client_thread *pthread_arg;
	pthread_t pthread;
	socklen_t client_address_len;

	memset(&address, 0, sizeof address);
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	if((socket_id = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
		perror("socket");
		exit(1);
	}

	if(bind(socket_id, (struct sockaddr *)&address, sizeof address) == -1)
    {
		perror("bind");
		exit(1);
	}

	if(listen(socket_id, BACKLOG) == -1)
    {
		perror("listen");
		exit(1);
	}

	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
		perror("signal");
		exit(1);
	}
	if(signal(SIGTERM, signal_handler) == SIG_ERR)
    {
		perror("signal");
		exit(1);
	}
	if(signal(SIGINT, signal_handler) == SIG_ERR)
    {
		perror("signal");
		exit(1);
	}

	if(pthread_attr_init(&pthread_attr) != 0)
    {
		perror("pthread_attr_init");
		exit(1);
	}
	if(pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED) != 0)
    {
		perror("pthread_attr_setdetachstate");
		exit(1);
	}

	while(true) 
    {
		pthread_arg = (socket_client_thread *)malloc(sizeof *pthread_arg);
		if(!pthread_arg)
    	{
			perror("malloc");
			continue;
		}

		client_address_len = sizeof(pthread_arg->client_address);
		server_socket = accept(socket_id, (struct sockaddr *)&pthread_arg->client_address, &client_address_len);
		if(server_socket == -1)
    	{
			perror("accept");
			free(pthread_arg);
			continue;
		}

		pthread_arg->server_socket = server_socket;
		if(pthread_create(&pthread, &pthread_attr, balance_load, (void *)pthread_arg) != 0)
    	{
			perror("pthread_create");
			free(pthread_arg);
			continue;
		}
	}
	return 0;
}

int getLoadServer(int idx)
{	
	char server_name[SERVER_NAME_LEN_MAX + 1] = "127.0.0.1\0";
	int server_port = SERVERPORTS[idx], socket_id;
	struct hostent *server_host;
	struct sockaddr_in server_address;
	server_host = gethostbyname("localhost");
	memset(&server_address, 0, sizeof server_address);
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	memcpy(&server_address.sin_addr.s_addr, server_host->h_addr, server_host->h_length);

	socket_id = socket(AF_INET, SOCK_STREAM, 0);
	connect(socket_id, (struct sockaddr *)&server_address, sizeof server_address);

	char name[MAX_LEN] = "__LoadBalancer__";
    send(socket_id,name,sizeof(name),0);
    char room[MAX_LEN] = "__getLoad?__";
    send(socket_id,room,sizeof(room),0);

	int reply = -1;
	recv(socket_id,&reply,sizeof(reply),0);
    char str[MAX_LEN]="#exit";
	send(socket_id,str,sizeof(str),0);
	close(socket_id);
	return reply;
}


void *balance_load(void *arg) 
{
	clientNumber++;
	int clientIdx = clientNumber;
	socket_client_thread *pthread_arg = (socket_client_thread *)arg;
	int server_socket = pthread_arg->server_socket;
	struct sockaddr_in client_address = pthread_arg->client_address;
	
	free(arg);
	char name[MAX_LEN],room[MAX_LEN];
    recv(server_socket,name,sizeof(name),0);
    recv(server_socket,room,sizeof(room),0);
	cout<<"Client ("<<name<<") connected.\n";
    if(roomServerDict.find(string(room)) != roomServerDict.end())
    {
        cout<<"Directing client to server for room no. "<<string(room)<<"\n";
        int optimalServerPort = roomServerDict[string(room)];
        send(server_socket,&optimalServerPort,sizeof(optimalServerPort),0);
    }
    else
    {
        cout<<"\nNew Room Id found, Finding optimal server for load balancing:\n\n";
        cout<<"Loads on Servers:\n";
        vector<int> loads(SERVERPORTS.size());
        for(int idx = 0;idx<loads.size();idx++)
        {
            loads[idx] = getLoadServer(idx);
			if(loads[idx] < 0) 
			{
				loads[idx] = INT_MAX;
				cout<<"Server "<<idx+1<<" : "<<"Not Responding"<<"\n";	
			}
			else
            cout<<"Server "<<idx+1<<" : "<<loads[idx]<<"\n";
        }
		cout<<"\n";
        int optimalServerPort = SERVERPORTS[min_element(loads.begin(),loads.end()) - loads.begin()];
        send(server_socket,&optimalServerPort,sizeof(optimalServerPort),0);
        roomServerDict[string(room)] = optimalServerPort;
    }
	
	cout<<"Client ("<<name<<") disconnected.\n";
	close(server_socket);

	return NULL;
}

void signal_handler(int signal_number)
{
	exit(0);
}
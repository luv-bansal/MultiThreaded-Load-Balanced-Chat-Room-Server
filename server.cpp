#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
using namespace std;
#define MAX_LEN 256
#define NUM_COLORS 6
string default_colour="\033[0m";
string colors[]={"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m","\033[36m"};
int client_index_count = 0;
mutex cout_mutex,clients_mutex;

using namespace std;

struct Client{
	int client_id;
	string client_name;
	string client_room;
	int client_socket;
	thread client_thread;
};
vector<Client> clients;
string color(int code);

void handle_client_connection(int client_socket, int id);

int main(int argc, char *argv[])
{
	int PORT = argc > 1 ? atoi(argv[1]) : 0;
	if(!PORT) 
	{
		cout<<"Enter Port: \n";
		cin>>PORT;
	}
	int server_socket;
	if((server_socket=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("Socket: ");
		exit(-1);
	}
	struct sockaddr_in server;
	server.sin_family=AF_INET;
	server.sin_port=htons(PORT);
	server.sin_addr.s_addr=INADDR_ANY;
	bzero(&server.sin_zero,0);

	if((bind(server_socket,(struct sockaddr *)&server,sizeof(struct sockaddr_in)))==-1)
	{
		perror("Bind error: ");
		exit(-1);
	}
	if((listen(server_socket,8))==-1)
	{
		perror("Listen error: ");
		exit(-1);
	
	}
	struct sockaddr_in client;
	int client_socket;
	unsigned int len=sizeof(sockaddr_in);
	cout << colors[NUM_COLORS-1]<<"\n\t************CHAT ROOM SERVER: "<<PORT<<"************"<<"\n"<<default_colour;
	while(true)
	{
		if((client_socket = accept(server_socket,(struct sockaddr *)&client,&len)) == -1)
		{
			perror("Accept error: ");
			exit(-1);
		}
		client_index_count++;
		thread connection_thread(handle_client_connection,client_socket,client_index_count);
		lock_guard<mutex> guard(clients_mutex);
		clients.push_back({client_index_count, string("Anonymous"),"0",client_socket,(move(connection_thread))});
	}
	for(int i=0; i<clients.size(); i++){
		if(clients[i].client_thread.joinable()) clients[i].client_thread.join();
	}
	close(server_socket);
	return 0;
}

string color(int code){return colors[code%NUM_COLORS];}

void set_Client(int id, char name[], char room[])
{
	for(int i=0; i<clients.size(); i++)
	{
		if(clients[i].client_id==id) 
		{
			clients[i].client_name=string(name);
			clients[i].client_room=string(room);
		}	
	}	
}

void server_print(string str, bool endLine=true)
{	
	lock_guard<mutex> guard(cout_mutex);
	cout<<str;
	if(endLine) cout<<endl;
}

int broadcast_to_clients(string message, int sender_id, string sender_room)
{
	char temp[MAX_LEN];
	strcpy(temp,message.c_str());
	for(int i=0; i<clients.size(); i++)
	{
		if(clients[i].client_id!=sender_id && clients[i].client_room == sender_room) 
		{
			send(clients[i].client_socket,temp,sizeof(temp),0);
		}
	}		
	return 1;
}

int broadcast_to_clients(int num, int sender_id, string sender_room)
{
	for(int i=0; i<clients.size(); i++)
	{
		if(clients[i].client_id!=sender_id && clients[i].client_room == sender_room) 
		send(clients[i].client_socket,&num,sizeof(num),0);
	}	
	return 1;	
}

void end_connection(int id, string room)
{
	for(int i=0; i<clients.size(); i++)
	{
		if(clients[i].client_id == id && clients[i].client_room == room)
		{
			lock_guard<mutex> guard(clients_mutex);
			clients[i].client_thread.detach();
			clients.erase(clients.begin()+i);
			close(clients[i].client_socket);
			break;
		}
	}				
}

void handle_client_connection(int client_socket, int id)
{
	char name[MAX_LEN],room[MAX_LEN],str[MAX_LEN];
	recv(client_socket,name,sizeof(name),0);
	recv(client_socket,room,sizeof(room),0);
	set_Client(id,name,room);
	if(strcmp(name,"__LoadBalancer__") == 0 && strcmp(room,"__getLoad?__") == 0)
	{
		int noOfClients = clients.size()-1;
		send(client_socket,&noOfClients,sizeof(noOfClients),0);
		cout<<"Load on this server: "<<noOfClients<<"\n";
	}
	else
	{
		string initial_message=string(name)+string(" has joined Room: ");
		broadcast_to_clients("#NULL",id, room);	
		broadcast_to_clients(id,id,room);								
		broadcast_to_clients(initial_message+room,id,room);	
		server_print(color(id)+initial_message+room+default_colour);
	}
	while(true)
	{
		int bytes_received = recv(client_socket,str,sizeof(str),0);
		if(bytes_received <= 0)
			return;
		if(strcmp(str,"#exit") == 0)
		{
			if(strcmp(name,"__LoadBalancer__") != 0 || strcmp(room,"__getLoad?__") != 0)
			{
				string message=string(name)+string(" has left Room: ");		
				broadcast_to_clients("#NULL",id,room);			
				broadcast_to_clients(id,id,room);						
				broadcast_to_clients(message+room,id,room);
				server_print(color(id)+message+room+default_colour);
			}
			else
			{
				cout<<"Sent Load count to Loadbalancer\n";
			}
			end_connection(id,room);							
			return;
		}
		broadcast_to_clients(string(name),id,room);					
		broadcast_to_clients(id,id,room);		
		broadcast_to_clients(string(str),id,room);
	}
}
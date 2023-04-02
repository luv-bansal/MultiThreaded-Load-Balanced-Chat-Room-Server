
# Multi-Threaded Load Balanced Chat Room Server

### Overview
* <p align = "justify">This project aims to implement multithreaded chat room servers.</p> 
* <p align = "justify">It supports multiple client, multiple servers and a Load balancer that uses Round-Robin Load Balancing technique to distribute client load equally on all the servers.</p> 
* <p align = "justify">This project is compatible for all GNU C++ compilers on UNIX/LINUX based environments.</p> 
* <p align = "justify">It uses Socket programming and create connection between server and client using TCP.</p> 
* <p align = "justify">Multithreading ensures parallel communication between multiple clients in multiple chat-rooms.</p> 
* <p align = "justify">Mutex locks are mapped to each client to avoid race conditions while connection and communication.</p> 


### Setup and use Locally

* Clone the repository and change the directory.
```bash
  git clone https://github.com/Pasa1912/MultiThreaded-Load-Balanced-Chat-Room-Server.git
  cd MultiThreaded-Load-Balanced-Chat-Room-Server
```
* Compile the ```server.cpp```, ```loadbalancer.cpp``` and ```client.cpp``` files
```bash
 g++ server.cpp -lpthread -o server
 g++ loadbalancer.cpp -lpthread -o loadbalancer
 g++ client.cpp -lpthread -o client
```
* Run the files sequentially as - 

    * Assign a unique port to each of the server that needs to be created. These ports must be consecutive. Run the ```server``` file.
    ```bash
        ./server <port>
    ```
    * Run the ```loadbalancer``` file. It runs on port ```:6000``` by default.
    ```bash
       ./loadbalancer
    ```
    * Enter the port of the first server and the total number of servers created.
    * Run the ```client``` file and enter Name and Room ID to chat in.
    ```bash
       ./client
    ```
    *  Keep repeating the above step for every client that needs to be created. Enjoy :)
### Working
<p align = "justify">This project uses Socket Programming in C++ using Transmission Control Protocol( TCP ). The function of the <b>Loadbalancer</b> is to keep record of loads on each server and also keep watch on the servers that are not able to connect to the clients to appropriately distribute traffic equally on each of the working server. The Loadbalancer implemented in this project uses a <b>Dynamic Round-Robin Load Balancing</b> algorithm to equally distribute the client load to the servers. The concept of <b>Multi-threading</b> is used for parallel or concurrent execution of sending and receiving messages simultaneously by each client. Race Conditions that arise during connection and communication of server and client is resolved by mapping a separate mutex lock for each client.

The Gradual  Working process of this project is as - 
* The Client first makes a connection request to the Loadbalancer. It sends the details of Client name and Room ID it needs to connect to.
* The Loadbalancer then makes connection request to each Server and requests for the amount of Client load that Server is currently handling.
* The Loadbalancer thus compares load on each running Server and thus assigns weight to each server. Thus it directs the Client to the optimally chosen Server.
* The Client thus makes a connection request to the Server as chosen by Loadbalancer.
* The Server then accepts the connection request and run a thread for sending and receiving messages from each Client.

This working can be explained using the below Connection Diagram - </p>

<p align="center"><img src = "https://user-images.githubusercontent.com/67159796/174442113-5783e59b-93b8-4d80-9433-513ae44760ff.png"></p>

### Learnings 
<p align = "justify">Gained practical experience in Socket Programming in C++ using Transmission Control Protocol, Load Balancing and Distribution Techniques and Multi-threading concepts. Learnt using mutex locks to prevent race conditions among different threads. 
</p>

### Additional Tasks
* <p align = "justify">Used Multithreading to create Multiple Chat Rooms between Multiple Clients. </p>
* <p align = "justify">Used Load Balancer to distribute equal loads on Multiple Servers.</p>

### Demo Video
<p align="center"><video src = "https://user-images.githubusercontent.com/67159796/174442006-b1aa628d-27c9-4d80-87da-55839b670bee.mp4"></p>

### References

 - [TCP/IP Socket Connection](https://www.ibm.com/docs/en/zvse/6.2?topic=SSB27H_6.2.0/fa2ti_what_is_socket_connection.html)
 - [Server-Client Implementation using TCP](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/)
 - [Dynamic Round-Robin Load Balancing](https://www.nginx.com/resources/glossary/round-robin-load-balancing/)
 - [Multithreading in C++](https://medium.com/codex/c-multithreading-the-simple-way-95aa1f7304a2)
 - [Mutex Locks](https://docs.oracle.com/cd/E19455-01/806-5257/sync-12/index.html)
 - ACM IITR Lecture Series for Multithreaded Server


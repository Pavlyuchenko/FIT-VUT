# Metadata
*Subject*: IPK  
*Project*: Client for a chat server using IPK24-CHAT protocol  
*Author*: Michal Pavlíček xpavlim00  
*Description*: A client application capable of communicating with a remote server using IPK24-CHAT protocol (based on TCP and UDP protocols).  

# File Structure
*ipkchat24.c* 		- The main file bringing it all together. Has epoll waiting.  
fsm\_\* 		- A finite state machine that keeps track of the current application state (based on the diagram in IPK24-CHAT protocol). The most important file after ipkchat24.c, checks for stdin and socket events in each state and sends/processes data entered by the user or received from the server.   
*cli.c/.h*		- Functions and structs regarding the command line interface. Has argument parsing and parses user entered commands.  
*epoll\_handler.c/.h* 	- Creates epoll fds.  
*tcp/udp\_handler*	- Functions that deal with the format of the protocol messages. Either extracts data from user sent messages or encodes data to be sent to the server.  

# Theory
The project requires knowledge of basic networking principles and two transport protocols: TCP and UDP.  
The difference between the two is mainly reliability and connection. While TCP relies on connection to a server (so-called handshake), UDP does not establish a connection with a server. This makes TCP reliable, where it makes sure that packets are received in correct order and sends damaged or lost packets. UDP does not do that, and the packets can be delivered in any shape of form, making it unreliable. The main benefit of UDP is speed.  
TCP would be used in messaging applications, where speed is not really neccessar, while reliability is critical. UDP shines in data heavy transmissions, such as online gaming, where it is not a huge problem if a few packets here and there do not make it to the destination.  

# Narrative about interesting code sections
A finite state machine (FSM) is being implemented by fsm\_\*.c files. The FSM takes care of correct cli and server replies based on the current state of the application. From a network point-of-view, the files tcp\_handler and udp\_handler provide an interesting insight into the basic working of both these protocols. While in TCP the data is transferred in human readable text, UDP uses byte strings to convey important transmission data. Throughout the code, countless structs and enums are used to maintain code readability and scalability.

# Testing
Testing was done using multiple resources: Wireshark, custom server, Discord server, testing suite made by a fellow college (+ my own tests).

## Wireshark
In the inital phases of development, Wireshark was used as a main source of testing. The app provides user-friendly interface to examine bytes sent to a server. I used Wireshark to check what (and whether) data was sent, if they are in the correct format and so on. For UDP, this was much more useful, as no connection to an actual server was required and the data could be inspected on a specific port even though no server was actually listening. TCP was used along with a custom made server as TCP requires connection.

## Custom server
A tiny custom server was made for the purpose of testing TCP protocol. The server would listen to any messages and display them. This way it was easy to notice any mistakes made during programming. Later, the server was extended and it could also send messages to the client so as to check this functionality as well. This checked if the parsing functions worked properly and if the FSM was correctly programmed.

## Discord server
A provided Discord server was used to check whether there were any differences between the expected behaviour and the custom server. 

## Testing suite
A [suite](https://git.fit.vutbr.cz/xhobza03/ipk-client-test-server) consisting of 38 basic tests (all of which my code passed) that examined the basic functionality of both TCP and UDP variants. These tests proved to be a vital point during the development of this app, as they provided a clear feedback and allowed for quick debugging of the problem. Tests were run each time after compiling the app to ensure nothing was broken by the changes. I am aware of the sentence "Since it is your application, you should also be the author of test sets." in the Documentation instructions section and have no real excuse for not doing that.

# Bibliography
[Man7] man7.org. Available at: https://man7.org/. [Cited 2024-04-01].  
[The Open Group] The Open Group. Available at: https://pubs.opengroup.org/. [Cited 2024-04-01].  
[Ryšavý O., Veselý V.] Prezentace předmětu IPK [PowerPoint slides]. Computer Communications and Networks, Brno University of Technology. https://www.fit.vut.cz/.  
[Brian Hall] Beej's Guide to Network Programming. Available at: https://beej.us/guide/bgnet/html/. [Cited 2024-04-01].  

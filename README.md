# Team_CRYPTO--P2P_chat
 A peer-to-peer chat application in C++ that allows users to send and receive messages at the same time, supports communication with multiple peers, and enables users to check and retrieve a list of peers from whom they have received messages.

# Peer-to-Peer Chat Program in C++

## Overview
This is a simple peer-to-peer (P2P) chat application written in C++ using Winsock2. The program allows users to:
- Send and receive messages simultaneously.
- Communicate with multiple peers.
- Query and retrieve a list of peers from whom messages have been received.

## Features
- Peer-to-peer communication without a central server.
- Users can send messages directly to other peers using their IP address and port number.
- Keeps track of active peers in a session.
- Supports real-time message exchange.

## Prerequisites
Ensure you have the following installed:
- Windows OS (required for Winsock2 API).
- Microsoft Visual Studio (or any C++ compiler supporting Winsock2).

## How to Compile and Run

### 1. Clone the Repository

git clone https://github.com/KatariyaMohit/Team_CRYPTO--P2P_chat.git
cd Team_CRYPTO--P2P_chat

### 2. Compile the Code
If using g++, compile with:

g++ -o chat_program p2p.cpp -lws2_32

For Microsoft Visual Studio, create a new C++ project and include the source code.

### 3. Run the Program

.\chat_program.exe

## How It Works
1. The program initializes Winsock2 and detects the local IP address.
2. The user provides a team name and selects a port for listening.
3. A server thread starts listening for incoming messages.
4. The user can choose from the following options:
   - **Send a message**: Enter the recipient's IP address and port to send a message.
   - **Query active peers**: Display a list of peers that have sent messages.
   - **Connect to known peers**: Send a connection message to previously contacted peers.
   - **Quit the program**: Exit the chat application.
5. Messages are formatted as:

<IP ADDRESS:PORT> <team name> <your message>
   
6. The receiver stores the sender’s information and displays the message.

## Example Usage
### User 1:
- IP: 192.168.1.10, Port: 5000
- Starts the program and listens for messages.

### User 2:
- IP: 192.168.1.11, Port: 6000
- Connects to User 1 and sends a message.

**Output on User 1's Screen:**

Message from 192.168.1.11:6000 [TeamB]: Hello, how are you?

## Scenarios and Choices
### Choice 1: Send a Message
What Happens: You can send a message to another peer by entering their IP address and port number.

Next Steps:
- Enter the recipient's IP address.
- Enter the recipient's port number.
- Type your message and press Enter.
- Result: The message is sent to the specified peer, and the peer's information is added to your list of active peers.

### Choice 2: Query Active Peers
What Happens: You can see a list of all peers who have sent you messages.
Next Steps:
- The program waits for 2 seconds to collect responses from peers.
- Displays the list of active peers.
- Result: You see a list of IP addresses and ports of peers who have communicated with you.

### Choice 3: Connect to Peers
What Happens: You can connect to known peers or a new peer.
Next Steps:
- Choose to connect to known peers or a new peer.
- If connecting to known peers, the program will attempt to connect to all peers in your list.
- If connecting to a new peer, enter the new peer's IP address and port number.
- Result: The program sends a connection message to the specified peers, and their information is added to your list of active peers.

### Choice 0: Quit the Program
What Happens: You can exit the chat application.
Next Steps:
- The program sends an "exit" message to all active peers.
- Closes all connections and exits the program.
- Result: All peers are notified of your disconnection, and the program terminates.

## Notes
- Ensure that firewall settings allow communication on the chosen ports.
- This program is for educational purposes and does not include encryption.

## Contributions
Pull requests are welcome! If you find a bug or want to add features, feel free to open an issue.

Happy chatting! 😊


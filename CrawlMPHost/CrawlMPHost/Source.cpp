///Server
#include <iostream>
#include <WS2tcpip.h>

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")
#include <Windows.h>
#include <string>


#define WINVER 0x0500
using namespace std;

int port;


///host {w,a,s,d,q,e}  every player inputs that to

//////////////////////////////////////////////////WARNING THIS IS NOT USED AND IS PURELY FOR DOCUMENTION, CHANGE HEX VALUES IN vk_player ARRAYS to actually change keys used
char player2[6] = { 'T', 'F', 'G', 'H', 'R', 'Y' };
char player3[6] = { 'I', 'J', 'K', 'L', 'U', 'O' };
char player4[6] = { 'Z', 'X', 'C', 'V', 'B', 'N' }; //i just did the bottom keyboard row
//////////////////////////////////////////////////////////////////////////////////////////////////////WARNING END


int recieved[3] = { 0, 0, 0 }; ///this structure is described more in the client file but it is essentially { user_id, keypressed(0-5), 1 or 0 for pressed or released }

WORD vk_player2[6] = { 0x54, 0x46, 0x47, 0x48, 0x52, 0x59 };  //this the hex of the player keys and is what is actually used
WORD vk_player3[6] = { 0x49, 0x4a, 0x4b, 0x4c, 0x55, 0x4f };  //the above array is useless and is mostly for documentation of the keys used
WORD vk_player4[6] = { 0x5a, 0x58, 0x43, 0x56, 0x42, 0x4e };


//key should be 0-5 for the 6 possible keys, id should be 2-4(I used this because 1 would be the host)
void hold_key(int key, int sender_id) {

	INPUT inp;

	// Set up a generic keyboard event.
	inp.type = INPUT_KEYBOARD;
	inp.ki.wScan = 0; // hardware scan code for key
	inp.ki.time = 0;
	inp.ki.dwExtraInfo = 0;

	// Press the _____ key
	// virtual-key code for the ___ key

	if (sender_id == 2) {
		inp.ki.wVk = vk_player2[key];
	}
	else if (sender_id == 3) {
		inp.ki.wVk = vk_player3[key];
	}
	else if (sender_id == 4) {
		inp.ki.wVk = vk_player4[key];
	}

	inp.ki.dwFlags = 0;// 0 for key press
	SendInput(1, &inp, sizeof(INPUT));

}

//key should be 0-5 for the 6 possible keys, id should be 2-4
void release_key(int key, int sender_id) {

	INPUT inp;

	// Set up a generic keyboard event.
	inp.type = INPUT_KEYBOARD;
	inp.ki.wScan = 0; // hardware scan code for key
	inp.ki.time = 0;
	inp.ki.dwExtraInfo = 0;

	// Press the _____ key
	// virtual-key code for the ___ key

	if (sender_id == 2) {
		inp.ki.wVk = vk_player2[key];
	}
	else if (sender_id == 3) {
		inp.ki.wVk = vk_player3[key];
	}
	else if (sender_id == 4) {
		inp.ki.wVk = vk_player4[key];
	}

	// Release the ____ key
	inp.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
	SendInput(1, &inp, sizeof(INPUT));

	return;
}

void host() {
	////////////////////////////////////////////////////////////
	// INITIALIZE WINSOCK
	////////////////////////////////////////////////////////////

	// Structure to store the WinSock version. This is filled in
	// on the call to WSAStartup()
	WSADATA data;

	// To start WinSock, the required version must be passed to
	// WSAStartup(). This server is going to use WinSock version
	// 2 so I create a word that will store 2 and 2 in hex i.e.
	// 0x0202
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		cout << "Can't start Winsock! " << wsOk;
		return;
	}

	////////////////////////////////////////////////////////////
	// SOCKET CREATION AND BINDING
	////////////////////////////////////////////////////////////

	// Create a socket, notice that it is a user datagram socket (UDP)
	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);

	// Create a server hint structure for the server
	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
	serverHint.sin_family = AF_INET; // Address format is IPv4
	serverHint.sin_port = htons(port); // Convert from little to big endian

										// Try and bind the socket to the IP and port
	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		cout << "Can't bind socket! " << WSAGetLastError() << endl;
		return;
	}

	////////////////////////////////////////////////////////////
	// MAIN LOOP SETUP AND ENTRY
	////////////////////////////////////////////////////////////

	sockaddr_in client; // Use to hold the client information (port / ip address)
	int clientLength = sizeof(client); // The size of the client information

	char buf[1024];

	// Enter a loop
	while (true)
	{
		ZeroMemory(&client, clientLength); // Clear the client structure
		ZeroMemory(buf, 1024); // Clear the receive buffer

							   // Wait for message
		int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			cout << "Error receiving from client " << WSAGetLastError() << endl;
			continue;
		}
		
		for (int i = 0; i < 3; i++) {
			recieved[i] = buf[i] - '0';
		}
		
		if (recieved[2] == 1) {
			hold_key(recieved[1], recieved[0]);
		}
		else if (recieved[2] == 0) {
			release_key(recieved[1], recieved[0]);
		}

		// Display message and client info
		char clientIp[256]; // Create enough space to convert the address byte array
		ZeroMemory(clientIp, 256); // to string of characters

								   // Convert from byte array to chars
		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		// Display the message / who sent it
		cout << "Message recv from " << clientIp << " : " << buf << endl;
	}

	// Close socket
	closesocket(in);

	// Shutdown winsock
	WSACleanup();
}

int main() {
	cout << "Please input an open port to use: ";
	cin >> port;
	host();
	return 0;
}

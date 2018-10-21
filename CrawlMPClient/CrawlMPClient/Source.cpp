///Client
///USE UPPER CASE ASCII CHARS

#include <iostream>
#include <WS2tcpip.h>

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")
#include <Windows.h>
#include <string>

using namespace std;

//check if server is up
// so it will send id and rest in int array, first digit is id, second is the key 0-5 for the 6 wasdqe then the third is 1 for pressed and 0 for released, it only sends on key update 
//array would look like { 4, 5, 0};
// this say               id e  release

int port;
int outputs[3] = { 0,0,0 }; // this is the array that will be sent
bool keypresses[6] = { 0,0,0,0,0,0 }; //wasdqe presses
bool oldpresses[6] = { 0,0,0,0,0,0 }; //presses from last frame
const char *host_ip = new char[256];


//this turns the array into a string because I don't know if a normal array can be used on Winsock
string int_array_to_string(int int_array[], int size_of_array) {
	string returnstring = "";
	char buffer[33];
	for (int temp = 0; temp < size_of_array; temp++) {
		returnstring += to_string(int_array[temp]);
	}
	return returnstring;
}


void connect() {
	cout << "Please input an id(2-4, must be different from the other players and the lowest possible): "; 
	cin >> outputs[0];
	Sleep(500);
	cout << "Please input the hosts port: ";
	cin >> port;
	Sleep(500);
	cout << "Please input the hosts IP: ";
	string str_ip;
	cin >> str_ip;
	const char *host_ip = &str_ip[0];  //needs to start as string because it has "."s and cin puts that in string, but it needs to be a char[]* for winsock so I convert is here
	cout << host_ip << endl;
	cout << strlen(host_ip) << endl;


	////////////////////////////////////////////////////////////  I am not knowledgeable about Winsock so the code below is hardly what could be called original
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
	// CONNECT TO THE SERVER
	////////////////////////////////////////////////////////////

	// Create a hint structure for the server
	sockaddr_in server;
	server.sin_family = AF_INET; // AF_INET = IPv4 addresses
	server.sin_port = htons(port); // Little to big endian conversion
	cout << host_ip << endl;
	inet_pton(AF_INET, host_ip, &server.sin_addr); // Convert from string to byte array

													   // Socket creation, note that the socket type is datagram
	SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);
	cout << "Connected" << endl;
	while (true) {
		bool keypresses[6] = { bool(GetAsyncKeyState(int('W'))), bool(GetAsyncKeyState(int('A'))), bool(GetAsyncKeyState(int('S'))), bool(GetAsyncKeyState(int('D'))), bool(GetAsyncKeyState(int('Q'))), bool(GetAsyncKeyState(int('E'))) };
		if (keypresses != oldpresses) {
			for (int i = 0; i <= 5; i = i + 1) {
				if (keypresses[i] != oldpresses[i]) {
					outputs[1] = i;
					outputs[2] = keypresses[i];
					
					// Write out to that socket
					string s = int_array_to_string(outputs, 3);
					int sendOk = sendto(out, s.c_str(), s.size() + 1, 0, (sockaddr*)&server, sizeof(server));

					if (sendOk == SOCKET_ERROR)
					{
						cout << "That didn't work! " << WSAGetLastError() << endl;
						if (WSAGetLastError() == 10049) {
							cout << "The error was due to being unable to send to the server. Post on the GitHub page for more support" << endl;
						}
					}
					oldpresses[i] = keypresses[i];
				}
			}
		}
		Sleep(100);
	}
	// Close the socket
	closesocket(out);

	// Close down Winsock
	WSACleanup();
}


int main() {
	connect();
	cout << int(outputs) << endl;
	system("pause");
	return 0;
}

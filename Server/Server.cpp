#undef UNICODE

#define WIN32_LEAN_AND_MEAN


#include "pch.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ctime>
#include <clocale>
#include <random>
#include <limits.h>
#include <vector>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 120

class message {
private:
	//pola spakowane w string
	std::string date;      //g->dzien-miesiac-rok godzina:minuta#
	std::string operation; //o->operacja# : nawiazanie_polaczenia; zakonczenie_polaczenia; potwierdzenie; losowanie; modulo; mnozenie; dzielenie_calkowite; sortowanie_rosnaco; sortowanie_malejaco
	std::string status;    //s->status#: brak_bledu; niepoprawny_argument; wynik_poza_zakresem; wewnetrzny_blad_serwera
	std::string id;        //i->id# : id = int;
	std::string arg1;      //a->value# value = int
	std::string arg2;      //a->value# value = int
	std::string last;      //ostatni->bool# bool = {tak; nie}
public:
	//wartosci pol
	std::string message_str;
	char date_value[17];   //dzien-miesiac-rok godzina:minuta
	std::string oper_value;
	std::string stat_value;
	int id_value;          
	int arg1_value;        
	int arg2_value;       
	bool last_value;

	message() {}
	message(std::string recv) {
		date.clear();
		operation.clear();
		status.clear();
		id.clear();
		arg1.clear();
		arg2.clear();
		last.clear();
		std::string temp_str;
		int temp = 0;             //wystapienia #, identyfikuje pola
		//przegladanie calej wiadomosci, rozpoznawanie pol
		for (int i = 0; i < recv.size(); i++) {
			//czesc odpowiadajaca polom naglowka
			if (temp <= 3) {
				switch (temp) {
					//pole daty
				case 0: {
					//znak # oznacza koniec pola
					if (recv[i] == '#') {
						//dopisywanie do zmiennej znakow # i NULL
						temp_str = "#";
						date += temp_str;
						temp++;
					}
					//przepisywanie wartosci pola do wystapienia znaku #
					else date += recv[i];
					break;
				}
						//pole operacji
				case 1: {
					if (recv[i] == '#') {
						temp_str = "#";
						operation += temp_str;
						temp++;
					}
					else operation += recv[i];
					break;
				}
						//pole statusu
				case 2: {
					if (recv[i] == '#') {
						temp_str = "#";
						status += temp_str;
						temp++;
					}
					else status += recv[i];
					break;
				}
						//pole identyfikatora
				case 3: {
					if (recv[i] == '#') {
						temp_str = "#";
						id += temp_str;
						temp++;
					}
					else id += recv[i];
					break;
				}
				}
			}
			//czesc odpowiadajaca rescie pol
			else {
				//przy wystapieniu bledu reszta pol nie jest dolaczana
				if (status != "s->brak_bledu#") break;
				//operacje z dwoma polami argumentow
				if ((operation == "o->losowanie#") || (operation == "o->modulo#") || (operation == "o->mnozenie#")
					|| (operation == "o->dzielenie_calkowite#")) {
					switch (temp) {
						//pole argumentu
					case 4: {
						if (recv[i] == '#') {
							temp_str = "#";
							arg1 += temp_str;
							temp++;
						}
						else arg1 += recv[i];
						break;
					}
							//pole argumentu
					case 5: {
						if (recv[i] == '#') {
							temp_str = "#";
							arg2 += temp_str;
							temp++;
						}
						else arg2 += recv[i];
						break;
					}
							//pole ostatni
					case 6: {
						if (recv[i] == '#') {
							temp_str = "#";
							last += temp_str;
							temp++;
						}
						else last += recv[i];
						break;
					}
					}
				}
				//operacje z jednym polem argumentu
				else if ((operation == "o->sortowanie_rosnaco#") || (operation == "o->sortowanie_malejaco#")) {
					switch (temp) {
						//pole argumentu
					case 4: {
						if (recv[i] == '#') {
							temp_str = "#";
							arg1 += temp_str;
							temp++;
						}
						else arg1 += recv[i];
						break;
					}
							//pole ostatni
					case 5: {
						if (recv[i] == '#') {
							temp_str = "#";
							last += temp_str;
							temp++;
						}
						else last += recv[i];
						break;
					}
					}
				}
				else {
					switch (temp) {
						//pole ostatni
					case 4: {
						if (recv[i] == '#') {
							temp_str = "#";
							last += temp_str;
							temp++;
						}
						else last += recv[i];
						break;
					}
					}
				}
			}
		}
		//ustawianie pol wartosci z pol tekstowych
		if ((operation == "o->sortowanie_rosnaco#") || (operation == "o->sortowanie_malejaco#")) {
			std::string temp_str;
			//pole daty
			for (int i = 0; i < 16; i++) date_value[i] = date[i + 3];
			//pole operacji
			oper_value.clear();
			for (int i = 3; i < operation.size() - 1; i++) oper_value += operation[i];
			//pole statusu
			stat_value.clear();
			for (int i = 3; i < status.size() - 1; i++) stat_value += status[i];
			//pole id
			temp_str.clear();
			for (int i = 3; i < id.size() - 1; i++) temp_str += id[i];
			id_value = stoi(temp_str);
			//pole argumentu
			temp_str.clear();
			for (int i = 3; i < arg1.size() - 1; i++) temp_str += arg1[i];
			arg1_value = stoi(temp_str);
			//pole ostatni
			if (last == "ostatni->nie#") last_value = false;
			else last_value = true;
		}
		else if ((operation == "o->potwierdzenie#") || (operation == "o->nawiazanie_polaczenia#") ||
			(operation == "o->zakonczenie_polaczenia#")) {
			std::string temp_str;
			//pole daty
			for (int i = 0; i < 16; i++) date_value[i] = date[i + 3];
			//pole operacji
			oper_value.clear();
			for (int i = 3; i < operation.size() - 1; i++) oper_value += operation[i];
			//pole statusu
			stat_value.clear();
			for (int i = 3; i < status.size() - 1; i++) stat_value += status[i];
			//pole id
			temp_str.clear();
			for (int i = 3; i < id.size() - 1; i++) temp_str += id[i];
			id_value = stoi(temp_str);
		}
		else {
			std::string temp_str;
			//pole daty
			for (int i = 0; i < 16; i++) date_value[i] = date[i + 3];
			//pole operacji
			oper_value.clear();
			for (int i = 3; i < operation.size() - 1; i++) oper_value += operation[i];
			//pole statusu
			stat_value.clear();
			for (int i = 3; i < status.size() - 1; i++) stat_value += status[i];
			//pole id
			temp_str.clear();
			for (int i = 3; i < id.size() - 1; i++) temp_str += id[i];
			id_value = stoi(temp_str);
			//pole argumentu
			temp_str.clear();
			for (int i = 3; i < arg1.size() - 1; i++) temp_str += arg1[i];
			arg1_value = stoi(temp_str);
			//pole argumentu
			temp_str.clear();
			for (int i = 3; i < arg2.size() - 1; i++) temp_str += arg2[i];
			arg2_value = stoi(temp_str);
			//pole ostatni
			if (last == "ostatni->nie#") last_value = false;
			else last_value = true;
		}
	}
	~message() {}

	void get_current_time() {
		time_t czas;
		time(&czas);
		tm czasTM;
		localtime_s(&czasTM, &czas);

		setlocale(LC_ALL, "Polish");
		strftime(date_value, sizeof(date_value), "%d-%m-%Y %H:%M", &czasTM);
	}

	void pack_to_string() {
		//ustawianie pol tekstowych z pol wartosci
		std::string temp_str;
		//pole daty
		date = "g->";
		for (int i = 0; i < 16; i++) date += date_value[i];
		date += "#";
		message_str = date;
		//pole operacji
		operation = "o->" + oper_value + "#";
		//pole statusu
		status = "s->" + stat_value + "#";
		//pole id
		id = "i->" + std::to_string(id_value) + "#";
		if (oper_value == "potwierdzenie") {
			//pole ostatni
			if (last_value) last = "ostatni->tak#";
			else last = "ostatni->nie#";
			//pakowanie pol tekstowych do jednego string'a
			message_str = message_str + operation + status + id + last;
		}
		else {
			//pole argumentu
			arg1 = "a->" + std::to_string(arg1_value) + "#";
			//pole ostatni
			if (last_value) last = "ostatni->tak#";
			else last = "ostatni->nie#";
			//pakowanie pol tekstowych do jednego string'a
			message_str = message_str + operation + status + id + arg1 + last;
		}
	}

};

int main()
{
	WSADATA wsaData;
	SOCKET ClientSocket = INVALID_SOCKET;
	int iResult;
	int session_id = 0;
	std::string recv_str;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// Create a client socket
	ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ClientSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	sockaddr_in service, client;
	service.sin_family = AF_INET;
	service.sin_port = htons(1234);
	service.sin_addr.s_addr = INADDR_ANY;

	char buffer[DEFAULT_BUFLEN];
	int len = sizeof(sockaddr_in);

	// Setup the UDP socket
	iResult = bind(ClientSocket, (sockaddr*)&service, len);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	//oczekiwanie na nawiazanie polaczenia
	iResult = recvfrom(ClientSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&client, &len);
	if (iResult == SOCKET_ERROR) {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	//id dla nowej sesji
	session_id++;
	recv_str = buffer;
	message* request = new message(recv_str);
	std::cout << "Otrzymano zadanie nawiazania polaczenia z godziny: ";
	for (int i = 0; i < 16; i++) {
		std::cout << request->date_value[i];
	}
	std::cout << std::endl;
	//potwierdzenie nawiazania polaczenia
	message* answer = new message();
	answer->get_current_time();
	answer->oper_value = "potwierdzenie";
	answer->stat_value = "brak_bledu";
	answer->id_value = session_id;
	answer->last_value = true;
	answer->pack_to_string();
	strcpy_s(buffer, answer->message_str.c_str());
	iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	printf("Potwierdzono nawiazanie polaczenia z id sesji %d\n", session_id);
	bool end = false;
	do {
		//czekanie na zadanie
		iResult = recvfrom(ClientSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&client, &len);
		if (iResult == SOCKET_ERROR) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		recv_str = buffer;
		request = new message(recv_str);
		if (request->oper_value == "zakonczenie_polaczenia") {
			end = true;
			//wysylanie potwierdzenia odbioru
			answer = new message();
			answer->get_current_time();
			answer->oper_value = "potwierdzenie";
			answer->stat_value = "brak_bledu";
			answer->id_value = session_id;
			answer->last_value = true;
			answer->pack_to_string();
			strcpy_s(buffer, answer->message_str.c_str());
			iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Potwierdzono odbior" << std::endl;
		}
		else if (request->oper_value == "losowanie") {
			std::cout << "Otrzymano zadanie wylosowania liczby z zadanego przedzialu" << std::endl;
			//wysylanie potwierdzenia odbioru
			answer = new message();
			answer->get_current_time();
			answer->oper_value = "potwierdzenie";
			answer->stat_value = "brak_bledu";
			answer->id_value = session_id;
			answer->last_value = true;
			answer->pack_to_string();
			strcpy_s(buffer, answer->message_str.c_str());
			iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Potwierdzono odbior" << std::endl;
			//odpowiedz na zadanie
			answer = new message();
			std::default_random_engine gen;
			std::uniform_int_distribution<int> dist(request->arg1_value, request->arg2_value);
			answer->arg1_value = dist(gen);
			answer->get_current_time();
			answer->oper_value = "losowanie";
			answer->stat_value = "brak_bledu";
			answer->id_value = session_id;
			answer->last_value = true;
			answer->pack_to_string();
			strcpy_s(buffer, answer->message_str.c_str());
			iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Wyslano wynik losowania" << std::endl;
		}

		else if (request->oper_value == "modulo") {
			std::cout << "Otrzymano zadanie operacji modulo" << std::endl;
			//wysylanie potwierdzenia odbioru
			answer = new message();
			answer->get_current_time();
			answer->oper_value = "potwierdzenie";
			answer->stat_value = "brak_bledu";
			answer->id_value = session_id;
			answer->last_value = true;
			answer->pack_to_string();
			strcpy_s(buffer, answer->message_str.c_str());
			iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Potwierdzono odbior" << std::endl;
			//odpowiedz na zadanie
			answer = new message();
			answer->arg1_value = request->arg1_value % request->arg2_value;
			answer->get_current_time();
			answer->oper_value = "modulo";
			answer->stat_value = "brak_bledu";
			answer->id_value = session_id;
			answer->last_value = true;
			answer->pack_to_string();
			strcpy_s(buffer, answer->message_str.c_str());
			iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Wyslano wynik operacji modulo" << std::endl;

		}

		else if (request->oper_value == "mnozenie") {
			std::cout << "Otrzymano zadanie operacji mnozenia" << std::endl;
			//wysylanie potwierdzenia odbioru
			answer = new message();
			answer->get_current_time();
			answer->oper_value = "potwierdzenie";
			answer->stat_value = "brak_bledu";
			answer->id_value = session_id;
			answer->last_value = true;
			answer->pack_to_string();
			strcpy_s(buffer, answer->message_str.c_str());
			iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Potwierdzono odbior" << std::endl;
			//odpowiedz na zadanie
			//wynik poza zakresem
			if (((request->arg1_value == -1) && (request->arg2_value == INT_MIN)) ||
				((request->arg2_value == -1) && (request->arg1_value == INT_MIN)) ||
				(request->arg1_value > INT_MAX / request->arg2_value) ||
				(request->arg1_value < INT_MIN / request->arg2_value)) {
				answer = new message();
				answer->get_current_time();
				answer->oper_value = "mnozenie";
				answer->stat_value = "wynik_poza_zakresem";
				answer->id_value = session_id;
				answer->last_value = true;
				answer->pack_to_string();
				strcpy_s(buffer, answer->message_str.c_str());
				iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				std::cout << "Wystapil blad, operacja nie moze zostac zakonczona poprawnie" << std::endl;
			}
			//dane poprawne, liczenie wyniku
			else {
				answer = new message();
				answer->get_current_time();
				answer->oper_value = "mnozenie";
				answer->stat_value = "brak_bledu";
				answer->id_value = session_id;
				int resoult = request->arg1_value * request->arg2_value;
				answer->arg1_value = resoult;
				answer->last_value = true;
				answer->pack_to_string();
				strcpy_s(buffer, answer->message_str.c_str());
				iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				std::cout << "Wyslano wynik operacji mnozenia" << std::endl;
			}
		}

		else if (request->oper_value == "dzielenie_calkowite") {
			std::cout << "Otrzymano zadanie operacji dzielenia calkowitego" << std::endl;
			//wysylanie potwierdzenia odbioru
			answer = new message();
			answer->get_current_time();
			answer->oper_value = "potwierdzenie";
			answer->stat_value = "brak_bledu";
			answer->id_value = session_id;
			answer->last_value = true;
			answer->pack_to_string();
			strcpy_s(buffer, answer->message_str.c_str());
			iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Potwierdzono odbior" << std::endl;
			//odpowiedz na zadanie
			//bledny argument
			if (request->arg2_value == 0) {
				answer = new message();
				answer->get_current_time();
				answer->oper_value = "mnozenie";
				answer->stat_value = "niepoprawny_argument";
				answer->id_value = session_id;
				answer->last_value = true;
				answer->pack_to_string();
				strcpy_s(buffer, answer->message_str.c_str());
				iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				std::cout << "Wystapil blad, operacja nie moze zostac zakonczona poprawnie" << std::endl;
			}
			//dane poprawne, liczenie wyniku
			else {
				answer = new message();
				answer->get_current_time();
				answer->oper_value = "dzielenie_calkowite";
				answer->stat_value = "brak_bledu";
				answer->id_value = session_id;
				int resoult = request->arg1_value / request->arg2_value;
				answer->arg1_value = resoult;
				answer->last_value = true;
				answer->pack_to_string();
				strcpy_s(buffer, answer->message_str.c_str());
				iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				std::cout << "Wyslano wynik operacji dzielenia calkowitego" << std::endl;
			}
		}

		else if (request->oper_value == "sortowanie_rosnaco") {
			std::vector<int> numbers;
			std::cout << "Otrzymano zadanie operacji sortowania rosnaca" << std::endl;
			//potwierdzanie i odbieranie kolejnych liczb
			numbers.push_back(request->arg1_value);
			do {
				//wysylanie potwierdzenia odbioru
				answer = new message();
				answer->get_current_time();
				answer->oper_value = "potwierdzenie";
				answer->stat_value = "brak_bledu";
				answer->id_value = session_id;
				answer->last_value = true;
				answer->pack_to_string();
				strcpy_s(buffer, answer->message_str.c_str());
				iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				std::cout << "Potwierdzono odbior" << std::endl;
				iResult = recvfrom(ClientSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&client, &len);
				if (iResult == SOCKET_ERROR) {
					printf("recv failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				recv_str = buffer;
				request = new message(recv_str);
				numbers.push_back(request->arg1_value);
			} while (!request->last_value);
			//wysylanie potwierdzenia odbioru ostatniej liczby
			answer = new message();
			answer->get_current_time();
			answer->oper_value = "potwierdzenie";
			answer->stat_value = "brak_bledu";
			answer->id_value = session_id;
			answer->last_value = true;
			answer->pack_to_string();
			strcpy_s(buffer, answer->message_str.c_str());
			iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			//sortowanie oraz wysylanie wyniku
			std::sort(numbers.begin(), numbers.end(), [](const int & a, const int & b) {return a < b; });
			std::vector<int>::iterator it = numbers.begin();
			while (it != numbers.end()) {
				//wysylanie pojedynczej liczby
				answer = new message();
				answer->get_current_time();
				answer->oper_value = "sortowanie_rosnaco";
				answer->stat_value = "brak_bledu";
				answer->id_value = session_id;
				answer->arg1_value = *it;
				std::cout << answer->arg1_value << std::endl;
				if (it == numbers.end() - 1) answer->last_value = true;
				else answer->last_value = false;
				answer->pack_to_string();
				strcpy_s(buffer, answer->message_str.c_str());
				iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				//oczekiwanie potwierdzenia odbioru
				iResult = recvfrom(ClientSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&client, &len);
				if (iResult == SOCKET_ERROR) {
					printf("recv failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				recv_str = buffer;
				request = new message(recv_str);
				it++;
				std::cout << "otrzymano potwierdzenie" << std::endl;
			}
			std::cout << "Wyslano wynik sortowania rosnaco" << std::endl;
		}

		else if (request->oper_value == "sortowanie_malejaco") {
			std::vector<int> numbers;
			std::cout << "Otrzymano zadanie operacji sortowania malejaco" << std::endl;
			//potwierdzanie i odbieranie kolejnych liczb
			numbers.push_back(request->arg1_value);
			do {
				//wysylanie potwierdzenia odbioru
				answer = new message();
				answer->get_current_time();
				answer->oper_value = "potwierdzenie";
				answer->stat_value = "brak_bledu";
				answer->id_value = session_id;
				answer->last_value = true;
				answer->pack_to_string();
				strcpy_s(buffer, answer->message_str.c_str());
				iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				std::cout << "Potwierdzono odbior" << std::endl;
				iResult = recvfrom(ClientSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&client, &len);
				if (iResult == SOCKET_ERROR) {
					printf("recv failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				recv_str = buffer;
				request = new message(recv_str);
				numbers.push_back(request->arg1_value);
			} while (!request->last_value);
			//wysylanie potwierdzenia odbioru ostatniej liczby
			answer = new message();
			answer->get_current_time();
			answer->oper_value = "potwierdzenie";
			answer->stat_value = "brak_bledu";
			answer->id_value = session_id;
			answer->last_value = true;
			answer->pack_to_string();
			strcpy_s(buffer, answer->message_str.c_str());
			iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Potwierdzono odbior" << std::endl;
			//sortowanie oraz wysylanie wyniku
			std::sort(numbers.begin(), numbers.end(), [](const int & a, const int & b) {return a > b; });
			std::vector<int>::iterator it = numbers.begin();

			while (it != numbers.end()) {
				//wysylanie pojedynczej liczby
				answer = new message();
				answer->get_current_time();
				answer->oper_value = "sortowanie_malejaco";
				answer->stat_value = "brak_bledu";
				answer->id_value = session_id;
				answer->arg1_value = *it;
				if (it == numbers.end() - 1) answer->last_value = true;
				else answer->last_value = false;
				answer->pack_to_string();
				strcpy_s(buffer, answer->message_str.c_str());
				iResult = sendto(ClientSocket, buffer, answer->message_str.size(), 0, (sockaddr*)&client, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				//oczekiwanie potwierdzenia odbioru
				iResult = recvfrom(ClientSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&client, &len);
				if (iResult == SOCKET_ERROR) {
					printf("recv failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				recv_str = buffer;
				request = new message(recv_str);
				it++;
			}
			std::cout << "Wyslano wynik sortowania malejaco" << std::endl;
		}

		if ((answer->oper_value != "sortowanie_rosnaco") && (answer->oper_value != "sortowanie_malejaco")
			&& (request->oper_value != "zakonczenie_polaczenia")) {
			//oczekiwanie potwierdzenia odbioru
			iResult = recvfrom(ClientSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&client, &len);
			if (iResult == SOCKET_ERROR) {
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			recv_str = buffer;
			request = new message(recv_str);
			if (request->oper_value == "potwierdzenie") std::cout << "Otrzymano potwierdzenie odbioru" << std::endl;
		}
	} while (!end);

	printf("Connection closed\n");

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}

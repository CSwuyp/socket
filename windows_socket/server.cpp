// server.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <Winsock2.h>	//Socket�ĺ������á�
#include <windows.h>

#define BUF_SIZE 6400  //  ��������С 

#pragma comment (lib, "ws2_32")		// ʹ��WINSOCK2.Hʱ������Ҫ���ļ�WS2_32.LIB

DWORD WINAPI Rcv( LPVOID lpParam )
{
	SOCKET sClient = *(SOCKET*)lpParam;
	int retVal;
	char bufRecv[BUF_SIZE]; 
	memset( bufRecv, 0, sizeof( bufRecv ) );
	while(1)
	{
		retVal = recv( sClient, bufRecv, BUF_SIZE, 0 );
		if ( retVal == SOCKET_ERROR ) {
			printf( "recive faild!\n" );
			break;
		} else {
			printf( "�յ��ͻ�����Ϣ��%s\n", bufRecv );
		} 
	}
	return 0;
}

DWORD WINAPI Snd( LPVOID lpParam )
{
	SOCKET sClient = *(SOCKET*)lpParam;
	int retVal;
	char bufSend[BUF_SIZE]; 
	memset( bufSend, 0, sizeof( bufSend ) );
	while(1)
	{
		gets( bufSend );
		retVal = send( sClient, bufSend, strlen(bufSend)+sizeof(char), 0 );
		if ( retVal == SOCKET_ERROR ) {
			printf( "send faild!\n" );
			break;
		} 
	}
	return 0;
}


int main(int argc, char* argv[])
{
	// ��ʼ���׽��ֶ�̬��
	WSADATA wsaData;
	if ( WSAStartup(MAKEWORD(2, 2), &wsaData) != 0 ) {
		printf( "winsock load faild!\n" );
		return 1;
	}
	
	//  ����������׽��� 
	SOCKET sServer = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( sServer == INVALID_SOCKET ) {
		printf( "socket faild!\n" );
		WSACleanup();
		return -1;
	}
	
	//  ����˵�ַ 
	sockaddr_in addrServ;

	addrServ.sin_family = AF_INET; 
	addrServ.sin_port = htons( 9999 );
	addrServ.sin_addr.s_addr = htonl( INADDR_ANY ); 
	
	//  ���׽��� 
	if ( bind( sServer, ( const struct sockaddr* )&addrServ, sizeof(addrServ) ) == SOCKET_ERROR ) {
		printf( "bind faild!\n" );
		closesocket( sServer );
		WSACleanup(); 
		return -1;
	} 
	
	printf("Server is On IP:[%s],port:[%d]\n",inet_ntoa(addrServ.sin_addr),ntohs(addrServ.sin_port)); 
	
	//  �����׽���  ���ֱ�ʾ����ܼ����ͻ����� 
	if ( listen( sServer, 5 ) == SOCKET_ERROR ) {
		printf( "listen faild!\n" );
		closesocket( sServer );
		WSACleanup();
		return -1; 
	} 
	
	SOCKET sClient; //  �ͻ����׽���
	
	sockaddr_in addrClient;
	int addrClientLen = sizeof( addrClient );
	
	
	sClient = accept( sServer, ( sockaddr FAR* )&addrClient, &addrClientLen ); 
	if ( sClient == INVALID_SOCKET ) {
		printf( "accept faild!\n" );
		closesocket( sServer );
		WSACleanup();
		return -1; 
	}
	printf("accepted client IP:[%s],port:[%d]\n",inet_ntoa(addrClient.sin_addr),ntohs(addrClient.sin_port));
	
	HANDLE hThread1, hThread2;
	DWORD dwThreadId1, dwThreadId2;

	hThread1 = ::CreateThread(NULL, NULL, Snd, (LPVOID*)&sClient, 0, &dwThreadId1);
	hThread2 = ::CreateThread(NULL, NULL, Rcv, (LPVOID*)&sClient, 0, &dwThreadId2);

	::WaitForSingleObject(hThread1, INFINITE);
	::WaitForSingleObject(hThread2, INFINITE);
	::CloseHandle(hThread1);
	::CloseHandle(hThread2);
		
	closesocket( sClient ); 
	WSACleanup(); // ��Դ�ͷ�

	return 0;
}


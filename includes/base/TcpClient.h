#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

class TcpSocket;

class TcpClient
{
public:
	
	static TcpSocket*	Connect(const char* host, int nPort, int nTimeoutMs);
	
private:
	
	TcpClient();
	
	~TcpClient();
};

#endif // TCP_CLIENT_H
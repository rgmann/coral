#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netinet/in.h>
#include <sys/select.h>

class TcpSocket;

class TcpServer
{
public:
	
	TcpServer();
	
	~TcpServer();
	
	bool	start(int nPort);
	
	void	stop();
	
	TcpSocket*	acceptClient(int nTimeoutMs);
	
private:
	
	int		m_fdListenSocket;
	
	int		m_nPort;
	
	fd_set	m_ServerSet;
};

#endif // TCP_SERVER_H


#include	"unp.h"
#include	<time.h>

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				buff[MAXLINE];
	time_t				ticks;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	// INADDR_ANY 表示服务器进程可以在任意一个网络接口上（当然前提是服务器有多个网络接口）接受客户端连接
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(13);	/* daytime server */

	// bind 将服务器的 well-known port（这里是 13）绑定到上面创建的套接字上
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	// 将套接字转换为监听套接字，如此来自客户的连接可以在该套接字上由内核接受
	// LISTENQ 来自 unp.h，定义了在监听描述符（listenfd）上排队的最大客户连接数
	Listen(listenfd, LISTENQ);

	// 无限循环
	for ( ; ; ) {
		// 服务器进程调用 accept 后即被睡眠，等待客户端连接的到达并被内核接受
		// 细节是，TCP 三路握手之后，accept 返回已连接描述符（connfd），服务器通过 connfd 与客户端通信
		connfd = Accept(listenfd, (SA *) NULL, NULL);

        ticks = time(NULL);
		// snprintf函数在这个字符串末尾添加一个回车符和一个回行符
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));

		// 服务端主动断开和客户端的连接
		Close(connfd);
	}
}

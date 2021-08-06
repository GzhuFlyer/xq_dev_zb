#ifndef SERVER_HPP
#define SERVER_HPP


#define ERR_EXIT(M) \
        do  \
        {   \
            perror(M);  \
            exit(EXIT_FAILURE); \
        }while(0)
void SetServerWriteConnFd(int conn);
void startTcpServer(void);

#endif
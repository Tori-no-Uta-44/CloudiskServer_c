#include <openssl/sha.h>
#include <openssl/md5.h>
#include <func.h>
#include "thread_pool.h"
#define BUFFER_SIZE 1024

void calculate_md5(const char *filename, unsigned char *md5_sum);    //计算文件md5 
void recvMd5(int clientfd,unsigned char *md5_sum);
void sendMd5(int sockfd,const char* filename);
void check(const char* filename,unsigned char *md5_sum2);

void calculate_sha256(const char *filename, unsigned char *output_hash);
void recv_sha256(int clientfd,unsigned char *sha_256);
void send_sha256(int sockfd,const char* filename);
void check_sha256(unsigned char *sha_curr,unsigned char *sha_recv);
void hash_to_hex_string(const unsigned char *hash, char *hex_string);



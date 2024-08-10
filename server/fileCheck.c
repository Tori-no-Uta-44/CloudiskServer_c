#include "fileCheck.h"
#define BUFFER_SIZE 1024

void calculate_md5(const char *filename, unsigned char *md5_sum)    //计算文件md5 
{
    MD5_CTX md5_ctx;
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes;
    FILE *file;

    MD5_Init(&md5_ctx);
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        MD5_Update(&md5_ctx, buffer, bytes);
    }
    MD5_Final(md5_sum, &md5_ctx);
    fclose(file);
}

void recvMd5(int clientfd,unsigned char *md5_sum)
{
    //接收对端发送的md5码
    int ret = recvn(clientfd,md5_sum,MD5_DIGEST_LENGTH);
    if(ret<0)
    {
        error(1,errno,"recv md5sum\n");
    }
}
void sendMd5(int sockfd,const char* filename)
{
    unsigned char md5_sum[MD5_DIGEST_LENGTH];
    calculate_md5(filename, md5_sum);
    int ret = sendn(sockfd,&md5_sum,MD5_DIGEST_LENGTH);
    if(ret<0)
    {
        error(1,errno,"send md5sum\n");
    }
}
void check(const char* filename,unsigned char *md5_sum2)
{
    //获取当前文件的md5码
    unsigned char md5_sum1[MD5_DIGEST_LENGTH];
    calculate_md5(filename, md5_sum1);
    //比对两个md5码的值
    if (memcmp(md5_sum1, md5_sum2, MD5_DIGEST_LENGTH) == 0) {
        printf("两个文件的 MD5 校验和匹配。\n");
    } else {
        printf("两个文件的 MD5 校验和不匹配。\n");
    }
}

void calculate_sha256(const char *filename, unsigned char *output_hash) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    unsigned char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) != 0) {
        SHA256_Update(&sha256, buffer, bytes_read);
    }

    SHA256_Final(output_hash, &sha256);
    fclose(file);
}
void send_sha256(int sockfd,const char *filename)
{
    unsigned char buffer[SHA256_DIGEST_LENGTH];
    calculate_sha256(filename, buffer);
    int ret = sendn(sockfd,&buffer,SHA256_DIGEST_LENGTH);
    if(ret<0)
    {
        error(1,errno,"send SHA_256\n");
    }

}
void recv_sha256(int clientfd,unsigned char *sha_256)
{

    //接收对端发送的md5码
    int ret = recvn(clientfd,sha_256,SHA256_DIGEST_LENGTH);
    if(ret<0)
    {
        error(1,errno,"recv SHA_256\n");
    }
}
void check_sha256(unsigned char* sha_curr,unsigned char* sha_recv)
{
    //比对两个md5码的值
    if (memcmp(sha_curr, sha_recv, SHA256_DIGEST_LENGTH) == 0) {
        printf("两个文件的 SHA256 校验和匹配。\n");
    } else {
        printf("两个文件的 SHA256 校验和不匹配。\n");
    }
}
void hash_to_hex_string(const unsigned char *hash, char *hex_string) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hex_string + (i * 2), "%02x", hash[i]);
    }
    hex_string[SHA256_DIGEST_LENGTH * 2] = '\0'; // 确保字符串以 NULL 结尾
}

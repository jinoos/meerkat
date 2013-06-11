#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <event2/buffer.h>

long gettime()
{
    struct timeval mytime;
    gettimeofday(&mytime, NULL);
    
    return (mytime.tv_sec * 1000000) + mytime.tv_usec;
}

typedef struct
{
    int loop;
} thread_data;

void run(void *arg);

int main(int argc, char **argv)
{
    int all_count = 1000 * 100;

    int thread_count = 10;

    pthread_t p_thread[thread_count];

    thread_data data;
    data.loop = (int) all_count / thread_count;

    printf("Each thread will execute %d items\n", data.loop);

    int thread_id;
    int status;

    int i;
    for(i = 0; i < thread_count; i++)
    {
        thread_id = pthread_create(&p_thread[i], NULL, run, &data);
        printf("Started thread #%d\n", i);
    }

    sleep(10000);

    return 0;
}

void run(void *arg)
{
    thread_data *data = (thread_data*) arg;

    int client_len;
    int client_sockfd;

    FILE *fp_in;
    char buf_in[255];
    char buf_get[255];

    struct sockaddr_in clientaddr;

    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    clientaddr.sin_family = AF_INET;
    //clientaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientaddr.sin_addr.s_addr = inet_addr("192.168.122.6");
    clientaddr.sin_port = htons(7758);

    client_len = sizeof(clientaddr);

    if (connect(client_sockfd, (struct sockaddr *)&clientaddr, client_len) < 0)
    {
        perror("Connect error: ");
        exit(0);
    }

    struct evbuffer *buf = evbuffer_new();
    int len;
    char *str, *str2, *str3;

    long start, end;
    long start2, end2;

    char services[] = "*GET SERVICES\r\n";

    int i, j, count;
    long all_time = 0;

    start = gettime();

//    printf("Client starting loop : %d\n", data->loop);

    for(j = 0; j < data->loop; j++)
    {
        memset(buf_in, 0, 255);

        start2 = gettime();

        write(client_sockfd, services, strlen(services));

        while( (str = evbuffer_readln(buf, NULL, EVBUFFER_EOL_CRLF)) == NULL)
        {
            len = read(client_sockfd, buf_get, 254);
            evbuffer_add(buf, buf_get, len);
        }

//        printf("%s\n", str);
        free(str);

        while( (str = evbuffer_readln(buf, NULL, EVBUFFER_EOL_CRLF)) == NULL)
        {
            len = read(client_sockfd, buf_get, 254);
            evbuffer_add(buf, buf_get, len);
        }

//        printf("%s\n", str);
        count = atoi(str);
        free(str);

        for(i=0; i<count; i++)
        {
            while( (str = evbuffer_readln(buf, NULL, EVBUFFER_EOL_CRLF)) == NULL)
            {
                len = read(client_sockfd, buf_get, 254);
                evbuffer_add(buf, buf_get, len);
            }

//            printf("%s\n", str);
            free(str);
        }

        all_time += gettime() - start2;

    }
    end = gettime();
    printf("Received : %lusec %luusec REQ/sec : %.2f\n", (end - start)/1000000, (end-start)%1000000, (((double)(data->loop)/(double)(end - start))*1000000));
    printf("All processed time : %lu, REQ/sec(process time) : %lu\n", all_time, (long) all_time / j);

    close(client_sockfd);

    return;
}

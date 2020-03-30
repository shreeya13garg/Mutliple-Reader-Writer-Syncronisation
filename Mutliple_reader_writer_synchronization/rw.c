#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <string.h>

int volatile count;
sem_t mutex, wrt;
char volatile original[500];
void *Reader(void *arguments);
void *Writer(void *arguments);
struct Node
{
        volatile char msg[500];
        struct SharedQue *link;
};

struct  SharedQue
{
        struct Node *front;
        struct Node *rear;
        int volatile size;
};

void Printall(struct SharedQue *q)
{
        struct Node *temp = q->front;
        printf("\n successfully added\n");
        /*printf("\n Queue contents are :\n");
        while (temp != NULL)
        {
                printf("%s", temp->msg);
                temp = temp->link;
        }
        */
}
void Printread(struct SharedQue *q)
{
        struct Node *temp = q->front;
        if(temp==NULL)
        {
                printf("\n Queue is empty :\n");
        }
        printf("\n Queue contents are :\n");
        while (temp != NULL)
        {
                printf("%s\n", temp->msg);
                temp = temp->link;
        }
}

void enqueue(struct SharedQue *q, char values[500])
{
        struct Node *temp = (struct Node*)malloc(sizeof(struct Node));
        strcpy(temp->msg, values);
        temp->link = NULL;
        if (q->rear == NULL)
        {
                q->front = q->rear = temp;
                q->size = q->size + 1;
                Printall(q);
                return;
        }

        q->rear->link = temp;
        q->rear = temp;
        q->size = q->size + 1;
        Printall(q);
}
struct SharedQue *New()
{
        struct SharedQue *q = (struct SharedQue*)malloc(sizeof(struct SharedQue));
        q->front = q->rear = NULL;
        q->size = 0;
        return q;
}

struct SharedQue *q;
int main()
{
        q = New();
        int i=0;
        int writer_num = 0;
        int reader_num=0;
        sem_init(&wrt, 0,1);
        sem_init(&mutex, 0,1);
        pthread_t readers_all[50], writers_all[50];
        printf("\nEnter number of writers thread (max-50) : ");
        if (scanf("%d", &writer_num) <= 0 || writer_num < 1 || writer_num > 50)
        {
                printf("Error:No negative numbers\n");
                return 0;
        }

        printf("\nEnter number of readers thread (max-50) : ");
        if (scanf("%d", &reader_num) <= 0 || reader_num < 1 || reader_num > 50)
        {
                printf("Error:Only positive numbers\n");
                return 0;
        }

        for(int j = 0; j < writer_num; j++)
                pthread_create(&writers_all[j], NULL, Writer, (void *)j);
        for(i = 0; i < reader_num; i++)
                pthread_create(&readers_all[i], NULL, Reader, (void *)i);

        

        for(int k= 0; k < writer_num; k++)
                pthread_join(writers_all[k], NULL);

        for(int l = 0; l < reader_num; l++)
                pthread_join(readers_all[l], NULL);

        sem_destroy(&wrt);
        sem_destroy(&mutex);
        printf("\n");
        return 0;
}

void *Reader(void *arguments)
{
        
                sleep(rand()%5+1);

                int reader_id = (int)arguments;
                //printf("Reader: %d", reader_id);
                sem_wait(&mutex);
                count++;
                if(count == 1)
                {
                        
                        printf("\nReader waiting for the writer to write\n");
                        sem_wait(&wrt);
                        
                        
                }

                sem_post(&mutex);
                printf("Reader Entered: %d", reader_id);
                Printread(q);
                sem_wait(&mutex);
                count--;
                if(count == 0)
                        printf("\nrelasing write lock\n");
                        sem_post(&wrt);
                        
                sem_post(&mutex);

        
}

void *Writer(void *arguments)
{
       
                sleep(rand()%5+1);
                int writer_id = (int)arguments;
                // printf("Writer: %d", writer_id);
                sem_wait(&wrt);
                char msg[100]="shreeya";
                //printf("\nEnter message : ");
                //printf(msg);
                printf("Writer Entered: %d", writer_id);
                enqueue(q, msg);
                strcpy(original, msg);
                sem_post(&wrt);

        
}
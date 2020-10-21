#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
int MAX;  // Количество элементов в массиве
int THREAD_MAX; //количество потоков
int part=0; //переменная, которая будет хранить количество потоков
int* a ;    //указатель на массив для хранения элементов

void merge(int low, int mid, int high)
{
    int n1 = mid - low + 1, nr = high - mid, i, j;
    int* left = calloc(n1,sizeof(int));
    int* right = calloc(nr,sizeof(int));
    
    for(i = 0; i < n1; i++)
        left[i] = a[i + low];
    
    for(i = 0; i < nr; i++)
        right[i] = a[i + mid + 1];
    
    int k = low;
    i = j = 0;
    
    // объединяем левую и правую половины в порядке возрастания
    while(i < n1 && j < nr)
    {
        if(left[i] <= right[j])
            a[k++] = left[i++];
        else
            a[k++] = right[j++];
    }
    
    // вставляем оставшиеся значения из левой половины
    while(i < n1) {
        a[k++] = left[i++];
    }
    
    // вставляем оставшиеся значения из правой половины
    while(j < nr) {
        a[k++] = right[j++];
    }
}

void merge_sort(int low, int high)
{
    // вычисление средней точки массива
    int mid = low + ((high - low) / 2);
    if(low < high) {
        //сортировка первой половины
        merge_sort(low, mid);
        // сортировка второй половины
        merge_sort(mid + 1, high);
        // объединение половинок
        merge(low, mid, high);
    }
}

//функция сортировки с использованием потоков
void* merge_sort_tread()
{
    //получает идентификатор потока
    int thread_part = part;
    part+=1;
    // вычисляем нижнюю и верхнюю границу массива
    int low = thread_part * (MAX / THREAD_MAX);
    int high = (thread_part + 1) * (MAX / THREAD_MAX) - 1;
    //   обновление средней точки
    int mid = low + (high - low) / 2;
    if(low < high)
    {
        merge_sort(low, mid);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }
}

void merge_rec(int tread_m)//рекурсивная функция сбора данных после отработки потоков
{
    if(tread_m>THREAD_MAX)
        return;
    merge_rec(tread_m*2);
    int minsize = MAX/tread_m;
    for(int i = 0;i < tread_m; i++)
    {
        int low = i * minsize;
        int high = (i + 1) * (minsize) - 1;
        int mid = low + (high - low) / 2;
        merge(low, mid, high);
    }
    
}

int main(int argc, char* argv[])
{
    MAX = atoi(argv[1]);
    THREAD_MAX = MAX/ 2;
    a = calloc(MAX,sizeof(int));//выделение памяти под левую половину
    printf("Входной массив: \n");
    for(int i = 0; i < MAX; i++) {
        scanf("%d", &a[i]);
    }
    pthread_t threads[THREAD_MAX];//создаем массив идентификаторов потока
    int status;
    for(int i = 0; i < THREAD_MAX; i++)
    {
        printf("part- %d\n",part);
        status = pthread_create(&threads[i], NULL, merge_sort_tread, NULL);
        if (status != 0)
            printf("main error: can't create thread, status = %d\n", status);
        part++;
    }
    for(int i = 0; i < THREAD_MAX; i++)
        pthread_join(threads[i], NULL);
    merge_rec(2);
    merge(0, (MAX - 1)/2, MAX - 1);
    printf("Выходной массив: \n");
    for(int i = 0; i < MAX; i++)
        printf("%d\n", a[i]);
    printf("Количество потоков: %d\n",THREAD_MAX);
    return 0;
}

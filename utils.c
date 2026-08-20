#include <sys/time.h>
#include <stddef.h>
#include <stdio.h>
#include "codexion.h"


long long get_current_time_ms(void)
{
    struct timeval  tv;
    long long       time_ms;

    if (gettimeofday(&tv, NULL) == -1)
        return (-1);

    time_ms = (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
    return (time_ms);
}
int main()
{
    long long i = get_current_time_ms();
    printf("%lld", i);
}

#include <signal.h>
#include <unistd.h>

static void dothis(int signum) {
    write(1, "h", 1);
    alarm(5);
}


int main()
{
    struct sigaction psa;
    void (*func_ptr)(int) = &dothis;
    psa.sa_handler = func_ptr;
    psa.sa_flags = 0;
    sigemptyset(&psa.sa_mask);

    sigaction(SIGALRM, &psa, NULL);
    alarm(10);
    return 0;
}


__attribute__ ((section(".text.start")))
void _start()
{
    main();
}

#include <syslog.h>

int main()
{
    openlog("listener", LOG_PID, LOG_LOCAL0 );
    syslog(LOG_LOCAL0  |  LOG_NOTICE, "informations");
    closelog();
    return 0;
}

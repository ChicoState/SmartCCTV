#include <string>    /* for std::string */
#include <mqueue.h>  /* for mqd_t, mq_open(), mq_send(), mq_close() */
#include <syslog.h>  /* for syslog() */
#include "write_message.h"

using std::string;

// You can change this to make the syslog() output to a different file.
#define log_facility LOG_LOCAL0


bool write_message(const string& message)
{
    mqd_t message_writer;  // message queue file descriptor
    string message_handler_name ="/SmartCCTV_Message_handler";

    if ( (message_writer = mq_open(message_handler_name.c_str(), O_WRONLY | O_NONBLOCK)) == -1)
    {
        // failed to open the message queue
        syslog(log_facility | LOG_ERR, "Failed to open %s", message_handler_name.c_str());
        return false;
    }

    if (mq_send(message_writer, message.c_str(), message.length(), 0) == -1)
    {
        // failed to send the message
        syslog(log_facility | LOG_ERR, "Failed to send the message: %s", message.c_str());
        return false;
    }
    
    mq_close(message_writer);
    return true;
}


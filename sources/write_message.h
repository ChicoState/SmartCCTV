#ifndef WRITE_MESSAGE_H
#define WRITE_MESSAGE_H

#include <string>  /* for std::string */

/**
 * This function writes an error message to the GUI process, which is then displayed
 * in the label for error messages.
 * The purpose of this function is to let the user know if there is an error in the daemon
 * or any other backend low level part of the SmartCCTV system.
 *
 * @param const string& message - The message to write to the screen.
 *
 * @return bool - true  if it wrote that message succesfully.
 *                false if it failed to write that message.
 */
bool write_message(const std::string& message);

#endif  /* WRITE_MESSAGE_H */


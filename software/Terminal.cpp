/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Terminal.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/time.h>

/////////////////////////////////////////////////////////////////////////////
// Global Functions
/////////////////////////////////////////////////////////////////////////////

// Returns true if the user presses a key in the terminal
bool kbhit()
{
    struct timeval tv;
    fd_set fds;
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    
    return FD_ISSET(STDIN_FILENO, &fds);
}

// Set the terminal blocking mode. If blocking is enabled, then
// enable canonical mode, otherwise disable canonical mode.
//
// Canonical mode waits for the user to press enter
void setblocking(bool blocking)
{
    struct termios ttystate;
 
    // get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
 
    if (blocking)
    {
        // turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    else
    {
        // turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        // minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// If echo is true, then enable echo when entering
// text input on the console
void setecho(bool echo)
{
    struct termios ttystate;
 
    // get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
 
    if (echo)
    {
        // turn on echo
        ttystate.c_lflag |= ECHO;
    }
    else
    {
        // turn off echo
        ttystate.c_lflag &= ~ECHO;
    }
    
    // set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

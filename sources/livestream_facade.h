/**
 * File Name:   livestream_facade.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  5/16/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/17/20
 *
 * Description:
 * This file contains the declaration of the class LiveStream_facade, as well as it's helper functions.
 * The helper functions are primarily signal handlers and other functions that are caled by the signal
 * handlers, since such functions cannot be member methods of a class.
 * LiveStream_facade is an implementation of both the facade and singleton design patterns.
 */

#ifndef LIVESTREAM_FACADE_H
#define LIVESTREAM_FACADE_H

#include <string>

using std::string;

// You can change this to make the syslog() output to a different file.
#define log_facility LOG_LOCAL0


struct LiveStream_viewer_data;

class LiveStream_facade {
  public:
    /**
     * The constructor is run in the GUI process only.
     * It it is supposed to initialize all private data members in the class.
     * In reality, most of them have been moved into the global struct liveStream_viewer_data
     * becase they need to be accessed by the signal handlers, which cannot be member functions.
     */
    LiveStream_facade();

    /**
     * This function starts up the livestream viewer process if it is not already running.
     * This function is called in the GUI process only.
     *
     * @param const string& home_directory - The livestream viewer process gets the location of the home
     *                                       directory from the GUI process.
     *
     * @return int - 0 if it succeeded running the livestream viewer process
     *               1 if it failed because the livestream viewer process was already running
     *               2 if it failed because you don't have permissions to run the livestream viewer process
     *                              (it failed to create and/or open the PID file)
     */
    int run_livestream_viewer(const string& home_directory);

    /**
     * This function is called only in the GUI process.
     *
     * @return bool - true if the livestream viewer process is already running
     *                false if the livestream viewer process is not already running
     */
    bool is_livestream_running();

  private:
    /**
     * This is a utility function which removes the PID file of the livestream viewer process.
     *
     * This function is called only in the GUI process.
     */
    void remove_my_pid_file();

    /**
     * This function forks the LiveStream Viewer process off the GUI process and detatches it.
     * It also writes the PID of the LiveStream process to the PID file and sets up the signal handlers.
     */
    void become_livestream_process();

    /**
     * This function holds the main functionality of the LiveStream Viewer process.
     * The functions above were just for setting everything up.
     * As the name implies, this function opens up the window of the LiveStream Viewer.
     * Upon startup, it also detects if the camera daemon process is already running, and if so it
     * connects to that process and lets the camera daemon know that it should start saving images to the
     * livestream directory.
     *
     * This function is called in the LiveStream Viewer process only.
     * It never returns, and the program stays in this function for it's entire lifetiem until it gets killed.
     */
    void open_viewer_window();

    LiveStream_viewer_data* private_data;  // A pointer to the global private data.
    int my_pid;                            // The PID of the LiveStream viewer.
};


/* Helper functions */

/**
 * This helper function is a signal handler.
 * The only reason why it's not a member method of the class is because C/C++ doesn't allow signal handlers
 * to be member methods.
 *
 * This function is called whenever LiveStream Viewer process recieves a terminate signal.
 * It first cleans up the resources of the process and lets the camera daemon process (if it's still running)
 * know that it is turning off.
 */
void terminate_livestream(int);

/**
 * This helper function is a signal handler.
 * The only reason why it's not a member method of the class is because C/C++ doesn't allow signal handlers
 * to be member methods.
 *
 * The livestream viewer recieves SIGUSR1 when the camera daemon process starts up.
 * This function handles that signal by connecting to that proces.
 */
void camera_daemon_starts_up(int);

/**
 * This helper function is a signal handler.
 * The only reason why it's not a member method of the class is because C/C++ doesn't allow signal handlers
 * to be member methods.
 *
 * The livestream viewer recieves SIGUSR2 when the camera daemon process shuts down.
 * This function handles that signal by disconnecting from that proces.
 */
void camera_daemon_shuts_down(int);

/**
 * This function tries to find a camera? directory in the directory /tmp/SmartCCTV_livestream/
 * The ? stands for any single digit. This is because you can have camera0 or camera1, or camera2 opened
 * if you have multiple cameras to choose from.
 * This function automatically finds the camera that is currently in use by the SmartCCTV Camera Daemon,
 * and it sets that directory to be watched.
 *
 * This function modifies liveStream_viewer_data.streamDir to hold the directory of the active camera if
 * it detects one. If the directory of the active camera was already previously set, and this function is
 * called again, it doesn't to anything, and it exits without errors.
 *
 * This function is called by the LiveStream Viewer process only.
 * Although originally intended to be a member method, this is not the case because it is called by
 * signal handler functions, which cannot be member methods or call other member methods.
 *
 * @return bool - true if the directory of the active camera was found
 *                false otherwise
 */
bool find_camera_directory();

/**
 * This function gets and returns the PID of the SmartCCTV Camera Daemon process if it exists.
 * This function is called by the LiveStream Viewer process only.
 *
 * This function terminates the program in the PID file of the SmartCCTV Viewer process has become corrupted.
 * If the PID file of the SmartCCTV Viewer process was corrupted or tampered before the LiveStream Viewer
 * started, or during, then ther's no way to get the PID of the daemon, and no way to signal the daemon to
 * start saving images into the directory, so the LiveStream Viewer process cannot start, so it should
 * terminate.
 * In case this happens, the function pushes a custom message to the GUI.
 *
 * @return int - The PID of the SmartCCTV Camera Daemon process, or -1 if this process does not exist.
 *
 */
int get_daemon_pid();


#endif  /* LIVESTREAM_FACADE_H */


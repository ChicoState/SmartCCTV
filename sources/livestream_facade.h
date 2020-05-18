#ifndef LIVESTREAM_FACADE_H
#define LIVESTREAM_FACADE_H

#include <string>

using std::string;

// You can change this to make the syslog() output to a different file.
#define log_facility LOG_LOCAL0


struct LiveStream_viewer_data;

class LiveStream_facade {
  public:
    LiveStream_facade();

    int run_livestream_viewer(const string& home_directory);

    bool is_livestream_running();

  private:
    void remove_my_pid_file();

    void become_livestream_process();

    void open_viewer_window();

    LiveStream_viewer_data* private_data;  // A pointer to the global private data.
    int my_pid;                            // The PID of the LiveStream viewer.
};


/* Helper functions */

bool find_camera_directory();

int get_daemon_pid();

#endif  /* LIVESTREAM_FACADE_H */


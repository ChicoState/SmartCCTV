#ifndef LIVESTREAM_WINDOW_H
#define LIVESTREAM_WINDOW_H

#include <string>      /* for std::string */
#include <SDL2/SDL.h>  /* for SDL_Window */

using std::string;

// You can change this to make the syslog() output to a different file.
#define log_facility LOG_LOCAL0


class LiveStream_window {
  public:
    LiveStream_window(const string& streamDir, const string& default_images_directory, const bool& SmartCCTV_daemon_is_running);
    ~LiveStream_window();

    void open();

    void finalize();

    void set_streamDir(const string& streamDir);

  private:
    void process_events();

    void draw_image(const string& image_name);

    string streamDir;
    string default_images_directory;
    SDL_Event event;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
    SDL_Texture* texture;
    const bool& is_camera_daemon_running;
};


#endif  /* LIVESTREAM_WINDOW_H */


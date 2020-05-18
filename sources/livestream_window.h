/**
 * File Name:   livestream_window.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  5/17/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/17/20
 *
 * Description:
 * This file contains the declaration of the LiveStream_window class.
 * An instance of this class represents a single viewer window that is responsible for displaying the
 * live stream from a single camera.
 */

#ifndef LIVESTREAM_WINDOW_H
#define LIVESTREAM_WINDOW_H

#include <string>      /* for std::string */
#include <SDL2/SDL.h>  /* for SDL_Window */

using std::string;

// You can change this to make the syslog() output to a different file.
#define log_facility LOG_LOCAL0


class LiveStream_window {
  public:
    /**
     * The consturctor initializes all the private variables to their initial values.
     */
    LiveStream_window(const string& streamDir, const string& default_images_directory, const bool& SmartCCTV_daemon_is_running);

    /**
     * The destructor calls LiveStream_window::finalize().
     * However because the LiveStream Viewer Window is terminated by clicking on the [X] in the corner,
     * the destructor should never execute.
     */
    ~LiveStream_window();

    /**
     * Short for "open window of the livestream viewer"
     * this function contains the main functionality of the LiveStream Viewer Window.
     * If the camera daemon is not running, it displays a default image.
     * otherwise is uses inotify to watch the directory streamDir and display images on the screen as they
     * are created by the the camera daemon, and after it displays the images it immediatley deletes them
     * to keep from overloading the file system with frames of images.
     *
     * If the camera daemon is not running, "SmartCCTV is not running" is displayed.
     * If the camera daemon is running but is not producing images, "NO SIGNAL" is displayed.
     */
    void open();

    /**
     * This function deallocates dynamic memory resources associated with the SDL API.
     */
    void finalize();

    /**
     * Setter method for streamDir.
     *
     * @param const string& streamDir - The new value.
     */
    void set_streamDir(const string& streamDir);

  private:
    /**
     * This helper function is used to process events.
     * It is used for user interactions, such as allowing the user to terminate the program by clicking
     * the [X] in the corner.
     */
    void process_events();

    /**
     * This function draws an image on the screen.
     * The first time this function is called, it initializes the window and the renderer to match the
     * dimentions of the passed in image.
     * Any subsequent times the window and the renderer and re-used.
     *
     * This function terminates the program if it runs into an unrecoverable error.
     *
     * @param const string& image_name - The full name of the image file to be displayed on the screen,
     *                                   including the absolute path to it.
     */
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


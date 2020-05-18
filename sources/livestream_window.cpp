#include "livestream_window.h"
#include "write_message.h"

//#include <sys/types.h>
//#include <sys/stat.h>
#include <syslog.h>       /* for syslog() */
#include <errno.h>        /* for errno() */
#include <sys/inotify.h>  /* for inotify_init(), inotify_add_watch() */
#include <unistd.h>       /* for pause(), sleep(), read() */
#include <dirent.h>       /* for opendir(), readdir(), closedir() */
#include <cstring>        /* for strerror() */
#include <string>         /* for std::string */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using std::string;

#define MAX_EVENT_MONITOR 2048
// file name length
#define NAME_LEN 32
// size of one event
#define MONITOR_EVENT_SIZE (sizeof(struct inotify_event))
// buffer length
#define BUFFER_LEN (MAX_EVENT_MONITOR * (MONITOR_EVENT_SIZE + NAME_LEN))


extern int exit_code;

extern void terminate_livestream(int);


LiveStream_window::LiveStream_window(const string& streamDir, const string& default_images_directory, const bool& SmartCCTV_daemon_is_running)
 : streamDir(streamDir), default_images_directory(default_images_directory), event(), window(nullptr), renderer(nullptr), surface(nullptr), texture(nullptr), is_camera_daemon_running(SmartCCTV_daemon_is_running)
{
    // Attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        syslog(log_facility | LOG_ERR, "Error initializing SDL: %s", SDL_GetError());
        exit_code = EXIT_FAILURE;
        terminate_livestream(0);
    }

    // create a randerer, which sets up the graphics hardware
    //Uint32 rander_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    //renderer = SDL_CreateRenderer(win
}


void LiveStream_window::open()
{
    // If the LiveStream Viewer is started up, and the SmartCCTV Daemon is not yet running
    // (the camera directory doesn't exist), it will go to sleep until when the SmartCCTV Daemon starts up,
    // it sends the LiveStream Viewer a signal, which wakes it up and it continues on down.
    // This is to prevent busy waiting antipattern.
    // is_camera_daemon_running is a const reference to liveStream_viewer_data.SmartCCTV_daemon_is_running
    /*
    while (!is_camera_daemon_running) {
        pause();
    }

    while (true) {
        if (is_camera_daemon_running) {
            syslog(log_facility | LOG_NOTICE, "inotify is working");
        } else {
            syslog(log_facility | LOG_NOTICE, "camera daemon not on");
        }
        sleep(5);
    }
    */

    int window_width = 0, window_height = 0;


    string not_running = default_images_directory + "not_running.bmp";
    string no_signal   = default_images_directory + "no_signal.bmp";

    if (!is_camera_daemon_running) {
        draw_image(not_running);
    }
    while (!is_camera_daemon_running) {
        // draw the image to the window
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(1000);
        process_events();
        // pause();  // prevents the window from appearing
    }

    // Get rid of any residue or glichy images.
    if (auto dir = opendir(streamDir.c_str())) {
        while (auto f = readdir(dir)) {
            if (!f->d_name || f->d_name[0] == '.') {
                continue;  // Skip everything that starts with a dot
            } else if (f->d_name != nullptr) {
                if (unlink(f->d_name) == -1) {
                    const char* const error_message = strerror(errno);
                    syslog(log_facility | LOG_ERR, "Error cannot delete %s : %s", f->d_name, error_message);
                }
            }
        }
        closedir(dir);
    }

    int inotify_fd = 0;
    char buffer[BUFFER_LEN];

    if ( (inotify_fd = inotify_init()) == -1) {
        syslog(log_facility | LOG_ERR, "Error initializing inotify: %m");
        exit_code = EXIT_FAILURE;
        terminate_livestream(0);
    }

    int watch_desc = inotify_add_watch(inotify_fd, streamDir.c_str(), IN_CREATE);
    if (watch_desc == -1) {
        const char* const error_message = strerror(errno);
        syslog(log_facility | LOG_ERR, "Could not add watch to folder %s : %s", streamDir.c_str(), error_message);
        exit_code = EXIT_FAILURE;
        terminate_livestream(0);
    }

    int i = 0;
    while (true)
    {
        i = 0;
        int total_read = read(inotify_fd, buffer, BUFFER_LEN);
        if (total_read < 0) {
            // this could be due to interrupted system call
            // if a signal is sent at the time, it prevents read() from completing
            // so try to read() again after the signal handler returns
            syslog(log_facility | LOG_ERR, "inotify read() error : %m");
            if (is_camera_daemon_running) {
                draw_image(no_signal);
            } else {
                draw_image(not_running);
            }
            process_events();
            continue;
        } else if (total_read == 0) {
            if (is_camera_daemon_running) {
                draw_image(no_signal);
            } else {
                draw_image(not_running);
            }
            process_events();
        }

        while (i < total_read) {
            struct inotify_event* event = (struct inotify_event*) &buffer[i];
            if (event->len > 0) {
                // if ( file is created) && (it is not a directory) )
                if ( (event->mask & IN_CREATE) && !(event->mask & IN_ISDIR) ) {
                    string image_file = streamDir + "/";
                    image_file += event->name;
                    //syslog(log_facility | LOG_NOTICE, "file %s appeared", event->name);
                    draw_image(image_file);
                    if (unlink(image_file.c_str()) == -1) {
                        const char* const error_message = strerror(errno);
                        syslog(log_facility | LOG_ERR, "Error cannot delete %s : %s", image_file.c_str(), error_message);
                    }
                    process_events();
                }
                 // move onto the next event
                i += event->len + MONITOR_EVENT_SIZE;
            }
        }

        process_events();
    }


    /*
    window = SDL_CreateWindow("SmartCCTV LiveStream Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (window == nullptr) {
        syslog(log_facility | LOG_ERR, "Error creating window: %s", SDL_GetError());
    }
    */
}


void LiveStream_window::finalize()
{
    if (surface != nullptr)   SDL_FreeSurface(surface);
    if (texture != nullptr)   SDL_DestroyTexture(texture);
    if (renderer != nullptr)  SDL_DestroyRenderer(renderer);
    if (window != nullptr)    SDL_DestroyWindow(window);
    SDL_Quit();
}


void LiveStream_window::set_streamDir(const string& streamDir)
{
    this->streamDir = streamDir;
}


void LiveStream_window::process_events()
{
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) {
            write_message("LiveStream Viewer window was closed.");
            exit_code = EXIT_SUCCESS;
            terminate_livestream(0);
        }
    }
}


void LiveStream_window::draw_image(const string& image_name)
{
    static bool window_initialized = false;

    int window_width = 0, window_height = 0;

    if (surface != nullptr) {
        SDL_FreeSurface(surface);
        surface = nullptr;
    }

    while (true) {
        surface = IMG_Load(image_name.c_str());
        if (surface != nullptr)  // success
            break;
        const char* const error_message = SDL_GetError();
        // Trying to open the image file while SmartCCTV is writing it.
        if (strcmp("Error reading from datastream", error_message) == 0) {
            SDL_Delay(5);
            continue;
        // This is a bug that sometimes happens.
        // It just failed to open the image for some reason.
        } else if (strcmp("Unsupported image format", error_message) == 0) {
            SDL_Delay(5);
            continue;
        } else {
            syslog(log_facility | LOG_ERR, "Error opening image %s : %s", image_name.c_str(), error_message);
            exit_code = EXIT_FAILURE;
            terminate_livestream(0);
        }
    }
    window_width = surface->w;
    window_height = surface->h;

    // Initialize the SDL window (this is only run once).
    if (!window_initialized) {
        window = SDL_CreateWindow("SmartCCTV LiveStream Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
        if (window == nullptr) {
            syslog(log_facility | LOG_ERR, "Error creating window: %s", SDL_GetError());
            exit_code = EXIT_FAILURE;
            terminate_livestream(0);
        }

        Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
        renderer = SDL_CreateRenderer(window, -1, render_flags);
        if (renderer == nullptr) {
            syslog(log_facility | LOG_ERR, "Error creating renderer: %s", SDL_GetError());
            exit_code = EXIT_FAILURE;
            terminate_livestream(0);
        }

        window_initialized = true;
    }

    // Create a new texture for each surface (image).
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = nullptr;
    if (texture == nullptr) {
        syslog(log_facility | LOG_ERR, "Error creating texture: %s", SDL_GetError());
        exit_code = EXIT_FAILURE;
        terminate_livestream(0);
    }

    // clear the window
    SDL_RenderClear(renderer);

    // draw the image to the window
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}


LiveStream_window::~LiveStream_window()
{
    finalize();
}


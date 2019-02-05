//
// Created by joao on 2/5/19.
//

#ifndef PVIEWER_VIDEOPLAYER_H
#define PVIEWER_VIDEOPLAYER_H

#include "SDL.h"
#define GL_GLEXT_PROTOTYPES
#include "SDL_video.h"
#include "SDL_opengl.h"
#include "SDL_syswm.h"


#include <GL/glx.h>
#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>
#include <gst/gl/x11/gstgldisplay_x11.h>
#include <gst/gl/gl.h>

#include <string>

class VideoPlayer {
private:
    //Singleton
    static VideoPlayer* _ourInstance;

    //GL Context
    GstGLContext *_sdl_context;
    GstGLDisplay *_sdl_gl_display;
    Display* _sdl_display;
    Window _sdl_win;
    GLXContext _sdl_gl_context;

    //Gstreamer loop
    GMainLoop* _loop;
    GstElement* _pipeline;
    GAsyncQueue *_queue_input_buf;
    GAsyncQueue *_queue_output_buf;
    GstVideoInfo _info;
    GLuint _texture;
private:
    VideoPlayer(Display* sdlDisplay, Window sdlWindow, GLXContext glxContext);
    ~VideoPlayer();
public:
    static VideoPlayer* getInstance(Display* sdlDisplay, Window sdlWindow, GLXContext glxContext);
    static VideoPlayer* getInstance();
    static void deleteInstance();

    void loadFile(std::string& filename);
    void play();
    void pause();
    void stop();
    void bindTexture();
    void releaseTexture();

    GstGLDisplay* getDisplay();
    GstGLContext* getContext();
};


#endif //PVIEWER_VIDEOPLAYER_H

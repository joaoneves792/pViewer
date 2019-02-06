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
extern "C" {
#include <gst/gst.h>
#include <glib.h>
#include <gst/gl/x11/gstgldisplay_x11.h>
#include <gst/gl/gl.h>
}

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
    static bool _reset;
    static GMainLoop* _loop;
    static GstElement* _pipeline;
    static GAsyncQueue *_queue_input_buf;
    static GAsyncQueue *_queue_output_buf;
    static GstVideoInfo _info;
    static GLuint _texture;
    static GstBuffer* _buffer;

    //Video
    int _width;
    int _height;
    bool _hold;
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

    long getDuration();
    long getCurrentTime();
    bool seekForward();
    bool seekBackward();

    GstGLDisplay* getDisplay();
    GstGLContext* getContext();

    int getWidth();
    int getHeight();

    static void on_gst_buffer (GstElement * fakesink, GstBuffer * buf, GstPad * pad, gpointer data);
};


#endif //PVIEWER_VIDEOPLAYER_H

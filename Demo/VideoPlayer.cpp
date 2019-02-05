//
// Created by joao on 2/5/19.
//

#include "VideoPlayer.h"
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
#include <sstream>

//----------------Singleton------------------
VideoPlayer* VideoPlayer::_ourInstance = nullptr;


//----------------Callbacks------------------
static void on_gst_buffer (GstElement * fakesink, GstBuffer * buf, GstPad * pad, gpointer data) {

}

static gboolean sync_bus_call (GstBus * bus, GstMessage * msg, gpointer data){
    VideoPlayer* vp = VideoPlayer::getInstance();
    if(!vp)
        return FALSE;
    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_NEED_CONTEXT: {
            const gchar *context_type;

            gst_message_parse_context_type(msg, &context_type);
            g_print("got need context %s\n", context_type);

            if (g_strcmp0(context_type, GST_GL_DISPLAY_CONTEXT_TYPE) == 0) {
                GstContext *display_context = gst_context_new(GST_GL_DISPLAY_CONTEXT_TYPE, TRUE);
                gst_context_set_gl_display(display_context, vp->getDisplay());
                gst_element_set_context(GST_ELEMENT (msg->src), display_context);
                return TRUE;
            } else if (g_strcmp0(context_type, "gst.gl.app_context") == 0) {
                GstContext *app_context = gst_context_new("gst.gl.app_context", TRUE);
                GstStructure *s = gst_context_writable_structure(app_context);
                gst_structure_set(s, "context", GST_TYPE_GL_CONTEXT, vp->getContext(), NULL);
                gst_element_set_context(GST_ELEMENT (msg->src), app_context);
                return TRUE;
            }
            break;
        }
        default:
            break;
    }
    return FALSE;
}

static void end_stream_cb (GstBus * bus, GstMessage * msg, GMainLoop * loop){

}
//----------------Class methods--------------
VideoPlayer* VideoPlayer::getInstance(Display *sdlDisplay, Window sdlWindow, GLXContext glxContext) {
    if(!_ourInstance)
        _ourInstance = new VideoPlayer(sdlDisplay, sdlWindow, glxContext);
    return _ourInstance;
}

VideoPlayer* VideoPlayer::getInstance() {
    if(_ourInstance)
        return _ourInstance;
    else
        return nullptr;
}

void VideoPlayer::deleteInstance() {
    delete _ourInstance;
    _ourInstance = nullptr;
}

VideoPlayer::VideoPlayer(Display *sdlDisplay, Window sdlWindow, GLXContext glxContext) {
    gst_init (0, nullptr);
    _loop = g_main_loop_new (nullptr, FALSE);


    _sdl_display = sdlDisplay;
    _sdl_win = sdlWindow;
    _sdl_gl_context = glxContext;

    glXMakeCurrent (_sdl_display, None, nullptr);
    _sdl_gl_display = (GstGLDisplay *) gst_gl_display_x11_new_with_display(_sdl_display);
    _sdl_context = gst_gl_context_new_wrapped (_sdl_gl_display, (guintptr) _sdl_gl_context,
                   gst_gl_platform_from_string ("glx"), GST_GL_API_OPENGL);

    //Create the elements in the pipeline
    _pipeline = gst_element_factory_make ("playbin", "play0");
    GstElement* upload = gst_element_factory_make("glupload", "glupload0");
    GstElement* effect = gst_element_factory_make("gleffects", "gleffects");
    GstElement* sink = gst_element_factory_make("fakesink", "fakesink0");


    /* Create the gl sink bin, add the elements and link them */
    GstElement* bin = gst_bin_new ("gl_sink_bin");
    gst_bin_add_many (GST_BIN (bin), upload, effect, sink, NULL);
    gst_element_link_many (upload, effect, sink, NULL);
    GstPad* pad = gst_element_get_static_pad (upload, "sink");
    GstPad* ghost_pad = gst_ghost_pad_new ("sink", pad);
    gst_pad_set_active (ghost_pad, TRUE);
    gst_element_add_pad (bin, ghost_pad);
    gst_object_unref (pad);

    //Configure the pipeline
    g_object_set (G_OBJECT (sink), "signal-handoffs", TRUE, NULL);
    g_object_set (G_OBJECT (sink), "sync", 1, NULL);
    g_signal_connect (sink, "handoff", G_CALLBACK (on_gst_buffer), NULL);
    _queue_input_buf = g_async_queue_new ();
    _queue_output_buf = g_async_queue_new ();
    g_object_set_data (G_OBJECT (sink), "queue_input_buf", _queue_input_buf);
    g_object_set_data (G_OBJECT (sink), "queue_output_buf", _queue_output_buf);
    g_object_set_data (G_OBJECT (sink), "loop", _loop);

    //Hijack playbin's video-sink
    g_object_set (GST_OBJECT (_pipeline), "video-sink", bin, NULL);

    GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (_pipeline));
    gst_bus_add_signal_watch (bus);
    g_signal_connect (bus, "message::error", G_CALLBACK (end_stream_cb), _loop);
    g_signal_connect (bus, "message::warning", G_CALLBACK (end_stream_cb), _loop);
    g_signal_connect (bus, "message::eos", G_CALLBACK (end_stream_cb), _loop);
    gst_bus_enable_sync_message_emission (bus);
    g_signal_connect (bus, "sync-message", G_CALLBACK (sync_bus_call), NULL);
    gst_object_unref (bus);

    //Make sure gl context is shared
    pause();
    glXMakeCurrent(_sdl_display, _sdl_win, _sdl_gl_context);
}

VideoPlayer::~VideoPlayer() {
    stop();
    gst_object_unref(GST_OBJECT (_pipeline));
    g_main_loop_unref(_loop);
    g_async_queue_unref(_queue_input_buf);
    g_async_queue_unref(_queue_output_buf);
}

void VideoPlayer::pause() {
    gst_element_set_state (GST_ELEMENT (_pipeline), GST_STATE_PAUSED);
}

void VideoPlayer::play() {
    gst_element_set_state (GST_ELEMENT (_pipeline), GST_STATE_PLAYING);
}

void VideoPlayer::stop() {
    gst_element_set_state (GST_ELEMENT (_pipeline), GST_STATE_NULL);
}

GstGLContext* VideoPlayer::getContext() {
    return _sdl_context;
}

GstGLDisplay* VideoPlayer::getDisplay() {
    return _sdl_gl_display;
}

void VideoPlayer::loadFile(std::string &filename) {
    std::stringstream ss;
    ss << "file://" << filename;
    g_object_set (G_OBJECT (_pipeline), "uri", ss.str().c_str(), NULL);
}
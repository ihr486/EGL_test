#include <stdio.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

int main(int argc, const char *argv[])
{
    bcm_host_init();

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if(display == EGL_NO_DISPLAY) {
        fprintf(stderr, "No display found.\n");
        return 1;
    }

    EGLBoolean result = eglInitialize(display, NULL, NULL);
    if(result == EGL_FALSE) {
        fprintf(stderr, "Failed to initialize OpenGL ES.\n");
        return 1;
    }

    static const EGLint attribute_list[] =
    {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };
   
    EGLConfig config;
    EGLint num_config;
    result = eglChooseConfig(display, attribute_list, &config, 1, &num_config);
    if(result == EGL_FALSE) {
        fprintf(stderr, "Failed to configure OpenGL ES.\n");
        return 1;
    }

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
    if(context == EGL_NO_CONTEXT) {
        fprintf(stderr, "Failed to create an OpenGL context.\n");
        return 1;
    }

    unsigned int width, height;
    int status = graphics_get_display_size(0, &width, &height);
    if(status < 0) {
        fprintf(stderr, "Failed to retrieve display information.\n");
        return 1;
    }

    printf("Display: %d * %d\n", width, height);

    DISPMANX_DISPLAY_HANDLE_T dispman_display = vc_dispmanx_display_open(0);
    DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start(0);

    VC_RECT_T dst_rect, src_rect;

    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = width;
    dst_rect.height = height;

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = width << 16;
    src_rect.height = height << 16;

    DISPMANX_ELEMENT_HANDLE_T dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T)0);

    vc_dispmanx_update_submit_sync(dispman_update);

    EGL_DISPMANX_WINDOW_T nativewindow;
    nativewindow.element = dispman_element;
    nativewindow.width = width;
    nativewindow.height = height;

    EGLSurface surface = eglCreateWindowSurface(display, config, &nativewindow, NULL);
    if(surface == EGL_NO_SURFACE) {
        fprintf(stderr, "Failed to create an OpenGL surface.\n");
        return 1;
    }

    result = eglMakeCurrent(display, surface, surface, context);
    if(result == EGL_FALSE) {
        fprintf(stderr, "Failed to activate the OpenGL context.\n");
        return 1;
    }

    glClearColor(1, 1, 1, 1);

    glClear(GL_COLOR_BUFFER_BIT);

    glFlush();

    eglSwapBuffers(display, surface);

    sleep(1);

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    eglTerminate(display);

    return 0;
}

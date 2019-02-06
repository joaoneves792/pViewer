//
// Created by joao on 11/21/17.
//

#include <sstream>
#include <iostream>
#include "CGJengine.h"
#include "actions.h"
#include "constants.h"
#include "SceneGraph/SceneNode.h"
#include "Browser.h"

#include <SDL.h>

#define ESCAPE 27

extern bool running;
extern SDL_Window* WindowHandle;

void reshape(int w, int h);

void setupActions() {
    SceneGraph *scene = ResourceManager::getInstance()->getScene(SCENE);
    InputManager *im = InputManager::getInstance();

    im->setActionInterval(10); //Update every 10ms

    /*Update camera movement*/
    const static float movementRate = 0.005f; //magic number

    static float scale = 1.0f;

    im->addKeyAction('w', [&](int dt){
       static SceneNode* quad = ResourceManager::getInstance()->getScene(SCENE)->findNode(QUAD);
       scale += dt*movementRate;
       quad->scale(scale*ASPECT_RATIO, scale, 1.0f);
    });
    im->addKeyAction('s', [&](int dt){
        static SceneNode* quad = ResourceManager::getInstance()->getScene(SCENE)->findNode(QUAD);
        if(scale > 0.1f)
            scale -= dt * movementRate;
        else
            scale = 0.1f;
        if(scale >= 0.1f)
            quad->scale(scale*ASPECT_RATIO, scale, 1.0f);
    });

    im->addKeyAction('W', [=](int timeDelta){
        scene->findNode(QUAD)->translate(0.0f, 0.0f, (timeDelta * movementRate));
    });
    im->addKeyAction('S', [=](int timeDelta){
        scene->findNode(QUAD)->translate(0.0f, 0.0f, -(timeDelta * movementRate));
    });
    im->addKeyAction(ESCAPE, [=](int timeDelta){
        running = false;
    });
    im->addKeyActionOnce(' ', [=](){
        VRCamera* cam = (VRCamera*)ResourceManager::getInstance()->getCamera(SPHERE_CAM);
        if(cam)
            cam->recenter();
    });

    im->addSpecialKeyActionOnce(SDLK_F11, [=](){
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        if(fullscreen) {
            SDL_SetWindowFullscreen(WindowHandle, SDL_WINDOW_FULLSCREEN);
            SDL_DisplayMode dm;
            SDL_GetCurrentDisplayMode(0, &dm);
            reshape(dm.w, dm.h);
        }else
            SDL_SetWindowFullscreen(WindowHandle, 0);

       //glutFullScreenToggle();
    });

    im->addSpecialKeyActionOnce(PVIEWER_MOUSE_WHEEL_UP, [=](){
       Browser::getInstance()->next();
    });
    im->addSpecialKeyActionOnce(PVIEWER_MOUSE_WHEEL_DOWN, [=](){
        Browser::getInstance()->prev();
    });
    im->addKeyActionOnce('x', [=](){
       static VideoPlayer* vp = VideoPlayer::getInstance();
       vp->togglePause();
    });
}
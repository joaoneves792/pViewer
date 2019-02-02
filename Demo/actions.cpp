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
#include "GL/freeglut.h"


#define ESCAPE 27
#define SPACE 0x20

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
       glutPostRedisplay();
    });
    im->addKeyAction('s', [&](int dt){
        static SceneNode* quad = ResourceManager::getInstance()->getScene(SCENE)->findNode(QUAD);
        if(scale > 0.1f)
            scale -= dt * movementRate;
        else
            scale = 0.1f;
        if(scale >= 0.1f)
            quad->scale(scale*ASPECT_RATIO, scale, 1.0f);
        glutPostRedisplay();
    });

    im->addKeyAction('W', [=](int timeDelta){
        scene->findNode(QUAD)->translate(0.0f, 0.0f, (timeDelta * movementRate));
        glutPostRedisplay();
    });
    im->addKeyAction('S', [=](int timeDelta){
        scene->findNode(QUAD)->translate(0.0f, 0.0f, -(timeDelta * movementRate));
        glutPostRedisplay();
    });
    im->addKeyAction(ESCAPE, [=](int timeDelta){
        glutLeaveMainLoop();
    });
    im->addKeyActionOnce(' ', [=](){
        VRCamera* cam = (VRCamera*)ResourceManager::getInstance()->getCamera(SPHERE_CAM);
        if(cam)
            cam->recenter();
        glutPostRedisplay();
    });

    im->addSpecialKeyActionOnce(GLUT_KEY_F11, [=](){
       glutFullScreenToggle();
    });

    im->addSpecialKeyActionOnce(3, [=](){
       Browser::getInstance()->next();
        glutPostRedisplay();
    });
    im->addSpecialKeyActionOnce(4, [=](){
        Browser::getInstance()->prev();
        glutPostRedisplay();
    });
}
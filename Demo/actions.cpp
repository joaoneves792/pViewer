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
    const float movementRate = 0.005f; //magic number


    im->addKeyAction('w', [=](int timeDelta){
        scene->findNode(QUAD)->translate(0.0f, 0.0f, (timeDelta * movementRate));
        glutPostRedisplay();
    });
    im->addKeyAction('s', [=](int timeDelta){
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

    im->addSpecialKeyActionOnce(3, [=](){
       Browser::getInstance()->next();
        glutPostRedisplay();
    });
    im->addSpecialKeyActionOnce(4, [=](){
        Browser::getInstance()->prev();
        glutPostRedisplay();
    });
}
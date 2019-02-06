//
// Created by joao on 11/18/17.
//

#include "scene.h"
#include <string>
#include <sstream>
#include <ctgmath>
#include <cmath>
#include <iostream>
#include "CGJengine.h"
#include "shaders.h"
#include "meshes.h"
#include "constants.h"
#include "SceneGraph/SceneNode.h"
#include "Browser.h"


#include <filesystem>

extern int inVR;

void setupScene(){
    auto rm = ResourceManager::getInstance();
    loadMeshes();
    loadShaders();

    /*Generic quad mesh for multiple purposes*/
    Mesh* quad = new QuadMesh();
    rm->addMesh(QUAD, quad);

    Camera* camera = nullptr;

    if(inVR){
        auto VRCam = ResourceManager::Factory::createOpenHMDCamera(SPHERE_CAM, Vec3(0.0f, GROUND_LEVEL, 0.0f), Quat(1.0f, 0.0f, 0.0f, 0.0f));
        if(VRCam->isReady()) {
            VRCam->perspective(1.0f, 1000.0f);
            camera = VRCam;
        }else{
            std::cerr << "Unable to initialize VR" << std::endl;
            exit(-1);
        }
    }else{
        ResourceManager::getInstance()->destroyCamera(SPHERE_CAM);
        auto freeCamera = ResourceManager::Factory::createFreeCamera(SPHERE_CAM, Vec3(0.0f, GROUND_LEVEL, 0.0f), Quat(1.0f, 0.0f, 0.0f, 0.0f));
        freeCamera->perspective((float)PI/4.0f, 0, 1.0f, 1000.0f);
        camera = freeCamera;
    }

    SceneNode* root = ResourceManager::Factory::createScene(SCENE, camera);
    root->translate(0.0f, GROUND_LEVEL, 0.0f);


    SceneNode* quadNode = new SceneNode(QUAD, quad, rm->getShader(QUAD_SHADER));
    quadNode->translate(0.0f, 0.0f, -5.0f);
    quadNode->scale(ASPECT_RATIO, 1.0f, 1.0f);
    root->addChild(quadNode);

}

void loadInput(const std::string& filename){
    static SceneNode* quad = ResourceManager::getInstance()->getScene(SCENE)->findNode(QUAD);
    static Browser* browser = Browser::getInstance();

    browser->init(filename);

    GLint widthLoc = ResourceManager::getInstance()->getShader(QUAD_SHADER)->getUniformLocation("texWidth");
    GLint heightLoc = ResourceManager::getInstance()->getShader(QUAD_SHADER)->getUniformLocation("texHeight");
    GLint flipLoc = ResourceManager::getInstance()->getShader(QUAD_SHADER)->getUniformLocation("flip");

    quad->setPreDraw([=](){
        glActiveTexture(GL_TEXTURE0);
        bool flip = browser->bindTexture();
        auto texWidth = (float)(browser->getCurrentWidth());
        auto texHeight = (float)(browser->getCurrentHeight());
        glUniform1f(widthLoc, texWidth);
        glUniform1f(heightLoc, texHeight);
        glUniform1f(flipLoc, (flip)?-1.0f:1.0f);
    });

    quad->setPostDraw([=](){
       browser->releaseTexture();
    });

}

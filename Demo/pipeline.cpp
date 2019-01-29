//
// Created by joao on 12/19/17.
//

#include <iostream>
#include "CGJengine.h"
#include "constants.h"
#include "pipeline.h"

extern bool inVR;

void setupPipeline(){
    ResourceManager* rm = ResourceManager::getInstance();
    QuadMesh* quad = (QuadMesh*)rm->getMesh(QUAD);
    SceneGraph* scene = rm->getScene(SCENE);

    /*Create the framebuffers*/
    ResourceManager::Factory::createColorTextureFrameBuffer(SIDE_FBO1, WIN_X, WIN_Y);

    rm->getFrameBuffer(SIDE_FBO1)->setInternalFormats(GL_RGBA8, GL_RGBA16F, GL_DEPTH_COMPONENT32);

    /*Create the ortho camera*/
    auto finalCamera = ResourceManager::Factory::createHUDCamera(ORTHO_CAM, -1, 1, 1, -1, 0, 1, true);

    /*Create the pipeline scenegraph*/
    auto renderPipeline = ResourceManager::Factory::createScene(PIPELINE, finalCamera);
    renderPipeline->translate(0.0f, 0.0f, -0.2f);

    /*General purpose blitting stage*/
    auto blit = new SceneNode(BLIT, quad, rm->getShader(BLIT_SHADER));
    blit->setProcessingLevel(BLIT_LEVEL);
    blit->setPreDraw([=](){
        glDepthMask(GL_FALSE);
    });
    blit->setPostDraw([=](){
        glDepthMask(GL_TRUE);
    });
    renderPipeline->addChild(blit);

    /*FXAA stage*/
    auto fxaa = new SceneNode(FXAA, quad, rm->getShader(FXAA_SHADER));
    fxaa->setProcessingLevel(FXAA_LEVEL);
    renderPipeline->addChild(fxaa);
}

void executePipeline(FrameBuffer* targetFramebuffer){
    static SceneGraph* scene = ResourceManager::getInstance()->getScene(SCENE);
    static SceneGraph* pipeline = ResourceManager::getInstance()->getScene(PIPELINE);

    static ColorTextureFrameBuffer* sideBuffer1 = (ColorTextureFrameBuffer*)ResourceManager::getInstance()->getFrameBuffer(SIDE_FBO1);

    /*static Shader* blit = ResourceManager::getInstance()->getShader(BLIT_SHADER);
    static GLint renderTargetLoc = blit->getUniformLocation("renderTarget");*/

    sideBuffer1->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene->draw();

    /*Apply FXAA and render to screen*/
    glActiveTexture(GL_TEXTURE0);
    sideBuffer1->bindTexture();
    if(!targetFramebuffer)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    else
        targetFramebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pipeline->draw(FXAA_LEVEL);
}
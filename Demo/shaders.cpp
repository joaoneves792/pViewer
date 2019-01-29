//
// Created by joao on 11/21/17.
//

#include "CGJengine.h"
#include "shaders.h"
#include "constants.h"


#define UPLOAD_MVP [=](const Mat4& M, const Mat4& V, const Mat4& P){ glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(P * V * M)); }

void loadShaders(){
    /*Sky Shader*/
    /*auto skyShader = ResourceManager::Factory::createShader(SKY_SHADER, "res/shaders/skyv.glsl", "res/shaders/skyf.glsl");
    skyShader->setAttribLocation("aPos", VERTICES__ATTR);
    skyShader->link();
    MVPLocation = skyShader->getUniformLocation("MVP");
    skyShader->setMVPFunction([=](const Mat4& M, const Mat4& V, const Mat4& P) {
        Mat4 rotOnlyMV = V*M;
        rotOnlyMV[3][0] = 0.0f;
        rotOnlyMV[3][1] = 0.0f;
        rotOnlyMV[3][2] = 0.0f;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(P * rotOnlyMV));
    });*/

    /*Quad shader*/
    auto quadShader = ResourceManager::Factory::createShader(QUAD_SHADER, "res/shaders/quadv.glsl", "res/shaders/2Df.glsl");
    quadShader->setAttribLocation("inPosition", VERTICES__ATTR);
    quadShader->link();
    GLint MVPLocation = quadShader->getUniformLocation("MVP");
    quadShader->setMVPFunction(UPLOAD_MVP);

    /*General purpose blit shader*/
    auto blitShader = ResourceManager::Factory::createShader(BLIT_SHADER, "res/shaders/quadv.glsl", "res/shaders/blitf.glsl");
    blitShader->setAttribLocation("inPosition", VERTICES__ATTR);
    blitShader->link();
    MVPLocation = blitShader->getUniformLocation("MVP");
    blitShader->setMVPFunction(UPLOAD_MVP);

    /*FXAA Shader*/
    auto fxaaShader = ResourceManager::Factory::createShader(FXAA_SHADER, "res/shaders/quadv.glsl", "res/shaders/fxaaf.glsl");
    fxaaShader->setAttribLocation("inPosition", VERTICES__ATTR);
    fxaaShader->link();
    MVPLocation = fxaaShader->getUniformLocation("MVP");
    fxaaShader->setMVPFunction(UPLOAD_MVP);
}
//
// Created by joao on 2/5/19.
//

#include <CGJengine.h>
#include "MediaInterface.h"
#include "VideoPlayer.h"

MediaInterface* MediaInterfaceFactory(const std::string& filename){
    if (FreeImage_GetFileType(filename.c_str(), 0) != FIF_UNKNOWN)
        return new Image(filename);
    std::string extension = filename.substr(filename.find_last_of('.')+1);
    if(extension == "mp4")
        return new Video(filename);
    return nullptr;
}
MediaInterface::~MediaInterface() {}

//--------------Image----------------------
Image::Image(const std::string &filename) {
    _filename = std::string(filename);
}
void Image::load() {
    _texture = ResourceManager::Factory::createTexture(_filename);
}
void Image::unload() {
    _texture.reset();
    ResourceManager::getInstance()->destroyTexture(_filename);
}
int Image::getHeight() {
    return _texture->getHeight();
}
int Image::getWidth() {
    return _texture->getWidth();
}
bool Image::bindTexture() {
    _texture->bind();
    return false;
}
void Image::releaseTexture() {
    //Empty
}

//------------Video-----------------------
Video::Video(const std::string &filename) {
    _vp = VideoPlayer::getInstance();
    _filename = std::string(filename);
}
void Video::load() {
    _vp->loadFile(_filename);
    _vp->play();
}

void Video::unload() {
    _vp->stop();
}
int Video::getHeight() {
    return _vp->getHeight();
}
int Video::getWidth() {
    return _vp->getWidth();
}
bool Video::bindTexture() {
    _vp->bindTexture();
    return true;
}
void Video::releaseTexture() {
    _vp->releaseTexture();
}
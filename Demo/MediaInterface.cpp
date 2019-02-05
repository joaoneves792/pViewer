//
// Created by joao on 2/5/19.
//

#include <CGJengine.h>
#include "MediaInterface.h"

MediaInterface* MediaInterfaceFactory(const std::string& filename){
    if (FreeImage_GetFileType(filename.c_str(), 0) != FIF_UNKNOWN)
        return new Image(filename);
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
void Image::bindTexture() {
    _texture->bind();
}
void Image::releaseTexture() {
    //Empty
}

//
// Created by joao on 2/5/19.
//
#include <regex>
#include <filesystem>

#include <CGJengine.h>
#include "MediaInterface.h"
#include "VideoPlayer.h"


extern std::regex video_extensions;

MediaInterface* MediaInterfaceFactory(const std::filesystem::path &path){
    if (FreeImage_GetFileType(path.string().c_str(), 0) != FIF_UNKNOWN)
        return new Image(path.string());
    if(std::regex_match(path.extension().string(), video_extensions))
        return new Video(path.string());
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

bool Image::seek(int forward) {
    return false;
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
bool Video::seek(int forward) {
    if(forward > 0){
        return _vp->seekForward();
    }else{
        return _vp->seekBackward();
    }
}
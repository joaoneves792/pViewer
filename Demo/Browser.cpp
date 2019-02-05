//
// Created by joao on 1/29/19.
//

#include "Browser.h"

#include <list>
#include <filesystem>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <FreeImage.h>

Browser* Browser::_ourInstance = nullptr;

Browser* Browser::getInstance() {
    if(_ourInstance == nullptr){
        _ourInstance = new Browser();
    }
    return _ourInstance;
}

void Browser::deleteInstance() {

    _ourInstance = nullptr;
}

void Browser::init(const std::string &filename) {
    std::filesystem::path filePath = filename;
    std::filesystem::path directory;
    if(filePath.has_extension()) {
        directory = filePath.parent_path();
    }else{
        directory = filePath;
    }

    for (const auto &entry : std::filesystem::directory_iterator(directory))
        if (FreeImage_GetFileType(entry.path().string().c_str(), 0) != FIF_UNKNOWN)
            _files.insert(_files.end(), entry.path());

    _files.sort();
    _current = 0;
    if(filePath.has_extension()) {
        for (auto it = _files.begin(); it != _files.end(); it++) {
            _current++;
            if (it->string() == filename) {
                _it = it;
                break;
            }
        }
    }else{
        _it = _files.begin();
        _current = 1;
    }
    _total = _files.size();



    _texture = ResourceManager::Factory::createTexture(_it->string());
    _width = _texture->getWidth();
    _height = _texture->getHeight();
}

/*
int Browser::rrNext() {
    return  (_index + 1) % 3;
}

int Browser::rrPrev() {
    int i = (_index - 1) % 3;
    if(i < 0)
        i = 2;
    return i;

}
*/

std::list<std::filesystem::path>::iterator Browser::rrNextIt() {
    _current++;
    if(_current > _total)
        _current = 1;
    if(std::next(_it) == _files.end())
        return _files.begin();
    return std::next(_it);
}

std::list<std::filesystem::path>::iterator Browser::rrPrevIt() {
    _current--;
    if(_current < 1)
        _current = _total;
    if(_it == _files.begin())
        return std::prev(_files.end());
    return std::prev(_it);
}

void Browser::bindTexture() {
    _texture->bind();
}

void Browser::releaseTexture() {
    //empty for now
}

int Browser::getCurrentWidth() {
    return _width;
}

int Browser::getCurrentHeight() {
    return _height;
}

void Browser::next() {
    _texture.reset();
    ResourceManager::getInstance()->destroyTexture(_it->string());
    _it = rrNextIt();
    _texture = ResourceManager::Factory::createTexture(_it->string());
    _width = _texture->getWidth();
    _height = _texture->getHeight();
}

void Browser::prev(){
    _texture.reset();
    ResourceManager::getInstance()->destroyTexture(_it->string());
    _it = rrPrevIt();
    _texture = ResourceManager::Factory::createTexture(_it->string());
    _width = _texture->getWidth();
    _height = _texture->getHeight();
}

const std::string Browser::getCurrentName() {
    std::stringstream ss;
    ss << _it->filename() << " (" << _current << "/" << _total << ")";
    return ss.str();
}
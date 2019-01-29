//
// Created by joao on 1/29/19.
//

#include "Browser.h"

#include <list>
#include <filesystem>
#include <string>
#include <iostream>
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
    std::filesystem::path directory = filePath.parent_path();

    for (const auto &entry : std::filesystem::directory_iterator(directory))
        if(FreeImage_GetFileType(entry.path().string().c_str(),0) != FIF_UNKNOWN)
            _files.insert(_files.end(), entry.path());

    _files.sort();
    for(auto it = _files.begin(); it != _files.end(); it++){
        if(it->string() == filename){
            _it = it;
            break;
        }
    }


    _index = 1;

    //TODO Check if iterator goes around
    _textures[0] = ResourceManager::Factory::createTexture(std::prev(_it)->string());
    _textures[1] = ResourceManager::Factory::createTexture(_it->string());
    _textures[2] = ResourceManager::Factory::createTexture(std::next(_it)->string());

}

int Browser::rrNext() {
    return  (_index + 1) % 3;
}

int Browser::rrPrev() {
    int i = (_index - 1) % 3;
    if(i < 0)
        i = 2;
    return i;

}

std::list<std::filesystem::path>::iterator Browser::rrNextIt() {
    if(_it == _files.end())
        return _files.begin();
    return std::next(_it);
}

std::list<std::filesystem::path>::iterator Browser::rrPrevIt() {
    if(_it == _files.begin())
        return _files.end();
    return std::next(_it);
}

Texture* Browser::getCurrentTexture() {
    return _textures[_index];
}

void Browser::next() {
    int replaceIndex = rrPrev();
    ResourceManager::getInstance()->destroyTexture(rrPrevIt()->string());
    _it = rrNextIt();
    std::cout << "N " << rrNextIt()->string() << std::endl;
    _textures[replaceIndex] = ResourceManager::Factory::createTexture(rrNextIt()->string());
    _index = rrNext();

}

void Browser::prev(){
    /*int replaceIndex = rrNext();
    ResourceManager::getInstance()->destroyTexture(rrNextIt()->string());
    _it = rrPrevIt();
    std::cout << "P " << rrPrevIt()->string() << std::endl;
    _textures[replaceIndex] = ResourceManager::Factory::createTexture(std::prev(_it)->string());
    _index = rrPrev();*/
}

const std::string Browser::getCurrentName() {
    return _it->filename().string();
}
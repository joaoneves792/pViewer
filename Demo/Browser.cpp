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

    for (const auto &entry : std::filesystem::directory_iterator(directory)) {

        std::string extension = entry.path().string().substr(entry.path().string().find_last_of('.')+1);
        if (FreeImage_GetFileType(entry.path().string().c_str(), 0) != FIF_UNKNOWN ||
            (extension == "mp4"))
            _files.insert(_files.end(), entry.path());
    }
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

    _currentMedia = MediaInterfaceFactory(_it->string());
    _currentMedia->load();
}

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

bool Browser::bindTexture() {
    return _currentMedia->bindTexture();
}

void Browser::releaseTexture() {
    _currentMedia->releaseTexture();
}

int Browser::getCurrentWidth() {
    return _currentMedia->getWidth();
}

int Browser::getCurrentHeight() {
    return _currentMedia->getHeight();
}

void Browser::next() {
    _currentMedia->unload();
    delete _currentMedia;
    _it = rrNextIt();
    _currentMedia = MediaInterfaceFactory(_it->string());
    _currentMedia->load();
}

void Browser::prev(){
    _currentMedia->unload();
    delete _currentMedia;
    _it = rrPrevIt();
    _currentMedia = MediaInterfaceFactory(_it->string());
    _currentMedia->load();
}

const std::string Browser::getCurrentName() {
    std::stringstream ss;
    ss << _it->filename() << " (" << _current << "/" << _total << ")";
    return ss.str();
}
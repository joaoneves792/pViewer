//
// Created by joao on 1/29/19.
//

#ifndef CGJDEMO_BROWSER_H
#define CGJDEMO_BROWSER_H


#include <list>
#include <filesystem>
#include <string>
#include <CGJengine.h>
#include <memory>

class Browser {
private:
    static Browser* _ourInstance;
    Browser() = default;

    std::list<std::filesystem::path> _files;
    std::list<std::filesystem::path>::iterator _it;

    std::shared_ptr<Texture> _texture;

    unsigned long _current;
    unsigned long _total;

public:
    static Browser* getInstance();
    static void deleteInstance();

    void init(const std::string& filename);

    std::shared_ptr<Texture> getCurrentTexture();
    const std::string getCurrentName();
    void next();
    void prev();
private:
    std::list<std::filesystem::path>::iterator rrNextIt();
    std::list<std::filesystem::path>::iterator rrPrevIt();


};


#endif //CGJDEMO_BROWSER_H

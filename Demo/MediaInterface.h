//
// Created by joao on 2/5/19.
//

#ifndef PVIEWER_MEDIAINTERFACE_H
#define PVIEWER_MEDIAINTERFACE_H

#include <string>

class MediaInterface {
public:
    virtual void load() = 0;
    virtual void unload() = 0;
    virtual void bindTexture() = 0;
    virtual void releaseTexture() = 0;
    virtual int getHeight() = 0;
    virtual int getWidth() = 0;
    virtual ~MediaInterface() = 0;
};

class Image : public MediaInterface{
private:
    std::string _filename;
    std::shared_ptr<Texture> _texture;
public:
    Image(const std::string& filename);
    ~Image() = default;
    void load() final;
    void unload() final;
    void bindTexture() final;
    void releaseTexture() final;
    int getHeight() final;
    int getWidth() final;

};

MediaInterface* MediaInterfaceFactory(const std::string& filename);

#endif //PVIEWER_MEDIAINTERFACE_H

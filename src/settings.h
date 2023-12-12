#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include "utils/sceneparser.h"

struct Settings {
    std::string sceneFilePath;
    float nearPlane = 1;
    float farPlane = 1;
    bool perPixelFilter = false;
    bool kernelBasedFilter = false;
    bool cel;
    bool part=false;
    bool dith=false;
    RenderData m_data;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H

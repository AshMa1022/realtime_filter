#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include "../projects-realtime-AshMa1022/src/utils/sceneparser.h"

struct Settings {
    std::string sceneFilePath;
    float nearPlane = 1;
    float farPlane = 1;
    bool perPixelFilter = false;
    bool kernelBasedFilter = false;
    bool cel;
    RenderData m_data;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H

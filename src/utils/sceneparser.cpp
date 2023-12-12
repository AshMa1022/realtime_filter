#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

void addShape(RenderData &renderData, SceneNode *root, glm::mat4 &m){

    glm::mat4 ctm = m;

    // compute different transformation ctm base on their type
    for(SceneTransformation* t:root->transformations){
        switch(t->type){
        case TransformationType::TRANSFORMATION_TRANSLATE:
            ctm = ctm *glm::translate(glm::mat4{1.f},t->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            ctm = ctm *glm::scale(glm::mat4{1.f},t->scale);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            ctm = ctm *glm::rotate(glm::mat4{1.f},t->angle,
                                    t->rotate);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            ctm = ctm *t->matrix;
            break;
        defualt:
            ctm = ctm* glm::mat4{1.f};
        }


    }

    // Add primitive shape into the shapes vector
    for(ScenePrimitive* a: root->primitives){
        RenderShapeData pri= {*a,ctm};
        renderData.shapes.push_back(pri);
    }


    // parse the light
    for(SceneLight* li:root->lights){
        glm::vec4 pos;
        glm::vec4 dir;
        if(li->type != LightType::LIGHT_DIRECTIONAL){
            pos = ctm * glm::vec4{0,0,0,1.f};
        }
        if(li->type !=LightType::LIGHT_POINT){
            dir = ctm * li->dir;
        }
        SceneLightData a = {li->id, li->type,
                            li->color,li->function,
                            pos,dir,li->penumbra,li->angle,
                            li->width,li->height};

        renderData.lights.push_back(a);
    }


    // Recursion that go through the all the child node base on dfs
    for(SceneNode* child: root->children){
        addShape(renderData,child,ctm);
    }

}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // Task 5: populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    SceneNode *root = fileReader.getRootNode();
    renderData.shapes.clear();
    glm::mat4 ctm(1.0f);

    addShape(renderData,root,ctm);

    return true;
}


//
//  model.hpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#ifndef model_hpp
#define model_hpp

#include <stdio.h>
#include <vector>
#include <iostream>



#include "mesh.hpp"

class Model {
public:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;
    bool gammaCorrection;
    glm::mat4 model;
    
    Model (std::string const &path, bool gamma);
    void draw(Shader shader);
private:
    /*  functions  */
    //  loads a model with help from assimp
    void loadModel (std::string const &path);
    void processNode (aiNode *node, const aiScene *scene);
    Mesh processMesh (aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures (aiMaterial *material, aiTextureType type, std::string typeName);
};

unsigned int TextureFromFile (const char *path, const std::string &directory, bool gamma);

#endif /* model_hpp */

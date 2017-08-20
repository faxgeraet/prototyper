//
//  mesh.cpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include "mesh.hpp"

Mesh::Mesh (std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    numVertices = this->vertices.size();
    
    feedBuffers();
}
void Mesh::feedBuffers()
{
    //  create Buffers and assign their respective ID
    
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    
    //  bind the vertex array object
    
    glBindVertexArray(VAO);
    
    /*  feed data to the buffers  */
    
    // vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    //  element buffer
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    /*  set access to the data  */
    
    //  vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    //  normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    //  texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    
    //  unbind vertex array object
    glBindVertexArray(0);
    
    /*  The mesh data can now be deleted since it is now stored in the vertex and element buffers.  */

    this->vertices.clear();
    this->indices.clear();
    this->textures.clear();
}

void Mesh::draw(Shader shader)
{
    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::stringstream ss;
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            ss << diffuseNr++; // transfer unsigned int to stream
        else if(name == "texture_specular")
            ss << specularNr++; // transfer unsigned int to stream
        else if(name == "texture_normal")
            ss << normalNr++; // transfer unsigned int to stream
        else if(name == "texture_height")
            ss << heightNr++; // transfer unsigned int to stream
        number = ss.str();
        //  set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        //  finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);
    }
    
    //  draw mesh:
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLuint)numVertices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    //  reset texture unit to default
    glActiveTexture(GL_TEXTURE0);
    
}

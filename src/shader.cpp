//
//  shader.cpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include "proto.hpp"


/*  constructor  */

Shader::Shader (const char* vPath, const char* fPath, const char* gPath=nullptr)
{
    // (1) get f/v-shader source code from file path
    std::string vSource;
    std::string fSource;
    std::string gSource;
    std::ifstream vFile;
    std::ifstream fFile;
    std::ifstream gFile;
    const char* vCode;
    const char* fCode;
    const char* gCode;

    //  ensure ifstream objects can throw exceptions
    vFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    gFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        //  open files
        vFile.open(vPath);
        fFile.open(fPath);


        //  read file contents into stringstreams
        std::stringstream vStream, fStream, gStream;
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();

        //  close files
        vFile.close();
        fFile.close();

        //  get string from stream
        vSource = vStream.str();
        fSource = fStream.str();

        //  check if specified since geo shader is optional
        if(gPath!=nullptr)
        {
            gFile.open(gPath);
            gStream << gFile.rdbuf();
            gFile.close();
            gSource = gStream.str();
            gCode = gSource.c_str();

        }
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    vCode = vSource.c_str();
    fCode = fSource.c_str();

    //  (2) now compile shaders
    unsigned int vID, fID, gID=NULL;

    //  vertex shader
    vID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vID, 1, &vCode, NULL);
    glCompileShader(vID);
    checkCompileErrors(vID, "VERTEX");

    //  fragment shader
    fID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fID, 1, &fCode, NULL);
    glCompileShader(fID);
    checkCompileErrors(fID, "FRAGMENT");

    //  if specified, compile geometry shader as well

    if(gPath != nullptr)
    {
        gID = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gID, 1, &gCode, NULL);
        glCompileShader(gID);
        checkCompileErrors(gID, "GEOMETRY");
    }

    //  shader program creation

    ID = glCreateProgram();

    //  attach shaders to program
    glAttachShader(ID, vID);
    glAttachShader(ID, fID);
    if (gPath != nullptr)
        glAttachShader(ID, gID);
    glLinkProgram(ID);
    checkCompileErrors (ID, "PROGRAM");

    //  shaders no longer needed -> delete
    glDeleteShader(vID);
    glDeleteShader(fID);
    if(gPath != nullptr)
        glDeleteShader(gID);

}


/*  functions  */

void Shader::use()
{
    glUseProgram(ID);
}


/* uniform setters  */

//  bool
void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

//  int
void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

//  float
void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

//  vec2
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

//  vec3
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

//  vec4
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

//  mat2
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

//  mat3
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

//  mat4
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


/*  error checking  */

void Shader::checkCompileErrors (GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM")
    {
        glGetShaderiv (shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

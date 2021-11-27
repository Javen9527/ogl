#include "ShaderManager.h"

ShaderManager::ShaderManager(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    std::ifstream is;
    is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        is.open(vertexPath);
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        is.close();
        vertexCode = content;

        is.open(fragmentPath);
        std::stringstream fStream;
        fStream << is.rdbuf();
        is.close();
        fragmentCode = fStream.str();

        // load geometry shader
        if (geometryPath != nullptr)
        {
            is.open(geometryPath);
            std::stringstream gStream;
            gStream << is.rdbuf();
            is.close();
            geometryCode = gStream.str();
        }
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    
    unsigned int vertex, fragment;
    // vertex
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, eVERTEX);

    // fragment
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, eFRAGMENT);
    
    unsigned int geometry;
    if (geometryPath != nullptr)
    {
        const char *gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, eGEOMETRY);
    }

    // Program
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    if (geometryPath != nullptr)
        glAttachShader(m_id, geometry);
    glLinkProgram(m_id);
    checkCompileErrors(m_id, ePROGRAM);
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr)
        glDeleteShader(geometry);
}

void ShaderManager::checkCompileErrors(const GLuint shader, const eShaderType type)
{
    GLint success;
    GLchar infoLog[1024];

    switch (type)
    {
    case eVERTEX:
    case eFRAGMENT:
    case eGEOMETRY:
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
        break;
    }
    case ePROGRAM:
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
        break;
    }
    default:
        break;
    }
}

// utility uniform functions
inline void ShaderManager::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}
inline void ShaderManager::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}
inline void ShaderManager::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}
inline void ShaderManager::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}
inline void ShaderManager::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
}
inline void ShaderManager::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}
inline void ShaderManager::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}
inline void ShaderManager::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}
inline void ShaderManager::setVec4(const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
}
inline void ShaderManager::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
inline void ShaderManager::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
inline void ShaderManager::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
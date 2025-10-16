#include "program.h"

ProgramUPtr Program::Create(
    const std::vector<ShaderPtr>& shaders) {
    auto program = ProgramUPtr(new Program());
    if (!program->Link(shaders))
        return nullptr;
    return std::move(program);
}

bool Program::Link(
    const std::vector<ShaderPtr>& shaders) {
    m_program = glCreateProgram();
    for (auto& shader: shaders)
        glAttachShader(m_program, shader->Get());
    glLinkProgram(m_program);

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to link program: {}", infoLog);
        return false;
    }

    return true;
}

void Program::SetUniform(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

void Program::SetUniform(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}



void Program::Use() const {
    glUseProgram(m_program);
}

Program::~Program() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
}

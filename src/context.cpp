#include "context.h"
#include "image.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    auto projection = glm::perspective(glm::radians(45.0f),
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);

    auto view = glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f));

    auto model = glm::rotate(glm::mat4(1.0f),
        glm::radians((float)glfwGetTime() * 120.0f),
        // glm::radians(100.0f),
        glm::vec3(1.0f, 0.5f, 0.0f));

    auto transform = projection * view * model;

    m_program->SetUniform("transform", transform);
    
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

bool Context::Init() {
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
    };

    uint32_t indices[] = {
         0,  2,  1,  2,  0,  3,
         4,  5,  6,  6,  7,  4,
         8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices, sizeof(float) * 120);

    // m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

    m_indexBuffer = Buffer::CreateWithData(
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices, sizeof(uint32_t) * 36);

    ShaderPtr vertShader = Shader::CreateFromFile("../../shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("../../shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    //{}로 리스트 바로 전달가능
    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    auto image = Image::Load("../../image/container.jpg");
    if (!image) 
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels",
        image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    // checker image as texture
    // auto image = Image::Create(512, 512);
    // image->SetCheckImage(16, 16);

    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("../../image/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    glm::vec4 vec(1.0f,  0.0f, 0.0f, 1.0f);
    auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    auto rot = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));
    vec = trans * rot * scale * vec;
    SPDLOG_INFO("transformed vec: [{}, {}, {}]", vec.x, vec.y, vec.z);


    // glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);
    // glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);

    // auto transform = glm::rotate(
    //     glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)),
    //     glm::radians(90.f), glm::vec3(0.0f,0.0f, 1.0f));

//     
// glEnable(GL_CULL_FACE);

    //x축으로 -55도 회전
    // auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //카메라는 원점으로부터 z축 방향으로 -3만큼 떨어짐
    // auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    //종횡비 4:3, 세로화각 45도의 원근투영
    // auto projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
    // auto transform = projection * view * model;
    // auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
    // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    // m_program->SetUniform("transform", transform);

    // //위치(1,0,0)의 점. 동차 좌표계 사용
    // glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    // //단위행열 기준 (1,1,0)만큼 평행이동하는 행렬
    // auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    // //단위행렬 기준 z축으로 90도 만큼 회전하는 행렬
    // auto rot = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // //단위행렬기준 모든 축에 대해 3배율 확대하는 행렬
    // auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));
    // //확대 -> 회전 -> 평행이동 순으로 점에 선형 변환 적용
    // vec = trans * rot * scale * vec;
    // //(3,0,0) - > (0,3,0) -> (1,4,0)
    // SPDLOG_INFO("transformed vec: [{}, {}, {}]", vec.x, vec.y, vec.z);

    return true;
}

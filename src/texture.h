#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture {
public:
    //image로 부터 텍스쳐를 만들어서 unique pointer return
    static TextureUPtr CreateFromImage(const Image* image);
    ~Texture();

    const uint32_t Get() const { return m_texture; }
    void Bind() const;
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;
    
private:
    Texture() {}
    void CreateTexture();
    void SetTextureFromImage(const Image* image);

    //texture ID가지고있는 멤버변수
    uint32_t m_texture { 0 };
};

#endif // __TEXTURE_H__
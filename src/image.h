#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"

CLASS_PTR(Image)
class Image {
public:
    static ImageUPtr Load(const std::string& filepath);
    static ImageUPtr Create(int width, int height, int channelCount = 4);
    ~Image();

    //함수 뒤의 const: 이 함수는 클래스의 멤버 변수(m_width 등)를 변경하지 않는다는 약속입니다. 이렇게 하면 실수로 데이터를 바꾸는 것을 막을 수 있습니다.
    const uint8_t* GetData() const { return m_data; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }

    void SetCheckImage(int gridX, int gridY);
    
private:
    Image() {};
    bool LoadWithStb(const std::string& filepath);
    bool Allocate(int width, int height, int channelCount);
    
    //{ 0 }, { nullptr }: C++11부터 도입된 문법으로, 변수를 선언과 동시에 기본값으로 초기화합니다.
    int m_width { 0 };
    int m_height { 0 };
    int m_channelCount { 0 };
    uint8_t* m_data { nullptr };
};

#endif // __IMAGE_H__
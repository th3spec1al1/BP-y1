#ifndef CBMP_HPP
#define CBMP_HPP

#include <iostream>
#include <fstream>
#include <stdint.h>

struct Color{
    float r, g, b;

    Color();
    Color(float r, float g, float b);
};

class Image{
public:
    Image(int width, int height);
    ~Image();

    Color GetColor(int x, int y) const;
    void SetColor(const Color& color, int x, int y);

    void Create(const char* path) const;

private:
    int m_width;
    int m_height;

    Color* m_colors; 
};

#endif
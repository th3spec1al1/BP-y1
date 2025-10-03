#include "createBMP.hpp"

Color::Color() :r(0), g(0), b(0){
}

Color::Color(float r, float g, float b) :r(r), g(g), b(b){
}

Image::Image(int width, int height) :m_width(width), m_height(height), m_colors(new Color[width*height]){
}
Image::~Image(){
    delete[] m_colors;
}

Color Image::GetColor(int x, int y) const{
    return m_colors[y*m_width + x];
}

void Image::SetColor(const Color &color, int x, int y){
    m_colors[y*m_width + x].r = color.r;
    m_colors[y*m_width + x].g = color.g;
    m_colors[y*m_width + x].b = color.b;
}

void Image::Create(const char *path) const{
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary);

    if (!file.is_open()){
        printf("Error: Cannot open file: %s\n", path);
        exit(1);
    }

    unsigned char bmpPad[3] = {0, 0, 0};
    const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);

    const int fileHeaderSize = 14;
    const int informationHeaderSize = 40;
    const int fileSize = fileHeaderSize + informationHeaderSize + m_width * m_height * 3 + paddingAmount * m_height;

    unsigned char fileHeader[fileHeaderSize];

    fileHeader[0] = 'B';
    fileHeader[1] = 'M';

    fileHeader[2] = fileSize;
    fileHeader[3] = fileSize >> 8;
    fileHeader[4] = fileSize >> 16;
    fileHeader[5] = fileSize >> 24;

    fileHeader[6] = 0;
    fileHeader[7] = 0;

    fileHeader[8] = 0;
    fileHeader[9] = 0;

    fileHeader[10] = fileHeaderSize + informationHeaderSize;
    fileHeader[11] = 0;
    fileHeader[12] = 0;
    fileHeader[13] = 0;

    unsigned char InformationHeader[informationHeaderSize];

    InformationHeader[0] = informationHeaderSize;
    InformationHeader[1] = 0;
    InformationHeader[2] = 0;
    InformationHeader[3] = 0;

    InformationHeader[4] = m_width;
    InformationHeader[5] = m_width >> 8;
    InformationHeader[6] = m_width >> 16;
    InformationHeader[7] = m_width >> 24;

    InformationHeader[8] = m_height;
    InformationHeader[9] = m_height >> 8;
    InformationHeader[10] = m_height >> 16;
    InformationHeader[11] = m_height >> 24;

    InformationHeader[12] = 1;
    InformationHeader[13] = 0;

    InformationHeader[14] = 24;
    InformationHeader[15] = 0;

    InformationHeader[16] = 0;
    InformationHeader[17] = 0;
    InformationHeader[18] = 0;
    InformationHeader[19] = 0;

    InformationHeader[20] = 0;
    InformationHeader[21] = 0;
    InformationHeader[22] = 0;
    InformationHeader[23] = 0;

    InformationHeader[24] = 0;
    InformationHeader[25] = 0;
    InformationHeader[26] = 0;
    InformationHeader[27] = 0;

    InformationHeader[28] = 0;
    InformationHeader[29] = 0;
    InformationHeader[30] = 0;
    InformationHeader[31] = 0;

    InformationHeader[32] = 0;
    InformationHeader[33] = 0;
    InformationHeader[34] = 0;
    InformationHeader[35] = 0;

    InformationHeader[36] = 0;
    InformationHeader[37] = 0;
    InformationHeader[38] = 0;
    InformationHeader[39] = 0;

    file.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
    file.write(reinterpret_cast<char*>(InformationHeader), informationHeaderSize);

    for (int y=0; y<m_height; y++){
        for(int x=0; x<m_width; x++){
            unsigned char r = static_cast<unsigned char>(GetColor(x,y).r * 255.0f);
            unsigned char g = static_cast<unsigned char>(GetColor(x,y).g * 255.0f);
            unsigned char b = static_cast<unsigned char>(GetColor(x,y).b * 255.0f);

            unsigned char color[] = {b, g, r};

            file.write(reinterpret_cast<char*>(color), 3);
        }

        file.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
    }

    file.close();
}

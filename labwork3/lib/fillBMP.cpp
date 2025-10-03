#include <iostream>
#include <fstream>
#include "fillBMP.hpp"

void Fill(const int width, const int height, const char *path, uint64_t**& matrix){

    Image image(width, height);

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){

            if (matrix[y][x] == 0){
                image.SetColor(Color(1.0f, 1.0f, 1.0f), x, y);
            }
            else if (matrix[y][x] == 1){
                image.SetColor(Color(0.0f, 1.0f, 0.0f), x, y);
            }
            else if (matrix[y][x] == 2){
                image.SetColor(Color(1.0f, 0.0f, 1.0f), x, y);
            }
            else if (matrix[y][x] == 3){
                image.SetColor(Color(1.0f, 1.0f, 0.0f), x, y);
            }
            else if (matrix[y][x] >= 4){
                image.SetColor(Color(0.0f, 0.0f, 0.0f), x, y);
            }
        }
    }

    image.Create(path);

    
}
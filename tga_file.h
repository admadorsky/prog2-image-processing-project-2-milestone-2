#ifndef MADORSKY_ANTHONY_PROJECT2_TGA_FILE_H
#define MADORSKY_ANTHONY_PROJECT2_TGA_FILE_H

#include <vector>
#include <string>
using namespace std ;

struct Header {
    char idLength ;
    char colorMapType ;
    char dataTypeCode ;
    short colorMapOrigin ;
    short colorMapLength ;
    char colorMapDepth ;
    short xOrigin ;
    short yOrigin ;
    short width ;
    short height ;
    char bitsPerPixel ;
    char imageDescriptor ;
};

struct Pixel {
    int red ;
    int green ;
    int blue ;
};

class tga_file {
public:
    Header header ;
    vector<Pixel> image_data ;

    Pixel ReadPixel(int b, int g, int r) ;
    void ReadTGAFile(string file_name) ;
    void WriteTGAFile(string file_name) ;

    tga_file Multiply(tga_file &top_layer) ;
    tga_file Subtract(tga_file &top_layer) ;
    tga_file Screen(tga_file &top_layer) ;
    tga_file Overlay(tga_file &top_layer) ;

    // channels: 0->blue 1->green 2->red
    tga_file AddByChannel(int addend, int channel) ;
    // channels: 0->blue 1->green 2->red
    tga_file MultiplyByChannel(int factor, int channel) ;

    tuple<tga_file, tga_file, tga_file> SeparateChannels() ;

    // input tuple: blue, green, red
    void CombineChannels(tuple<tga_file, tga_file, tga_file> channels) ;

    tga_file FlipImage() ;
};

#endif //MADORSKY_ANTHONY_PROJECT2_TGA_FILE_H
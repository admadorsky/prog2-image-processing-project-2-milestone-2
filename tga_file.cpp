#include "tga_file.h"
#include <iostream>
#include <fstream>
#include <tuple>

// stores integer rgb values into a Pixel data container
Pixel tga_file::ReadPixel(int b, int g, int r) {
    // initialize the container
    Pixel output_pixel ;
    // store values
    output_pixel.blue = b ;
    output_pixel.green = g ;
    output_pixel.red = r ;
    return output_pixel ;
}

void tga_file::ReadTGAFile(string file_name) {
    Header tga_header ;
    // try opening the file
    ifstream inTGA(file_name, ios::in | ios::binary) ;
    // if it fails to open, let the user know
    if (!inTGA.is_open()) {
        cout << "Invalid file name." << endl ;
    } else {
        // otherwise we start reading the file from the header
        inTGA.read((char*) &tga_header, 1) ;
        inTGA.read((char*) &(tga_header.colorMapType), 1) ;
        inTGA.read((char*) &(tga_header.dataTypeCode), 1) ;
        inTGA.read((char*) &(tga_header.colorMapOrigin), 2) ;
        inTGA.read((char*) &(tga_header.colorMapLength), 2) ;
        inTGA.read((char*) &(tga_header.colorMapDepth), 1) ;
        inTGA.read((char*) &(tga_header.xOrigin), 2) ;
        inTGA.read((char*) &(tga_header.yOrigin), 2) ;
        inTGA.read((char*) &(tga_header.width), 2) ;
        inTGA.read((char*) &(tga_header.height), 2) ;
        inTGA.read((char*) &(tga_header.bitsPerPixel), 1) ;
        inTGA.read((char*) &(tga_header.imageDescriptor), 1) ;

        this->header = tga_header ;

        // intialize rgb values
        int b = 0 ;
        int g = 0 ;
        int r = 0 ;

        // iterate through all pixels (count found by width x height) and read them into pixel containers
        for (int p = 0; p < (this->header.height * this->header.width); p++) {
            // read current bgr values from the file
            inTGA.read((char *) &b, 1);
            inTGA.read((char *) &g, 1);
            inTGA.read((char *) &r, 1);
            // add the current pixel container to the vector of all pixels
            this->image_data.push_back(ReadPixel(b, g, r));
        }
    }
}

void tga_file::WriteTGAFile(string file_name) {
    ofstream outTGA(file_name, ios::out | ios::binary) ;
    // try opening the file
    if (!outTGA.is_open()) {
        // if it fails to open, let the user know
        cout << "Invalid file name." << endl ;
    } else {
        // start writing to the new file from the header
        outTGA.clear() ;
        outTGA.write((char*) &this->header.idLength, 1) ;
        outTGA.write((char*) &this->header.colorMapType, 1) ;
        outTGA.write((char*) &this->header.dataTypeCode, 1) ;
        outTGA.write((char*) &this->header.colorMapOrigin, 2) ;
        outTGA.write((char*) &this->header.colorMapLength, 2) ;
        outTGA.write((char*) &this->header.colorMapDepth, 1) ;
        outTGA.write((char*) &this->header.xOrigin, 2) ;
        outTGA.write((char*) &this->header.yOrigin, 2) ;
        outTGA.write((char*) &this->header.width, 2) ;
        outTGA.write((char*) &this->header.height, 2) ;
        outTGA.write((char*) &this->header.bitsPerPixel, 1) ;
        outTGA.write((char*) &this->header.imageDescriptor, 1) ;

        // iterate through all pixels (count found by width x height) and write them into the file
        for (int p = 0; p < (this->header.height * this->header.width); p++) {
            outTGA.write((char*) &(this->image_data.at(p).blue), 1) ;
            outTGA.write((char*) &(this->image_data.at(p).green), 1) ;
            outTGA.write((char*) &(this->image_data.at(p).red), 1) ;
        }
    }
    outTGA.close() ;
}

// helper function to clip values at 255 on the high end, and at 0 on the low end
int Clamp(int input_val) {
    // initalize the output value
    int clamped ;
    // clip high values
    if (input_val > 255) { clamped = 255 ; }
        // clip low values
    else if (input_val < 0) { clamped = 0 ; }
        // otherwise, leave the value alone
    else { clamped = input_val ; }

    return clamped ;
}

// helper function to go from 0-255 integer values to 0-1 float values
tuple<float, float, float> Normalize(Pixel &input_pixel) {
    // divide each channel by its max value (255), and store as a float
    float b = (float)input_pixel.blue / 255.0f ;
    float g = (float)input_pixel.green / 255.0f ;
    float r = (float)input_pixel.red / 255.0f ;
    // return a tuple with rgb channels
    return {b, g, r} ;
}

// helper function to go from 0-1 float values to 0-255 integer values
Pixel UnNormalize(tuple<float, float, float> &input_norms) {
    // unnormalize the blue channel by multiplying by it's max value (255) and making use of the Clamp() function
    int b = Clamp((get<0>(input_norms) * 255) + 0.5f) ;
    // unnormalize the green channel
    int g = Clamp((get<1>(input_norms) * 255) + 0.5f) ;
    // unnormalize the red channel
    int r = Clamp((get<2>(input_norms) * 255) + 0.5f) ;
    // store the unnormalized values into a pixel data container & return it
    Pixel output ;
    output.blue = b ;
    output.green = g ;
    output.red = r ;
    return output ;
}

// formula: NP1 * NP2
tga_file tga_file::Multiply(tga_file &top_layer) {
    // initalize an output image
    tga_file multiplied ;
    multiplied.header = this->header ;
    // iterate through all pixels
    for (int i = 0; i < (this->header.width * this->header.height); i++) {
        // normalize both layers
        tuple<float, float, float> bottom_layer_normalized = Normalize(this->image_data.at(i)) ;
        tuple<float, float, float> top_layer_normalized = Normalize(top_layer.image_data.at(i)) ;
        // perform the multiplication with normalized channels
        float multiplied_blue = get<0>(bottom_layer_normalized) * get<0>(top_layer_normalized) ;
        float multiplied_green = get<1>(bottom_layer_normalized) * get<1>(top_layer_normalized) ;
        float multiplied_red = get<2>(bottom_layer_normalized) * get<2>(top_layer_normalized) ;
        // store the results in a tuple and unnormalize for output
        tuple<float, float, float> normalized_multiplied = {multiplied_blue, multiplied_green, multiplied_red} ;
        multiplied.image_data.push_back(UnNormalize(normalized_multiplied)) ;
    }
    return multiplied ;
}

// formula: P1 - P2
tga_file tga_file::Subtract(tga_file &top_layer) {
    // initalize an output image
    tga_file subtracted ;
    subtracted.header = this->header ;
    // iterate through all pixels
    for (int i = 0; i < (this->header.width * this->header.height); i++) {
        // initalize a pixel data container
        Pixel sub_pixel ;
        // perform the subtraction making use of the Clamp() function
        sub_pixel.blue = Clamp(top_layer.image_data.at(i).blue - this->image_data.at(i).blue) ;
        sub_pixel.green = Clamp(top_layer.image_data.at(i).green - this->image_data.at(i).green) ;
        sub_pixel.red = Clamp(top_layer.image_data.at(i).red - this->image_data.at(i).red) ;
        // add the results to the output image's list of pixels
        subtracted.image_data.push_back(sub_pixel) ;
    }
    return subtracted ;
}

// formula: 1 - [1 - (1 - NP1) * (1 - NP2)]
tga_file tga_file::Screen(tga_file &top_layer) {
    // initalize an output image
    tga_file screened ;
    screened.header = this->header ;
    // iterate through all pixels
    for (int i = 0; i < (this->header.width * this->header.height); i++) {
        // normalize both layers
        tuple<float, float, float> bottom_layer_normalized = Normalize(this->image_data.at(i));
        tuple<float, float, float> top_layer_normalized = Normalize(top_layer.image_data.at(i));
        // perform the screen transformation with normalized values
        float screened_blue_norm = 1 - ((1 - get<0>(bottom_layer_normalized)) * (1 - get<0>(top_layer_normalized))) ;
        float screened_green_norm = 1 - ((1 - get<1>(bottom_layer_normalized)) * (1 - get<1>(top_layer_normalized))) ;
        float screened_red_norm = 1 - ((1 - get<2>(bottom_layer_normalized)) * (1 - get<2>(top_layer_normalized))) ;
        // store the results in a tuple and unnormalize for output
        tuple<float, float, float> normalized_screened = {screened_blue_norm, screened_green_norm, screened_red_norm} ;
        screened.image_data.push_back(UnNormalize(normalized_screened)) ;
    }
    return screened ;
}

// formula (brighter than 0.5): 1 - [2 * (1 - NP1) * (1 - NP2)]
// formula (dimmer than or exactly 0.5): 2 * NP1 * NP2
tga_file tga_file::Overlay(tga_file &top_layer) {
    // initalize an output image
    tga_file overlayed ;
    overlayed.header = this->header ;
    // iterate through all pixels
    for (int i = 0; i < (this->header.width) * (this->header.height); i++) {
        // normalize both layers
        tuple<float, float, float> top_layer_normalized = Normalize(this->image_data.at(i));
        tuple<float, float, float> bottom_layer_normalized = Normalize(top_layer.image_data.at(i));
        // initalize resulting normal channels
        float overlayed_blue_norm ;
        float overlayed_green_norm ;
        float overlayed_red_norm ;
        // perform the overlay transformation with normalized values
        // for blue:
        if (get<0>(bottom_layer_normalized) <= 0.5) { overlayed_blue_norm =
                                                              2 * get<0>(bottom_layer_normalized) * get<0>(top_layer_normalized) ; }
        else { overlayed_blue_norm =
                       1 - (2 * (1 - get<0>(top_layer_normalized)) * (1 - get<0>(bottom_layer_normalized))) ; }
        // for green:
        if (get<1>(bottom_layer_normalized) <= 0.5) { overlayed_green_norm =
                                                              2 * get<1>(bottom_layer_normalized) * get<1>(top_layer_normalized) ; }
        else { overlayed_green_norm =
                       1 - (2 * (1 - get<1>(top_layer_normalized)) * (1 - get<1>(bottom_layer_normalized))) ; }
        // for red:
        if (get<2>(bottom_layer_normalized) <= 0.5) { overlayed_red_norm =
                                                              2 * get<2>(bottom_layer_normalized) * get<2>(top_layer_normalized) ; }
        else { overlayed_red_norm =
                       1 - (2 * (1 - get<2>(top_layer_normalized)) * (1 - get<2>(bottom_layer_normalized))) ; }
        // store the results in a tuple and unnormalize for output
        tuple<float, float, float> normalized_overlayed = {overlayed_blue_norm, overlayed_green_norm, overlayed_red_norm} ;
        overlayed.image_data.push_back(UnNormalize(normalized_overlayed)) ;
    }
    return overlayed ;
}

/**
 * add a certain amount to a specific channel
 * @param addend integer describing how much to add
 * @param channel 0:blue, 1:green, 2:red
 * @return tga_file with edited and clamped values
 */
tga_file tga_file::AddByChannel(int addend, int channel) {
    // initalize an output image
    tga_file added ;
    added.header = this->header ;
    added.image_data = this->image_data ;
    // iterate through all pixels
    for (int i = 0; i < (this->header.width) * (this->header.height); i++) {
        // add to the blue channel, copy the others
        if (channel == 0) {
            added.image_data.at(i).blue = Clamp(this->image_data.at(i).blue + addend) ;
            added.image_data.at(i).green = this->image_data.at(i).green ;
            added.image_data.at(i).red = this->image_data.at(i).red ;
            // add to the green channel, copy the others
        } else if (channel == 1) {
            added.image_data.at(i).blue = this->image_data.at(i).blue ;
            added.image_data.at(i).green = Clamp(this->image_data.at(i).green + addend) ;
            added.image_data.at(i).red = this->image_data.at(i).red ;
            // add to the red channel, copy the others
        } else if (channel == 2) {
            added.image_data.at(i).blue = this->image_data.at(i).blue ;
            added.image_data.at(i).green = this->image_data.at(i).green ;
            added.image_data.at(i).red = Clamp(this->image_data.at(i).red + addend) ;
        }
    }
    return added ;
}

/**
 * multiply a certain channel by a specific amount
 * @param factor integer describing how much to multiply by
 * @param channel 0:blue, 1:green, 2:red
 * @return tga_file with edited and clamped values
 */
tga_file tga_file::MultiplyByChannel(int factor, int channel) {
    // initalize an output image
    tga_file multiplied ;
    multiplied.header = this->header ;
    multiplied.image_data = this->image_data ;
    // iterate through all pixels
    for (int i = 0; i < (this->header.width) * (this->header.height); i++) {
        // multiply the blue channel, copy the others
        if (channel == 0) {
            multiplied.image_data.at(i).blue = Clamp(this->image_data.at(i).blue * factor) ;
            multiplied.image_data.at(i).green = this->image_data.at(i).green ;
            multiplied.image_data.at(i).red = this->image_data.at(i).red ;
            // multiply the green channel, copy the others
        } else if (channel == 1) {
            multiplied.image_data.at(i).blue = this->image_data.at(i).blue ;
            multiplied.image_data.at(i).green = Clamp(this->image_data.at(i).green * factor) ;
            multiplied.image_data.at(i).red = this->image_data.at(i).red ;
            // multiply the red channel, copy the others
        } else if (channel == 2) {
            multiplied.image_data.at(i).blue = this->image_data.at(i).blue ;
            multiplied.image_data.at(i).green = this->image_data.at(i).green ;
            multiplied.image_data.at(i).red = Clamp(this->image_data.at(i).red * factor) ;
        }
    }
    return multiplied ;
}

/**
 * separate an image into each of its three color channels
 * @return tuple = {blue channel, green channel, red channel}
 */
tuple<tga_file, tga_file, tga_file> tga_file::SeparateChannels() {
    // initalize an image for the blue channel
    tga_file blue_channel ;
    blue_channel.header = this->header ;
    blue_channel.image_data = this->image_data ;
    // initalize an image for the green channel
    tga_file green_channel ;
    green_channel.header = this->header ;
    green_channel.image_data = this->image_data ;
    // initalize an image for the red channel
    tga_file red_channel ;
    red_channel.header = this->header ;
    red_channel.image_data = this->image_data ;
    // iterate through all pixels
    for (int i = 0; i < (this->header.width) * (this->header.height); i++) {
        // use blue data for all 3 channels of the blue_channel image
        blue_channel.image_data.at(i).blue = this->image_data.at(i).blue ;
        blue_channel.image_data.at(i).green = this->image_data.at(i).blue ;
        blue_channel.image_data.at(i).red = this->image_data.at(i).blue ;
        // use green data for all 3 channels of the green_channel image
        green_channel.image_data.at(i).blue = this->image_data.at(i).green ;
        green_channel.image_data.at(i).green = this->image_data.at(i).green ;
        green_channel.image_data.at(i).red = this->image_data.at(i).green ;
        // use red data for all 3 channels of the red_channel image
        red_channel.image_data.at(i).blue = this->image_data.at(i).red ;
        red_channel.image_data.at(i).green = this->image_data.at(i).red ;
        red_channel.image_data.at(i).red = this->image_data.at(i).red ;
    }
    return {blue_channel, green_channel, red_channel} ;
}

// combine separate images for r, g, and b channels into one image
void tga_file::CombineChannels(tuple<tga_file, tga_file, tga_file> channels) {
    // modify the object's header to match one of the channels
    this->header = get<0>(channels).header ;
    this->image_data = get<0>(channels).image_data ;
    // iterate through all pixels
    for (int i = 0; i < (this->header.width) * (this->header.height); i++) {
        // copy each of the pixel's channels
        this->image_data.at(i).blue = get<0>(channels).image_data.at(i).blue ;
        this->image_data.at(i).green = get<1>(channels).image_data.at(i).green ;
        this->image_data.at(i).red = get<2>(channels).image_data.at(i).red ;
    }
}

// rotate an image 180 degrees
tga_file tga_file::FlipImage() {
    // initalize an output image
    tga_file flipped ;
    flipped.header = this->header ;
    flipped.image_data = this->image_data ;
    // initialize a backwards counter... starts at the last pixel's index
    int j = (this->header.width) * (this->header.height) - 1 ;
    // iterate through all pixels
    for (int i = 0; i < (this->header.width) * (this->header.height); i++) {
        // copy each pixel's data, making use of the backwards counter
        flipped.image_data.at(j).blue = this->image_data.at(i).blue ;
        flipped.image_data.at(j).green = this->image_data.at(i).green ;
        flipped.image_data.at(j).red = this->image_data.at(i).red ;
        // iterate the backwards counter
        j-- ;
    }
    return flipped ;
}
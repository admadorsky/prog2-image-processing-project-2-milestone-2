#include <iostream>
#include <vector>
#include <fstream>
#include <tuple>
#include "tga_file.h"

using namespace std ;

void PrintHelpMessage() {
    cout << "Project 2: Image Processing, Summer 2023\n"
            "\nUsage:\n"
            "\t./project2.out [output] [firstImage] [method] [...]" << endl ;
}

bool CheckInFilePath(string file_path) {
    ifstream inTGA(file_path, ios::in | ios::binary) ;
    if (!inTGA.is_open()) {
        return false ;
    }
    return true ;
}

bool CheckOutFilePath(string file_path) {
    ofstream outTGA(file_path, ios::in | ios::binary) ;
    if (!outTGA.is_open()) {
        return false ;
    }
    return true ;
}

 /**
  * @param str
  * @return -1 indicates error state
  */
int StringToInt(string str) {
    int integer = -1 ;
    try {
        integer = stoi(str) ;
    } catch (invalid_argument) {
        cout << "Invalid argument, expected number." << endl ;
    }
    return integer ;
}

int main(int argc, char* argv[]) {

    int blue = 0 ;
    int green = 1 ;
    int red = 2 ;

    vector<string> arguments ;
    for (int i = 1; i < argc; i++) {
        string current_argument = argv[i] ;
        arguments.push_back(current_argument) ;
    }

    if (arguments.size() == 0) {
        PrintHelpMessage();
        return 0;
    }

    if (arguments.at(0) == "--help") {
        PrintHelpMessage();
        return 0;
    }

    tga_file tracking_image;
    // check if the user didn't pass any arguments
    if (arguments.size() < 1) {
        PrintHelpMessage();
        return 0 ;
    } else if (arguments.size() == 1) {
        cout << "Invalid file name." << endl;
        return 0 ;
    }

    // read the input file and the name for the output image
    string output_file_path = arguments.at(0);
    string input_file_path = arguments.at(1);

    if (!CheckInFilePath(input_file_path)) {
        cout << "Invalid file name." << endl ;
        return 0 ;
    }

    /* FOR DEBUGGING PURPOSES
    cout << "output path: " << output_file_path << endl ;
    cout << "input path: " << input_file_path << endl ; */
    // initialize and read the input file into tracking_image
    tracking_image.ReadTGAFile(input_file_path);
    // iterate through the passed arguments
    for (int i = 2; i < arguments.size();) {
        // MULTIPLY ------------------------------------------------------------------------------------------------
        if (arguments.at(i) == "multiply") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // store the user-specified path, with error handling
            string top_layer_filename = arguments.at(i + 1);
            if (!CheckInFilePath(top_layer_filename)) {
                cout << "Invalid argument, file does not exist." << endl ;
                return 0 ;
            }

            // use the passed file path to create a tga_image object
            tga_file top_layer;
            top_layer.ReadTGAFile(top_layer_filename);

            //print what the program is doing
            if (i < 3) { cout << "Multiplying " << input_file_path << " and " << arguments.at(i + 1) << " ..." << endl;
            } else { cout << "... and multiplying previous step and " << arguments.at(i + 1) << " ..." << endl; }

            // multiply the tracking image by top_layer
            tracking_image = tracking_image.Multiply(top_layer);

            // skip to the next method call
            i += 2;

        // SUBTRACT ----------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "subtract") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // store the user-specified path, with error handling
            string top_layer_filename = arguments.at(i + 1);
            if (!CheckInFilePath(top_layer_filename)) {
                cout << "Invalid argument, file does not exist." << endl ;
                return 0 ;
            }

            // use the passed file path to create a tga_image object
            tga_file top_layer;
            top_layer.ReadTGAFile(top_layer_filename);

            //print what the program is doing
            if (i < 3) { cout << "Subtracting " << arguments.at(i + 1) << " from " << input_file_path << " ..." << endl;
            } else { cout << "... and subtracting " << arguments.at(i + 1) << " from previous step ..." << endl; }

            // subtract top_layer from the tracking image
            tracking_image = tracking_image.Subtract(top_layer);

            // skip to the next method call
            i += 2 ;

        // OVERLAY -------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "overlay") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // store the user-specified path, with error handling
            string top_layer_filename = arguments.at(i + 1);
            if (!CheckInFilePath(top_layer_filename)) {
                cout << "Invalid argument, file does not exist." << endl ;
                return 0 ;
            }

            // use the passed file path to create a tga_image object
            tga_file top_layer;
            top_layer.ReadTGAFile(top_layer_filename);

            //print what the program is doing
            if (i < 3) { cout << "Overlaying " << input_file_path << " and " << arguments.at(i + 1) << " ..." << endl;
            } else { cout << "... and overlaying previous step and " << arguments.at(i + 1) << " ..." << endl; }

            // overlay top_layer onto the tracking image
            tracking_image = tracking_image.Overlay(top_layer);

            // skip to the next method call
            i += 2 ;

        // SCREEN --------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "screen") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // store the user-specified path, with error handling
            string top_layer_filename = arguments.at(i + 1);
            if (!CheckInFilePath(top_layer_filename)) {
                cout << "Invalid argument, file does not exist." << endl ;
                return 0 ;
            }

            // use the passed file path to create a tga_image object
            tga_file top_layer;
            top_layer.ReadTGAFile(top_layer_filename);

            //print what the program is doing
            if (i < 3) { cout << "Screening " << input_file_path << " and " << arguments.at(i + 1) << " ..." << endl;
            } else { cout << "... and screening previous step and " << arguments.at(i + 1) << " ..." << endl; }

            // subtract top_layer from the tracking image
            tracking_image = top_layer.Screen(tracking_image);

            // skip to the next method call
            i += 2 ;

        // COMBINE -------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "combine") {

            // make sure two parameters were passed after the function argument
            if (i >= arguments.size() - 2) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // store the user-specified path, with error handling
            string green_channel_filename = arguments.at(i + 1);
            string blue_channel_filename = arguments.at(i + 2);

            if (!CheckInFilePath(green_channel_filename) || !CheckInFilePath(blue_channel_filename)) {
                cout << "Invalid argument, file does not exist." << endl ;
                return 0 ;
            }

            // use the passed file path to create a tga_image object
            tga_file green_channel;
            tga_file blue_channel;
            green_channel.ReadTGAFile(green_channel_filename);
            blue_channel.ReadTGAFile(blue_channel_filename);

            // print what the program is doing
            if (i < 3) { cout << "Combining " << input_file_path << " and " << arguments.at(i + 1) << " and " << arguments.at(i + 2) << " ..." << endl;
            } else { cout << "... and combining previous step and " << arguments.at(i + 1) << " and " << arguments.at(i + 2) << " ..." << endl; }

            // combine the r, g, and b channels
            tracking_image.CombineChannels({blue_channel, green_channel, tracking_image});

            // skip to the next method call
            i += 3 ;

        // FLIP ----------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "flip") {
            //print what the program is doing
            if (i < 3) { cout << "Flipping " << input_file_path << " ..." << endl;
            } else { cout << "... and flipping output of previous step ..." << endl; }

            // flip the tracking image
            tracking_image = tracking_image.FlipImage();

            // skip to the next method call
            i++ ;

        // ONLYRED -----------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "onlyred") {

            // print what the program is doing
            if (i < 3) { cout << "Retrieving the red channel of " << input_file_path << " ..." << endl;
            } else { cout << "... and retrieving the red channel of previous step ..." << endl; }

            // retrieve the separate channels and overwrite the tracking image with the red channel
            tuple<tga_file, tga_file, tga_file> separated_channels = tracking_image.SeparateChannels();
            tracking_image = get<2>(separated_channels);

            // skip to the next method call
            i++ ;

        // ONLYGREEN ---------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "onlygreen") {
            // print what the program is doing
            if (i < 3) cout << "Retrieving the green channel of " << input_file_path << " ..." << endl;
            else cout << "... and retrieving the green channel of previous step ..." << endl;

            // retrieve the separate channels and overwrite the tracking image with the green channel
            tuple<tga_file, tga_file, tga_file> separated_channels = tracking_image.SeparateChannels();
            tracking_image = get<1>(separated_channels);

            // skip to the next method call
            i++ ;

        // ONLYBLUE ----------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "onlyblue") {
            // print what the program is doing
            if (i < 3) cout << "Retrieving the blue channel of " << input_file_path << " ..." << endl;
            else cout << "... and retrieving the blue channel of previous step ..." << endl;

            // retrieve the separate channels and overwrite the tracking image with the blue channel
            tuple<tga_file, tga_file, tga_file> separated_channels = tracking_image.SeparateChannels();
            tracking_image = get<0>(separated_channels);

            // skip to the next method call
            i++ ;

        // ADDRED ------------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "addred") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // try converting parameter to int, with error handling
            int addend = StringToInt(arguments.at(i + 1)) ;
            if (addend == -1) {
                return 0;
            }

            // print what the program is doing
            if (i < 3) cout << "Adding +" << addend << " to red channel of " << input_file_path << " ..." << endl;
            else cout << "... and adding +" << addend << " to red channel of previous step ..." << endl;

            // add the appropriate value to the red channel of the tracking image
            tracking_image = tracking_image.AddByChannel(addend, red);

            // skip to the next method call
            i += 2 ;

        // ADDGREEN ----------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "addgreen") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // try converting parameter to int, with error handling
            int addend = StringToInt(arguments.at(i + 1)) ;
            if (addend == -1) {
                return 0;
            }

            // print what the program is doing
            if (i < 3) { cout << "Adding +" << addend << " to green channel of " << input_file_path << " ..." << endl;
            } else { cout << "... and adding +" << addend << " to green channel of previous step ..." << endl; }

            // add the appropriate value to the green channel of the tracking image
            tracking_image = tracking_image.AddByChannel(addend, green);

            // skip to the next method call
            i += 2 ;

        // ADDBLUE -------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "addblue") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // try converting parameter to int, with error handling
            int addend = StringToInt(arguments.at(i + 1)) ;
            if (addend == -1) {
                return 0;
            }

            // print what the program is doing
            if (i < 3) {
                cout << "Adding +" << addend << " to blue channel of " << input_file_path << " ..." << endl;
            } else {
                cout << "... and adding +" << addend << " to blue channel of previous step ..." << endl;
            }

            // add the appropriate value to the blue channel of the tracking image
            tracking_image = tracking_image.AddByChannel(addend, blue);

            // skip to the next method call
            i += 2 ;

        // SCALERED ------------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "scalered") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // try converting parameter to int, with error handling
            int factor = StringToInt(arguments.at(i + 1)) ;
            if (factor == -1) {
                return 0;
            }

            // print what the program is doing
            if (i < 3) { cout << "Scaling the red channel of" << input_file_path << " by " << factor << " ..." << endl;
            } else { cout << "... and scaling the red channel of previous step by " << factor << " ..." << endl; }

            // multiply the red channel by the appropriate value
            tracking_image = tracking_image.MultiplyByChannel(factor, red);

            // skip to the next method call
            i += 2 ;

        // SCALEGREEN ----------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "scalegreen") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // try converting parameter to int, with error handling
            int factor = StringToInt(arguments.at(i + 1)) ;
            if (factor == -1) {
                return 0;
            }

            // print what the program is doing
            if (i < 3) { cout << "Scaling the green channel of" << input_file_path << " by " << factor << " ..." << endl;
            } else { cout << "... and scaling the green channel of previous step by " << factor << " ..." << endl; }

            // multiply the red channel by the appropriate value
            tracking_image = tracking_image.MultiplyByChannel(factor, green);

            // skip to the next method call
            i += 2 ;

        // SCALEBLUE -----------------------------------------------------------------------------------------------
        } else if (arguments.at(i) == "scaleblue") {
            // make sure a parameter was passed after the function argument
            if (i >= arguments.size() - 1) {
                cout << "Missing argument." << endl;
                return 0;
            }

            // try converting parameter to int, with error handling
            int factor = StringToInt(arguments.at(i + 1)) ;
            if (factor == -1) {
                return 0;
            }

            // print what the program is doing
            if (i < 3) { cout << "Scaling the blue channel of" << input_file_path << " by " << factor << " ..." << endl;
            } else { cout << "... and scaling the blue channel of previous step by " << factor << " ..." << endl; }

            // multiply the red channel by the appropriate value
            tracking_image = tracking_image.MultiplyByChannel(factor, blue);

            // skip to the next method call
            i += 2 ;
        } else {
            cout << "Invalid method name." << endl ;
            return 0;
        }
    }
    cout << "... and saving output to " << output_file_path << "!" << endl;
    tracking_image.WriteTGAFile(output_file_path);

    return 0 ;
}

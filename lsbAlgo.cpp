#include <iostream>
#include <fstream>

//#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

// Checks whether the bit is set or not at a particular position.
// Returns true if set
// Returns false if not set
bool isBitSet(char ch, int pos) {
	// 7 6 5 4 3 2 1 0
	ch = ch >> pos;
	if(ch & 1)
		return true;
	return false;
}

int funcEncode(int argc, char** argv) {

	/*
	./encode inputImage.png textfile.txt output_Image.png
	argv[0] = inputImage.png
	argv[1] = textfile.txt
	argv[2] = output_Image.png
	*/

	// Checks if proper number of arguments are passed
	if(argc != 3) {
		cout << "Please enter the correct number of arguments!" << "\n";
		exit(-1);
	}

	// Stores original inputImage
	Mat inputImage = imread(argv[0]);
	if(inputImage.empty()) {
		cout << "Error in Image!\n";
		exit(-1);
	}

	// Open file for text information
	ifstream file(argv[1]);
	if(!file.is_open()) {
		cout << "Unable to open file!\n";
		exit(-1);
	}

	// char to work on
	char ch;
	// reads the first char from the file
	file.get(ch);
	// contains information about which bit of char to work on
	int bit_count = 0;
	// to check whether file has ended
	bool null_char = false;
	// to check if the whole message is encoded or not
	bool encoded = false;

	/*
	To hide text into inputImages. We are taking one char (8 bits) and each of the 8 bits are stored
	in the Least Significant Bits (LSB) of the pixel values (Red,Green,Blue).
	We are manipulating bits in such way that changing LSB of the pixel values will not make a huge difference.
	The inputImage will still look similiar to the naked eye.
	*/

    rowLoop:
	for(int r=inputImage.rows-1; r>=0; r--) {
		for(int c=inputImage.cols; c >= 0; c--} {
			for(int color=0; color < 3; color++) {

				// stores the pixel details
				Vec3b pixel = inputImage.at<Vec3b>(Point(r,c));

				// if bit is 1 : change LSB of present color value to 1.
				// if bit is 0 : change LSB of present color value to 0.
				if(isBitSet(ch,7-bit_count))
					pixel.val[color] |= 1;
				else
					pixel.val[color] &= 0;

				// update the inputImage with the changed pixel values
				inputImage.at<Vec3b>(Point(r,c)) = pixel;

				// increment bit_count to work on next bit
				bit_count++;

				// if null_char is true and bit_count is 8, then our message is successfully encode.
				if(null_char && bit_count == 8) {
					encoded  = true;
					break rowLoop;
				}

				// if bit_count is 8 we pick the next char from the file and work on it
				if(bit_count == 8) {
					bit_count = 0;

                    //get next character from the file
					file.get(ch);

					// if EndOfFile(EOF) is encountered insert NULL char to the inputImage
					if(file.eof()) {
						null_char = true;
						ch = '\0';
					}
				}

			}
		}
	}

	// whole message was not encoded
	if(!encoded) {
		cout << "Message too big. Try with larger inputImage.\n";
		exit(-1);
	}

	// Writes the stegnographic Image
	imwrite(argv[2],inputImage);

    return 0;
}



int funcDecode(char* argv) {

	/*
	./decode output_Image.png
	argv= output_Image.png
	*/

	// Checks if proper number of arguments are passed
	if(argv==NULL) {
		cout << "Enter valid argument!" << "\n";
		exit(-1);
	}

	// Stores original inputImage
	Mat inputDImage = imread(argv[1]);
	if(inputDImage.empty()) {
		cout << "Error in image!\n";
		exit(-1);
	}

	// char to work on
	char ch=0;
	// contains information about which bit of char to work on
	int bit_count = 0;

	/*
	To extract the message from the inputImage, we will iterate through the pixels and extract the LSB of
	the pixel values (RGB) and this way we can get our message.
	*/
	for(int r=inputDImage.rows-1; r>=0; r--) {
		for(int c=inputDImage.cols; c >=0 ; c--) {
			for(int color=0; color < 3; color++) {

				// stores the pixel details
				Vec3b pixel = inputDImage.at<Vec3b>(Point(r,c));

				// manipulate char bits accolording to the LSB of pixel values
				if(isBitSet(pixel.val[color],0))
					ch |= 1;

				// increment bit_count to work on next bit
				bit_count++;

				// bit_count is 8, that means we got our char from the encoded inputImage
				if(bit_count == 8) {

					// NULL char is encountered
					if(ch == '\0')
						goto OUT;

					bit_count = 0;
					cout << ch;
					ch = 0;
				}
				else {
					ch = ch << 1;
				}

			}
		}
	}
	OUT:;


    return 0;
}

int main(){
    int choice=0;

    cout<<"Welcome to LSB Steganography!"<<endl;
    cout<<"Please choose one of the following options:"<<endl;
    cout<<"1. Encode \n";
    cout<<"2. Decode \n";
    cout<<"3. Exit\n";
    cin>>choice;

    switch(choice)
    {
        case 1:
        const char *str[3]={"inputImage.png","textFile.txt","outputImage.png"};
        funcEncode(*str);
        break;
        case 2:
        char strD[100]="inputDImage.png";
        funcDecode(strD);
        break;
        case 3:
        exit(0);
        default:
        cout<<"Please enter valid choice!"<<endl;
    }


    return 0;
}

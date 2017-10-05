/**
 * Recovers deleted images from memory card.
 */

#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[1];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    bool inJPG = false; //for determining if we've found the first
    int fileCount = 0; //for creating file names in order
    BYTE buffer [512]; //initializing array with bytes
    int blocks = 0; //keeps track of the number of iterations in one image
    FILE *outptr; //pointer for output file

    //while buffers are full
    while(fread(buffer, 512, 1, inptr) == 1)
    {
        //ift he first  bytes indicate JPEG
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            //reset number of blocks to 0
            blocks = 0;

            // and if there hasn't been a JPG before
            if (!inJPG)
            {
                //create the first image file
                char filename[6];
                sprintf(filename, "%03i.jpg", fileCount);
                outptr = fopen(filename, "w");
                inJPG = true;
            }
            //if there has been an image before
            else if (inJPG)
            {
                //close the last image and create a new one
                fclose(outptr);
                char filename[6];
                sprintf(filename, "%03i.jpg", fileCount);
                outptr = fopen(filename, "w");
            }
            fileCount++;
        }

        //if buffer is part of an image, copy it into the image file
        if (inJPG)
        {
            fseek(outptr, blocks * 512, SEEK_SET);
            fwrite(&buffer, 512, 1, outptr);
            blocks++;
        }
    }
}
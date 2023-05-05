#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "wav.h"

int check_format(WAVHEADER header);
int get_block_size(WAVHEADER header);

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 3)
    {
        printf("Usage: ./reverse input.wav output.wav\n");
        return 1;
    }

    // Open input file for reading
    char *infile = argv[1];
    char *outfile = argv[2];
    FILE *input = fopen(infile, "r");
    if (input == NULL)
    {
        fclose(input);
        printf("Could not open file %s", infile);
        return 2;
    }

    // Read header
    WAVHEADER header;
    fread(&header, 44, 1, input);

    // Use check_format to ensure WAV format
    if (!check_format(header))
    {
        fclose(input);
        printf("Input is not a WAV file.\n");
        return 3;
    }

    // Open output file for writing
    FILE *output = fopen(outfile, "w");
    if (output == NULL)
    {
        fclose(output);
        printf("Could not create file %s", outfile);
        return 4;
    }

    // Write header to file
    fwrite(&header, 44, 1, output);


    // Use get_block_size to calculate size of block
    int block_size = get_block_size(header);

    // Write reversed audio to file
    BYTE buffer[block_size];
    // put SCANNER at the very end.
    fseek(input, -1 * block_size, SEEK_END);
    // while we still haven't touched the end of the header
    while (ftell(input) >= 44)
    {
        fread(&buffer, block_size, 1, input); // read the current block from input to buffer
        fwrite(&buffer, block_size, 1, output); // write the buffer to output
        fseek(input, -2 * block_size, SEEK_CUR); // go two block sizes back (since we moved one block size forward to grab this chunck.)
    }

    fclose(input);
    fclose(output);
}

int check_format(WAVHEADER header)
{
    bool W = false;
    bool A = false;
    bool V = false;
    bool E = false;
    if (header.format[0] == 'W')
    {
        W = true;
    }
    if (header.format[1] == 'A')
    {
        A = true;
    }
    if (header.format[2] == 'V')
    {
        V = true;
    }
    if (header.format[3] == 'E')
    {
        E = true;
    }
    if (W && A && V && E)
    {
        return 1; // one is good in this case *eyeroll*
    }
    else
    {
        return 0;
    }
}

// returns size of each block of audio data given the current
// header file. 
int get_block_size(WAVHEADER header)
{
    float bps = header.bitsPerSample / 8.0;
    int chan = header.numChannels;
    float temp = bps * chan;
    int size = (int) round(temp);
    return size;
}

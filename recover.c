#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Recovers jpegs from a file.
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./recover image");
        return 1;
    }

    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        printf("Cannot open file!\n");
        return 1;
    }
    
    // Initialize file cursor to zero. Define a byte as 8 bits.
    rewind(card);
    typedef uint8_t BYTE; 

    // Read 512 byte block, starting at byte 0. If not a jpeg, move to next 512 byte block.
    BYTE bytes[512];
    memset(bytes, 0, sizeof(bytes)); // zero out array
    int n = 0;
    do
    {
        // Cursor initially at zero.
        int tell = ftell(card);
        printf("initial tell = %i\n", tell);
        // fread reads 512 bytes and sets cursor forward 512 bytes.
        fread(bytes, sizeof(BYTE), 512, card);
        tell = ftell(card);
        printf("tell after read = %i\n", tell);
        
        // Keep reading until the first jpeg file is found.
        if (bytes[0] != 0xff && bytes[1] != 0xd8 && bytes[2] != 0xff && (bytes[3] & 0xf0) != 0xe0)
        {
            n += 512;
            printf("No jpeg found. Next seeking jpeg at byte %i\n", n);
        }
    }
    while (bytes[0] != 0xff && bytes[1] != 0xd8 && bytes[2] != 0xff && (bytes[3] & 0xf0) != 0xe0);
    // Above: Two methods for finding if 4th byte is 0xe0 through 0xef:
    // (bytes[3] >= 0xe0)
    // OR Bitwise arithmetic: multiplying? each bit with bitwise '&' so that last 4 bits == 0:
    // (bytes[3] & 0xf0) == 0xe0)
    
    
    printf("Jpeg found at n = %i\n", n);
    // Once a jpeg is found, rewind by 512 bytes to make sure file cursor is at correct place:
    fseek(card, n, 0);
    int tell = ftell(card);
    printf("Cursor now set to %i\n", tell);


    // Clear the end of file indicator.
    clearerr(card);
    int jpeg_name = 0;
    int end_of_file = 0; // end_of_file is a bool initialized to 0, will be set to 1 when the end of the file is reached.
    do
    {
        // Increments the name of a jpeg by one each time a new jpeg file needs to be written.
        // Printing a name not to the output, but to a string -> using sprintf()
        char *filestring = malloc(sizeof(char) * 3);
        sprintf(filestring, "%03i.jpg", jpeg_name);
        jpeg_name ++;
        FILE *jpeg = fopen(filestring, "w");

        BYTE size_arr[512];
        int size_jpeg = 0;

        int new_image = 0; // new_image is a bool, will be set to 1 when a new image is found.
        do
        {
            new_image = 0;
            size_jpeg ++;
            tell = ftell(card);
            printf("writing... block %i at byte %i\n", size_jpeg, tell);
            fread(size_arr, sizeof(BYTE), 512, card);
            fwrite(size_arr, sizeof(BYTE), 512, jpeg);
            n += 512;
            tell = ftell(card);
            printf("After reading and writing, cursor is at %i, n = %i\n", tell, n);
            
            // after writing current block, read next block to see if new image is found.
            fread(size_arr, sizeof(BYTE), 512, card);
            tell = ftell(card);
            printf("Checking for new image at block %i\n", tell);
            if (size_arr[0] == 0xff && size_arr[1] == 0xd8 && size_arr[2] == 0xff && size_arr[3] >= 0xe0)
            {
                printf("\n\nnew image found. size_arr[0] = %i, [1] = %i, [2] = %i, 3 = %i\n", size_arr[0], size_arr[1], size_arr[2], size_arr[3]);
                new_image = 1;
            }
            
            // feof() == 0 (in other words, false) until end of file is reached.
            if (feof(card) != 0) 
            {
                end_of_file = 1;
            }
            // Since checking for new image with fread also set file cursor forward, must rewind with fseek:
            fseek(card, n, 0);
            tell = ftell(card);
            printf("cursor set back to %i\n", tell);
        }
        // Loop will end if either a new jpeg is found, or end of file is reached.
        while (new_image == 0 && end_of_file == 0); 
        printf("total blocks = %i\n", size_jpeg);
        
        // Close current jpeg and free malloc()ed memory after every jpeg is written.
        fclose(jpeg);
        free(filestring);
    }
    while (end_of_file == 0);

    fclose(card);
    return 0;
}
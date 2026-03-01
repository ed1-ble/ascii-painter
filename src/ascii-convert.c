#include<stdio.h>
#include<unistd.h>
#include<sys/ioctl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

void get_term_size(struct winsize* ws) {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, ws);
}

void print_image(unsigned char* pixel_data, int width, int height, int cols) {

    int rows = cols/2 * height/width;

    // Downsize the image
    int pixels_per_col = width/cols;
    int pixels_per_row = height/rows;
    
    unsigned char* term_image = malloc(rows * cols); 

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int start_x = (int) (c * pixels_per_col);
            int start_y = (int) (r * pixels_per_row);
            int end_x = (int) ((c+1) * pixels_per_col);
            int end_y = (int) ((r+1) * pixels_per_row);
            
            if (end_x > width) end_x = width;
            if (end_y > height) end_y = height;

            long sum = 0;
            int count = 0;
            for (int y = start_y; y < end_y; y++) {
                for (int x = start_x; x < end_x; x++) {
                    sum += pixel_data[y * width + x];
                    count++;
                }
            }
            if (count == 0) continue;
            term_image[r * cols + c] = (unsigned char) (sum/count);
        }
    }
    
    // Match the new image with ascii symbols

    char buffer[rows*cols];
    const char symbols[] = " .:*@";
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int i = r * cols + c;
            int idx = (term_image[i] * (sizeof(symbols)-1)) / 255;
            printf("%c", symbols[idx]);
        }
        printf("\n");
    }

    free(term_image);
    
}

int main(int argc, char** argv) {
    int width, height, channels; //channels is num of 8bit components/pixel
    unsigned char *pixel_data = stbi_load(argv[1],&width,&height,&channels,1);

    //     N=#comp     components
    //       1           grey
    //       2           grey, alpha
    //       3           red, green, blue
    //       4           red, green, blue, alpha
    
    struct winsize ws;
    get_term_size(&ws);

    printf("Terminal Size: %d x %d \n",ws.ws_col,ws.ws_row);

    print_image(pixel_data, width, height,ws.ws_col);
     
    stbi_image_free(pixel_data);
    return 0;
}

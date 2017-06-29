/**
 * @file imageio.c
 *
 * @author José Galaviz <jgc@fciencias.unam.mx>
 * @version 1.0
 * \date 7/Sep/2012 - 17:52
 *
 * Facultad de Ciencias,
 * Universidad Nacional Autónoma de México, México.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, visit the following URL:
 * (http://www.gnu.org/licenses/gpl.html)
 * or write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @section DESCRIPTION
 * Image input/output library. A collection of functions to read and
 * write images in JPEG and PNG formats. Only supports 3 channel (RGB)
 * jpg images and 4 channel ARGB png. However, even in the case of a 3
 * channel jpg, all pictures are translated from, and to a memory
 * buffer where each pixel is represented by an unsigned integer (32
 * bits) divided into 4 channels of 8 bits each: ARGB, the most
 * significant byte is used for the alpha channel, the least
 * significant for the blue.
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"imageio.h"

/**
 * \brief Reads a JPEG image from file.
 */
unsigned int  **
readJPGImage(char *fname, int *width, int *height)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW        row_pointer[1];
    unsigned int  **im;
    unsigned int   *fullimage;
    int             i, j;
    register unsigned int red, green, blue;

    FILE           *infile = fopen(fname, "rb");

    if (!infile) {
        return NULL;
    }

    /*
     * set the error handler (default used)
     */
    cinfo.err = jpeg_std_error(&jerr);
    /*
     * setup decompression process
     */
    jpeg_create_decompress(&cinfo);
    /*
     * this makes the library read from infile
     */
    jpeg_stdio_src(&cinfo, infile);
    /*
     * reading the image header which contains image information
     */
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    /*
     * Size of actual image and size of formal image differ: that means that
     * actual image is the formal image rescaling. We only accept non-rescaled
     * images
     */
    if ((cinfo.output_width != cinfo.image_width) ||
        (cinfo.output_height != cinfo.image_height)) {
        jpeg_abort((j_common_ptr) & cinfo);
        return NULL;
    }
    *width = cinfo.output_width;
    *height = cinfo.output_height;

    /*
     * memory to store the row pointers
     */
    im = (unsigned int **) malloc(*height * sizeof(unsigned int *));
    /*
     * the full image in a single memory block
     */
    fullimage =
        (unsigned int *) malloc(*height * *width * sizeof(unsigned int));

    /*
     * each im entry, points to the respective image row
     */
    for (i = 0; i < *height; i++)
        im[i] = fullimage + i * *width;
    /*
     * memory for the temporary row storage
     */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width *
                                              cinfo.num_components);

    /*
     * the jpeg input file has 3 bytes (RGB). We store also an alpha channel
     * with 0
     */
    /*
     * therefore the total number of bytes per pixel is 4 RGBA = 32 bits int
     */
    i = 0;
    while (cinfo.output_scanline < *height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (j = 0; j < *width; j++) {
            im[i][j] = 0;       /* A */
            red = (unsigned int) row_pointer[0][3 * j]; /* R */
            green = (unsigned int) row_pointer[0][3 * j + 1];   /* G */
            blue = (unsigned int) row_pointer[0][3 * j + 2];    /* B */
            im[i][j] = 0XFF000000 | (red << 16) | (green << 8) | blue;
        }
        i++;
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    fclose(infile);
    return im;
}

/**
 * \brief Reads a PNG image from file.
 */
unsigned int  **
readPNGImage(char *fname, int *width, int *height)
{
    unsigned char   header[8];
    unsigned int  **im;
    unsigned int   *fullimage;
    png_byte        color_type;
    png_byte        bit_depth;
    png_structp     png_ptr;
    png_infop       info_ptr;
    int             i, j, res;
    register unsigned int red, green, blue, alpha;
    FILE           *infile = fopen(fname, "rb");

    if (!infile)
        return NULL;            /* not readable file */

    res = fread(header, 1, 8, infile);
    if (png_sig_cmp(header, 0, 8) || !res) {
      fclose(infile);
      return NULL;            /* File is not recognized as a PNG file */
    }

    /*
     * initialize stuff
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
      fclose(infile);
      return NULL;            /* png_create_read_struct failed */
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
      fclose(infile);
      return NULL;            /* png_create_info_struct failed */
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
      fclose(infile);
      return NULL;            /* Error during init_io */
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    *width = png_get_image_width(png_ptr, info_ptr);
    *height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if ((color_type != PNG_COLOR_TYPE_RGB_ALPHA) || (bit_depth != 8)) {
      fclose(infile);
      return NULL;            /* only RGBA 8 bits per channel is allowed */
    }

    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    /*
     * read file
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
      fclose(infile);
      return NULL;            /* Error during read_image */
    }

    /*
     * memory to store the row pointers
     */
    im = (unsigned int **) malloc(*height * sizeof(unsigned int *));
    /*
     * the full image in a single memory block
     */
    fullimage =
        (unsigned int *) malloc(*height * *width * sizeof(unsigned int));

    /*
     * each im entry, points to the respective image row
     */
    for (i = 0; i < *height; i++)
        im[i] = fullimage + i * *width;

    png_read_image(png_ptr, (png_bytepp) im);

    /*
     * now we need to exchange the channels red and blue
     */
    for (i = 0; i < *height; i++) {
        for (j = 0; j < *width; j++) {
            red = (im[i][j] & 0X000000FF) << 16;        /* R */
            green = (im[i][j] & 0X0000FF00);    /* B */
            blue = (im[i][j] & 0X00FF0000) >> 16;       /* B */
            alpha = (im[i][j] & 0XFF000000);    /* alpha */
            im[i][j] = 0X00000000 | alpha | red | green | blue;
        }
    }
    fclose(infile);
    return im;
}

/**
 * \brief Writes an image to a PNG file.
 */
int
writePNGImage(unsigned int **img, char *fname, int width, int height)
{
    png_structp     png_ptr;
    png_infop       info_ptr;

    /*
     * create file
     */
    FILE           *outfile = fopen(fname, "wb");
    if (!outfile)
        return -1;

    /*
     * initialize stuff
     */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
      fclose(outfile);
      return -2;              /* cannot create write struct */
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
      fclose(outfile);
      return -3;              /* cannot create info struct */
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
      fclose(outfile);
      return -4;              /* error during init i/o */
    }

    png_init_io(png_ptr, outfile);


    /*
     * write header
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
      fclose(outfile);
      return -5;              /* error writing header */
    }

    /*
     * param 5: bit depth = bits used for each chanel (8 bis). param 6: color
     * type
     */
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    /*
     * Weird!! in order to preserve the order I must say it's inverted
     */
    png_set_bgr(png_ptr);

    /*
     * write bytes
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
      fclose(outfile);
      return -6;              /* Error writing bytes */
    }

    png_write_image(png_ptr, (png_bytep *) img);

    /*
     * end write
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
      fclose(outfile);
      return -7;              /* Error during end of write */
    }

    png_write_end(png_ptr, NULL);

    fclose(outfile);
    return 1;
}

/**
 * \brief Writes an image to a JPG file.
 */
int
writeJPGImage(unsigned int **img, char *fname, int width, int height)
{
    int             bytes_per_pixel = 3;        /* or 1 for GRACYSCALE images */
    int             color_space = JCS_RGB;      /* or JCS_GRAYSCALE for
                                                 * grayscale images */
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW        row_pointer[1];
    unsigned char  *line = NULL;
    int             i, j;
    FILE           *outfile = fopen(fname, "wb");

    if (!outfile) {
        printf("Error opening output jpeg file %s\n!", fname);
        return -1;
    }
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    /*
     * Setting the parameters of the output file here
     */
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = bytes_per_pixel;
    cinfo.in_color_space = color_space;
    /*
     * default compression parameters, we shouldn't be worried about these
     */
    jpeg_set_defaults(&cinfo);
    /*
     * Now do the compression ..
     */
    jpeg_start_compress(&cinfo, TRUE);
    /*
     * like reading a file, this time write one row at a time
     */
    i = 0;
    line = (unsigned char *) malloc(width * bytes_per_pixel);
    while (cinfo.next_scanline < cinfo.image_height) {
        for (j = 0; j < width; j++) {
            line[j * bytes_per_pixel] = (img[i][j] & 0X00FF0000) >> 16;
            line[j * bytes_per_pixel + 1] = (img[i][j] & 0X0000FF00) >> 8;
            line[j * bytes_per_pixel + 2] = (img[i][j] & 0X000000FF);
        }
        row_pointer[0] = (JSAMPROW) line;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
        ++i;
    }
    /*
     * similar to read file, clean up after we're done compressing
     */
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    free(line);
    fclose(outfile);
    /*
     * success code is 1!
     */
    return 1;
}

/*
 * imageio.c ends here
 */

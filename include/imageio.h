/**
 * @file imageio.h
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
#ifndef IMAGEIO_H
#define IMAGEIO_H

#include<jpeglib.h>
#include<png.h>

/**
 * \brief Reads a JPEG image from file.
 *
 * Reads an image from a JPEG file. It's assumed that each pixel in
 * the image has RGB components with 8 bits for each color. The image
 * read is stored in a memory buffer, that can be accessed trough a
 * 2-dimensional array whose pointer is returned as function call
 * result. The width and height of image is also returned trough the
 * respective parameters passed by reference.
 * For example if the function is called:
 *
 * imgptr = readJPGImage("picture.jpg", &wdt, &ht);
 *
 * then the pixel in row i, column j in the image can be accessed as
 * a unsigned int: imgptr[i][j].
 *
 * The most significant byte corresponds to the alpha channel (which
 * is not present in jpg, but added by the function), the next bytes
 * correspond to the R, G and B channels.
 * If some problem occurs then the function returns NULL.
 *
 * @param[in] fname is the name of file which contains the image.
 * @param[out] width is the image width.
 * @param[out] height is the image height
 * \return a pointer to the image row array or NULL if case of error.
 * \pre the file whose name is passed must exist and be readable.
 * \post the width and height numbers are non negative integers with
 * the actual size of image. If no errors occur, then the pointer
 * returned has enough memory allocated to contain 4 bytes for each
 * pixel packed in an unsigned integer (32 bits) in order ARGB.
 */
unsigned int  **readJPGImage(char *fname, int *width, int *height);

/**
 * \brief Reads a PNG image from file.
 *
 * Reads an image from a PNG file. It's assumed that each pixel in
 * the image has ARGB components with 8 bits for each channel. The
 * image read is stored in a memory buffer, that can be accessed
 * trough a 2-dimensional array whose pointer is returned as function
 * result. The width and height of image is also returned trough the
 * respective parameters passed by reference.
 * For example if the function is called:
 *
 * imgptr = readPNGImage("picture.jpg", &wdt, &ht);
 *
 * then the pixel in row i, column j in the image can be accessed as
 * a unsigned int: imgptr[i][j].
 *
 * The most significant byte corresponds to the alpha channel, the
 * next bytes correspond to the R, G and B channels.
 * If some problem occurs then the function returns NULL.
 *
 * @param[in] fname is the name of file which contains the image.
 * @param[out] width is the image width.
 * @param[out] height is the image height
 * \return a pointer to the image row array or NULL if case of error.
 * \pre the file whose name is passed must exist and be readable.
 * \post the width and height numbers are non negative integers with
 * the actual size of image. If no errors occur, then the pointer
 * returned has enough memory allocated to contain 4 bytes for each
 * pixel packed in an unsigned integer (32 bits) in order ARGB.
 */
unsigned int  **readPNGImage(char *fname, int *width, int *height);

/**
 * \brief Writes an image to a PNG file.
 *
 * Given a memory buffer, which contains the pixel information of some
 * image in the format ARGB (packed in unsigned int's), this functions
 * writes a file in PNG format with such image.
 *
 * @param[in] img is the image row array.
 * @param[in] fname is the name of the file where tha image will be writen.
 * @param[in] width is the image width.
 * @param[in] height is the image height
 * \return an integer which represents the result of the operation:
 * 1 if success, a negative number otherwise.
 * \pre The image buffer must be non NULL and must contain the each
 * pixel encoded in ARGB format.
 * \post new file, whose name is provided, in PNG format and an
 * integer with a result code. 1 if the operation was sucessful or a
 * negative number otherwise.
 */
int             writePNGImage(unsigned int **img, char *fname, int width,
                              int height);

/**
 * \brief Writes an image to a JPG file.
 *
 * Given a memory buffer, which contains the pixel information of some
 * image in the format ARGB (packed in unsigned int's), this functions
 * writes a file in JPG format with such image, excluding the alpha channel.
 *
 * @param[in] img is the image row array.
 * @param[in] fname is the name of the file where tha image will be writen.
 * @param[in] width is the image width.
 * @param[in] height is the image height
 * \return an integer which represents the result of the operation:
 * 1 if success, a negative number otherwise.
 * \pre The image buffer must be non NULL and must contain the each
 * pixel encoded in ARGB format.
 * \post new file, whose name is provided, in PNG format and an
 * integer with a result code. 1 if the operation was sucessful or a
 * negative number otherwise.
 */
int             writeJPGImage(unsigned int **img, char *fname, int width,
                              int height);

#endif
/*
 * imageio.h ends here
 */

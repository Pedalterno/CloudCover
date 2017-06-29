/**
 * @file test-imageio.c
 *
 * @author José Galaviz <jgc@fciencias.unam.mx>
 * @version 1.0
 * \date 17/Sep/2012 - 11:56
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
 * Unit test for imageio.
 *
 */
#include<stdio.h>
#include<stdlib.h>
#include"imageio.h"

#define ROJO  0XFFFF0000
#define VERDE 0XFF00FF00
#define AZUL  0XFF0000FF
#define TRANSP_ROJO 0X00FF0000

/**
 * \brief Verifies if there is one and only one value in each pixel of
 * an image buffer.
 *
 * @param[in] img is the buffer image that must be veirified.
 * @param[in] ancho is the image width.
 * @param[in] alto is the image height.
 * @param[in] value that must appear in every pixel.
 * \return 1 if the buffer image is correct, 0 otherwise.
 */
int
uniqueValueIn(unsigned int **img, int ancho, int alto, unsigned int value)
{
    int             good = 1;
    int             i, j;

    for (i = 0; (i < alto) && good; i++) {
        for (j = 0; (j < ancho) && good; j++) {
            good = (img[i][j] == value);
        }
    }
    return good;
}

/**
 * \brief Verifies if there is one and only one value in each pixel of
 * an image buffer considering a little variation in JPEG buffers.
 *
 * @param[in] img is the buffer image that must be veirified.
 * @param[in] ancho is the image width.
 * @param[in] alto is the image height.
 * @param[in] value that must appear in every pixel. The value in each
 * pixel must be this or this minus one or this plus one in some other
 * channel.
 * \return 1 if the buffer image is correct, 0 otherwise.
 */
int
uniqueAlmostValueIn(unsigned int **img, int ancho, int alto, unsigned int value)
{
    int             good = 1;
    int             i, j;

    for (i = 0; (i < alto) && good; i++) {
        for (j = 0; (j < ancho) && good; j++) {
            /*
             * in JPEG, quantization makes imprecise colors
             */
            if ((value == ROJO) || (value == AZUL))
                good = (img[i][j] == value) ||
                    (img[i][j] == (0XFF000000 | (value & value << 1)));
            else                /* green has a bit of blue */
                good = (VERDE) || (VERDE << 1) || (VERDE << 1 | 0XFF000001);
        }
    }
    return good;
}

/**
 * \brief Sets the whole buffer to a given value
 *
 * @param[in] img is the buffer image that must be set.
 * @param[in] ancho is the image width.
 * @param[in] alto is the image height.
 * @param[in] value that must appear in every pixel.
 */
void
setUniqueValue(unsigned int **img, int ancho, int alto, unsigned int value)
{
    int             i, j;

    for (i = 0; i < alto; i++) {
        for (j = 0; j < ancho; j++) {
            img[i][j] = value;
        }
    }
}

/**
 * \brief Compare two different buffers.
 *
 * @param[in] b1 is the first buffer to be compared.
 * @param[in] b2 is the second buffer.
 * @param[in] ancho is the buffer width.
 * @param[in] alto is the buffer height.
 * \return 1 is both buffers coincide pixel per pixel, 0 otherwise.
 */
int
compareBuffers(unsigned int **b1, unsigned int **b2, int ancho, int alto)
{
    int             good = 1;
    int             i, j;

    for (i = 0; (i < alto) && good; i++) {
        for (j = 0; (j < ancho) && good; j++) {
            good = (b1[i][j] == b2[i][j]);
        }
    }
    return good;
}

/**
 * \brief Testing program. Performs tha unit testing for each
 * functions in the imageio library.
 */
int
main()
{
    int             ancho, alto;
    unsigned int  **imagen;
    unsigned int  **im2;

    int             success = 0;
    int             totaltests = 14;

    /*
     * ==========================================================================
     *                           PNG TEST
     * ==========================================================================
     */
    /*
     * Reading and writing a pure red image PNG
     */
    imagen = readPNGImage("Imgs/red.png", &ancho, &alto);
    fprintf(stderr, "PNG reading red image: \t\t");
    if (uniqueValueIn(imagen, ancho, alto, ROJO)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");
    fprintf(stderr, "Testing PNG write...\t\t");
    writePNGImage(imagen, "test.png", ancho, alto);
    im2 = readPNGImage("test.png", &ancho, &alto);
    if (compareBuffers(imagen, im2, ancho, alto)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");

    /*
     * Reading and writing a pure green image PNG
     */
    imagen = readPNGImage("Imgs/green.png", &ancho, &alto);
    fprintf(stderr, "PNG reading green image: \t");
    if (uniqueValueIn(imagen, ancho, alto, VERDE)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");
    fprintf(stderr, "Testing PNG write...\t\t");
    writePNGImage(imagen, "test.png", ancho, alto);
    im2 = readPNGImage("test.png", &ancho, &alto);
    if (compareBuffers(imagen, im2, ancho, alto)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");

    /*
     * Reading and writing a pure blue image PNG
     */
    imagen = readPNGImage("Imgs/blue.png", &ancho, &alto);
    fprintf(stderr, "PNG reading blue image: \t");
    if (uniqueValueIn(imagen, ancho, alto, AZUL)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");
    fprintf(stderr, "Testing PNG write...\t\t");
    writePNGImage(imagen, "test.png", ancho, alto);
    im2 = readPNGImage("test.png", &ancho, &alto);
    if (compareBuffers(imagen, im2, ancho, alto)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");

    /*
     * Reading and writing a pure red transparent image PNG
     */
    imagen = readPNGImage("Imgs/red-transp.png", &ancho, &alto);
    fprintf(stderr, "PNG reading trans red image: \t");
    if (uniqueValueIn(imagen, ancho, alto, TRANSP_ROJO)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");
    fprintf(stderr, "Testing PNG write...\t\t");
    writePNGImage(imagen, "test.png", ancho, alto);
    im2 = readPNGImage("test.png", &ancho, &alto);
    if (compareBuffers(imagen, im2, ancho, alto)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");

    /*
     * ==========================================================================
     *                           JPEG TEST
     * ==========================================================================
     */
    /*
     * Reading and writing an almost pure red image JPG
     */
    imagen = readJPGImage("Imgs/red.jpg", &ancho, &alto);
    fprintf(stderr, "JPG reading red image: \t\t");
    if (uniqueAlmostValueIn(imagen, ancho, alto, ROJO)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");
    fprintf(stderr, "Testing JPG write...\t\t");
    writeJPGImage(imagen, "test.jpg", ancho, alto);
    im2 = readJPGImage("test.jpg", &ancho, &alto);
    if (compareBuffers(imagen, im2, ancho, alto)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");

    /*
     * Reading and writing an almost pure green image JPG
     */
    imagen = readJPGImage("Imgs/green.jpg", &ancho, &alto);
    fprintf(stderr, "JPG reading green image: \t");
    if (uniqueAlmostValueIn(imagen, ancho, alto, VERDE)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");
    fprintf(stderr, "Testing JPG write...\t\t");
    writeJPGImage(imagen, "test.jpg", ancho, alto);
    im2 = readJPGImage("test.jpg", &ancho, &alto);
    if (compareBuffers(imagen, im2, ancho, alto)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");

    /*
     * Reading and writing an almost pure blue image JPG
     */
    imagen = readJPGImage("Imgs/blue.jpg", &ancho, &alto);
    fprintf(stderr, "JPG reading blue image: \t");
    if (uniqueAlmostValueIn(imagen, ancho, alto, AZUL)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");
    fprintf(stderr, "Testing JPG write...\t\t");
    writeJPGImage(imagen, "test.jpg", ancho, alto);
    im2 = readJPGImage("test.jpg", &ancho, &alto);
    if (compareBuffers(imagen, im2, ancho, alto)) {
        fprintf(stderr, "OK\n");
        success++;
    }
    else
        fprintf(stderr, "Wrong!!\n");
    fprintf(stderr, "%d / %d tests passed\n", success, totaltests);
    if (success == totaltests) {
        fprintf(stderr, "\n imageio COMPLETE TEST SUCCESSFUL!\n");
        return 1;
    }
    else {
        fprintf(stderr, "\n imageio SOME ERRORS FOUND\n");
        return 0;
    }
}                               /* test-imageio.c ends here */

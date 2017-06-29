/**
 * @file imageinfo.c
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
#include"timedate.h"
#include"imageinfo.h"

/**
 * Tag list to retrieve EXINFO data from JPEG file.
 */
exifreg         tgslist[TGSINTRST] = {
    {2, 36864, ""}
    ,                           /* EXIF Version */
    {2, 40962, "Width"}
    ,
    {2, 40963, "Height"}
    ,
    {2, 36867, "Capture date and time"}
    ,
    {2, 40961, "ColorSpace"}
    ,
    {0, 271, "Manufacturer"}
    ,
    {0, 272, "Model"}
    ,
    {2, 33434, "Exposure time"}
    ,
    {2, 33437, "F-number"}
    ,
    {2, 34855, "ISO speed ratings"}
    ,
    {2, 37121, "Components configuration"}
    ,
    {2, 37377, "Shutter speed value"}
    ,
    {2, 37378, "Aperture"}
    ,
};

/*
 * Some other EXINFO tags
 */
/*
 * {4, 1, "Interoperability index"}, {4, 2, "Interoperability version"}, {1,
 * 259, "Compression"}, {0, 274, "Orientation"}, {0, 282, "X-resolution
 * (pixels/resunit)"}, {0, 283, "Y-resolution (pixels/resunit)"}, {0, 296,
 * "Resolution unit (resunit)"}, {0, 531, "YCbCr positioning"}, {2, 37385,
 * "Flash"}, {2, 34850, "Exposure program"}, {2, 37380, "Exposure bias value"},
 * {2, 37383, "Metering mode"}, {2, 41486, "Focal plane X-resolution"}, {2,
 * 41487, "Focal plane Y-resolution"}, {2, 41488, "Focal plane resolution
 * unit"}, {2, 41985, "Custom rendered"}, {2, 41986, "Exposure mode"}, {2,
 * 41987, "White balance"}, {2, 41990, "Scene capture type"}
 */

/**
 * \brief Gets the EXIF metadata from a JPEG file.
 */
int
getImgInfo(char *fname, double az, char *utcoff,
           ImageInfo * imin, CamAndShotInfo * casi)
{
    ExifData       *ed;
    unsigned int    tag;
    /* const char     *name;  */
    char            valor[81];
    ExifIfd         iIFD;
    int             i;
    FILE           *infile = fopen(fname, "rb");
    int             c[4];
    unsigned int    signature;
    char           *from;
    char          **cadptr = (char **) casi;
    int             offsethr, offsetmn;
    double          sgn = 1.0;
    double          valoff;
    double          jdate;

    if (!infile)
        return -1;              /* file error */

    for (i = 0; i < 4; i++)
        c[i] = fgetc(infile);
    fclose(infile);
    signature = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
    if (signature != JPEGEXIFMN)
        return -2;              /* No JPEG + EXIF file */

    /*
     * NULL pointers given for storage
     */
    if ((imin == NULL) || (casi == NULL))
        return -3;
    if ((az < 0) || (az >= 360))
        return -4;              /* azimuth out of range */

    /*
     * time offset out of range
     */
    if (utcoff != NULL) {
        if (!isValidUTC(utcoff))
            return -5;
        offsethr = strtol(utcoff + 4, (char **) NULL, 10);
        offsetmn = strtol(utcoff + 7, (char **) NULL, 10);
        if (*(utcoff + 3) == '+')
            sgn *= -1;
        valoff = timeDayFrac(offsethr, offsetmn, 0.0);
        valoff *= sgn;
    }
    else {
        valoff = 0.0;
    }

    from = fname + strlen(fname);
    while ((from > fname) && (*from != '/'))
        --from;
    /*
     * ImageInfo: filename without path
     */
    if (*from == '/')
        from++;
    imin->filename = (char *) malloc(strlen(from) + 1);
    strcpy(imin->filename, from);

    /*
     * ImageInfo: file format
     */
    imin->format = JPEGEXIF;
    /*
     * ImageInfo: Azimuth
     */
    imin->azimuth = az;

    ed = exif_data_new_from_file(fname);
    iIFD = (ExifIfd) tgslist[0].idx;
    tag = tgslist[0].tgnum;
    exif_content_get_value(ed->ifd[iIFD], tag, valor, 80);
    /*
     * ImageInfo: EXIF version
     */
    imin->exifversion = (char *) malloc(strlen(valor) + 1);
    strcpy(imin->exifversion, valor);
    /*
     * ImageInfo: clear color mode
     */
    imin->colormode = 0;

    for (i = 1; i < TGSINTRST; i++) {
        iIFD = (ExifIfd) tgslist[i].idx;
        tag = tgslist[i].tgnum;
        /*        name = tgslist[i].tgname;  */
        if (exif_content_get_entry(ed->ifd[iIFD], tag)) {
            exif_content_get_value(ed->ifd[iIFD], tag, valor, 80);
            valor[80] = '\x0';
            /*
             * ImageInfo: width
             */
            if (i == 1)
                imin->width = strtol(valor, (char **) NULL, 10);
            /*
             * ImageInfo: height
             */
            else if (i == 2)
                imin->height = strtol(valor, (char **) NULL, 10);
            /*
             * ImageInfo: Date and time info
             */
            else if (i == 3) {
                from = valor;
                sscanf(from, "%4d:%2d:%2d %2d:%2d:%2d",
                       &(imin->year),
                       &(imin->month),
                       &(imin->day),
                       &(imin->UTChr),
                       &(imin->UTCmin), &(imin->UTCsec));
                /*
                 * correction to time obtained from EXIF, to obtain UTC time
                 */
                if (utcoff != NULL) {
                    jdate = julianDate(imin->year, imin->month, imin->day,
                                       imin->UTChr, imin->UTCmin,
                                       (double) imin->UTCsec);
                    jdate += valoff;
                    /*
                     * valoff is dummy here
                     */
                    calDateTime(jdate, &imin->year, &imin->month, &imin->day,
                                &imin->UTChr, &imin->UTCmin, &valoff);
                }
            }
            /*
             * ImageInfo: color mode
             */
            else if (i == 4)
                imin->colormode = (!strcmp(valor, "sRGB")) ? RGB : UNKNOWN;
            else {
                /*
                 * CamAndShotInfo: all fields
                 */
                cadptr[i - 5] = (char *) malloc(strlen(valor) + 1);
                strcpy(cadptr[i - 5], valor);
            }
        }
    }
    return 1;                   /* success */
}
/*
 * imageinfo.c ends here
 */

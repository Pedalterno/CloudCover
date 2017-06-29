/**
 * @file test-imageinfo.c
 *
 * @author José Galaviz <jgc@fciencias.unam.mx>
 * @version 1.0
 * \date 25/Sep/2012 - 12:39
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
 * Unit test of imageinfo.
 *
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"imageinfo.h"

int
main()
{
    ImageInfo       imginfo;
    CamAndShotInfo  caminfo;
    int             totaltests = 21;
    int             success = 0;
    char            cad[30];

    fprintf(stderr, "Testing EXIF information retrieval... %d tests\n",
            totaltests);

    /*
     * getImgInfo Parameters: Image filename, Azimuth Hour offset to obtain UTC
     * from EXIF data Minute offset to obtain UTC from EXIF data ImageInfo
     * structure to store info CamAndShotInfo structure to store camera and
     * shooting info
     */
    getImgInfo("Imgs/11841.jpg", 235.4, "UTC-05:00", &imginfo, &caminfo);

    if (!strcmp(imginfo.filename, "11841.jpg"))
        success++;
    else {
      fprintf(stderr,"Filename test failed %s != 11841.jpg\n",
              imginfo.filename);
    }
    if (imginfo.format == 2)
        success++;
    else {
      fprintf(stderr,"Format test failed %d != 2\n",
              imginfo.format);
    }
    if (imginfo.colormode == 1)
        success++;
    else {
      fprintf(stderr,"Colormode test failed %d != 1\n",
              imginfo.colormode);
    }
    if (!strcmp(imginfo.exifversion, "Exif Version 2.21"))
        success++;
    else {
      fprintf(stderr,"Exif version test failed %s != Exif Version 2.21\n",
              imginfo.exifversion);
    }
    if (imginfo.width == 4368)
        success++;
    else {
      fprintf(stderr,"Width test failed %d != 4368\n",
              imginfo.width);
    }
    if (imginfo.height == 2912)
        success++;
    else {
      fprintf(stderr,"Height test failed %d != 2912\n",
              imginfo.height);
    }
    if (imginfo.year == 2008)
        success++;
    else {
      fprintf(stderr,"Year test failed %d != 2008\n",
              imginfo.year);
    }
    if (imginfo.month == 12)
        success++;
    else {
      fprintf(stderr,"Month test failed %d != 12\n",
              imginfo.month);
    }
    if (imginfo.day == 1)
        success++;
    else {
      fprintf(stderr,"Day test failed %d != 1\n",
              imginfo.day);
    }
    /*
     * if (imginfo.UTChr == 23) success++; if (imginfo.UTCmin == 6) success++;
     */
    if (imginfo.UTChr == 4)
        success++;
    else {
      fprintf(stderr,"Hour test failed %d != 4\n",
              imginfo.UTChr);
    }
    if (imginfo.UTCmin == 6)
        success++;
    else {
      fprintf(stderr,"Minute test failed %d != 6\n",
              imginfo.UTCmin);
    }
    if (imginfo.UTCsec == 42)
        success++;
    else {
      fprintf(stderr,"Second test failed %d != 42\n",
              imginfo.UTCsec);
    }
    imginfo.azimuth = floor(imginfo.azimuth * 10000.0) / 10000.0;
    sprintf(cad, "%7.3f", imginfo.azimuth);
    if (!strcmp(cad, "235.400"))
        success++;
    else {
      fprintf(stderr,"Azimuth test failed %s != 235.400\n",
              cad);
    }

    if (!strcmp(caminfo.makeby, "Canon"))
        success++;
    if (!strcmp(caminfo.model, "Canon EOS 5D"))
        success++;
    if (!strcmp(caminfo.exposure, "1/250 sec."))
        success++;
    if (!strcmp(caminfo.fnumber, "f/11.0"))
        success++;
    if (!strcmp(caminfo.isospeed, "400"))
        success++;
    if (!strcmp(caminfo.components, "Y Cb Cr -"))
        success++;
    if (!strcmp(caminfo.shutterspeed, "8.00 EV (1/256 sec.)"))
        success++;
    if (!strcmp(caminfo.aperture, "7.00 EV (f/11.3)"))
        success++;
#ifdef DEBUG
     fprintf(stderr, "************** EXIF Data ****************\n");
     fprintf(stderr, "Filename: %s Format: %d Colormode: %d Exif: %s\n",
             imginfo.filename, imginfo.format, imginfo.colormode,
             imginfo.exifversion);
     fprintf(stderr, "Width: %d, Height: %d\n",
             imginfo.width, imginfo.height);
     fprintf(stderr, "Date: %d / %d / %d\tTime: %d:%d:%d\n",
             imginfo.year, imginfo.month, imginfo.day,
             imginfo.UTChr, imginfo.UTCmin, imginfo.UTCsec);
     fprintf(stderr,"\nCamera and shot...\n");
     fprintf(stderr, "Manufacturer:%s\n",caminfo.makeby);
     fprintf(stderr, "Model: %s\n",caminfo.model);
     fprintf(stderr, "Exposure: %s\n",caminfo.exposure);
     fprintf(stderr, "F number: %s\n",caminfo.fnumber);
     fprintf(stderr, "ISO speed: %s\n",caminfo.isospeed);
     fprintf(stderr, "Components: %s\n",caminfo.components);
     fprintf(stderr, "Shutter: %s\n",caminfo.shutterspeed);
     fprintf(stderr, "Aperture: %s\n",caminfo.aperture);
#endif
    fprintf(stderr, "%d of %d tests passed\n", success, totaltests);
    if (success == totaltests)
      return 1;
    else
      return 0;
}/* test-imageinfo.c ends here */

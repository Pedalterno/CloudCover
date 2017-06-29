/**
 * @file imageinfo.h
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
 * Image information retrieval and manipulation. Some image
 * information is obtained from EXIF data stored in the JPEG file,
 * some other info is obtained from configuration and geographic
 * information files.
 *
 */
#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include<libexif/exif-data.h>
#include<libexif/exif-ifd.h>

/**
 * Number of EXIF tags used
 */
#define TGSINTRST 13

/**
 * JPEG + EXIF magic number.
 * JPEG signature if 0XFFD8
 * With JFIF = JPEG + FF
 * With EXIF = JFIF + E1
 */
#define JPEGEXIFMN 0XFFD8FFE1

/**
 * colormodes
 */
enum COLORMODE {
  UNKNOWN = 0,
  /** Image color mode: RGB (formally speaking sRGB) */
  RGB = 1,
  /** Image color mode: BGR */
  BGR = 2,
  /** Image color mode: Alpha + RGB */
  ARGB = 3,
  /** Image color mode: Alpha + BGR */
  ABGR = 4
};

/**
 * file formats
 */
enum FILEFORMATS {
  /** Image file format: JPEG (JFIF) */
  JPEG = 1,
  /** Image file format: JPEG + EXIF */
  JPEGEXIF = 2,
  /** Image file format: PNG */
  PNG = 3
};

/** Structure to retreive the EXIF data */
typedef struct {
  /** idx tag */
  int             idx;
  /** tag number */
  int             tgnum;
  /** tag name */
  char           *tgname;
} exifreg;

/** Structure to store the EXIF info and some other image data */
typedef struct {
  /** Image filename, no path */
    char           *filename;
  /** Image format code  */
    char            format;
  /** Image color mode code */
    char            colormode;
  /** Image EXIF version used */
    char           *exifversion;
  /** Number of columns */
    unsigned int    width;
  /** Number of rows */
    unsigned int    height;
  /** Year in which image was captured */
    int    year;
  /** Month in which image was captured */
    int    month;
  /** Day in which image was captured */
    int    day;
  /** UTC hour in which image was captured */
    int    UTChr;
  /** UTC minute in which image was captured */
    int    UTCmin;
  /** UTC second in which image was captured */
    int    UTCsec;
  /** Azimuth, picture orientation, angle [0, 360) */
    double          azimuth;
} ImageInfo;

/** Structure to store the camera and shooting info */
typedef struct {
  /** Camera manufacturer */
    char           *makeby;
  /** Camera model */
    char           *model;
  /** Exposure time */
    char           *exposure;
  /** Aperture */
    char           *fnumber;
  /** ISO sensitivity */
    char           *isospeed;
  /** Color components */
    char           *components;
  /** Shutter speed (Exposure Value) */
    char           *shutterspeed;
  /** Aperture (Exposure Value) */
    char           *aperture;
} CamAndShotInfo;

/**
 * \brief Gets the EXIF metadata from a JPEG file.
 *
 * Retrieves tha EXIF information contained in a JPEG file. We use
 * only some of the tags defined in the EXIF 2.2 standard. The
 * information retrieved from file is stored in two different
 * structures: ImageInfo for the data related directly with the image,
 * and a CamShotInfo for the data related with the camera and
 * conditions in which the picture was captured.
 *
 * @param[in] fname is the name of jpeg file.
 * @param[in] az is the azimuth value that must be stored in the
 * ImageInfo structure (next parameter). It's the angle between the
 * geographical north and the middle point in the bottom of image.
 * @param[in] utcoff is a char string with a valid representation of a
 * time zone in ISO format (UTC|+|-|HH:MM). The set of valid strings
 * is the validUTCOffsets array.
 * @param[out] imin is the ImageInfo structure where the image data
 * will be stored.
 * @param[out] casi is the CamAndShotInfo structure where the camera
 * and specific shoting information will be stored.
 *
 * \return
 * - 1 success.
 * - -1 file unreadable.
 * - -2 no JPEG + EXIF file.
 * - -3 NULL struct pointer (imin or casi).
 * - -4 azimuth out of range.
 * - -5 time zone offset out of range.
 *
 * \pre file must be readable and JPEG + EXIF format.
 * \pre az: must be non-negative value in [0, 360).
 * \pre offsethr: its value plus the hour
 * registered in the EXIF info must be greater than or equal to 0
 * and less than 24.
 * \pre offsetmn: its value plus the minutes
 * registered in the EXIF info must be greater than or equal to 0 and
 * less than 60.
 * \pre imin != NULL
 * \pre casi != NULL
 *
 * \post The following fields of ImageInfo structure will be filled by the
 * function call:
 * - filename
 * - format = JPEGEXIF
 * - colormode = RGB or UNKNOWN
 * - exifversion
 * - width
 * - height
 * - year (capture)
 * - month (capture)
 * - day (capture)
 * - UTChr (capture hour, UTC)
 * - UTCmin (capture minute, UTC)
 * - UTCsec (capture second, UTC)
 * - azimuth set to the respective parameter (clockwise angle, north = 0
 * east = 90)
 * \post The following fields of CamAndShotInfo will be filled by the
 * function call:
 * - makeby Camera manufacturer
 * - model  Camera model
 * - exposure Exposure time
 * - fnumber  Aperture
 * - isospeed ISO sensitivity
 * - components Color components
 * - shutterspeed Shutter speed (Exposure Value)
 * - aperture   Aperture (Exposure Value)
 */
int             getImgInfo(char *fname, double az, char *utcoff,
                           ImageInfo * imin, CamAndShotInfo * casi);

#endif
/*
 * imageinfo.h ends here
 */

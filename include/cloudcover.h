#ifndef CLOUDCOVER_H
#define CLOUDCOVER_H
/**
 * @file cloudcover.h
 *
 * @author José Galaviz <jgc@fciencias.unam.mx>
 * @version 1.0
 * \date 9/Jun/2013 - 13:49
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
 * Constant definitions for cloud cover program.
 *
 */
/* Maximum file name string length */
#define MAXFNLEN 256
/* time zone string length */
#define TZLEN 10

/* Error messages */
#define ERR_NARGS "Error: Invalid number of command line arguments\n"
#define ERR_INFIL "Error: Input file unreadable\n"
#define ERR_CFFIL "Error: Configuration file unreadable\n"
#define ERR_WTFIL "Error: Trimmed image file cannot be written\n"
#define ERR_WSFIL "Error: Segmented image file cannot be written\n"
#define ERR_GINFO "Error: reading geographic point location data\n"
#define ERR_IINFO "Error: reading EXIF data from image file\n"

/* Operation messages */
#define MSG_WTFIL "Trimmed image file written\n"
#define MSG_WSFIL "Segmented image file written\n"
#define MSG_CCI1 "Calculating CCI\n"
#define MSG_CCI2 "CCI calculation done\n"

/* More error messages */
char             *diagmsg[15] = {
   "\x0\x0",
   "Empty file name\x0",
   "Cannot create XML parser\x0",
   "Cannot read XML file\x0",
   "Parser error\x0",
   "Error parsing azimuth\x0",
   "Invalid azimuth\x0",
   "Error parsing R/B treshold\x0",
   "Invalid R/B treshold\x0",
   "Error parsing neighborhood side size\x0",
   "Invalid neighborhood side size\x0",
   "Error parsing voting treshold\x0",
   "Invalid voting treshold\x0",
   "Mask file is unreadable\x0",
   "Geographic location file is unreadable\x0"
};

/**
 * Number of categories in which the radial distance in the interest area
 * is divided.
 */
#define NUMCAT 36

/**
 * The pixels in the image interest region are classified according to the
 * square of its distance to the image center. This is used to perform a
 * correction of area, since the lens used deforms the image in fuction of
 * the radial distance. Therefore, given the square of the radial distance
 * of some pixel, perform a search of such value in the array
 * below. Retrieve the first index greater or equal to the given value. The
 * index obtained could be used as index in the factors array (defined
 * below), to obtain the weight factor used for that pixel.
 */
const int categories[NUMCAT] = {
    4225,
    44944,
    88209,
    133956,
    184900,
    242064,
    308025,
    386884,
    492804,
    929296,
    1028196,
    1102500,
    1162084,
    1212201,
    1258884,
    1299600,
    1336336,
    1371241,
    1401856,
    1432809,
    1461681,
    1488400,
    1512900,
    1537600,
    1560001,
    1582564,
    1602756,
    1623076,
    1640961,
    1661521,
    1679616,
    1695204,
    1713481,
    1729225,
    1745041,
    1750329
};

/**
 * Array of correction factor used to weight eah pixel according to its
 * radial distance to the image center.
 */
const double factors[NUMCAT] = {
    1.00,
    0.99,
    0.98,
    0.97,
    0.96,
    0.95,
    0.94,
    0.93,
    0.92,
    0.91,
    1.00,
    0.99,
    0.98,
    0.97,
    0.96,
    0.95,
    0.94,
    0.93,
    0.92,
    1.01,
    1.02,
    1.03,
    1.04,
    1.05,
    1.06,
    1.07,
    1.08,
    1.09,
    1.10,
    1.11,
    1.12,
    1.13,
    1.14,
    1.15,
    1.16,
    1.17
 };

#define FALSE 0
#define TRUE  1

/* default values for execution parameters */
#define MSKFILE "imagemask.png"
#define GPLFILE "site.xml"
#define RBTRESH 0.8
#define NEIGHBS 5
#define VOTESFL 10
#define AZIMUTH 0.0
#define LATITUD 19.3
#define LONGITU -99.2
#define ELEVATI 2240
#define UTCZONE "UTC-06:00"

/* constats used to read the XML configuration file */
#define TAGMSK "MskFile"
#define TAGLOC "LocationFile"
#define TAGAZI "Azimuth"
#define TAGTRE "RBTreshold"
#define TAGCON "Convolution"
/* Offsets needed to read XML config file */
#define OFFMSK 0 /* mask file */
#define OFFLOC 1 /* location file */
#define OFFAZI 2 /* azimuth */
#define OFFRBT 3 /* RB treshold */
#define OFFNSZ 4 /* neighborhood size */
#define ATTRSZ 1 /* as attribute of convolution */
#define OFFVTF 5 /* votes to flip */
#define ATTRVF 3 /* as attribute of convolution */
#endif
/* cloudcover.h ends here */

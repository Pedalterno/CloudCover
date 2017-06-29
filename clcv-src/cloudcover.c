/**
 * @file cloudcover.c
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
 * Main program for Clound Cover Index (CCI) calculation.
 *
 */
#define _GNU_SOURCE
#include<stdio.h>
#include<sys/stat.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>
#include<expat.h>
#include"imageio.h"
#include"geoinfo.h"
#include"imageinfo.h"
#include"timedate.h"
#include"cloudcover.h"

/* Command line input params */

/* Image input filename */
char              *infname;

/* Configuration XML filename */
char              *cffname;

/* Trimmed image filename */
char              *trfname;

/* Segmented image filename */
char              *sgfname;

/* Input params in config file */
struct cfgparams {
   /* Red/Blue treshold: r/b pix ratio < treshold -> sky */
   double            rbtreshold;
   /* Neightborhood size in the convolution process */
   int               neighbsize;
   /* Number of votes needed to flip the pixel classification */
   int               votes2flip;
   /* Name for the mask file */
   char              msfname[MAXFNLEN];
   /* Name for the geolocation file */
   char              glfname[MAXFNLEN];
   /* Azimuth for the camera orientation */
   double            azimuth;
} cfgvals;

/* Geographic information read from the geoinfo file */
double            latitude;
double            longitude;
double            elevation;
char              timezn[TZLEN];

/* Image data */
int               width, height;
unsigned int      **image = NULL;
unsigned int      **imageseg = NULL;
unsigned int      **imagecnv = NULL;

/* Output data */
int               year;
int               month;
int               day;
int               hour;
int               minute;
int               sec;

double            jdn;
/*
int               juldate;
double            julfrac;
*/

double            ccindex;

/* Auxiliary variables used by XML parser */
char             *valores[6];
int               reading, idxrd;

GeoInfo        locinfo;
ImageInfo      imginfo;
CamAndShotInfo phinfo;

/**
 * \brief Set the default values for the configuration variables.
 * This function access the global variables where such parameters are
 * stored.
 */
void
setDefaults() {
   int               i;

   infname = cffname = sgfname = trfname = NULL;
   for (i = 0; i < MAXFNLEN; i++) {
      cfgvals.msfname[i] = cfgvals.glfname[i] = '\x0';
   }
   strcpy(cfgvals.msfname, MSKFILE);
   strcpy(cfgvals.glfname, GPLFILE);
   cfgvals.rbtreshold = RBTRESH;
   cfgvals.neighbsize = NEIGHBS;
   cfgvals.votes2flip = VOTESFL;
   cfgvals.azimuth = AZIMUTH;
   latitude = LATITUD;
   longitude = LONGITU;
   elevation = ELEVATI;
   for (i = 0; i < TZLEN; i++) {
      timezn[i] = '\x0';
   }
   strcpy(timezn, UTCZONE);

   year = 0;
   month = 0;
   day = 0;
   hour = 0;
   minute = 0;
   sec = 0;
   jdn = 0.0;
   ccindex = 0.0;
}

/**
 * \brief Verifies if a given file is readable.
 *
 * Given a file name, verifies if such file is readable.
 *
 * @param[in] fname is the file name
 * \return 0 if file cannot be read, 1 otherwise.
 */
int
checkFileForRead(char *fname) {
   struct stat       st;
   int               status;
   /* Estado del archivo: existencia (stat != -1), size != 0, archivo regular y
      permisos de lectura o escritura */
   status = stat(fname, &st);
   if ((status == -1) || (st.st_size == 0) ||
       !S_ISREG(st.st_mode) || !(st.st_mode & S_IRUSR)) {
      return 0;
   }
   else
      return 1;
}

/**
 * Expat processing element routine.
 */
static void       XMLCALL
procesa(void *data, const char *s, int len) {
   if (reading) {
      if (idxrd != 4) {
         valores[idxrd] = (char *) malloc(len + 1);
         strncpy(valores[idxrd], s, len);
      }
      idxrd++;
   }
}

/**
 * Expat start of element routine.
 */
static void       XMLCALL
inicio(void *data, const char *el, const char **attr) {
   if (!strcasecmp(el, TAGMSK)) {
      reading = 1;
      idxrd = OFFMSK;
   }
   else if (!strcasecmp(el, TAGLOC)) {
      reading = 1;
      idxrd = OFFLOC;
   }
   else if (!strcasecmp(el, TAGAZI)) {
      reading = 1;
      idxrd = OFFAZI;
   }
   else if (!strcasecmp(el, TAGTRE)) {
      reading = 1;
      idxrd = OFFRBT;
   }
   else if (!strcasecmp(el, TAGCON)) {
      reading = 1;
      idxrd = OFFNSZ;
      valores[OFFNSZ] = (char *) malloc(5);
      valores[OFFVTF] = (char *) malloc(5);
      strncpy(valores[OFFNSZ], attr[ATTRSZ], 5);
      strncpy(valores[OFFVTF], attr[ATTRVF], 5);
   }
   else {
      reading = 0;
   }
}

/**
 * Expat end of element routine.
 */
static void       XMLCALL
fin(void *data, const char *el) {
   reading = 0;
}

/**
 * \brief Read the execution parameters from an XML file.
 *
 * Given the name of an XML configuration file, this function reads the
 * execution parameters from such file and validate them (if possible).
 * @param[in] fname is the name of XML file which contain the config values.
 * @param[out] cfgv is the sructure where the config values will be stored.
 * \return 0 if success. An error code otherwise.
 *
 */
int
getConfig(char *fname, struct cfgparams *cfgv) {
   char              Buff[1024];
   int               len;
   int               done = 0;
   char             *endptr;
   FILE             *file = fopen(fname, "rb");
   if (file == NULL)
      return 1;
   XML_Parser        p = XML_ParserCreate(NULL);
   if (!p) {
      fclose(file);
      return 2;
   }

   XML_SetElementHandler(p, inicio, fin);
   XML_SetCharacterDataHandler(p, procesa);
   do {
      len = (int) fread(Buff, 1, 1024, file);
      if (!len || ferror(file)) {
         fclose(file);
         return 3;
      }
      done = feof(file);

      idxrd = 0;
      if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
         fclose(file);
         return 4;
      }
   } while (!done);
   XML_ParserFree(p);
   strncpy(cfgv->msfname, valores[0], MAXFNLEN);
   strncpy(cfgv->glfname, valores[1], MAXFNLEN);
   cfgv->azimuth = strtod(valores[2], &endptr);
   if (endptr == valores[2]) {
      fclose(file);
      return 5;
   }
   if ((cfgv->azimuth < 0.0) || (cfgv->azimuth > 360)) {
      fclose(file);
      return 6;
   }

   cfgv->rbtreshold = strtod(valores[3], &endptr);
   if (endptr == valores[3]) {
      fclose(file);
      return 7;
   }
   if ((cfgv->rbtreshold < 0.0) || (cfgv->rbtreshold > 1.0)) {
      fclose(file);
      return 8;
   }

   cfgv->neighbsize = strtod(valores[4], &endptr);
   if (endptr == valores[4]) {
      fclose(file);
      return 9;
   }
   if (cfgv->neighbsize <= 0.0) {
      fclose(file);
      return 10;
   }

   cfgv->votes2flip = strtod(valores[5], &endptr);
   if (endptr == valores[5]) {
      fclose(file);
      return 11;
   }
   if ((cfgv->votes2flip < 0.0) ||
       (cfgv->votes2flip > (cfgv->neighbsize) * (cfgv->neighbsize))) {
      fclose(file);
      return 12;
   }

   fclose(file);
   /* Validation of mask and geo-location files */
   if (!checkFileForRead(cfgv->msfname))   /* mask file readable */
      return 13;
   if (!checkFileForRead(cfgv->glfname))   /* geo file readable */
      return 14;

   return 0;
}

/**
 * \brief Displays a message and terminates program execution.
 *
 * Display in stderr a complete description of program usage, an error
 * message and terminate the program execution with an error code returned
 * to OS.
 * @param[in] prgname is the binary executable program.
 * @param[in] errmsg error message string.
 * @param[in] errcode error code returned to OS.
 */
void
usage(char *prgname, char *errmsg, int errcode) {
   fprintf(stderr, "Cloud Cover Index Calculation\n");
   fprintf(stderr, "Program to estimate the Cloud Cover Index (CCI).\n");
   fprintf(stderr, "A whole sky photograph must be processed.\n");
   fprintf(stderr, "This program is Free Software and can be distributed\n");
   fprintf(stderr, "under the GNU General Public License.\n\n");
   fprintf(stderr, "cloudcover Ver. 1.0, Copyright (C) 2013, José Galaviz,\n");
   fprintf(stderr, "Departamento de Matemáticas,\n");
   fprintf(stderr, "Facultad de Ciencias,\n");
   fprintf(stderr, "Universidad Nacional Autónoma de México\n");
   fprintf(stderr, "jgc@fciencias.unam.mx\n\n");
   fprintf(stderr, "Usage:\t%s ", prgname);
   fprintf(stderr, "-c <XML config file (optional)> ");
   fprintf(stderr, "-t <trimmed image file (optional)> ");
   fprintf(stderr, "-s <segmented image file (optional)> ");
   fprintf(stderr, "<input image file> \n");
   fprintf(stderr, "Output (tab separated):\n");
   fprintf(stderr, "Year, Month, Date, Hour, Min, Sec, JD, JH, ");
   fprintf(stderr, "Lat, Lon, Ele, Azim, RBThr,NSide, Conv, CCI\n");
   fprintf(stderr, "JD = Julian Day Number.\n");
   fprintf(stderr, "JH = Julian Hour (fraction of the day).\n");
   fprintf(stderr, "Ele = Elevation (in meters).\n");
   fprintf(stderr, "RBThr = Red/Blue Threshold used to classify.\n");
   fprintf(stderr, "NSide = Neighborhood side size used in convolution.\n");
   fprintf(stderr, "Conv = Number of votes used to change class.\n");
   fprintf(stderr, "CCI = Cloud Cover Index (fraction of sky covered).\n");
   fprintf(stderr, "\n%s\n", errmsg);
   exit(errcode);
}

/**
 * \brief Display an error message and terminate program.
 * This function is called after XML parsing.
 * @param[in] c is the error code given by the XML parsing function.
 */
void
diagnostic(int c) {
   fprintf(stderr, "XML Configuration file problem\n");
   fprintf(stderr, "%s\n", diagmsg[c]);
   exit(4);
}

/**
 * \brief Capture all the command line options.
 * Capture the name of files given in command line and validate them
 * (verify readability).
 * @param[in] na is the original commnd line argument counter.
 * @param[in] la are the command line string.
 * @param[out] confile is the config file name.
 * @param[out] trifile is the trimmed file name.
 * @param[out] segfile is the segmented file name.
 */
void
catchParams(int na, char *la[], char **inpfile,
            char **confile, char **trifile, char **segfile) {
   char c;

   while ((c = getopt(na, la, "c:t:s:")) != -1) {
      switch (c) {
         case 'c':
            *confile = (char *) malloc(MAXFNLEN);
            strncpy(*confile, optarg, MAXFNLEN);
            break;
         case 't':
            *trifile = (char *) malloc(MAXFNLEN);
            strncpy(*trifile, optarg, MAXFNLEN);
            break;
         case 's':
            *segfile = (char *) malloc(MAXFNLEN);
            strncpy(*segfile, optarg, MAXFNLEN);
            break;
      }
   }
   if (optind != (na - 1))
      usage(la[0], ERR_NARGS, 1);
   /* Input image file name */
   *inpfile = (char *) malloc(MAXFNLEN);
   strncpy(*inpfile, la[optind], MAXFNLEN);

   /* Validate command line arguments */
   if (!checkFileForRead(*inpfile))  /* input file readable */
      usage(la[0], ERR_INFIL, 2);
   if (!checkFileForRead(*confile))  /* config file readable */
      usage(la[0], ERR_CFFIL, 3);
}

/**
 * \brief Display in, standard error, the values of config params.
 *
 */
void
logParams() {
   fprintf(stderr, "Input: %s|\n", infname);
   if (cffname != NULL) {
      fprintf(stderr, "Use config file\n");
      fprintf(stderr, "Config: %s|\n", cffname);
   }
   if (sgfname != NULL) {
      fprintf(stderr, "Use segmented file\n");
      fprintf(stderr, "Segmented: %s|\n", sgfname);
   }
   if (trfname != NULL) {
      fprintf(stderr, "Use trimmed file\n");
      fprintf(stderr, "Trimmed: %s|\n", trfname);
   }
   fprintf(stderr, "Mask file: %s\n", cfgvals.msfname);
   fprintf(stderr, "Geo-location file: %s\n", cfgvals.glfname);
   fprintf(stderr, "Azimuth: %f\n", cfgvals.azimuth);
   fprintf(stderr, "R/B Treshold: %f\n", cfgvals.rbtreshold);
   fprintf(stderr, "Convolution neighborhood side size: %d\n",
           cfgvals.neighbsize);
   fprintf(stderr, "Convolution voting treshold: %d\n", cfgvals.votes2flip);
}

/**
 * \brief Determines the weight factor used for a given pixel.
 *
 * Given the radial distance between some pixel and the image center, this
 * function gets the index that must be used in the factors array to weight
 * that pixel in the pixel counting.
 * @param[in] t target to be search.
 * @param[in] carr is the array where the search must be performed (in our
 * case the categories array must be used.
 * @param[in] low minimum index in the carr array to be used for search
 * (inclusive).
 * @param[in] ipp maximum index in the carr array to be used for search
 * (inclusive).
 * \return the index in the carra array where the first value greater or
 * equal to the given value (t) is stored. This is the index that must be
 * used in the factors array.
 */
int catsearch(int t, const int carr[], int low, int upp) {
  int l, u, m;
  if ((low < upp) && (carr != NULL)) {
    l = low;
    u = upp;
    while (l <= u) {
      m = (int)((l + u) / 2);
      if (carr[m] < t) l = m + 1;
      else if (carr[m] > t) u = m - 1;
      else return m;
    }
    if (l <= upp)
      return l;
  }
  return -1;
}

/**
 * \brief Read an image from a JPEG file and cuts it to the size of a given
 * mask.
 *
 * Given a mask, whose size is enough to contain, exactly, the interest
 * region of the images, this function reads and crop an original image to
 * get only the interest region.
 *
 * @param[in] fname is the name of the file which contains the original
 * JPEG RGB EXIF image to be processed for the program.
 * @param[in] mskf is the filename of a PNG image which is the mask used to
 * filter only the interest region of the whole original image.
 * @param[out] wd is the width of interest region and, therefore, the width
 * of resulting buffer image returned by the function. It's set from the
 * width of mask.
 * @param[out] hg is the height of interest region and, therefore, the height
 * of resulting buffer image returned by the function. It's set from the
 * height of mask.
 * \return a 2-dimensional buffer with the data of image interest
 * region. The data of pixel in row i, column j of such region is stored as
 * the integer in the position [i][j] of the buffer returned by the
 * function. The most significant byte is the A (alpha) component
 * (transparency) of the pixel, followed by the R (red) component, the next
 * one is the G (green), the least significant bye is the B (blue)
 * component. Thus the integer (hex format) is AARRGGBB.
 */
unsigned int **readAndCut(char *fname, char *mskf, int *wd, int *hg) {
   int wm, hm, wi, hi, i, j, mv, mh;
   unsigned int **img = readJPGImage(fname, &wi, &hi);
   unsigned int **msk = readPNGImage(mskf, &wm, &hm);
   unsigned int **res = (unsigned int **) malloc(hm *
                                                 sizeof(unsigned int *));
   unsigned int *whole = (unsigned int *) malloc(hm * wm *
                                                 sizeof(unsigned int));
   for (i = 0; i < hm; i++)
      res[i] = whole + i * wm;

   mh = (int) ((double)wi - (double)wm) / 2.0;
   mv = (int) ((double)hi - (double)hm) / 2.0;
   for (i = 0; i < hm; i++) {
      for (j = 0; j < wm; j++) {
         res[i][j] = msk[i][j] & img[i + mv][j + mh];
      }
   }
   free(img);
   free(msk);
   *wd = wm;
   *hg = hm;
   return res;
}

/**
 * \brief Convolution operator to smooth borderline.
 *
 * Given the segmented image, this function smooth the borderline between
 * the two diferent classes.
 * @param[in] sdsz side size of neighborhood. This must be an odd integer.
 * @param[in] mv minimum number of votes in the neighborhood needed to
 * change the central pixel value.
 * @param[in] img is the image to be convolved.
 * @param[in] w image width.
 * @param[in] h image height.
 * \return A new image buffer with the resulting convolved image.
 *
 */
unsigned int **convolution(int sdsz, int mv,
                            unsigned int **img, int w, int h ) {
   int i, j, pelcolor, nesi = (int) ((double)sdsz / 2.0);
   int  n, m, nv;
   unsigned int **res = (unsigned int **) malloc(h *
                                                 sizeof(unsigned int *));
   unsigned int *whole = (unsigned int *) malloc(h * w *
                                                 sizeof(unsigned int));
   for (i = 0; i < h; i++)
      res[i] = whole + i * w;

   for (i = nesi; i < h - nesi; i++) {
      for (j = nesi; j < w - nesi; j++) {
         pelcolor = img[i][j];
         if (pelcolor == 0X00000000) {
            res[i][j] = 0X00000000;
         }
         else {
            nv = 0;
            for (n = i - nesi; n <= i + nesi; n++) {
               for (m = j - nesi; m <= j + nesi; m++) {
                  if ((img[n][m] ^ pelcolor) != 0) {
                     nv++;
                  }
               }
            }
            if (nv >= mv) res[i][j] = 0XFF000000 | ~pelcolor;
            else res[i][j] = 0XFF000000 | pelcolor;
         }
      }
   }
   return res;
}

/**
 * \brief Classify the image pixels in two different cathegories (sky and
 * cloud).
 *
 * Uses the R/B criteria for pixel classification. If the ratio R/B of
 * pixel is below the threshold the pixel is classified as sky, if is above
 * or equal the threshold, is classified as cloud pixel.
 * @param[in] thr is the threshold used to classify.
 * @param[out] img is the input image.
 * @param[in] wd is the image width.
 * @param[in] hg is the image height.
 * \return the new segmented image. Black opaque pixels = Sky. White opaque
 * pixels = Cloud.
 */
unsigned int **filterRB(double thr, unsigned int **img, int wd, int hg) {
   int i, j, pelcolor;
   double ratio;
   unsigned int **res = (unsigned int **) malloc(hg *
                                                 sizeof(unsigned int *));
   unsigned int *whole = (unsigned int *) malloc(hg * wd *
                                                 sizeof(unsigned int));
   for (i = 0; i < hg; i++)
      res[i] = whole + i * wd;

   for (i = 0; i < wd; i++) {
      for (j = 0; j < hg; j++) {
         pelcolor = img[i][j];
         /*outside region, black and transparent */
         if ((pelcolor & 0X00FFFFFF) == 0) {
            res[i][j] =  0X00000000;
         }
         else { /* inside region */
            /* take the red chanel value */
            ratio = (double)((pelcolor & 0X00FF0000) >> 16);
            /* divide by the blue chanel value */
            ratio = ratio / (double)(pelcolor & 0X000000FF);
            /* if ratio is below the threshold */
            if (ratio < thr) {
               /* black (but opaque) means sky */
               res[i][j] =  0XFF000000;
            }
            else { /* ratio above or equal the threshold */
               /* white and opaque means cloud */
               res[i][j] = 0XFFFFFFFF;
            }
         } /* inside picture */
      } /* for j */
   } /* for i */
   return res;
}

/**
 * \brief Calculate the Cloud Cover Index.
 *
 * Performs the calculation of CCI (Cloud Cover Index) in the segmentes
 * image. A correction factor is used given that the lens used deforms
 * areas in diferent way according to the radial distance of pixel.
 *
 * @param[in] img is the image where the CCI will be calculated.
 * @param[in] w image width.
 * @param[in] h image height.
 * @param[out] ta is the total weighted area in the image interest
 * region (whole sky area).
 * @param[out] tp total number of pixels in the interest region.
 * \return a double value with the proportion of image covered by clouds,
 * according to the segmented image (white pixels proportion in the
 * interest region)
 */
double cloudcoverindex(unsigned int **img, int w, int h, double *ta, int *tp) {
   double total = 0.0, clouds = 0.0;
   int    pels = 0;
   double sqdist, rcenter, ccenter;
   int i, j, pelcolor, idxc;
   rcenter = (int) ((double) h / 2.0);
   ccenter = (int) ((double) w / 2.0);

   for (i = 0; i < h; i++) {
      for (j = 0; j < w; j++) {
         pelcolor = img[i][j];
         if ((pelcolor == 0XFF000000) || (pelcolor == 0XFFFFFFFF)) {
            sqdist = (i - rcenter) * (i - rcenter) +
               (j - ccenter) * (j - ccenter);
            idxc = catsearch(sqdist, categories, 0, NUMCAT - 1);
            total += factors[idxc];
            pels++;
            if (pelcolor == 0XFFFFFFFF) {
               clouds += factors[idxc];
            }
         }
      }
   }
   *ta = total;
   *tp = pels;
   return clouds / total;
}

/**
 * \brief Program to estimate the Cloud Cover Index from whole sky
 * photographs.
 *
 * Given a digital picture of whole sky, this program estimate the
 * proportion of sky covered by clouds.
 * The command line parameters:
 * -c <configuration file>
 * -s <segmented image file> (optional)
 * -t <trimmed image file> (optional)
 * -input image file
 *
 * The output is: the segmented and trimmed images (if requested), and the
 * following 15 data values in a text line:
 * year month day hour minute sec juliandate
 * latitude longitude elevation azimuth
 * rbtreshold neighbsize votes2flip
 * ccindex
 *
 * year, month, day, hour, minute, sec and julian date when image was
 * taken.
 * latitude, longitude and elevation of the geographic location where the
 * picture was taken.
 * azimuth refers to the orientation of camera at the moment the picture
 * was taken, 0 means the north is centered at the bottom of image,
 * othewise denotes the clockwise angle in degrees where the north is.
 * rbthreshold is the threshold used to classify the pixels, if the R/B
 * ratio is above this threshold the pixel classified as "cloud".
 * neighbsize and votes2flip are used to regulate the behavior of the
 * convolution operator.
 * ccindex is, finally, the proportion of sky covered by clouds.
 */
int
main(int argc, char *argv[]) {
   int               res;
   double            totalarea;
   int               totalpels;

   setDefaults();
   /*
      Process command line */
   if (argc < 2)
      usage(argv[0], ERR_NARGS, 1);
   /* catch all the command line input parameters */
   catchParams(argc, argv, &infname, &cffname, &trfname, &sgfname);
   /*  Get the configuration params from xml file */
   res = getConfig(cffname, &cfgvals);
   if (res)
      diagnostic(res);
#ifdef DEBUG
   freopen("CloudCover.log", "w", stderr);
   logParams();
#endif
   /* reading geographic point location info */
   res = getGeoInfo(cfgvals.glfname, &locinfo);
   if (res) {
      fprintf(stderr, ERR_GINFO);
      exit(5);
   }
   latitude = locinfo.latitude;
   longitude = locinfo.longitude;
   elevation = locinfo.elevation;

   /* reading exif data from image file */
   res = getImgInfo(infname, cfgvals.azimuth, timezn, &imginfo, &phinfo);
   if (res != 1) {
      fprintf(stderr, ERR_IINFO);
      exit(6);
   }
   year = imginfo.year;
   month = imginfo.month;
   day = imginfo.day;
   hour = imginfo.UTChr;
   minute = imginfo.UTCmin;
   sec = imginfo.UTCsec;
   jdn = julianDate(year, month, day, hour, minute, (double) sec);

   /* reading and trimming image file */
   image  = readAndCut(infname, cfgvals.msfname, &width, &height);

   if (trfname != NULL) {
      res = writePNGImage(image, trfname, width, height);
      if (res != 1)
         fprintf(stderr, ERR_WTFIL);
      else
         fprintf(stderr, MSG_WTFIL);
   }
   imageseg = filterRB(cfgvals.rbtreshold, image, width, height);
   imagecnv = convolution(cfgvals.neighbsize, cfgvals.votes2flip,
                          imageseg, width, height);
   if (sgfname != NULL) {
      res = writePNGImage(imagecnv, sgfname, width, height);
      if (res != 1)
         fprintf(stderr, ERR_WSFIL);
      else
         fprintf(stderr, MSG_WSFIL);
   }
   fprintf(stderr, MSG_CCI1);
   ccindex = cloudcoverindex(imagecnv, width, height, &totalarea, &totalpels);
   fprintf(stderr, MSG_CCI2);

   /*
Year, Month, Date, Hour, Min, Sec, JD,
%d %d %d %d %d %f %f %f %f %f %f %f %d %d %f\n
Lat, Lon, Ele, Azim,

RBThr, NSide, Conv, CCI.
   */
   printf("%d %d %d %d %d %d %f %f %f %f %f %f %d %d %f\n",
          year, month, day, hour, minute, sec, jdn,
          latitude, longitude, elevation, cfgvals.azimuth,
          cfgvals.rbtreshold, cfgvals.neighbsize, cfgvals.votes2flip, ccindex);

   return 0;
} /* cloudcover.c ends here */

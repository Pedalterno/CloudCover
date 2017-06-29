/**
 * @file geoinfo.c
 *
 * @author José Galaviz <jgc@fciencias.unam.mx>
 * @version 1.0
 * \date 1/Oct/2012 - 15:35
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
 * Library functions to retrieve geographical data from a xml file.
 *
 */
#include<expat.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"geoinfo.h"
#include"timedate.h"

/**
 * Indexes used internally to access geographical data stored in XML file
 */
enum IDXARRAY {
  LAT = 0, /* Internal use. Index of latitude in parsing array */
  LON = 1, /* Internal use. Index of longitude in parsing array */
  ELE = 2, /* Internal use. Index of elevation in parsing array */
  TZO = 3, /* Internal use. Index of timezone in parsing array */
  BUFFSIZE = 8192 /* input file buffer size */
};


int             reading, idxrd;
char           *valores[4];

/*
 * Expat processing element routine.
 */
static void     XMLCALL
procesa(void *data, const char *s, int len)
{
    if (reading) {
        valores[idxrd] = (char *) malloc(len + 1);
        strncpy(valores[idxrd], s, len);
        valores[idxrd][len] = '\x0';
        idxrd++;
    }
}

/*
 * Expat start of element routine.
 */
static void     XMLCALL
inicio(void *data, const char *el, const char **attr)
{
    if (!strcmp(el, "latitude")) {
        reading = 1;
        idxrd = LAT;
    }
    else if (!strcmp(el, "longitude")) {
        reading = 1;
        idxrd = LON;
    }
    else if (!strcmp(el, "elevation")) {
        reading = 1;
        idxrd = ELE;
    }
    else if (!strcmp(el, "timezone")) {
        reading = 1;
        idxrd = TZO;
    }
    else {
        reading = 0;
    }
}

/*
 * Expat end of element routine.
 */
static void     XMLCALL
fin(void *data, const char *el)
{
    reading = 0;
}

/**
 * \brief Reads the geographical information from an XML file.
 */
int
getGeoInfo(char *fname, GeoInfo * gi)
{
    char            Buff[BUFFSIZE];
    int             len, i;
    int             done = 0;
    char           *endptr;
    FILE           *file = fopen(fname, "rb");
    for (i = 0; i < 10; gi->timezone[i++] = '\x0');
    if (file == NULL)
        return 0;
    XML_Parser      p = XML_ParserCreate(NULL);
    if (!p) {
      fclose(file);
      return 1;
    }

    XML_SetElementHandler(p, inicio, fin);
    XML_SetCharacterDataHandler(p, procesa);
    do {
        len = (int) fread(Buff, 1, BUFFSIZE, file);
        if (!len || ferror(file)) {
          fclose(file);
          return 2;
        }
        done = feof(file);

        idxrd = 0;
        if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
          fclose(file);
          return 3;
        }
    } while (!done);
    XML_ParserFree(p);
    gi->latitude = strtod(valores[0], &endptr);
    if (endptr == valores[0]) {
      fclose(file);
      return 4;
    }
    if ((gi->latitude < MINLAT) || (gi->latitude > MAXLAT)) {
      fclose(file);
      return 5;
    }
    gi->longitude = strtod(valores[1], &endptr);
    if (endptr == valores[1]) {
      fclose(file);
      return 6;
    }
    if ((gi->longitude < MINLON) || (gi->longitude > MAXLON)) {
      fclose(file);
      return 7;
    }
    gi->elevation = strtod(valores[2], &endptr);
    if (endptr == valores[2]) {
      fclose(file);
      return 8;
    }
    if ((gi->elevation < MINELE) || (gi->elevation > MAXELE)) {
      fclose(file);
      return 9;
    }
    len = strlen(valores[3]);
    if ((len > 0) && (len < 10) && isValidUTC(valores[3]))
        strcpy(gi->timezone, valores[3]);
    else {
      fclose(file);
      return 10;
    }
    fclose(file);
    return 0;
}
/*
 * geoinfo.c ends here
 */

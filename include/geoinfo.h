/**
 * @file geoinfo.h
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
 * Reads the relevant geographic information from a xml file. The
 * information is sotored in a GeoInfo structure.
 */
#ifndef GEOINFO_H
#define GEOINFO_H

/**
 * Valid range values for geographical data
 */
enum RANGES {
  /** Minimum latitude */
  MINLAT = -90,
  /** Maximum latitude */
  MAXLAT = 90,
  /** Minimum longitude */
  MINLON = -180,
  /** Maximum longitude */
  MAXLON = 180,
  /** Minimum elevation (sea level, i.e. geographic altitude) */
  MINELE = 0,
  /** Maximum elevation (Mnt. Everest) */
  MAXELE = 8840
};

/** Structure to store the geographic data */
typedef struct {
  /** angle in [-90, 90] */
  double          latitude;
  /** angle in [-180, 180] */
  double          longitude;
  /** meters, [0, 8840] */
  double          elevation;
  /** Timezone (e.g. "UTC-06:00") */
  char            timezone[10];
} GeoInfo;

/**
 * \brief Reads the geographical information from an XML file.
 *
 * The function read, from a XML file, the geographical information.
 * The data read are: latitude, longitude, elevation and timezone.
 *
 * IMPORTANT. Even the XML file contains the daylight saving time
 * information, this fuction currently (Ver. 1.0, ocober 2012) does
 * not parse that data.
 *
 * @param[in] fname is the name of the XML file which contains the
 * relavant information.
 * @param[out] gi is the GeoInfo structure where the data will be
 * stored.
 * \return
 * - 0 success
 * - 1 I/O error, XML file cannot be open to read
 * - 2 I/O error, file cannot be read
 * - 3 XML error, parse error
 * - 4 XML error, latitude cannot be read
 * - 5 latitude out of range [-90, 90]
 * - 6 XML error, longitude cannot be read
 * - 7 longitude out of range [-180, 180]
 * - 8 XML error, elevation cannot be read
 * - 9 elevation out of range [0, 8840]
 * - 10 Invalid UTC Offset.
 */
int             getGeoInfo(char *fname, GeoInfo * gi);
#endif
/*
 * geoinfo.h ends here
 */

/**
 * @file test-geoinfo.c
 *
 * @author José Galaviz <jgc@fciencias.unam.mx>
 * @version 1.0
 * \date 1/Oct/2012 - 19:36
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
 * Unit test for geoinfo
 *
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"geoinfo.h"

int
main()
{
    GeoInfo         gis;
    int             success = 0, total = 4;
    char            cadlat[30], cadlon[30];

    int             res = getGeoInfo("../etc/Tlahuizcalpan.xml", &gis);
    if (res) return 0;
    /* trunc to 4 decimal places */
    gis.latitude = floor(gis.latitude * 10000.0) / 10000.0;
    gis.longitude = floor(gis.longitude * 10000.0) / 10000.0;
    sprintf(cadlat, "%f", gis.latitude);
    sprintf(cadlon, "%f", gis.longitude);
    if (!strcmp("19.323400", cadlat)) {
      success++;
    }
    else {
      fprintf(stderr, "Latitude test failed %s != 19.323400\n", cadlat);
    }
    if (!strcmp("-99.179100", cadlon)) {
      success++;
    }
    else {
      fprintf(stderr, "Longitude test failed %s != -99.179100\n", cadlon);
    }
    if (gis.elevation == 2240) {
      success++;
    }
    else {
      fprintf(stderr, "Elevation test failure %f\n", gis.elevation);
    }
    if (!strcmp(gis.timezone, "UTC-06:00")) {
      success++;
    }
    else {
      fprintf(stderr, "Timezone test failed %s\n", gis.timezone);
    }
    fprintf(stderr, " %d successful of %d tests \n", success, total);
    if (success == total)
      return 1;
    else
      return 0;
}/* test-geoinfo.c ends here */

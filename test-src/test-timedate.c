/**
 * @file test-timedate.c
 *
 * @author José Galaviz <jgc@fciencias.unam.mx>
 * @version 1.0
 * \date 29/Sep/2012 - 12:55
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
 * Unit test for time and date calculations.
 *
 */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"timedate.h"

int
main()
{
    int             y, m, d, h, mi;
    double          sec;
    double          jd;

    int             yr, mr, dr, hr, mir;
    double          secr;
    double          jdr;
    double          cacho;

    int             res, success = 0;
    int             total = 13;

    /*
     * 2012 / 09 / 28 23:59:59.9
     */
    y = 2012;
    m = 9;
    d = 28;
    h = 23;
    mi = 59;
    sec = 59.9;
    jd = 2456199.4999;
    jdr = julianDate(y, m, d, h, mi, sec);
    /*
     * round to 4 decimal places
     */
    jdr = floor(jdr * 10000.0) / 10000.00;
    jd = floor(jd * 10000.0) / 10000.00;
    res = calDateTime(jd, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "In:  %4d/%02d/%02d %02d:%02d:%07.4f\t",
            y, m, d, h, mi, sec);
    fprintf(stderr, "Out: Julian date = %12.4f\n", jdr);
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian date = %12.4f\n", jd);
#endif
    if ((jd == jdr) &&
        (y == yr) && (m == mr) && (d == dr) && (h == hr) && (mi == mir)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    /*
     * 2008 / 02 / 28 23:59:59.9
     */
    y = 2008;
    m = 2;
    d = 28;
    h = 23;
    mi = 59;
    sec = 59.9;
    jd = 2454525.4999;
    jdr = julianDate(y, m, d, h, mi, sec);
    /*
     * round to 4 decimal places
     */
    jdr = floor(jdr * 10000.0) / 10000.00;
    jd = floor(jd * 10000.0) / 10000.00;
    res = calDateTime(jd, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "In:  %4d/%02d/%02d %02d:%02d:%07.4f\t",
            y, m, d, h, mi, sec);
    fprintf(stderr, "Out: Julian date = %12.4f\n", jdr);
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian date = %12.4f\n", jd);
#endif
    if ((jd == jdr) &&
        (y == yr) && (m == mr) && (d == dr) && (h == hr) && (mi == mir)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    /*
     * 2008 / 02 / 29 12:00:00
     */
    y = 2008;
    m = 2;
    d = 29;
    h = 13;
    mi = 0;
    sec = 0.0;
    jd = 2454526.0416;
    jdr = julianDate(y, m, d, h, mi, sec);
    /*
     * round to 4 decimal places
     */
    jdr = floor(jdr * 10000.0) / 10000.00;
    jd = floor(jd * 10000.0) / 10000.00;
    res = calDateTime(jd + 0.0001, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "In:  %4d/%02d/%02d %02d:%02d:%07.4f\t",
            y, m, d, h, mi, sec);
    fprintf(stderr, "Out: Julian date = %12.4f\n", jdr);
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian date = %12.4f\n", jd);
#endif
    if ((jd == jdr) &&
        (y == yr) && (m == mr) && (d == dr) && (h == hr) && (mi == mir)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    cacho = timeDayFrac(15, 5, 30.0);
    calDateTime(jd + cacho, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian offset = %12.4f\n", jd + cacho);
#endif
    if ((y == yr) && (mr == 3) && (dr == 1) && (hr == 4) && (mir == 5)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    calDateTime(jd - cacho, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian offset = %12.4f\n", jd - cacho);
#endif
    if ((y == yr) && (mr == 2) && (dr == 28) && (hr == 21) && (mir == 54)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    /*
     * 2000 / 02 / 28 23:59:59.9
     */
    y = 2000;
    m = 2;
    d = 28;
    h = 23;
    mi = 59;
    sec = 59.9;
    jd = 2451603.4999;
    jdr = julianDate(y, m, d, h, mi, sec);
    /*
     * round to 4 decimal places
     */
    jdr = floor(jdr * 10000.0) / 10000.00;
    jd = floor(jd * 10000.0) / 10000.00;
    res = calDateTime(jd, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "In:  %4d/%02d/%02d %02d:%02d:%07.4f\t",
            y, m, d, h, mi, sec);
    fprintf(stderr, "Out: Julian date = %12.4f\n", jdr);
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian date = %12.4f\n", jd);
#endif
    if ((jd == jdr) &&
        (y == yr) && (m == mr) && (d == dr) && (h == hr) && (mi == mir)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    /*
     * 2000 / 02 / 29 23:59:59.9
     */
    y = 2000;
    m = 2;
    d = 29;
    h = 23;
    mi = 59;
    sec = 59.9;
    jd = 2451604.4999;
    jdr = julianDate(y, m, d, h, mi, sec);
    /*
     * round to 4 decimal places
     */
    jdr = floor(jdr * 10000.0) / 10000.00;
    jd = floor(jd * 10000.0) / 10000.00;
    res = calDateTime(jd, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "In:  %4d/%02d/%02d %02d:%02d:%07.4f\t",
            y, m, d, h, mi, sec);
    fprintf(stderr, "Out: Julian date = %12.4f\n", jdr);
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian date = %12.4f\n", jd);
#endif
    if ((jd == jdr) &&
        (y == yr) && (m == mr) && (d == dr) && (h == hr) && (mi == mir)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    /*
     * 2100 / 02 / 29 23:59:59.9 invalid date
     */
    y = 2100;
    m = 2;
    d = 29;
    h = 12;
    mi = 0;
    sec = 0.0;
    jdr = julianDate(y, m, d, h, mi, sec);
    /*
     * round to 4 decimal places
     */
    jdr = floor(jdr * 10000.0) / 10000.00;
    jd = -68570.0;
    res = calDateTime(jd, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "res = %d  jdr = %f\n", res, jdr);
#endif
    if ((res == 0) && (jdr < 0)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    /*
     * 2100 / 02 / 28 12:00:0.0
     */
    y = 2100;
    m = 2;
    d = 28;
    h = 12;
    mi = 0;
    sec = 0.0;
    jd = 2488128.0;
    jdr = julianDate(y, m, d, h, mi, sec);
    res = calDateTime(jd, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "In:  %4d/%02d/%02d %02d:%02d:%07.4f\t",
            y, m, d, h, mi, sec);
    fprintf(stderr, "Out: Julian date = %12.4f\n", jdr);
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian date = %12.4f\n", jd);
#endif
    if ((jd == jdr) &&
        (y == yr) && (m == mr) && (d == dr) && (h == hr) && (mi == mir)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    /*
     * 2100 / 03 / 1 13:00:0.0
     */
    y = 2100;
    m = 3;
    d = 1;
    h = 13;
    mi = 0;
    sec = 0.0;
    jd = 2488129.04169;
    jdr = julianDate(y, m, d, h, mi, sec);
    /*
     * round to 4 decimal places
     */
    jdr = floor(jdr * 10000.0) / 10000.00;
    jd = floor(jd * 10000.0) / 10000.00;
    res = calDateTime(jd + 0.0001, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "In:  %4d/%02d/%02d %02d:%02d:%07.4f\t",
            y, m, d, h, mi, sec);
    fprintf(stderr, "Out: Julian date = %12.4f\n", jdr);
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian date = %12.4f\n", jd);
#endif
    if ((jd == jdr) &&
        (y == yr) && (m == mr) && (d == dr) && (h == hr) && (mi == mir)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    /*
     * 2012 / 12 / 31 12:00:0.0
     */
    y = 2012;
    m = 12;
    d = 31;
    h = 12;
    mi = 0;
    sec = 0.0;
    jd = 2456293.000000;
    jdr = julianDate(y, m, d, h, mi, sec);
    /*
     * round to 4 decimal places
     */
    jdr = floor(jdr * 10000.0) / 10000.00;
    jd = floor(jd * 10000.0) / 10000.00;
    res = calDateTime(jd, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "In:  %4d/%02d/%02d %02d:%02d:%07.4f\t",
            y, m, d, h, mi, sec);
    fprintf(stderr, "Out: Julian date = %12.4f\n", jdr);
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian date = %12.4f\n", jd);
#endif
    if ((jd == jdr) &&
        (y == yr) && (m == mr) && (d == dr) && (h == hr) && (mi == mir)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    /*
     * 2013 / 1 / 1 0:00:1.0
     */
    y = 2013;
    m = 1;
    d = 1;
    h = 0;
    mi = 0;
    sec = 1.0;
    jd = 2456293.500012;
    jdr = julianDate(y, m, d, h, mi, sec);
    /*
     * round to 4 decimal places
     */
    jdr = floor(jdr * 10000.0) / 10000.00;
    jd = floor(jd * 10000.0) / 10000.00;
    res = calDateTime(jd, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "In:  %4d/%02d/%02d %02d:%02d:%07.4f\t",
            y, m, d, h, mi, sec);
    fprintf(stderr, "Out: Julian date = %12.4f\n", jdr);
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian date = %12.4f\n", jd);
#endif
    if ((jd == jdr) &&
        (y == yr) && (m == mr) && (d == dr) && (h == hr) && (mi == mir)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    cacho = timeDayFrac(5, 5, 30.0);
    calDateTime(jd - cacho, &yr, &mr, &dr, &hr, &mir, &secr);
#ifdef DEBUG
    fprintf(stderr, "Out: %4d/%02d/%02d %02d:%02d:%07.4f\t",
            yr, mr, dr, hr, mir, secr);
    fprintf(stderr, "In:  Julian offset = %12.4f\n", jd - cacho);
#endif
    if ((yr == 2012) && (mr == 12) && (dr == 31) && (hr == 18) && (mir == 54)) {
        success++;
        fprintf(stderr, "ok\n");
    }

    fprintf(stderr, "%d successful of %d tests passed\n", success, total);
    if (success == total)
      return 1;
    else
      return 0;
}/* test-timedate.c ends here */

/*cl-osd - A simple open source osd for e-osd and g-osd
Copyright (C) 2011 Ernani Reis & Carl Ljungstr�m

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/

#ifndef HOME_H_
#define HOME_H_

#include "gpsutils.h"

#ifdef GPS_ENABLED

static uint32_t gHomeDistance = 0; // Distance to home in meters
static uint16_t gHomeBearing = 0; // Direction to home

static TGpsPos gHomePos = {}; 
static uint8_t gHomePosSet = 0;

#ifdef HOME_SET_AT_FIX
static uint8_t gHomeFixCount = 0;
#endif //HOME_SET_AT_FIX

// TODO: Some translations left
static void calcHome(int32_t currLat, int32_t currLong, int32_t homeLat, int32_t homeLong, uint32_t* const distanceResult, uint16_t* const bearingResult) {
  // calculates bearing and distance to reference point
  // vehicle in currLat, currLong
  // reference point on homeLat, homeLong

  // returns nothing
  // saves result to distance and bearing pointers

  // computes using integers, only.
  // presents errors ~ 5m, and 5� for short distances (few km)
  // should present considerable errors for longer distances

  // considers flat earth around vehicle and reference

  // Ernani Reis, Aug 2011
  
  if (currLat == homeLat && currLong == homeLong) {
	  *distanceResult = 0;
	  *bearingResult = 0;
	  return;
  }


  // cosseno de lat recicla p/ seno de home
  int8_t c;
  // Delta lat & lon
  int32_t deltaLat; 
  int32_t deltaLong;
  uint32_t distance;
  uint16_t bearing;
      
  c = myCos(absi32(homeLat + currLat) / 2000000); // c ~ cos(lat) -> long to fix the delta

  int32_t currLatSec100 = wgs84ToSec100(currLat); // Convert wgs84 to sec * 100
  int32_t homeLatSec100 = wgs84ToSec100(homeLat);
  int32_t currLongSec100 = wgs84ToSec100(currLong);
  int32_t homeLongSec100 = wgs84ToSec100(homeLong);

  deltaLong = homeLongSec100 - currLongSec100; // Calculate deltas
  deltaLat = homeLatSec100 - currLatSec100; // delta long ainda com erro (Delta still long with error?)

  deltaLong = deltaLong * c;   // corrige dla p/ lat da regi�o
  deltaLong = deltaLong / 100; // cosine is 0-100 so divide with 100

  // Make sure we don't go over uin32_t when doing sqr of deltaLong & deltaLat
  uint8_t mult = 1;
  while ((absi32(deltaLong) >= 0xFFFF) 
         || (absi32(deltaLat) >= 0xFFFF)) {
	  deltaLong /= 2;
	  deltaLat /= 2;
	  mult *= 2;
  }
  
  uint32_t a = (deltaLong * deltaLong);
  uint32_t b = (deltaLat * deltaLat);

  // Make sure that the squared deltas don't go over uin32_t when adding them together.
  // This should only activate at long distances!
  uint8_t mult2 = 1;
  while ((a >> 24) + (b >> 24) > 0xFF) {
    a /= 4;
    b /= 4;
    mult2 *= 2;
  }
  
  distance = calcSqrt(a + b);
  distance *= mult2;

  // Only calculate bearing if bearing pointer is set.
  if (bearingResult != NULL) {
    bearing = 0;
    if (distance > 0) { // Over home -> do not compute; Home forced to 0
      if (absi32(deltaLong) >= absi32(deltaLat)) { // low angle, sine is better
        c = (absi32(deltaLat) * 100) / distance; // computes sine -> dist > 0
        while((mySin(bearing) <= c) && (bearing < 90)) {
          ++bearing;
        }
      } 
	    else { // high angle, cosine is better
        c = (absi32(deltaLong) * 100) / distance; // Computes cosine
        while((mySin(bearing) <= c) && (bearing < 90)) {
          ++bearing;
        }
        bearing = 90 - bearing;
      }
      if (deltaLat == 0) {
        if (deltaLong >= 0) { // Home = East
          bearing = 90; 
        } 
	      else { // Home = West
          bearing = 270;
        }
      } 
      else if (deltaLat > 0) {
        if (deltaLong >= 0) { // NE quadrant
          bearing = 90 - bearing;
        } 
	      else { // NW quadrant
          bearing = 270 + bearing;
        }
      } 
	    else {
        if (deltaLong >= 0) { // SE quadrant
          bearing = 90 + bearing;
        } 
	      else { // SW quadrant
          bearing = 270 - bearing;
        }
      }
    }
    *bearingResult = bearing;
	}
  distance = distance * 309; // ate aqui em segundo * 100
  distance = distance / 1000; // metros, agora
  distance *= mult;
#ifdef IMPERIAL_SYSTEM  
  distance *= 3281;
	distance /= 1000;
#endif //IMPERIAL_SYSTEM
  *distanceResult = distance;
}

#endif //GPS_ENABLED

#endif /* HOME_H_ */
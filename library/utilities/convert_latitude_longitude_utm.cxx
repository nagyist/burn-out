/*ckwg +5
 * Copyright 2010-2015 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include <vnl/vnl_math.h>
#include <sstream>
#include <sstream>

#include "convert_latitude_longitude_utm.h"

#include <logger/logger.h>
VIDTK_LOGGER("convert_latitude_longitude_utm_cxx");


namespace vidtk
{

// WGS84 ellipsoid
double convert_latitude_longitude_utm::equatorial_radius_ = 6378137;
double convert_latitude_longitude_utm::polar_radius_ = 6356752.3142;

utm_coordinate
convert_latitude_longitude_utm
::convert_latitude_longitude_to_utm( double latDeg, double lonDeg )
{
  // http://www.uwgb.edu/dutchs/UsefulData/UTMFormulas.htm
  // and http://www.uwgb.edu/dutchs/UsefulData/UTMConversions1.xls

  const double deg2Rad = vnl_math::pi / 180.0;

  double latRad = latDeg * deg2Rad;
  double lonRad = lonDeg * deg2Rad;

  /*
    if (IS_DEBUG_ENABLED() )
    {
    std::stringstream oss;
    oss.precision( 10 );
    oss << "Angle radians : " << latRad << "\t" << lonRad;
    LOG_INFO( oss.str() );
    }
  */

  int    lonZone = vnl_math::floor( lonDeg / 6.0 ) + 31;
  double lonZoneMeridianDeg = 6 * lonZone - 183;
  double lonZoneMeridianRad = lonZoneMeridianDeg * vnl_math::pi / 180.0;

  double a = equatorial_radius_;
  double b = polar_radius_;

  double k0 = 0.9996;
  double e = sqrt( 1 - b*b / (a*a) );
  double e2 = e * e / ( 1 - e * e );
  double n = ( a - b ) / ( a + b );
  double n2 = n * n;
  double n3 = n2 * n;
  double n4 = n2 * n2;

  double cos2 = cos( latRad ) * cos( latRad );
  double cos3 = cos2 * cos( latRad );
  double cos4 = cos3 * cos( latRad );
  double tan2 = tan( latRad ) * tan( latRad );

  double p = lonRad - lonZoneMeridianRad;
  double p2 = p * p;
  double p3 = p2 * p;
  double p4 = p3 * p;


  double A = a*(1-n+(5*n2/4.0)*(1-n)+(81.0*n4/64.0)*(1-n));
  double B = (3.0*a*n/2.0)*(1-n-(7.0*n*n/8.0)*(1-n)+55.0*n4/64.0);
  double C = (15.0*a*n*n/16.0)*(1-n+(3.0*n*n/4.0)*(1-n));
  double D = (35.0*a*n3/48.0)*(1-n+11.0*n*n/16.0);
  double E = (315.0*a*n4/51.0)*(1-n);
  double S = A * latRad - B * sin( 2 * latRad ) + C * sin( 4 * latRad ) - D * sin( 6 * latRad ) + E * sin( 8 * latRad );

  /*
  if (IS_DEBUG_ENABLED() )
  {
  LOG_DEBUG( "Lon zone : " << lonZone );
  LOG_DEBUG( "Lon zone central meridian : " << lonZoneMeridianDeg );

  LOG_DEBUG( "Delta lon rad : " << p );

  LOG_DEBUG( "Meridional Arc S : " << S );
  LOG_DEBUG( "\t A : " << A );
  LOG_DEBUG( "\t B : " << B );
  LOG_DEBUG( "\t C : " << C );
  LOG_DEBUG( "\t D : " << D );
  LOG_DEBUG( "\t E : " << E );
  }
  */

  double nu = a / sqrt( 1 - e * e * sin( latRad ) * sin( latRad ) );

  double K1 = S * k0;
  double K2 = nu*sin( latRad )*cos( latRad )/2.0; // k0 * nu * sin( 2.0 * latRad  ) / 4.0;
  double K3 = ( k0 * nu * sin( latRad ) * cos3 / 24.0 ) * ( 5 - tan2 + 9 * e2 * cos2 + 4 * e2 * e2 * cos4 );
  double northing = K1 + K2 * p2 + K3 * p4;

  if ( northing < 0 )
    {
    northing = 10000000 + northing;
    }

  double K4 = k0 * nu * cos( latRad );
  double K5 = ( k0 * nu * cos3  / 6.0 ) * (1 - tan2 + e2 * cos2 );
  double easting = 500000 + K4 * p + K5 * p3;

  /*
  if (IS_DEBUG_ENABLED() )
  {
  LOG_DEBUG( "K1 : " << K1 );
  LOG_DEBUG( "K2 : " << K2 );
  LOG_DEBUG( "K3 : " << K3 );
  LOG_DEBUG( "K4 : " << K4 );
  LOG_DEBUG( "K5 : " << K5 );

  LOG_DEBUG( "Raw northing : " << northing );
  LOG_DEBUG( "Raw easting : " << easting );
  }
  */

  utm_coordinate::ZoneType zone = utm_zone( latDeg, lonDeg );

  utm_coordinate res( northing, easting, zone );
  return res;
}

utm_coordinate::ZoneType
convert_latitude_longitude_utm
::utm_zone( double lat, double lon )
{
  // There are some discontinuities in the lettering...
  const char lat_zones[] = { 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X' };
  const int lat_zones_length = sizeof( lat_zones );

  int lat_idx = vnl_math::floor( ( lat + 80.0 ) / 8.0 );
  int lon_idx = vnl_math::floor( ( lon + 180.0 ) / 6.0 ) + 1; // zone numbers start at 1

  if ( lat_idx >= lat_zones_length || lat_idx < 0 )
    {
      LOG_ERROR( "Error: Latitude zone outside expected range. Computed : " << lat_idx );
      lat_idx = vnl_math::max( lat_idx, 0 );
      lat_idx = vnl_math::min( lat_idx, lat_zones_length - 1);
    }

  //LOG_DEBUG( "Lat idx : " << lat_idx << " Lon idx : " << lon_idx );

  utm_coordinate::ZoneType zone;

  std::stringstream sstream;
  sstream << lon_idx << lat_zones[ lat_idx ];
  zone = sstream.str();

  return zone;
}

utm_coordinate
convert_latitude_longitude_utm
::convert_latitude_longitude_to_utm( vnl_vector_fixed< double, 2 >& lat_lon )
{
  return convert_latitude_longitude_to_utm( lat_lon.get( 0 ), lat_lon.get( 1 ) );
}

} // vidtk

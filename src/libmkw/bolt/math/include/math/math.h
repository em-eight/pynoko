#pragma once

#define PI 3.14159265358979323846

inline double DEG2RAD( const double deg )
{
	return deg * PI/180;
}
inline double RAD2DEG( const double rad )
{
	return rad * 180/PI;
}

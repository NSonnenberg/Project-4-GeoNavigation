#pragma once
#ifndef SUPPORT_H
#define SUPPORT_H
#include "provided.h"
#include <string>

inline
bool operator==(GeoCoord &a, GeoCoord &b)
{
	double dist = distanceEarthKM(a, b);
	return (dist < 0.01);
	//return (a.latitude == b.latitude && a.longitude == b.longitude);
}

inline
bool operator==(const GeoCoord &a, const GeoCoord &b)
{
	double dist = distanceEarthKM(a, b);
	return (dist < 0.01);
	//return (a.latitude == b.latitude && a.longitude == b.longitude);
}

inline
bool operator<(const GeoCoord &a, const GeoCoord &b)
{
	if (a.latitude < b.latitude)
		return true;
	else if (a.latitude == b.latitude)
		return (a.longitude < b.longitude);
	return false;
}

inline
bool operator>(const GeoCoord &a, const GeoCoord &b)
{
	if (a.latitude > b.latitude)
		return true;
	else if (a.latitude == b.latitude)
		return (a.longitude > b.longitude);
	return false;
}

inline
std::string directionFinder(const GeoSegment& gs) 
{
	double angle = angleOfLine(gs);
	if ((angle >= 0 && angle <= 22.5) || (angle > 337.5 && angle < 360))
		return "east";
	else if (22.5 < angle && angle <= 67.5)
		return "northeast";
	else if (67.5 < angle && angle <= 112.5)
		return "north";
	else if (112.5 < angle && angle <= 157.5)
		return "northwest";
	else if (157.5 < angle && angle <= 202.5)
		return "west";
	else if (202.5 < angle && angle <= 247.5)
		return "southwest";
	else if (247.5 < angle && angle <= 292.5)
		return "south";
	else if (292.5 < angle && angle <= 337.5)
		return "southeast";
	else
		return "";

}

#endif

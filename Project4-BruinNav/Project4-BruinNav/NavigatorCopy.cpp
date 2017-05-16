#include "provided.h"
#include "MyMap.h"
#include "support.h"
#include <iostream>
#include <queue>
#include <string>
#include <list>
#include <vector>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	struct NavCoord
	{
		bool operator<(const NavCoord &a) const
		{
			return (a.totalDistance < totalDistance);
		}
		bool operator==(const NavCoord &a) const
		{
			return (a.coord == coord);
		}
		bool operator>(const NavCoord &a) const
		{
			return (a.totalDistance > totalDistance);
		}
		string streetName;
		double totalDistance;
		GeoCoord coord;
	};
	
	string directionFinder(double angle) const;
	MapLoader m_mapLoad;
	AttractionMapper m_atrMap;
	SegmentMapper m_segMap;
};


NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

string NavigatorImpl::directionFinder(double angle) const
{
	if (0 <= angle || angle <= 22.5 || 337.5 < angle || angle < 360)
		return "east";
	else if (22.5 < angle || angle <= 67.5)
		return "northeast";
	else if (67.5 < angle || angle <= 112.5)
		return "north";
	else if (112.5 < angle || angle <= 157.5)
		return "northwest";
	else if (157.5 < angle || angle <= 202.5)
		return "west";
	else if (202.5 < angle || angle <= 247.5)
		return "southwest";
	else if (247.5 < angle || angle <= 292.5)
		return "south";
	else if (292.5 < angle || angle <= 337.5)
		return "southeast";
	else
		return "";

}

bool NavigatorImpl::loadMapData(string mapFile)
{
	if (!m_mapLoad.load(mapFile))
		return false;
	m_atrMap.init(m_mapLoad);
	m_segMap.init(m_mapLoad);
	return true;  // This compiles, but may not be correct
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	// get coordinate of starting attraction
	GeoCoord s;
	if (!m_atrMap.getGeoCoord(start, s))
		return NAV_BAD_SOURCE;
	
	// get coordinate of ending attraction
	GeoCoord e;
	if (!m_atrMap.getGeoCoord(end, e))
		return NAV_BAD_DESTINATION;

	MyMap<NavCoord, NavCoord> paths;
	MyMap<GeoCoord, GeoCoord> visited;
	priority_queue<NavCoord, vector<NavCoord>, less<NavCoord>> q;

	// current GeoCoord
	NavCoord curr;
	vector<StreetSegment> firstSegs = m_segMap.getSegments(s);
	for (int k = 0; k < firstSegs.size(); k++)
	{
		StreetSegment segment = firstSegs[k];
		for (int j = 0; j < segment.attractions.size(); j++)
		{
			if (segment.attractions[j].geocoordinates == s)
			{
				NavCoord foundAttr;
				foundAttr.coord = segment.attractions[j].geocoordinates;
				foundAttr.streetName = segment.streetName;
				foundAttr.totalDistance = 1000; // infinity
				q.push(foundAttr);
				break;
			}
		}
	}
	curr.coord = s;
	curr.totalDistance = 0;
//	q.push(curr);

	bool found = false;

	// TODO: fix this condition so it you know works
	while (!q.empty())
	{
		vector<StreetSegment> currSegs = m_segMap.getSegments(curr.coord);
		for (int i = 0; i < currSegs.size(); i++)
		{
			StreetSegment segment = currSegs[i];

			GeoCoord* chk = visited.find(segment.segment.start);
			if (!(curr.coord == segment.segment.start) && chk == nullptr)
			{
				NavCoord start;
				start.coord = segment.segment.start;
				start.streetName = segment.streetName;
				start.totalDistance = distanceEarthMiles(start.coord, e);
				//start.totalDistance = distanceEarthMiles(curr.coord, start.coord) + distanceEarthMiles(start.coord, e);
				q.push(start);

			}

			GeoCoord* chk2 = visited.find(segment.segment.end);
			if (!(curr.coord == segment.segment.end) && chk2 == nullptr)
			{
				NavCoord end;
				end.coord = segment.segment.end;
				end.streetName = segment.streetName;
				end.totalDistance = distanceEarthMiles(end.coord, e);
				//end.totalDistance = distanceEarthMiles(curr.coord, end.coord) + distanceEarthMiles(end.coord, e);
				q.push(end);
			}

			for (int j = 0; j < segment.attractions.size(); j++)
			{
				if (segment.attractions[j].geocoordinates == e)
				{
					if (segment.streetName != curr.streetName)
						break;
					NavCoord foundAttr;
					foundAttr.coord = segment.attractions[j].geocoordinates;
					foundAttr.streetName = segment.streetName;
					foundAttr.totalDistance = distanceEarthMiles(foundAttr.coord, e);
					q.push(foundAttr);
					found = true;
					break;
				}
			}
		}

		NavCoord next = q.top();
		NavCoord* nextPrev = paths.find(next);

		cout << endl << "Checking Route " << next.streetName << " at coord (" << next.coord.latitude << ", " << next.coord.longitude << ")";
		visited.associate(curr.coord, curr.coord);
		if (nextPrev == nullptr)
			paths.associate(next, curr);
		q.pop();
		
		curr = next;
		if (found)
			break;
	}

	if (found)
	{
		list<NavSegment> coords;
		NavCoord j = curr;
		while (!(j.coord == s))
		{
			NavCoord *prev = paths.find(j);
			GeoSegment gs(prev->coord, j.coord);
			NavCoord *prevPrev = paths.find(*prev);
			double angle = angleOfLine(gs);
			string dir = directionFinder(angle);
			double dist = distanceEarthMiles(prev->coord, j.coord);
			NavSegment currSeg(dir, j.streetName, dist, gs);
			coords.push_front(currSeg);
			if (!(j.coord == e) && prevPrev != nullptr && j.streetName != prev->streetName)
			{
				GeoSegment nextSeg(prevPrev->coord, prev->coord);
				double turnAngle = angleBetween2Lines(nextSeg, gs);
				string direct;
				if (turnAngle >= 180)
					direct = "right";
				else
					direct = "left";
				NavSegment turn(direct, j.streetName);
				coords.push_front(turn);
			}
			j = *prev;
		}

		list<NavSegment>::iterator it;
		it = coords.begin();
		while (it != coords.end())
		{
			directions.push_back(*it);
			it++;
		}
		return NAV_SUCCESS;
		// create vector of nav segments
	}

	return NAV_NO_ROUTE;  // This compiles, but may not be correct
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}

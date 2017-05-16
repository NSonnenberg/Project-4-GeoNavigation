#include "provided.h"
#include "support.h"
#include <string>
#include "MyMap.h"
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> m_map;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	for (int i = 0; i < ml.getNumSegments(); i++)
	{
		StreetSegment currSeg;
		if (!ml.getSegment(i, currSeg))
			return; // why does this even return bool?
		if (currSeg.attractions.empty())
			continue;
		else
		{
			for (int j = 0; j < currSeg.attractions.size(); j++)
			{
				string atrName = currSeg.attractions[j].name;
				for (int k = 0; k < atrName.size(); k++)
				{
					atrName[k] = tolower(atrName[k]);
				}
				m_map.associate(atrName, currSeg.attractions[j].geocoordinates);
			}
		}
	}
}

//TODO: Figure out the case insensitivity
bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	for (int i = 0; i < attraction.size(); i++)
		attraction[i] = tolower(attraction[i]);
	const GeoCoord* coord = m_map.find(attraction);
	if (coord == nullptr)
		return false;
	gc = *coord;
	return true;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}

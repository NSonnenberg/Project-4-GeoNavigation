#include "provided.h"
#include "support.h"
#include "MyMap.h"
#include <vector>
#include <iostream>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
	MyMap<GeoCoord, vector<StreetSegment>> m_map;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
	for (int i = 0; i < ml.getNumSegments(); i++)
	{
		StreetSegment currSeg;
		if (!ml.getSegment(i, currSeg))
			return; // why does this even return bool?
		vector<StreetSegment> *start = m_map.find(currSeg.segment.start);
		if (start != nullptr)
		{
			start->push_back(currSeg);
		}

		else
		{
			vector<StreetSegment> seg;
			seg.push_back(currSeg);
			m_map.associate(currSeg.segment.start, seg);
		}

		vector<StreetSegment> *end = m_map.find(currSeg.segment.end);
		if (end != nullptr)
		{
			end->push_back(currSeg);
		}

		else
		{
			vector<StreetSegment> seg;
			seg.push_back(currSeg);
			m_map.associate(currSeg.segment.end, seg);
		}

		for (int j = 0; j < currSeg.attractions.size(); j++)
		{
			vector<StreetSegment> *atr = m_map.find(currSeg.attractions[j].geocoordinates);
			if (atr != nullptr)
			{
				atr->push_back(currSeg);
			}
			else
			{
				vector<StreetSegment> seg;
				seg.push_back(currSeg);
				m_map.associate(currSeg.attractions[j].geocoordinates, seg);
			}
		}
	}
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	const vector<StreetSegment> *segments = m_map.find(gc);
	return *segments;  // This compiles, but may not be correct
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}

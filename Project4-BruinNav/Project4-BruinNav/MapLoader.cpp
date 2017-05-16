#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	// TODO: get rid of print
	/*
	void print()
	{
		for (int i = 0; i < m_streetSegs.size(); i++)
		{
			cout << m_streetSegs[i]->streetName;
			cout << " | (" << m_streetSegs[i]->segment.start.latitude << ", " << m_streetSegs[i]->segment.start.longitude << ") ->";
			cout << " (" << m_streetSegs[i]->segment.end.latitude << ", " << m_streetSegs[i]->segment.end.longitude << ")" << endl;
		}
	}
	*/
	vector<StreetSegment*> m_streetSegs;
	size_t m_numSegs;
};

MapLoaderImpl::MapLoaderImpl()
{
	m_numSegs = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
	vector<StreetSegment*>::iterator it;
	it = m_streetSegs.begin();
	while (it != m_streetSegs.end())
	{
		StreetSegment* p = (*it);
		delete p;
		it = m_streetSegs.erase(it);
	}
}

bool MapLoaderImpl::load(string mapFile)
{
	ifstream dataFile(mapFile);
	if (!dataFile)
	{
		cerr << "Failed to Open File" << endl;
		return false;
	}

	int lineCount = 1;
	int atrCount = -1;
	int index = 0;
	int count = 0;
	string currLine;
	while (getline(dataFile, currLine))
	{
		if (1 == lineCount)
		{
			StreetSegment* sg = new StreetSegment;
			sg->streetName = currLine;
			m_streetSegs.push_back(sg);
			lineCount++;
			m_numSegs++;
			if (m_streetSegs.size() == 1)
			{
				continue;
			}
			index++;
			count++;
			continue;
		}

		if (2 == lineCount)
		{
			string::size_type pos = currLine.find(',');
			string lat1 = currLine.substr(0, pos);
			string rest;
			if (isspace(currLine[pos + 1]) || currLine[pos + 1] == ',')
				rest = currLine.substr(pos + 2, currLine.size() - 1);
			else
				rest = currLine.substr(pos + 1, currLine.size() - 1);
			pos = rest.find(' ');
			string long1 = rest.substr(0, pos);
			GeoCoord point1(lat1, long1);

			rest = rest.substr(pos + 1, rest.size() - 1);
			pos = rest.find(',');
			string lat2 = rest.substr(0, pos);
			string long2 = rest.substr(pos + 1, rest.size() - 1);
			GeoCoord point2(lat2, long2);
			
			GeoSegment gs(point1, point2);
			
			m_streetSegs[index]->segment = gs;
			lineCount++;
			count++;
			continue;
		}

		if (3 == lineCount)
		{
			atrCount = stoi(currLine);

			if (0 == atrCount)
			{
				lineCount = 1;
			}

			else
			{
				lineCount++;
				continue;
			}
			
			count++;
			continue;
		}

		if (4 == lineCount)
		{
			string::size_type pos = currLine.find('|');
			string atrName = currLine.substr(0, pos);
			string rest = currLine.substr(pos + 1, currLine.size() - 1);
			
			pos = rest.find(',');
			string atrLat = rest.substr(0, pos);
			string atrLong = rest.substr(pos + 1, rest.size() - 1);

			GeoCoord gc(atrLat, atrLong);
			Attraction atr;
			atr.name = atrName;
			atr.geocoordinates = gc;
			m_streetSegs[index]->attractions.push_back(atr);
			atrCount--;
			if (atrCount <= 0)
				lineCount = 1;
			count++;
			continue;
		}

	}

	//print();
	return true;  // This compiles, but may not be correct
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_numSegs; // This compiles, but may not be correct
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum < m_streetSegs.size() && segNum >= 0)
	{
		seg = *m_streetSegs[segNum];
		return true;
	}
	return false;  // This compiles, but may not be correct
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}

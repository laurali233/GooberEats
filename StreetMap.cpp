#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}


class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
    vector<GeoCoord*> getAllCoord() const;
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> streetM;
    vector<GeoCoord*> allcoor;
    vector<StreetSegment*> allseg;
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
    typename vector<GeoCoord*>::iterator it1 = allcoor.begin();//traverse the loop and delete each pointer
    while(it1!=allcoor.end()){
        delete *it1;
        it1++;
    }
    
    typename vector<StreetSegment*>::iterator it2 = allseg.begin();//traverse the loop and delete each pointer
    while(it2!=allseg.end()){
        delete *it2;
        it2++;
    }
}

bool StreetMapImpl::load(string mapFile)
{
     fstream MapData;
     MapData.open(mapFile); // open the file
     bool flag = true;
     while(flag){
         string name;
         if(!getline(MapData,name))
             break;
         string prenumber; // the number of street segments
         getline(MapData, prenumber);
         int number = stoi(prenumber);
         for(int i=0; i<number;i++){ // traverse (number) times
             string lat;
             MapData>>lat;//get the latitude
             string lon;
             MapData>>lon;//get the longtitude
             GeoCoord* coord1= new GeoCoord(lat,lon);
             allcoor.push_back(coord1);//push back the coordinate
             string lat2;
             MapData>>lat2; //get second latitude
             string lon2;
             MapData>>lon2;//get second longtitude
             GeoCoord* coord2 = new GeoCoord(lat2,lon2);
             allcoor.push_back(coord2);
             StreetSegment* seg1 = new StreetSegment(*coord1, *coord2, name);
             allseg.push_back(seg1);
             StreetSegment* reverseseg = new StreetSegment(*coord2,*coord1,name);
             allseg.push_back(reverseseg);
             if(!streetM.find(*coord1)){ // if we can find the coordinate
                 vector<StreetSegment> seg;//add the segment to the vector
                 seg.push_back(*seg1);
                 streetM.associate(*coord1, seg);
             }
             else{
                 vector<StreetSegment>* ptrToSegVector = streetM.find(*coord1);
                 (*ptrToSegVector).push_back(*seg1);//add the reverse segment to another coordinate
             }
             if(!streetM.find(*coord2)){//if it is not found
                 vector<StreetSegment> sseg; //push them into the street segment
                 sseg.push_back(*reverseseg);
                 streetM.associate(*coord2, sseg);
             }
             else{
                 vector<StreetSegment>* ptrToSegVector2 = streetM.find(*coord2);
                 (*ptrToSegVector2).push_back(*reverseseg);
             }
             string whatever;//to read the next line
             getline(MapData, whatever);
         }
     }
    return true;  // Delete this line and implement this function correctly
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment>* ptrToSegVector = streetM.find(gc); //O(1)
    if(ptrToSegVector==nullptr)
        return false;
    while(!segs.empty()){ //O(1)
        segs.pop_back();
    }
    segs.clear();
    segs = *ptrToSegVector;
    return true;  // Delete this line and implement this function correctly
}

vector<GeoCoord*> StreetMapImpl::getAllCoord() const{
    return allcoor;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}

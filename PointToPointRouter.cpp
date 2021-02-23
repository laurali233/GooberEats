#include "provided.h"
#include <list>
#include <set>
#include <map>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_ptrToSMap;
    bool isSameCoord (const GeoCoord& start, const GeoCoord& end) const;
    bool isCoordExist (const GeoCoord& coord) const;
    double NumF(const GeoCoord& current, const GeoCoord& start , const GeoCoord& end) const;
    struct Location{
        Location* m_parent;
        GeoCoord m_node;
        double m_fValue;
        double m_gValue;
        double m_hValue;
        Location(GeoCoord current, Location* parent, double gValue, double hValue):m_node(current),m_gValue(gValue),m_hValue(hValue),m_parent(parent){
            m_fValue = m_hValue+m_gValue;
        }
        Location(){}
    };
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_ptrToSMap = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    route.clear();
    
    if(isSameCoord(start, end)){//if the start point and end point is the same
        return DELIVERY_SUCCESS; //return find the route
    }
    if(!(isCoordExist(start)&&isCoordExist(end))){ // if it does not exist in the txt
        return BAD_COORD; //return bad coordinate
    }
    
    bool foundThePath = false;//set it to false
    map<GeoCoord,Location*> openlist; //a openlist to record the useful point
    map<GeoCoord,Location*> closedlist;// a closedlist to record the other
    openlist[start] = new Location(start,nullptr,0,distanceEarthMiles(start, end));//memory leak 一定要注意
    Location* current;
    while(openlist.size()!=0 && !foundThePath){ // as long as the openlist is not empty
        current = openlist.begin()->second; // get the location
        for (auto it = openlist.begin(); it!=openlist.end(); it++) {
            if(it->second->m_fValue<current->m_fValue){//compare the f value
            current = it->second;//set it to the
            }
        }
        if(current->m_node == end){//if current coordinate is the end coordinate
            foundThePath = true; //set the bool to true
            break;//exit the loo;
        }
        openlist.erase(openlist.find(current->m_node)); //erase the location in openlist with the coordinate
        vector<StreetSegment> segs;
        m_ptrToSMap->getSegmentsThatStartWith(current->m_node, segs);//get the segments connected with current coordinate
        for(int i=0; i<segs.size();i++){//for each connected segments
            GeoCoord successor = segs[i].end;
            double cost = current->m_gValue+distanceEarthMiles(current->m_node, successor);
            if(openlist.find(successor)!=openlist.end()){ // if the successor is in the openlist
                Location* temp = openlist[successor];
                if(temp->m_gValue<=cost)//compare the g_value
                    continue;
            }
            else if(closedlist.find(successor)!=closedlist.end()){//if the successor is in the closedlist
                Location* temp2 = closedlist[successor];
                if(temp2->m_gValue <= cost){//compare the g_value
                    continue;
                }
                openlist[successor] = temp2; //associate the successor with the location
                closedlist.erase(closedlist.find(successor));
            }
            else{
                double h_value = distanceEarthMiles(successor, end);//calculate the h_value
                Location* temp = new Location(successor,nullptr,0,h_value);
                openlist[successor] = temp;//associate the successor with temp
            }
            openlist[successor]->m_gValue = cost; // change the g_value
            openlist[successor]->m_parent = current; // change the parent to the current node
            openlist[successor]->m_fValue = openlist[successor]->m_gValue+openlist[successor]->m_hValue;//f_value=h_value+g_value
        }
        closedlist[current->m_node] = current;//move the current into closedlist
    }

    if(foundThePath==true){//if we find the path
        totalDistanceTravelled = current->m_gValue;//g_value accumulated and is equal to the distance
        while(current->m_parent != nullptr){//put each segment into the vector
            vector<StreetSegment> s;
            if(m_ptrToSMap->getSegmentsThatStartWith(current->m_parent->m_node, s)){
                for(int i = 0; i < s.size(); i++){
                    if(s[i].end==current->m_node){
                        route.push_front(s[i]);
                        break;
                    }
                }
            }
            current = current->m_parent;
        }
        for(auto it = openlist.begin();it!=openlist.end();it++){
            delete it->second;//free the memory by openlist
        }
        for(auto it = closedlist.begin();it!=closedlist.end();it++){
            delete it->second;//free the memory by closedlist
        }
        return DELIVERY_SUCCESS;
    }
    
    for(auto it = openlist.begin();it!=openlist.end();it++){
        delete it->second;
    }
    for(auto it = closedlist.begin();it!=closedlist.end();it++){
        delete it->second;
    }
   
    return NO_ROUTE;
}

bool PointToPointRouterImpl::isSameCoord (const GeoCoord& start, const GeoCoord& end) const{
    if((start.latitude == end.latitude)&&(start.longitude==end.longitude))
        return true;
    else
        return false;
}

bool PointToPointRouterImpl::isCoordExist(const GeoCoord& coord) const{
    vector<StreetSegment> temp;
    if(m_ptrToSMap->getSegmentsThatStartWith(coord, temp))
        return true;
    else
        return false;
}

double PointToPointRouterImpl::NumF(const GeoCoord& current, const GeoCoord& start , const GeoCoord& end) const{
    return (distanceEarthMiles(current, start)+distanceEarthMiles(current, end));
}




//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}




#include "provided.h"
#include <vector>
#include<time.h>
#include<cmath>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
    double getTheTotalDistance(vector<DeliveryRequest> m_order,GeoCoord start) const;
private:
    const StreetMap* m_sm;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_sm = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    srand((unsigned)time(NULL));
    int numOfLoc = deliveries.size();
    if(numOfLoc==1){
        return;
    }
    oldCrowDistance = getTheTotalDistance(deliveries,depot);
    newCrowDistance = oldCrowDistance;
    vector<DeliveryRequest> temp;
    vector<DeliveryRequest> current = deliveries;
    
    
    double e=1e-8,at=0.99,T=10000;//set the smallest temprature, the decrease rate and the initial temprature
    int L = numOfLoc*numOfLoc;
    while(L--){//traverse for L = coordinates^2 times
        current = deliveries;
        int c1 = rand()%numOfLoc, c2 = rand()%numOfLoc; //give a random location with in the range
        if(c1==c2){//if they are equal to each other
            L++;//make L increase by 1 so that we can traverse it one more time
            continue;
        }
        swap(current[c1], current[c2]);//swap the location
        double difference = getTheTotalDistance(current, depot)-newCrowDistance;
        double sj = rand()%RAND_MAX;
        if(difference<0){//if the new order result in smaller distance
            deliveries = current; //make it the current result
            newCrowDistance+=difference;
        }else if(exp(-(difference/T))>sj){//little chance it can enter and
            deliveries=current;
            newCrowDistance+=difference;
        }
        T*=at;//every time T becomes 0.99T
        if(T<e) break;//if T reach the smallest temprature, it breaks
    }
    
}

double DeliveryOptimizerImpl::getTheTotalDistance(vector<DeliveryRequest> m_order,GeoCoord start) const{
    double sum = 0.0;
    auto it = m_order.begin();
    auto it2 = m_order.begin()+1;
    sum+=distanceEarthMiles(start, it->location)+distanceEarthMiles(start, (m_order.end()-1)->location);//initial distance is from initial to start
    //plus the start to end
    while(it2!=m_order.end()){
        sum+=distanceEarthMiles(it2->location, it->location);//plus each segment's distance to the sum
        it++;
        it2++;
    }
    return sum;//return the total distance
}


//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}

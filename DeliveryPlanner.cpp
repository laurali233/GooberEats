#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_sm;
    DeliveryOptimizer* m_deliveryOpt;
    PointToPointRouter* m_pTopRouter;
    string getDir(double angel) const;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_sm = sm;
    m_deliveryOpt = new DeliveryOptimizer(sm);
    m_pTopRouter = new PointToPointRouter(sm);
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
    delete m_deliveryOpt;
    delete m_pTopRouter;
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    double temp;
    double temp2;
    vector<DeliveryRequest> newDelivery = deliveries;
    m_deliveryOpt->optimizeDeliveryOrder(depot, newDelivery, temp, temp2);
    DeliveryRequest a("dummy",depot);
    newDelivery.push_back(a);
    auto it = newDelivery.begin();
    for(int i =0; it!=newDelivery.end();i++,it++){
        GeoCoord pre;
        GeoCoord next;
        if(i==0){
            next = it->location;
            pre = depot;
        }
        else{
            next = it->location;
            it--;
            pre = it->location;
            it++;
        }
    
        double eachTravelledDistance;
        list<StreetSegment> routes;
        DeliveryResult result = m_pTopRouter->generatePointToPointRoute(pre, next, routes, eachTravelledDistance);
        if(result==NO_ROUTE)
            return NO_ROUTE;//
        if(result ==BAD_COORD)
            return BAD_COORD;//the coordinate does not exist
        totalDistanceTravelled+=eachTravelledDistance;
        // increase total distance when go through each segment
        auto eachSeg1 = routes.begin();
        eachSeg1--;
        auto eachSeg2 = routes.begin();
        DeliveryCommand m_command;
        bool flag = false;//avoid the situation where just finish a delivery
        for(;eachSeg2!=routes.end();eachSeg2++,eachSeg1++){
            double angleOnMap = angleOfLine(*eachSeg2);
            double angleBetween2;
            double distance;
            string name = eachSeg2->name;
            if(eachSeg2==routes.begin()){// for the first segment
                angleBetween2 = 0;//we always need a proceed command
                distance = distanceEarthMiles(eachSeg2->start, eachSeg2->end);
            }
            else{//when the segment is not the start
                angleBetween2 = angleBetween2Lines(*eachSeg1, *eachSeg2);
                distance = distanceEarthMiles(eachSeg1->end, eachSeg2->end);
            }
            if((angleBetween2<1 | angleBetween2>359)|((!commands.empty())&&(name == commands.back().streetName()))){
                if((!commands.empty())&&flag){
                    flag =true;//it moves so set the flag to true
                    m_command.increaseDistance(distance);//increase the distance
                    commands.pop_back();//pop the last element
                    commands.push_back(m_command);//replace with the one with the same name and longer distance
                }
                else{m_command.initAsProceedCommand(getDir(angleOnMap), name, distance);
                    commands.push_back(m_command);
                    flag=true;
                }
                eachSeg2++;//get the nex segment's name
                eachSeg1++;
                string name2 = eachSeg2->name;
                if(commands.back().streetName()!=name2){//if is not the same
                    eachSeg1--;//decrase the pointer to reenter the loop
                    eachSeg2--;
                }
                else{
                    m_command.increaseDistance(distanceEarthMiles(eachSeg1->end, eachSeg2->end));//increase the distance
                    commands.pop_back();//pop the last element
                    commands.push_back(m_command);//add the new one with same name and longer distance
                    flag = true;//it moves so set flag to true
                }
            }
            else if(angleBetween2>=1 && angleBetween2<180){
                m_command.initAsTurnCommand("left", name);
                commands.push_back(m_command);//push the left command
                m_command.initAsProceedCommand(getDir(angleOnMap), name, distance);
                commands.push_back(m_command);
                flag=true;
            }
            else if(angleBetween2>=180 && angleBetween2<=359){
                m_command.initAsTurnCommand("right", name);
                commands.push_back(m_command);//push the right command
                m_command.initAsProceedCommand(getDir(angleOnMap), name, distance);
                commands.push_back(m_command);
                flag=true;
            }
        }
        if(it!=(newDelivery.end()-1)){//the last one is not a delivery place but the depot
            m_command.initAsDeliverCommand(it->item);
            commands.push_back(m_command);}
    }
    return DELIVERY_SUCCESS;
}

string DeliveryPlannerImpl::getDir(double angle) const{//check the direction for each angle represents
    if(angle>=0 && angle< 22.5) return "east";
    else if(angle>=22.5&&angle<67.5) return "northeast";
    else if(angle>=67.5&&angle<112.5) return "north";
    else if(angle>=112.5&&angle<157.5) return "northwest";
    else if(angle>=157.5&&angle<202.5) return "west";
    else if(angle>=202.5&&angle<247.5) return "southwest";
    else if(angle>=247.5&&angle<292.5) return "south";
    else if(angle>= 292.5 && angle<337.5) return "southeast";
    else if(angle>=337.5) return "east";
    
    return "false"; //delete
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}

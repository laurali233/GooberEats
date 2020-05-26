//
//  main.cpp
//  Project4
//
//  Created by Martin on 3/4/20.
//  Copyright © 2020 Martin. All rights reserved.
//

/*#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "ExpandableHashMap.h"
#include <functional>
using namespace std;


int main(int argc, const char * argv[]) {
    StreetMap m;
    if(m.load("/Users/lauraaa/Desktop/mapdata.txt"))
        cerr << "Load success..." << endl;

//        vector<StreetSegment> a;
//    GeoCoord coord = GeoCoord("34.0923504", "-118.4332730");
//    GeoCoord p("34.0547778", "-118.4802585");
//    GeoCoord q("34.0664194", "-118.3811961");
//    s.getSegmentsThatStartWith(q, a);
//    for (auto it = a.begin(); it != a.end(); it++){
//        cout << (*it).start.latitude << " " << (*it).end.longitude << endl;
//    }

    
    GeoCoord p("34.0625329","-118.4470263");//1
    GeoCoord q("34.0636671", "-118.4461842");
    
    GeoCoord p2("34.0632405","-118.4470467");//2
    GeoCoord p3("34.0632405","-118.4470467");//3
    GeoCoord p4("34.0636533","-118.4470480");//4
    
    StreetSegment sg1(p,p2,",");
    StreetSegment sg2(p3,p4,"m");
    
    cout<<angleBetween2Lines(sg2, sg1);
    
    list<StreetSegment> route;
    double distanceTravelled = 0.0;
    vector<StreetSegment> s;
    bool b = m.getSegmentsThatStartWith(p, s);
    DeliveryRequest req("Sardines", q);
    vector<DeliveryCommand> commands;
    vector<DeliveryRequest> deliveries;
    deliveries.push_back(req);
    DeliveryPlanner planner(&m);
    double dis;
    planner.generateDeliveryPlan(p,deliveries,commands,dis);
    
    for(int i=0;i<commands.size();i++){
        cout << commands[i].description()<< endl;
    }
    
    
}*/

#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v);
bool parseDelivery(string line, string& lat, string& lon, string& item);

int main(int argc, char *argv[])
{
    clock_t start,end;
    start = clock();
    
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " mapdata.txt deliveries.txt" << endl;
        return 1;
    }

    StreetMap sm;
        
    if (!sm.load(argv[1]))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }

    GeoCoord depot;
    vector<DeliveryRequest> deliveries;
    if (!loadDeliveryRequests(argv[2], depot, deliveries))
    {
        cout << "Unable to load delivery request file " << argv[2] << endl;
        return 1;
    }

    cout << "Generating route...\n\n";

    DeliveryPlanner dp(&sm);
    vector<DeliveryCommand> dcs;
    double totalMiles;
    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
    if (result == BAD_COORD)
    {
        cout << "One or more depot or delivery coordinates are invalid." << endl;
        return 1;
    }
    if (result == NO_ROUTE)
    {
        cout << "No route can be found to deliver all items." << endl;
        return 1;
    }
    cout << "Starting at the depot...\n";
    for (const auto& dc : dcs)
        cout << dc.description() << endl;
    cout << "You are back at the depot and your deliveries are done!\n";
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << totalMiles << " miles travelled for all deliveries." << endl;
    
    end = clock();
    cout<< "Time used : " << (double)(end-start)/CLOCKS_PER_SEC << endl;
}

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v)
{
    ifstream inf(deliveriesFile);
    if (!inf)
        return false;
    string lat;
    string lon;
    inf >> lat >> lon;
    inf.ignore(10000, '\n');
    depot = GeoCoord(lat, lon);
    string line;
    while (getline(inf, line))
    {
        string item;
        if (parseDelivery(line, lat, lon, item))
            v.push_back(DeliveryRequest(item, GeoCoord(lat, lon)));
    }
    return true;
}

bool parseDelivery(string line, string& lat, string& lon, string& item)
{
    const size_t colon = line.find(':');
    if (colon == string::npos)
    {
        cout << "Missing colon in deliveries file line: " << line << endl;
        return false;
    }
    istringstream iss(line.substr(0, colon));
    if (!(iss >> lat >> lon))
    {
        cout << "Bad format in deliveries file line: " << line << endl;
        return false;
    }
    item = line.substr(colon + 1);
    if (item.empty())
    {
        cout << "Missing item in deliveries file line: " << line << endl;
        return false;
    }
    return true;
}


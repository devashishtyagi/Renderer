//
//  def.cpp
//  MinorProject
//  use clang++ -std=c++11 -stdlib=libc++ def.cpp to compile on terminal
//  Created by Keshav Choudhary on 20/02/13.
//  Copyright (c) 2013 Keshav Choudhary. All rights reserved.
//

#include "def.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

unsigned long long universalTid;

double InitializeData::convertDouble(string str) {
    double ans;
    stringstream sstr(str); // create a stringstream
    sstr>>ans; // push the stream into the num
    return ans;
}


void InitializeData::buildMap(const char *imageFile, const char *weatherMapFile) {
    ifstream imfile(imageFile), wmfile(weatherMapFile);
    int imx, imy, wmx, wmy, z;
    imfile>>imx>>imy>>z;
    wmfile>>wmx>>wmy;
    assert(imx == wmx && imy == wmy);

    imageStore.resize(imx);
    for(int i = 0; i < imx; i++) {
        imageStore[i].resize(imy);
        for(int j = 0; j < imy; j++) {
            imageStore[i][j].resize(z);
        }
    }

    weatherDegree.resize(wmx);
    for(int i = 0; i < wmx; i++) {
        weatherDegree[i].resize(wmy);
    }

    for(int k = 0; k < z; k++) {
        for(int i = 0; i < imx; i++) {
            for(int j = 0; j < imy; j++)
                imfile>>imageStore[i][j][k];
        }
    }

    for(int i = 0; i < wmx; i++) {
        for(int j = 0; j < wmy; j++)
            wmfile>>weatherDegree[i][j];
    }

    for(int i = 0; i < imx; i++) {
        for(int j = 0; j < imy; j++)
            colormap[weatherDegree[i][j]] = make_pair(imageStore[i][j][0], make_pair(imageStore[i][j][1], imageStore[i][j][2]));
    }

    imfile.close();
    wmfile.close();
}

//Populate the boundary Voxel List

void InitializeData::populateBoundaryVoxels (const char* fileName) {
	ifstream myfile (fileName);
    string data;
    if (myfile.is_open()) {
        int index = 0;
		int voxelID;
		double weathingD;
        while (myfile.good()) {
            
			getline (myfile, data);
            if (index == 0) {
				stringstream ss(data);
				ss>>voxelID>>weathingD;
            }
			else if (index == 9) {
				
				stringstream ss(data);
                string a[6];
                int b[6],c[6];
                ss>>a[0]>>b[0]>>c[0]>>a[1]>>b[1]>>c[1]>>a[2]>>b[2]>>c[2]>>a[3]>>b[3]>>c[3]>>a[4]>>b[4]>>c[4]>>a[5]>>b[5]>>c[5];
				
                //front face -- 0
                bool visible = false;
                if(b[0] == -1)
                    visible = true;
								
                // right face -- 1
                if(b[1] == -1)
                    visible = true;
				
                //back face -- 2
                if(b[2] == -1)
                    visible = true;
				
                //left face -- 3
                if(b[3] == -1)
                    visible = true;
				
                // top face -- 4
                if(b[4] == -1)
                    visible = true;
				
                //bottom face -- 5
                if(b[5] == -1)
                    visible = true;
				
				if(visible)
                    boundaryVoxelList.push_back(voxelID);
			}
			
            index = (index+1)%10;
        }
		
    }

}

// only those points which are a part of the visible voxels

void InitializeData::readPoints(const char* fileName) {
    ifstream myfile (fileName);
    vector<float> points(3);
    string data;
	int voxelID;
	double weathingD;

    if (myfile.is_open()) {
        int index = 0;
        bool insert = true;
        while (myfile.good()) {
            getline (myfile, data);
			if(index == 0) {
                insert = true;
				stringstream ss(data);
				ss>>voxelID>>weathingD;
                if (find(boundaryVoxelList.begin(),boundaryVoxelList.end(), voxelID) == boundaryVoxelList.end())
                    insert = false;
			}
				
            if (index >= 1 && index <= 8 && insert)
            {
                unordered_map<string,vertex>::const_iterator got = map.find (data);
                if ( got == map.end() ) // new vertex
                {
                    stringstream ss(data);
                    ss>>points[0]>>points[1]>>points[2];
                    vertex m(points[0],points[1],points[2]);
                    map[data] = m;
                }
            }

            index = (index+1)%10;
        }

    }
}

void InitializeData::split(string data, vector<string> points) {
    stringstream ss(data);
    ss>>points[0]>>points[1]>>points[2];
}

void InitializeData::triPush(string p1, string p2, string p3, pair<int,float> vd, bool visible) {

    unordered_map<string,vertex>::iterator point1 = map.find (p1);

    if(point1 == map.end())
        cout<<"Error :  Vertex 1 not found"<<endl;

    if(point1->second.curIndex >= 36)
        cout<<"Vertex allready in 36 traingles"<<endl;
    else {
        point1->second.neighbours[point1->second.curIndex] = universalTid;
        point1->second.curIndex++;
    }


    unordered_map<string,vertex>::iterator point2 =map.find (p2);

    if(point2 == map.end())
        cout<<"Error :  Vertex 2 not found"<<endl;

    if(point2->second.curIndex >= 36)
        cout<<"Vertex allready in 36 traingles"<<endl;
    else {
        point2->second.neighbours[point2->second.curIndex] = universalTid;
        point2->second.curIndex++;
    }

    unordered_map<string,vertex>::iterator point3 =map.find (p3);

    if(point3 == map.end())
        cout<<"Error :  Vertex 3 not found"<<endl;

    if(point3->second.curIndex >= 36)
        cout<<"Vertex allready in 36 traingles"<<endl;
    else {
        point3->second.neighbours[point3->second.curIndex] = universalTid;
        point3->second.curIndex++;
    }


    triangle t(&(point1->second),&(point2->second),&(point3->second),universalTid,vd.first,-vd.second,visible);

    Triangles.push_back(t);
    universalTid++;
}



void InitializeData::formTriangles(const char* fileName)
{
    ifstream myfile (fileName);

    pair<int,double> voxelDetails;
    vector<string> cube(8);
    string data;
	bool insert = true;

    if (myfile.is_open()) {
        int index = 0;
        while (myfile.good()) {
            getline (myfile, data);

            if(index == 0 ) {
                stringstream ss(data);
                int voxelID;
                string aux;
                double weathingD;
                ss>>voxelID>>aux;
                if (aux.length() > 0)
                    weathingD = convertDouble(aux.substr(1));
                else
                    weathingD = 0.0;
				insert = true;
                voxelDetails = make_pair(voxelID,weathingD); // would be used in each of the 12 traingles
                if (find(boundaryVoxelList.begin(),boundaryVoxelList.end(), voxelID) == boundaryVoxelList.end() )
                    insert = false;

            }
            else if (index >= 1 && index <= 8 )
                cube[index-1] = data;
            else if (index == 9 && insert){

                stringstream ss(data);
                string a[6];
                int b[6],c[6];
                ss>>a[0]>>b[0]>>c[0]>>a[1]>>b[1]>>c[1]>>a[2]>>b[2]>>c[2]>>a[3]>>b[3]>>c[3]>>a[4]>>b[4]>>c[4]>>a[5]>>b[5]>>c[5];

                //front face -- 0
                bool visible = false;
                if(b[0] == -1)
                    visible = true;

                triPush(cube[0], cube[1], cube[2],voxelDetails,visible);
                triPush(cube[0], cube[2], cube[3],voxelDetails,visible);

                //right face -- 1
                visible = false;
                if(b[1] == -1)
                    visible = true;

                triPush(cube[4], cube[0], cube[7],voxelDetails,visible);
                triPush(cube[4], cube[3], cube[7],voxelDetails,visible);

                //back face -- 2
                visible = false;
                if(b[2] == -1)
                    visible = true;

                triPush(cube[4], cube[5], cube[7],voxelDetails,visible);
                triPush(cube[5], cube[7], cube[6],voxelDetails,visible);

                //left face -- 3
                visible = false;
                if(b[3] == -1)
                    visible = true;

                triPush(cube[1], cube[5], cube[6],voxelDetails,visible);
                triPush(cube[1], cube[6], cube[2],voxelDetails,visible);

                // top face -- 4
                visible = false;
                if(b[4] == -1)
                    visible = true;

                triPush(cube[4], cube[5], cube[1],voxelDetails,visible);
                triPush(cube[4], cube[1], cube[0],voxelDetails,visible);

                //bottom face -- 5
                visible = false;
                if(b[5] == -1)
                    visible = true;

                triPush(cube[3], cube[2], cube[6],voxelDetails,visible);
                triPush(cube[3], cube[6], cube[7],voxelDetails,visible);
            }
            index = (index+1)%10;
        }
    }
}

map< double, pair< int, pair<int,int> > >::const_iterator findClose(map< double, pair< int, pair<int,int> > > mymap, double value) {
    auto last = mymap.begin();
    for(auto it = mymap.begin(); it != mymap.end(); it++) {
        if (it->first > value) {
            break;
        }
        else {
            last = it;
        }
    }
    return last;
}

// impart color to all the vertices depeding on their neighbourhood
void InitializeData::impartColor()
{
    // we can run through all vertices use the neighbourhood information and impart them a color using the corrosion values of the neighbours
	
    for ( auto it = map.begin(); it != map.end(); ++it )
    {
        vertex q = it->second;
        float sum = 0.0f;
        int count = 0;
        for (int i = 0; i < 36 ; i++ )
        {
            if(q.neighbours[i] != -1)
            {
                sum +=	 Triangles[q.neighbours[i]].corrosionLevel;
                count++;
            }
			
        }
        float val = sum / (1.0f * count);
		
        it->second.avgCorrosionLevel = val;
        //auto low = findClose(colormap, val);
		
        /*
		 float color = 1.0f - (1.0f-0.62f)*val;
		 float color;
		 if (val >= 0.5)
		 color = 1.0f;
		 else
		 color = 0.0f;
		 */
		
		//        it->second.r = (double)low->second.first/255.0;
		//        it->second.g = (double)low->second.second.first/255.0;
		//        it->second.b = (double)low->second.second.second/255.0;
		
        it->second.r = val;
        it->second.g = val;
        it->second.b = 1.0 - val;
    }
}



void InitializeData::initializeData()
{
    universalTid = 0;
    runningAlpha = 0.8;
    populateBoundaryVoxels("Data/voxels2.txt");
	cout<<"Voxels Selected "<<boundaryVoxelList.size()<<endl;
    readPoints("Data/voxels2.txt");
    cout<<"Reading done"<<map.size()<<endl;
    formTriangles("Data/voxels2.txt");
    cout<<"Traingles Formed"<<endl;
    // buildMap("Data/image.dat", "Data/weather.dat");
    cout<<"Weather Map read"<<endl;
    impartColor();
}

#ifdef __APPLE__
int main() {
	InitializeData* node = new InitializeData();
	node->initializeData();
	return 0;
}
#endif


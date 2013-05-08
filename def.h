#ifndef DEF_H
#define DEF_H
#endif // DEF_H

#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <cstring>
#include <string>

#define IMAGEFILE "Data/image.dat"
#define WEATHERMAPFILE "Data/weather.dat"
#define MAXCOLORMODELS 2

struct Vertex
{
    long long vid;
    float x,y,z;
    float r,g,b;
    long long neighbours[36]; // stores the tid's of the triangles that this vertex actually participates in
    int curIndex;
    float avgCorrosionLevel; // This avg Corrosion Value could be mapped to any color

    Vertex() {}
    Vertex(float x1, float y1,float z1)
    {
        x = x1;
        y = y1;
        z = z1;
        r = 0;
        g = 0;
        b = 0;
        for(int i=0;i<36;i++)
            neighbours[i]=-1;
        curIndex = 0;
        avgCorrosionLevel = 0.0f;
    }
    float getX()
    {return x;}

    float getY()
    {return y;}

    float getZ()
    {return z;}

};

typedef struct Vertex vertex;

struct Normal
{
    double x;
    double y;
    double z;

    Normal(){}

    Normal(double _x,double _y, double _z) {
        x = _x;
        y = _y;
        z = _z;
    }
};

typedef struct Normal normal;

struct Triangle
{
    vertex *first, *second, *third;
    unsigned long long tid,vid;
    bool isVisible;
    double corrosionLevel;
    normal fnormal;
    //probably can include normals
    //color of each vertex
    Triangle() {}

    Triangle(vertex *f, vertex *s, vertex *t,unsigned long long i, unsigned long long v, float CL, bool visible) {
        first = f;
        second = s;
        third = t;
        tid = i;
        vid = v;
        corrosionLevel = CL;
        isVisible = visible;

    }

};



typedef struct Triangle triangle;
class InitializeData
    {
private:
        // List of the files
        std::vector< std::string > VoxelFiles;
        // This would indicate whether sides would be rendered or not
        bool sides;
        // We will number the different color model and change them on a click
        /* Number of color models
         1 --> different colors for different regions
         2 --> compressed color range
        */
        int colorModel;
        // index of the current file being rendered
        int currentFile;
        std::vector< std::vector< std::vector<int> > > imageStore;
        std::vector< std::vector<double> > weatherDegree;
        std::map< double, std::pair< int, std::pair<int,int> > > colormap;
        void buildMap(const char* imageFile, const char* weatherMapFile);
        void split(std::string data, std::vector<std::string> points);
        void triPush(std::string p1, std::string p2, std::string p3, std::pair<int,float> vd, bool visible);
        void impartColor();
        void formTriangles(const char* fileName);
        double convertDouble(std::string str);
        void calculateNormals();
        std::vector<int> colorMap(float corval);
        void formTrianglesTopAndBottom(const char* fileName);
        void Calculate();
    public:
        void changeColorModel();
        void changeFile(bool increase);
        void changeTriangleArray();
        void changeSides();
        void initializeData(std::vector< std::string > files);
        std::unordered_map<std::string,vertex> map;
        std::vector<triangle> Triangles;

    };


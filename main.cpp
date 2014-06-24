//
//  main.cpp
//  Tessellation Test
//
//  Created by Keshav Choudhary on 2/21/2014.
//  Copyright (c) 2014 Keshav Choudhary. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cassert>
#include <cmath>


#define GLM_FORCE_RADIANS

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/norm.hpp"
#include "ImageMagick/Magick++.h"


using namespace std;

static uint windowWidth;
static uint windowHeight;
int cnt;
static 	GLenum drawingMode;

int h_height = 512, h_width = 512; //dimensions of the height map

vector< vector<GLfloat> > heightMap;
vector<vector<pair<int,pair<int,int> > > > height_map(h_width,vector<pair<int,pair<int,int> > >(h_height));


static uint shaderProgramme;

static uint vboVertex, vboColor, vboNormal, vboTex, vboTangent, vboTNormals, vboWeathering, vboWeatheringNeighbour, indices;
static uint vao;

static uint tessLevelInner, tessLevelOuter;
static uint weatheringMapH,weatheringMapW;

float tessLevelInnerVal, tessLevelOuterVal;
float WeatheringMapHeight, WeatheringMapWidth;

glm::vec3 ball_position(0.0f, 0.0f, 0.0f);

vector<GLfloat> points;
//= {
//	0.0,0.0,0.0,
//	1.0,0.0,0.0,
//	0.0,1.0,0.0
//};
vector<GLfloat> color;
//= {
//	1.0f, 0.0f,  0.0f,
//	0.0f, 1.0f,  0.0f,
//	0.0f, 0.0f,  1.0f
//};

vector<GLfloat> vnormals;
vector<GLfloat> vtangents;
vector<GLfloat> tnormals;
vector<GLfloat> displacements;
vector<GLfloat> texCordinates;
vector<GLfloat> weathering_degree;
vector<GLfloat> weathering_degree_neighbour;
vector<int> ind;
//= {
//	0,1,2
//};
int vertex_neighbours;



static GLsizei IndexCount;
static const GLuint PositionSlot = 0;
static const GLuint PositionSlotColor = 1;

float cam_speed = 10.0f; // 1 unit per second
float cam_yaw_speed = 300.0f; // 10 degrees per second

glm::vec3 cam_pos(0.0, 0.0, 10.0);
float cam_yaw = 0.0f; // y-rotation in degrees


glm::mat4 view_mat = glm::mat4();
glm::mat4 proj_mat = glm::mat4();
glm::mat4 model_mat = glm::mat4();

int view, proj, model, originalModel, weatheringMap, colorMap, displacementMap, normalMap ;

glm::fquat orientationQuaternion_, currentQuaternion;

double mouseX = 1;
double mouseY = 0;

bool mouse_pressed = false;

// Projection parameters
float near = 1.0f;  // near clipping plane
float far = 100.0f; // far clipping plane
float fov = glm::radians(75.0f);

Magick::Image* m_pImage;
Magick::Blob m_blob;

Magick::Image* m_pNormalMap;
Magick::Blob m_blob_normal;

GLenum m_textureTarget;
GLuint m_textureObj;
GLuint m_normalMapObj;


void buildView() {
    view_mat = glm::lookAt(cam_pos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

}

void buildProj() {
    float aspect = (float)windowWidth / (float)windowHeight; // aspect ratio
    proj_mat = glm::perspective(fov, aspect, near, far);
}

void buildModel() {
    model_mat = glm::mat4(1.0f);
}

static void checkError(GLint status, unsigned int shader_index, const char *msg)
{
    if (!status)
    {
        printf("%s\n", msg);
        int max_length = 2048;
        int actual_length = 0;
        char log[2048];
        glGetShaderInfoLog (shader_index, max_length, &actual_length, log);
        printf ("shader info log for GL index %i:\n%s\n", shader_index, log);
        exit(EXIT_FAILURE);
    }
}

string textFileRead (const char* fileName) {
    ifstream myfile (fileName);
    string data;
    if (myfile.is_open()) {
        string line;
        while (myfile.good()) {
            getline (myfile, line);
            data.append("\n");
            data.append(line);
        }
        myfile.close();
    }
    else
        cout<<"Could not open "<<string(fileName)<<endl;


    return data;
}


void useShaderPrograms(){
    glUseProgram(shaderProgramme);

    //	glUniformMatrix4fv (view, 1, GL_FALSE, &view_mat[0][0]);
    //	glUniformMatrix4fv (proj[prog], 1, GL_FALSE, &proj_mat[0][0]);
    //	glUniformMatrix4fv (model[prog], 1, GL_FALSE, &model_mat[0][0]);
    //	glUniformMatrix4fv (originalModel[prog], 1, GL_FALSE, &model_mat[0][0] );
    glUniform1f(weatheringMapH,WeatheringMapHeight);//, WeatheringMapWidth);
    glUniform1f(weatheringMapW,WeatheringMapWidth);
    glUniform1f(tessLevelInner, tessLevelInnerVal);
    glUniform1f(tessLevelOuter, tessLevelOuterVal);
    glUniformMatrix4fv (view, 1, GL_FALSE, &view_mat[0][0]);
    glUniformMatrix4fv (proj, 1, GL_FALSE, &proj_mat[0][0]);
    glUniformMatrix4fv (model, 1, GL_FALSE, &model_mat[0][0]);
    glUniform1i(weatheringMap, 0);
    glUniform1i(colorMap, 1);
    glUniform1i(displacementMap,2);
    glUniform1i(normalMap, 3);
    //	glUniformMatrix4fv (originalModel[prog], 1, GL_FALSE, &model_mat[0][0] );
    cout<<weatheringMapW<<endl;



}


void createShaderPrograms() {

    string vertexShader = textFileRead ("vs.glsl");
    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    const char* stra = vertexShader.c_str();
    glShaderSource (vs, 1, &stra, NULL);
    glCompileShader (vs);
    int params = -1;
    glGetShaderiv(vs,GL_COMPILE_STATUS,&params);
    checkError(params, vs, "Vertex Shader not compiled");

    string TessellationControlShader = textFileRead ("tcs.glsl");
    GLuint tcs = glCreateShader (GL_TESS_CONTROL_SHADER);
    const char* strb = TessellationControlShader.c_str();
    glShaderSource (tcs, 1, &strb, NULL);
    glCompileShader (tcs);
    params = -1;
    glGetShaderiv(tcs,GL_COMPILE_STATUS,&params);
    checkError(params, tcs, "Tessellation Control not compiled");

    string TessellationEvaluationShader = textFileRead ("tes.glsl");
    GLuint tes = glCreateShader (GL_TESS_EVALUATION_SHADER);
    const char* strc = TessellationEvaluationShader.c_str();
    glShaderSource (tes, 1, &strc, NULL);
    glCompileShader (tes);
    params = -1;
    glGetShaderiv(tes,GL_COMPILE_STATUS,&params);
    checkError(params, tes, "Tessellation Evaluation not compiled");

    string fragment_shader = textFileRead ("fs.glsl");
    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    const char* strd = fragment_shader.c_str();
    glShaderSource (fs, 1, &strd, NULL);
    glCompileShader (fs);
    params = -1;
    glGetShaderiv(fs,GL_COMPILE_STATUS,&params);
    checkError(params, fs, "Fragment Shader not compiled");


    shaderProgramme = glCreateProgram ();
    glAttachShader(shaderProgramme,vs);
    glAttachShader(shaderProgramme,tcs);
    glAttachShader(shaderProgramme,tes);
    glAttachShader(shaderProgramme,fs);

    glBindAttribLocation (shaderProgramme, 0, "vertex_position");
    glBindAttribLocation (shaderProgramme, 1, "vertex_colour");
    glBindAttribLocation (shaderProgramme, 2, "vertex_normal");
    glBindAttribLocation (shaderProgramme, 3, "texCordinates");
    //	glBindAttribLocation (shaderProgramme[i], 4, "displacements");

    glLinkProgram (shaderProgramme);

    //		view[i] = glGetUniformLocation (shaderProgramme[i], "view");
    //		proj[i] = glGetUniformLocation (shaderProgramme[i], "proj");
    //		model[i] = glGetUniformLocation (shaderProgramme[i], "model");
    //		originalModel[i] = glGetUniformLocation(shaderProgramme[i], "orig_model_mat");

    tessLevelInner = glGetUniformLocation (shaderProgramme, "tessLevelInner");
    tessLevelOuter = glGetUniformLocation (shaderProgramme, "tessLevelOuter");
    weatheringMapH = glGetUniformLocation (shaderProgramme, "weatheringMapH");
    weatheringMapW = glGetUniformLocation (shaderProgramme, "weatheringMapW");
    
    view = glGetUniformLocation (shaderProgramme, "view");
    proj = glGetUniformLocation (shaderProgramme, "proj");
    model = glGetUniformLocation (shaderProgramme, "model");
    weatheringMap = glGetUniformLocation(shaderProgramme, "gWeatheringMap");
    colorMap = glGetUniformLocation(shaderProgramme, "gColorMap");
    displacementMap = glGetUniformLocation(shaderProgramme, "gDisplacementMap");
    normalMap = glGetUniformLocation(shaderProgramme, "gNormalMap");
    //	originalModel = glGetUniformLocation(shaderProgramme[i], "orig_model_mat");

}

void generateVertexBuffer(){


    IndexCount = (int)ind.size();

    // IndexCount = sizeof(Faces) / sizeof(Faces[0]);
    vboVertex = 0;
    glGenBuffers(1, &vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), &points[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Create the VBO for indices:
    indices = 0;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(int), &ind[0], GL_STATIC_DRAW);


    vboColor = 0;
    glGenBuffers (1, &vboColor);
    glBindBuffer (GL_ARRAY_BUFFER, vboColor);
    glBufferData (GL_ARRAY_BUFFER, color.size() * sizeof (GLfloat), &color[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    vboNormal = 0;
    glGenBuffers (1, &vboNormal);
    glBindBuffer (GL_ARRAY_BUFFER, vboNormal);
    glBufferData (GL_ARRAY_BUFFER, vnormals.size() * sizeof (GLfloat), &vnormals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    vboTex = 0;
    glGenBuffers (1, &vboTex);
    glBindBuffer (GL_ARRAY_BUFFER, vboTex);
    glBufferData (GL_ARRAY_BUFFER, texCordinates.size() * sizeof (GLfloat), &texCordinates[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);

    vboTangent = 0;
    glGenBuffers (1, &vboTangent);
    glBindBuffer (GL_ARRAY_BUFFER, vboTangent);
    glBufferData (GL_ARRAY_BUFFER, vtangents.size() * sizeof (GLfloat), &vtangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, NULL);


    vboTNormals = 0;
    glGenBuffers (1, &vboTNormals);
    glBindBuffer (GL_ARRAY_BUFFER, vboTNormals);
    glBufferData (GL_ARRAY_BUFFER, tnormals.size() * sizeof (GLfloat), &tnormals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /*
    vboWeathering = 0;
    glGenBuffers (1, &vboWeathering);
    glBindBuffer (GL_ARRAY_BUFFER, vboWeathering);
    glBufferData (GL_ARRAY_BUFFER, weathering_degree.size() * sizeof (GLfloat), &weathering_degree[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 0, 0);


    vboWeatheringNeighbour = 0;
    glGenBuffers (1, &vboWeatheringNeighbour);
    glBindBuffer (GL_ARRAY_BUFFER, vboWeatheringNeighbour);
    glBufferData (GL_ARRAY_BUFFER, weathering_degree_neighbour.size() * sizeof (GLfloat), &weathering_degree_neighbour[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, vertex_neighbours, GL_FLOAT, GL_FALSE, 0, 0);    
    */
}

void generateVertexArray(){
    vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
}

void readHeightMapRGB(string filename) {
    string line;

    heightMap.resize(h_width, vector<float>(h_height));

    ifstream myfile (filename);
    if (myfile.is_open())
    {
        int cnt = 0;
        while ( getline (myfile,line) )
        {

            cnt ++;

            if(cnt > 1)
            {
                int v = 0;
                vector<int> val;
                for(int i = 0; i < (int)line.size(); i++){
                    if(val.size() == 5)
                        break;
                    int r = line[i]-'0';
                    if(r >=0 && r <= 9 )
                        v = v*10+r;
                    if(line[i] == ',' ||line[i] == ':')
                    {
                        val.push_back(v);
                        v = 0;
                    }


                }
                height_map[val[0]][val[1]] = make_pair(val[2],make_pair(val[3],val[4]));
                heightMap[val[0]][val[1]] = (float)val[2]/255.0;
            }
        }
        myfile.close();
    }
    else
        cout << "Unable to open file"<<endl;

}

void initMesh(unsigned int Index, const aiMesh* paiMesh)
{

    cout<<paiMesh->mNumVertices<<endl;

    const aiVector3D Zero3D(1.0f, 0.0f, 0.0f);

    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = paiMesh->HasNormals() ?  &(paiMesh->mNormals[i]) : &Zero3D ;
        const aiVector3D* pTangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mTangents[i]) : &Zero3D;
        const aiVector3D* pBitangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mBitangents[i]) : &Zero3D;
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        points.push_back(pPos->x);
        points.push_back(pPos->y);
        points.push_back(pPos->z);

        texCordinates.push_back(pTexCoord->x);
        texCordinates.push_back(pTexCoord->y);

        //cout<<pTexCoord->x<<" "<<pTexCoord->y<<endl;

        vnormals.push_back(pNormal->x);
        vnormals.push_back(pNormal->y);
        vnormals.push_back(pNormal->z);

        color.push_back(pTexCoord->x);
        color.push_back(pTexCoord->y);
        color.push_back(0.0f);

        glm::vec3 t(pTangent->x, pTangent->y, pTangent->z);
        glm::vec3 n(pNormal->x, pNormal->y, pNormal->z);
        glm::vec3 b(pBitangent->x, pBitangent->y, pBitangent->z);

        /* orthogonalize and normalize the tangent so we can use it in something approximating 
         * a T,N,B inverse matrix */
        glm::vec3 t_i = glm::normalize(t - n * glm::dot(n, t));
        
        /* get determinant of T,B,N 3x3 matrix by dot*cross method */
        float det = (glm::dot ( glm::cross(n,t) , b ) );
        if (det < 0.0f)
            det = -1.0f;
        else
            det = 1.0f;

        /* push back normalized tangent along with determinant value */
        vtangents.push_back(t_i.x);
        vtangents.push_back(t_i.y);
        vtangents.push_back(t_i.z);
        vtangents.push_back(det);

    }
    cout<<paiMesh->mNumFaces<<endl;
    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        ind.push_back(Face.mIndices[0]);
        ind.push_back(Face.mIndices[1]);
        ind.push_back(Face.mIndices[2]);
    }
}

void objloader(string fileName){

    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(fileName.c_str(), aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

    points.clear();
    vnormals.clear();
    texCordinates.clear();
    color.clear();

    cout<<"Number of meshes "<<pScene->mNumMeshes<<endl;

    for (unsigned int i = 0 ; i < pScene->mNumMeshes ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        initMesh(i, paiMesh);
    }

    cout<<ind.size()/3<<" "<<points.size()/3<<endl;
}

void readWethearingDegree(string fileName) {
    ifstream infile(fileName);
    unsigned int num_vertices = points.size()/3;

    weathering_degree.clear();

    int vertices, neighbours;
    neighbours = 0;
    infile>>vertices;

    vertex_neighbours = neighbours;

    for(unsigned int i = 0; i < num_vertices; i++) {
        GLfloat val;
        if (infile>>val) {
            weathering_degree.push_back(val);
            for(int j = 0; j < neighbours; j++) {
                infile>>val;
                weathering_degree_neighbour.push_back(val);
            }
        }
        else {
            cout<<"Insufficient number of points in weathering degree file "<<fileName<<endl;
            break;
        }
    }
}

void plyLoader(string fileName, string objFile) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_JoinIdenticalVertices );

    if (!scene) {
        cout<<importer.GetErrorString()<<endl;
    }
    else {

        points.clear();
        color.clear();
        vnormals.clear();
        vtangents.clear();
        tnormals.clear();
        ind.clear();

        for(unsigned int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[i];

            cout<<"PLY Mesh Count "<<mesh->mNumVertices<<endl;

            // cout<<mesh->GetNumColorChannels()<<endl;
            // cout<<mesh->HasVertexColors(0)<<endl;
            // cout<<mesh->HasNormals()<<endl;

            for(unsigned int j = 0; j < mesh->mNumVertices; j++) {

                const aiVector3D* pos = &(mesh->mVertices[j]);
                const aiColor4D* col = &(mesh->mColors[0][j]);
                const aiVector3D* nrml = &(mesh->mNormals[j]);

                // cout<<pos->x<<" "<<pos->y<<" "<<pos->z<<endl;
                // cout<<col->r<<" "<<col->g<<" "<<col->b<<endl;
                // cout<<nrml->x<<" "<<nrml->y<<" "<<nrml->z<<endl;

                points.push_back(pos->x); points.push_back(pos->y); points.push_back(pos->z);
                //color.push_back(col->r/255.0); color.push_back(col->g/255.0); color.push_back(col->b/255.0);
                color.push_back(abs(pos->x)); color.push_back(0.0); color.push_back(0.0);
                tnormals.push_back(nrml->x); tnormals.push_back(nrml->y); tnormals.push_back(nrml->z);
            }

            for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) {
                const aiFace& Face = mesh->mFaces[i];
                if (Face.mNumIndices != 3)
                    continue;
                ind.push_back(Face.mIndices[0]);
                ind.push_back(Face.mIndices[1]);
                ind.push_back(Face.mIndices[2]);
            }   
        }
    }

    const aiScene* objscene = importer.ReadFile(objFile.c_str(), aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

    if (!objscene) {
        cout<<importer.GetErrorString()<<endl;
    }
    else {
        for(unsigned int i = 0; i < objscene->mNumMeshes; i++) {
            const aiMesh* mesh = objscene->mMeshes[i];

            cout<<"OBJ Mesh Count "<<mesh->mNumVertices<<endl;

            const aiVector3D Zero3D(1.0f, 0.0f, 0.0f);
            for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
//                cout<<"index "<<i<<endl;
                const aiVector3D* pNormal   = mesh->HasNormals() ?  &(mesh->mNormals[i]) : &Zero3D ;
                const aiVector3D* pTangent = mesh->HasTangentsAndBitangents() ? &(mesh->mTangents[i]) : &Zero3D;
                const aiVector3D* pBitangent = mesh->HasTangentsAndBitangents() ? &(mesh->mBitangents[i]) : &Zero3D;
                const aiVector3D* texcoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &Zero3D;

                if (isnan(pTangent->x)) {
                    pTangent = &Zero3D;
                    pBitangent = &Zero3D;
                }

//                cout<<pNormal->x<<" "<<pNormal->y<<" "<<pNormal->z<<endl;
//                cout<<texcoord->x<<" "<<texcoord->y<<endl;
//                cout<<pTangent->x<<" "<<pTangent->y<<" "<<pTangent->z<<endl;
//                cout<<pBitangent->x<<" "<<pBitangent->y<<" "<<pBitangent->z<<endl;


                // Generate vtangents
                glm::vec3 t(pTangent->x, pTangent->y, pTangent->z);
                glm::vec3 n(pNormal->x, pNormal->y, pNormal->z);
                glm::vec3 b(pBitangent->x, pBitangent->y, pBitangent->z);

                /* orthogonalize and normalize the tangent so we can use it in something approximating
                 * a T,N,B inverse matrix */
                glm::vec3 t_i = glm::normalize(t - n * glm::dot(n, t));
                
                /* get determinant of T,B,N 3x3 matrix by dot*cross method */
                float det = (glm::dot ( glm::cross(n,t) , b ) );
                if (det < 0.0f)
                    det = -1.0f;
                else
                    det = 1.0f;

                /* push back normalized tangent along with determinant value */
                vtangents.push_back(t_i.x);
                vtangents.push_back(t_i.y);
                vtangents.push_back(t_i.z);
                vtangents.push_back(det);

                // push back vertex normals
                vnormals.push_back(n.x);
                vnormals.push_back(n.y);
                vnormals.push_back(n.z);

                // push back texture coordinates
                texCordinates.push_back(texcoord->x);
                texCordinates.push_back(texcoord->y);

            }
        }

        cout<<"Number of face "<<ind.size()/3<<"  and total number of points "<<points.size()/3<<endl;
    }
}

bool LoadTexture(const std::string& fileName){
    try {
        m_pImage = new Magick::Image(fileName);
        m_pImage->write(&m_blob, "RGBA");
    }
    catch (Magick::Error& Error) {
        std::cout << "Error loading texture '" << fileName << "': " << Error.what() << std::endl;
        return false;
    }
    m_textureObj = 0;
    m_textureTarget = GL_TEXTURE_2D;
    glGenTextures(1, &m_textureObj);
    glBindTexture(m_textureTarget, m_textureObj);
    glTexImage2D(m_textureTarget, 0, GL_RGBA, m_pImage->columns(), m_pImage->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

bool LoadWeatheringTexture(const std::string& fileName) {
    ifstream infile(fileName);
    int wmx, wmy;
    infile>>wmx>>wmy;
    GLfloat weatheringDegree[wmx*wmy];
    WeatheringMapHeight = wmx;
    WeatheringMapWidth = wmy;

    cout<<WeatheringMapHeight<<" "<<WeatheringMapWidth<<endl;
    for(int i = 0; i < wmx; i++) {
        for(int j = 0; j < wmy; j++)
            infile>>weatheringDegree[i*wmy + j];
    }

    m_textureObj = 0;
    m_textureTarget = GL_TEXTURE_2D;
    glGenTextures(1, &m_textureObj);
    glBindTexture(m_textureTarget, m_textureObj);
    glTexImage2D(m_textureTarget, 0, GL_RED, wmx, wmy, 0, GL_RED, GL_FLOAT, (void*)weatheringDegree);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void Bind(GLenum TextureUnit) {
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}


static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        cnt = (cnt+1)%3;
        switch (cnt) {
            case 0:
                drawingMode = GL_FILL;
                break;
            case 1:
                drawingMode = GL_LINE;
                break;
            case 2:
                drawingMode = GL_POINT;
                break;
            default:
                break;
        }

    }

    if(key == GLFW_KEY_R && action == GLFW_PRESS){
        model_mat = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
        glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(model_mat));
    }

    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) {
        tessLevelInnerVal += 10.0f;
        tessLevelOuterVal += 10.0f;
        glUniform1f(tessLevelInner, tessLevelInnerVal);
        glUniform1f(tessLevelOuter, tessLevelOuterVal);
    }

    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) {
        tessLevelInnerVal -= 10.0f;
        tessLevelOuterVal -= 10.0f;
        glUniform1f(tessLevelInner, tessLevelInnerVal);
        glUniform1f(tessLevelOuter, tessLevelOuterVal);        
    }
}

void initializeQuat()
{
    orientationQuaternion_ = glm::quat();
    orientationQuaternion_ = glm::normalize(orientationQuaternion_);
    currentQuaternion = glm::quat();
    currentQuaternion = glm::normalize(currentQuaternion);
}

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest){
    start = glm::normalize(start);
    dest = glm::normalize(dest);
 
    float cosTheta = glm::dot(start, dest);
    glm::vec3 rotationAxis;
 
    if (cosTheta < -1 + 0.001f){
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
        if (glm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
            rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
 
        rotationAxis = glm::normalize(rotationAxis);
        return glm::angleAxis(180.0f, rotationAxis);
    }
 
    rotationAxis = glm::cross(start, dest);
 
    
    float s = sqrt( (1+cosTheta)*2 );
    float invs = 1 / s;
 
    return glm::quat(
        s * 0.5f, 
        rotationAxis.x * invs,
        rotationAxis.y * invs,
        rotationAxis.z * invs
    );
 
}

static void cursorCallback(GLFWwindow* window, double x, double y){
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);

    if (state == GLFW_PRESS) {

        static glm::vec3 start(0.0, 0.0, 0.0);
        
        double mouse_x = (x/(double)windowWidth)*2-1;
        double mouse_y = 1-(y/double(windowHeight))*2;
        
        if(pow(mouse_x, 2) + pow(mouse_y, 2) > 1.0) 
            return;
        
        glm::vec3 dest = glm::vec3(mouse_x, mouse_y, sqrt(1.0 - pow(mouse_x,2) - pow(mouse_y,2)));

        if (mouse_pressed == false) {
            start = dest;
            mouse_pressed = true;
            return;
        }

        float sensitivity = 0.3;

        glm::quat relativeQuat = glm::normalize(RotationBetweenVectors(start, dest));
        orientationQuaternion_ = relativeQuat*orientationQuaternion_;

        // control sensitivity
        orientationQuaternion_ = glm::slerp(currentQuaternion, orientationQuaternion_, sensitivity);
    }
    else {
        mouse_pressed = false;
    }
}

void scrollCallback(GLFWwindow* window, double xoff, double yoff){
    cam_pos.z += yoff/10.0f;
    if (cam_pos == glm::vec3(0.0, 0.0, 0.0)) {
			cam_pos.z += yoff/10.0f;
	}
    buildView();
    glUniformMatrix4fv(view,1,GL_FALSE,&view_mat[0][0]);

}


static void resizeCallback(GLFWwindow* window,int width, int height){
    windowWidth = width;
    windowHeight = height;
	buildProj();
    glUniformMatrix4fv(proj, 1, GL_FALSE, glm::value_ptr(proj_mat));
	glViewport(0, 0, width, height);
}

void _update_fps_counter (GLFWwindow* window) {
  static double previous_seconds = glfwGetTime ();
  static int frame_count;
  double current_seconds = glfwGetTime ();
  double elapsed_seconds = current_seconds - previous_seconds;
  if (elapsed_seconds > 0.25) {
    previous_seconds = current_seconds;
    double fps = (double)frame_count / elapsed_seconds;
    char tmp[128];
    sprintf (tmp, "opengl @ fps: %.2f", fps);
    glfwSetWindowTitle (window, tmp);
    frame_count = 0;
  }
  frame_count++;
}

int main(int argc, const char * argv[])
{

    windowWidth = 640;
    windowHeight = 480;

    if(argc < 7)
    {
        cout<<"Not enough arguments"<<endl;
        return 1;
    }
    
    plyLoader(string(argv[1]), string(argv[2]));
    //objloader(string(argv[2]));
    //readWethearingDegree(string(argv[3]));

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "Tessellation Test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glewExperimental = true;

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // start GLEW extension handler
    glewInit ();

    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    int max_patch_vertices = 0;
    glGetIntegerv (GL_MAX_PATCH_VERTICES, &max_patch_vertices);
    printf ("Max supported patch vertices %i\n", max_patch_vertices);

    tessLevelInnerVal = 1.0f;
    tessLevelOuterVal = 1.0f;

// No textures needed as of now


    LoadWeatheringTexture(string(argv[4]));
    Bind(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE1);
    LoadTexture(string(argv[5]));
    Bind(GL_TEXTURE1);

    glActiveTexture(GL_TEXTURE2);
    LoadTexture(string(argv[6]));
    Bind(GL_TEXTURE2);

    glActiveTexture(GL_TEXTURE3);
    LoadTexture(string(argv[7]));
    Bind(GL_TEXTURE3);

    cout<<"Generating of texture complete"<<endl;


    initializeQuat();
    buildView();
    buildProj();
    buildModel();


    generateVertexArray();
    generateVertexBuffer();


    createShaderPrograms();
    useShaderPrograms();

    cout<<"Shaders compiled"<<endl;

    cnt = 0;
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetScrollCallback(window, scrollCallback);


    drawingMode = GL_FILL;

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    //	glEnable(GL_CULL_FACE);
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
         _update_fps_counter (window);
        // Manage rotation
        currentQuaternion = glm::slerp(currentQuaternion, orientationQuaternion_, 0.4f);

        model_mat = glm::toMat4(currentQuaternion);
        glUniformMatrix4fv(model, 1, GL_FALSE, &model_mat[0][0]);

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindVertexArray (vao);
        
        glPolygonMode(GL_FRONT_AND_BACK, drawingMode);//GL_POINT, GL_LINE, GL_FILL
        glPatchParameteri(GL_PATCH_VERTICES,3);
        
        glDrawElements(GL_PATCHES, IndexCount, GL_UNSIGNED_INT, 0);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}


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

static uint vboVertex,vboColor,vboNormal,vboTex,indices;
static uint vao;

static uint tessLevelInner, tessLevelOuter;

float tessLevelInnerVal, tessLevelOuterVal;

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
vector<GLfloat> displacements;
vector<GLfloat> texCordinates;

vector<int> ind;
//= {
//	0,1,2
//};


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

int view,proj,model,originalModel,colorMap,displacementMap;

glm::fquat orientationQuaternion_;

double mouseX = 1;
double mouseY = 0;

// Projection parameters
float near = 1.0f;  // near clipping plane
float far = 100.0f; // far clipping plane
float fov = 75.0f;

Magick::Image* m_pImage;
Magick::Blob m_blob;

GLenum m_textureTarget;
GLuint m_textureObj;



void initialize()
{
    orientationQuaternion_ = glm::quat();
    orientationQuaternion_ = glm::normalize(orientationQuaternion_);

}
void rotateit(const glm::detail::float32& degrees, const glm::vec3& axis)
{
    //    if ( axis == glm::vec3(0.0f, 1.0f, 0.0f) )
    //        orientationQuaternion_ =  glm::normalize(glm::angleAxis(degrees, axis)) * orientationQuaternion_;
    //    else
    //	{
    orientationQuaternion_ =  glm::normalize(glm::angleAxis(degrees, axis)) *  orientationQuaternion_;

    //	}
}

void buildView() {

    //	glm::quat temp = glm::conjugate(orientationQuaternion_);
    //view_mat = glm::mat4_cast(temp);
    //	view_mat = glm::mat4(1);
    //	view_mat = glm::translate(view_mat, glm::vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
    view_mat = glm::lookAt(cam_pos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

}

void buildProj() {
    float aspect = (float)windowWidth / (float)windowHeight; // aspect ratio
    proj_mat = glm::perspective(fov, aspect, near, far);
}

void buildModel() {
    model_mat = glm::mat4(1.0f);
}




static void checkError(GLint status, const char *msg)
{
    if (!status)
    {
        printf("%s\n", msg);
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

    glUniform1f(tessLevelInner, tessLevelInnerVal);
    glUniform1f(tessLevelOuter, tessLevelOuterVal);
    glUniformMatrix4fv (view, 1, GL_FALSE, &view_mat[0][0]);
    glUniformMatrix4fv (proj, 1, GL_FALSE, &proj_mat[0][0]);
    glUniformMatrix4fv (model, 1, GL_FALSE, &model_mat[0][0]);
    glUniform1i(colorMap, 0);
    glUniform1i(displacementMap,1);

    //	glUniformMatrix4fv (originalModel[prog], 1, GL_FALSE, &model_mat[0][0] );



}


void createShaderPrograms() {

    string vertexShader = textFileRead ("vs.glsl");
    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    const char* stra = vertexShader.c_str();
    glShaderSource (vs, 1, &stra, NULL);
    glCompileShader (vs);
    int params = -1;
    glGetShaderiv(vs,GL_COMPILE_STATUS,&params);
    checkError(params,"Vertex Shader not compiled");

    string TessellationControlShader = textFileRead ("tcs.glsl");
    GLuint tcs = glCreateShader (GL_TESS_CONTROL_SHADER);
    const char* strb = TessellationControlShader.c_str();
    glShaderSource (tcs, 1, &strb, NULL);
    glCompileShader (tcs);
    params = -1;
    glGetShaderiv(tcs,GL_COMPILE_STATUS,&params);
    checkError(params,"Tessellation Control not compiled");

    string TessellationEvaluationShader = textFileRead ("tes.glsl");
    GLuint tes = glCreateShader (GL_TESS_EVALUATION_SHADER);
    const char* strc = TessellationEvaluationShader.c_str();
    glShaderSource (tes, 1, &strc, NULL);
    glCompileShader (tes);
    params = -1;
    glGetShaderiv(tes,GL_COMPILE_STATUS,&params);
    checkError(params,"Tessellation Evaluation not compiled");

    string fragment_shader = textFileRead ("fs.glsl");
    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    const char* strd = fragment_shader.c_str();
    glShaderSource (fs, 1, &strd, NULL);
    glCompileShader (fs);
    params = -1;
    glGetShaderiv(fs,GL_COMPILE_STATUS,&params);
    checkError(params,"Fragment Shader not compiled");


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

    view = glGetUniformLocation (shaderProgramme, "view");
    proj = glGetUniformLocation (shaderProgramme, "proj");
    model = glGetUniformLocation (shaderProgramme, "model");
    colorMap = glGetUniformLocation(shaderProgramme, "gColorMap");
    displacementMap = glGetUniformLocation(shaderProgramme, "gDisplacementMap");
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
        const aiVector3D* pNormal   = paiMesh->HasNormals() ?  &(paiMesh->mNormals[i]):&Zero3D ;
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;


        points.push_back(pPos->x);
        points.push_back(pPos->y);
        points.push_back(pPos->z);

        texCordinates.push_back(pTexCoord->x);
        texCordinates.push_back(pTexCoord->y);

        vnormals.push_back(pNormal->x);
        vnormals.push_back(pNormal->y);
        vnormals.push_back(pNormal->z);

        color.push_back(pTexCoord->x);
        color.push_back(pTexCoord->y);
        color.push_back(0.0f);

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

    const aiScene* pScene = Importer.ReadFile(fileName.c_str(), aiProcess_JoinIdenticalVertices);

    points.clear();
    vnormals.clear();
    texCordinates.clear();
    color.clear();

    cout<<"Number of meshes "<<pScene->mNumMeshes<<endl;

    for (unsigned int i = 0 ; i < pScene->mNumMeshes ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        initMesh(i, paiMesh);
    }

    cout<<ind.size()<<" "<<points.size()<<endl;
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
}


static void cursorCallback(GLFWwindow* window, double x, double y){
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);

    if (state == GLFW_PRESS) {

        static glm::vec3 V2(0.0, 0.0, 0.0);
        double mouse_x = (x/(double)windowWidth)*2-1;
        double mouse_y = 1-(y/double(windowHeight))*2;
        if(mouse_x*mouse_x + mouse_y*mouse_y < 0.0001) return;
        glm::vec3 ballMovement = glm::vec3(mouse_x,mouse_y,0);
        ball_position = ball_position + ballMovement;

        float speed = glm::l1Norm(ballMovement);

        glm::vec3 downwards  = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 rotationAxis = glm::normalize(glm::cross(downwards, ballMovement));

        //glm::fquat rotation = glm::normalize(glm::angleAxis(speed, rotationAxis));
        glm::vec3 am(0.0f,-1.0f,0.0);
        //	rotateit(speed/5.0f, am);
        //	buildView();
        //	glUniformMatrix4fv (view[prog], 1, GL_FALSE, &view_mat[0][0]);
        model_mat = glm::rotate(model_mat, speed, rotationAxis);
        glUniformMatrix4fv(model,1,GL_FALSE,&model_mat[0][0]);

    }
}

void scrollCallback(GLFWwindow* window, double xoff, double yoff){
    cam_pos.z += yoff;
    if (cam_pos == glm::vec3(0.0, 0.0, 0.0)) {
			cam_pos.z += yoff;
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



int main(int argc, const char * argv[])
{

    windowWidth = 640;
    windowHeight = 480;

    if(argc < 4)
    {
        cout<<"Not enough arguments"<<endl;
        return 1;
    }

    objloader(string(argv[1]));


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

    tessLevelInnerVal = 3.0f;
    tessLevelOuterVal = 4.0f;

    LoadTexture(string(argv[2]));
    Bind(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE1);
    LoadTexture(string(argv[3]));
    Bind(GL_TEXTURE1);

    cout<<"Generating of texture complete"<<endl;

    initialize();
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



    GLenum drawingMode = GL_FILL;

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    //	glEnable(GL_CULL_FACE);
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
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


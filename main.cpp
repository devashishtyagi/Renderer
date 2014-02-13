//
//  draw.cpp
//  MinorProject
//
//  Created by Keshav Choudhary and Devashish Tyagi on 27/02/13.
//  Copyright (c) 2013 Devashish Tyagi and Keshav Choudhary. All rights reserved.
//

#include "draw.h"
#include "def.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include "imageloader/stb_image.h"
#include <random>

#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

using namespace std;

int window_width = 800, window_height = 600;
int h_height = 512, h_width = 512;

vector<GLfloat> points;
vector<GLfloat> color;
vector<GLfloat> vnormals;
vector<GLfloat> texCordinates;

vector< vector<GLfloat> > heightMap;
 vector<vector<pair<int,pair<int,int> > > > height_map(h_width,vector<pair<int,pair<int,int> > >(h_height));

GLenum draw_mode;

// View parameters
glm::vec3 cam_pos(0.0, 10.0, 30.0);
float cam_yaw = 0.0f; // y-rotation in degrees
glm::mat4 view_mat = glm::mat4();
glm::mat4 proj_mat = glm::mat4();
glm::mat4 model_mat = glm::mat4();

// Camera movement parameters
float camera_speed = 0.1f;

// Projection parameters
float near = 0.1f;  // near clipping plane
float far = 40.0f; // far clipping plane
float fov = 75.0f;

// Initialize Data Object
InitializeData *v;

// Shader programs
GLuint shader_program[4];
int currentProgram = 0;
vector<string> fsprograms = {"fs.glsl"};
unsigned int view_mat_location[4], proj_mat_location[4], model_mat_location[4], orig_model_mat_location[4];

// VBO data objects
unsigned int vbo_vertex, vbo_color, vbo_normal, vbo_tex, vao;

static void checkError(GLint status, const char *msg)
{
    if (!status)
    {
        printf("%s\n", msg);
        exit(EXIT_FAILURE);
    }
}

void shutDown(int returnCode) {
    printf("There was an error in running the code with error %d\n",returnCode);
    GLenum res = glGetError();
    const GLubyte *errString = gluErrorString(res);
    printf("Error is %s\n", errString);
    glfwTerminate();
    exit(returnCode);
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
    }
    return data;
}

void fillPoints(InitializeData *v)
{
    points.clear();
    color.clear();
    vnormals.clear();

    for(int i = 0;i < (int)v->Triangles.size();i++)
    {
        if (v->Triangles[i].isVisible)
        {
            // 1st vertex
            points.push_back(v->Triangles[i].first->x);
            points.push_back(v->Triangles[i].first->y);
            points.push_back(v->Triangles[i].first->z);

            color.push_back(v->Triangles[i].first->r);
            color.push_back(v->Triangles[i].first->g);
            color.push_back(v->Triangles[i].first->b);

           vnormals.push_back(v->Triangles[i].fnormal.x);
           vnormals.push_back(v->Triangles[i].fnormal.y);
           vnormals.push_back(v->Triangles[i].fnormal.z);

            // 2nd vertex
            points.push_back(v->Triangles[i].second->x);
            points.push_back(v->Triangles[i].second->y);
            points.push_back(v->Triangles[i].second->z);

            color.push_back(v->Triangles[i].second->r);
            color.push_back(v->Triangles[i].second->g);
            color.push_back(v->Triangles[i].second->b);

            vnormals.push_back(v->Triangles[i].fnormal.x);
            vnormals.push_back(v->Triangles[i].fnormal.y);
            vnormals.push_back(v->Triangles[i].fnormal.z);


            // 3rd vertex
            points.push_back(v->Triangles[i].third->x);
            points.push_back(v->Triangles[i].third->y);
            points.push_back(v->Triangles[i].third->z);

            color.push_back(v->Triangles[i].third->r);
            color.push_back(v->Triangles[i].third->g);
            color.push_back(v->Triangles[i].third->b);

            vnormals.push_back(v->Triangles[i].fnormal.x);
            vnormals.push_back(v->Triangles[i].fnormal.y);
            vnormals.push_back(v->Triangles[i].fnormal.z);


        }
    }
}

void fillColor() {

    color.clear();  
    for(int i = 0;i < (int)v->Triangles.size();i++)
    {
        if (v->Triangles[i].isVisible)
        {
            // 1st vertex
            color.push_back(v->Triangles[i].first->r);
            color.push_back(v->Triangles[i].first->g);
            color.push_back(v->Triangles[i].first->b);
            // 2nd vertex
            color.push_back(v->Triangles[i].second->r);
            color.push_back(v->Triangles[i].second->g);
            color.push_back(v->Triangles[i].second->b);
            // 3rd vertex
            color.push_back(v->Triangles[i].third->r);
            color.push_back(v->Triangles[i].third->g);
            color.push_back(v->Triangles[i].third->b);
        }
    }
}


void buildView() {
    view_mat = glm::lookAt(cam_pos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}

void buildProj() {
    float aspect = (float)window_width / (float)window_height; // aspect ratio
    proj_mat = glm::perspective(fov, aspect, near, far);
}

void buildModel() {
    float new_position = 0.0f;
    model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(new_position, 0.0, 0.0));
}

void useShaderProgram() {
	glUseProgram(shader_program[currentProgram]);

    glUniformMatrix4fv (view_mat_location[currentProgram], 1, GL_FALSE, &view_mat[0][0]);
    glUniformMatrix4fv (proj_mat_location[currentProgram], 1, GL_FALSE, &proj_mat[0][0]);
    glUniformMatrix4fv (model_mat_location[currentProgram], 1, GL_FALSE, &model_mat[0][0]);
    glUniformMatrix4fv (orig_model_mat_location[currentProgram], 1, GL_FALSE, &model_mat[0][0]);
}

void createShaderPrograms() {
	std::string vertex_shader = textFileRead ("vs.glsl");
	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
	const char* str = vertex_shader.c_str();
    glShaderSource (vs, 1, &str, NULL);
    glCompileShader (vs);
    int params = -1;
    glGetShaderiv(vs,GL_COMPILE_STATUS,&params);
    checkError(params,"Vertex Shader not compiled");

    for(int i = 0; i < (int)fsprograms.size(); i++) {
    	shader_program[i] = glCreateProgram();
	    
	    std::string fragment_shader = textFileRead (fsprograms[i].c_str());
	    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
	    const char* strb = fragment_shader.c_str();
	    glShaderSource (fs, 1, &strb, NULL);
	    glCompileShader (fs);
	    params = -1;
	    glGetShaderiv(fs,GL_COMPILE_STATUS,&params);
	    checkError(params,"Fragment Shader not compiled");

	    glAttachShader(shader_program[i],vs);
	    glAttachShader(shader_program[i],fs);
	    glLinkProgram (shader_program[i]);

    	glUseProgram(shader_program[i]);
    	view_mat_location[i] = glGetUniformLocation (shader_program[currentProgram], "view");
    	proj_mat_location[i] = glGetUniformLocation (shader_program[currentProgram], "proj");
    	model_mat_location[i] = glGetUniformLocation (shader_program[currentProgram], "model");
    	orig_model_mat_location[i] = glGetUniformLocation (shader_program[currentProgram], "orig_model_mat");
	}
}

void updateCameraPosKeyboard() {
    static bool tabEventActive = false;
    static bool leftEventActive = false;
    static bool rightEventActive = false;
    static bool spaceEventActive = false;
    static bool homeEventActive = false;
    static bool shiftEventActive = false;

    if (glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS) {
        cam_pos.z += camera_speed;
    }
    if (glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS) {
        cam_pos.z -= camera_speed;
    }

    if (glfwGetKey(GLFW_KEY_TAB) == GLFW_PRESS)
    {
        tabEventActive = true;
    }
    else {
        if (tabEventActive) {
            if (draw_mode == GL_TRIANGLES) {
                draw_mode = GL_LINE_STRIP;
            }
            else {
                draw_mode = GL_TRIANGLES;
            }
            tabEventActive = false;
        }
    }

    if(glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
        rightEventActive = true;
    }
    else {
        if (rightEventActive) {
            v->changeFile(true);
            fillPoints(v);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_vertex);
            glBufferData (GL_ARRAY_BUFFER, points.size() * sizeof (GLfloat), &points[0], GL_DYNAMIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_color);
            glBufferData (GL_ARRAY_BUFFER, color.size() * sizeof (GLfloat), &color[0], GL_DYNAMIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_normal);
            glBufferData (GL_ARRAY_BUFFER, vnormals.size() * sizeof (GLfloat), &vnormals[0], GL_DYNAMIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_tex);
            glBufferData (GL_ARRAY_BUFFER, texCordinates.size() * sizeof (GLfloat), &texCordinates[0], GL_DYNAMIC_DRAW);

            rightEventActive = false;
        }
    }

    if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
        leftEventActive = true;
    }
    else {
        if (leftEventActive) {
            v->changeFile(false);
            fillPoints(v);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_vertex);
            glBufferData (GL_ARRAY_BUFFER, points.size() * sizeof (GLfloat), &points[0], GL_DYNAMIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_color);
            glBufferData (GL_ARRAY_BUFFER, color.size() * sizeof (GLfloat), &color[0], GL_DYNAMIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_normal);
            glBufferData (GL_ARRAY_BUFFER, vnormals.size() * sizeof (GLfloat), &vnormals[0], GL_DYNAMIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_tex);
            glBufferData (GL_ARRAY_BUFFER, texCordinates.size() * sizeof (GLfloat), &texCordinates[0], GL_DYNAMIC_DRAW);


            leftEventActive = false;
        }
    }
    if (glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
        spaceEventActive = true;
    }
    else  {
        if(spaceEventActive) {
            v->changeColorModel();
            fillColor();
            glBindBuffer (GL_ARRAY_BUFFER, vbo_color);
            glBufferData (GL_ARRAY_BUFFER, color.size() * sizeof (GLfloat), &color[0], GL_DYNAMIC_DRAW);
            spaceEventActive = false;            
        }
    }

    if (glfwGetKey(GLFW_KEY_HOME) == GLFW_PRESS) {
        homeEventActive = true;
    }
    else  {
        if(homeEventActive) {
            v->changeSides();
            fillPoints(v);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_vertex);
            glBufferData (GL_ARRAY_BUFFER, points.size() * sizeof (GLfloat), &points[0], GL_DYNAMIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_color);
            glBufferData (GL_ARRAY_BUFFER, color.size() * sizeof (GLfloat), &color[0], GL_DYNAMIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_normal);
            glBufferData (GL_ARRAY_BUFFER, vnormals.size() * sizeof (GLfloat), &vnormals[0], GL_DYNAMIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_tex);
            glBufferData (GL_ARRAY_BUFFER, texCordinates.size() * sizeof (GLfloat), &texCordinates[0], GL_DYNAMIC_DRAW);


            homeEventActive = false;
        }
    }

    if (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS)
    {
        shiftEventActive = true;
    }
    else {
        if (shiftEventActive) {
        	currentProgram = (currentProgram+1)%4;
        	useShaderProgram();
            shiftEventActive = false;
        }
    }
}

void GLFWCALL updateCameraPosMouse(int x, int y) {
	static bool pressed = false;
    static glm::vec3 V2(0.0, 0.0, 0.0);
    glm::vec3 V1((double)x, (double)y, 0.0);
    V1[0] = V1[0]/((double) window_width/2.0) - 1.0;
    V1[1] = 1.0 - V1[1]/((double) window_height/2.0);
    if (pow(V1[0], 2) + pow(V1[0], 2) < 0.5) {
        V1[2] = sqrt(1.0 - pow(V1[0], 2) + pow(V1[1], 2));
    }
    else {
        V1[2] = 0.5/sqrt(pow(V1[0], 2) + pow(V1[1], 2));
    }

	bool performRotation = true;
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
		pressed = false;
        V2 = V1;
		return;
	}
    else if(!pressed){
		performRotation = false;
		pressed = true;
	}

	if (performRotation){
		V1 = glm::normalize(V1);
		V2 = glm::normalize(V2);
        glm::vec3 N = glm::cross(V2, V1);
        float angle = acos(min(1.0f, glm::dot(V2, V1)));
        if (isnan(angle)) {
			return;
		}
        model_mat = glm::rotate(model_mat, (float)(angle*180.0/M_PI), N);
	}

    V2 = V1;
}


void _update_fps_counter () {
    // timer/frame counter
    static double previous_seconds = glfwGetTime ();
    static int frame_count;
    double current_seconds = glfwGetTime ();
    double elapsed_seconds = current_seconds - previous_seconds;
    if (elapsed_seconds > 0.25) {
        previous_seconds = current_seconds;
        double fps = (double)frame_count / elapsed_seconds;
        char tmp[128];
        sprintf (tmp, "Voxel Viewer @ fps: %.2lf", fps);
        glfwSetWindowTitle (tmp);
        frame_count = 0;
    }
    frame_count++;
}

// Callback function to handle resize event
/* Callback function section */
void GLFWCALL resize(int width, int height) {
    float near = 0.1f; // clipping plane
    float far = 100.0f; // clipping plane
    float fov = 45.0f;
    float aspect = (float)width / (float)height; // aspect ratio

    proj_mat = glm::perspective(fov, aspect, near, far);

    glViewport(0, 0, width, height);

	window_width = width;
	window_height = height;
}

void readHeightMap(string filename) {
    int width = 256;
    int height = 256;
    heightMap.resize(width, vector<GLfloat>(height));

    ifstream infile(filename);
    if (infile.is_open()) {
        string line;
        for(int i = 0; i < width; i++) {
            getline(infile, line);
            stringstream ss(line);
            for(int j = 0; j < height; j++) {
                int h;
                ss>>h;
                heightMap[i][j] = (GLfloat)h/75.0;
            }
        }
    }
    else {
        cout<<"Could not open the file"<<endl;
    }
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

int main(int argc, char *argv[]) {
     if (!(argc >= 2)) {
        cerr << "The number of arguments is not appropriate - Terminating" << endl;
        return 1;
    }

    vector<string> a;
    for(int i=1;i<argc;i++)
        a.push_back(argv[i]);

    //fill in the data
/*    v = new InitializeData();
    v->initializeData(a);

    fillPoints(v);*/

    //string inputfile = "subdividedsubdivided_withnormal1.obj";
   // string inputfile = "cubeVoronoiAtlas.obj";
   // string inputfile = "cylindervoronoiatlas.obj";
   // string inputfile = "voronoiatlas.obj";
   //---------------------------------------------------
   //string inputfile = "teddyfinalwithfnormals.obj";
   //string inputfile = "teddyflatplane.obj";
   //string inputfile = "teddyaftervoroatlas.obj";
   //string inputfile = "teddypertriangle.obj";
   //----------------------------------------------------
   //string inputfile = "cubebigwithfnormals.obj";
   //string inputfile = "smallcylindervnormal.obj";
    //string inputfile = "cylinderflatplane.obj";
    //string inputfile = "cubevnormals.obj";
    //string inputfile = "Combined.obj";
   // string inputfile = "nonSubdividedvnormal.obj";
    //string inputfile = "combined.obj";
    //string inputfile = "efficientML.obj";
    //string inputfile = "cylinderHalfML.obj";
    //string inputfile = "cylinderFP.obj";
    //string inputfile = "nonSubdividedFP.obj";
    //string inputfile = "subdiv000FP.obj";
    string inputfile = "combined.obj";
    
    vector<tinyobj::shape_t> shapes;
  
    string errr = tinyobj::LoadObj(shapes, inputfile.c_str());
	
	std::cout<<"Reached here"<<endl;
    if (!errr.empty()) {
      std::cerr << errr << std::endl;
      exit(1);
    }
    
    points.clear();
    color.clear();
    vnormals.clear();

    //readHeightMap("Data/mountain.txt");
    readHeightMapRGB("Data/terrain.txt");
    float maxR = 0.0;
    std::cout<<"Total number of shapes "<<shapes.size()<<std::endl;
    for(int k = 0; k < (int)shapes.size(); k++) {
		for(int i = 0; i < (int)shapes[k].mesh.indices.size()/3; i++) {
			for(int j = 0; j < 3; j++) {
				if (k != 2)
					continue;
				int idx = shapes[k].mesh.indices[3*i+j];
				points.push_back(shapes[k].mesh.positions[3*idx+0]);
				points.push_back(shapes[k].mesh.positions[3*idx+1]);
				points.push_back(shapes[k].mesh.positions[3*idx+2]);
				vnormals.push_back(shapes[k].mesh.normals[3*idx+0]);
				vnormals.push_back(shapes[k].mesh.normals[3*idx+1]);
				vnormals.push_back(shapes[k].mesh.normals[3*idx+2]);
				float a = shapes[k].mesh.texcoords[2*idx+0];
				float b = shapes[k].mesh.texcoords[2*idx+1];
				int x = min(h_width-1, (int)(shapes[k].mesh.texcoords[2*idx+0]*(h_width-1)));
				int y = min(h_height-1, (int)(shapes[k].mesh.texcoords[2*idx+1]*(h_height-1)));
				texCordinates.push_back(heightMap[x][y]);
		//		color.push_back(1.0*height_map[x][y].first/255.0);
			//	color.push_back(1.0*height_map[x][y].second.first/255.0);
				//color.push_back(1.0*height_map[x][y].second.second/255.0);
				//std::cout<<height_map[x][y].first<<endl;
				color.push_back(a);// helps to check the parameterisation
				color.push_back(b);
				color.push_back(0.0);
				float r = hypot(shapes[k].mesh.positions[3*idx+0], shapes[k].mesh.positions[3*idx+1]);
				maxR = max(maxR, r);
			}
		}
	}

  /*  double pi = 3.14159265;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,10);
    
    cout<<"Start Generating Texture Coordinates"<<endl;

    for(int i = 0; i < (int)points.size()/3; i++) {
        int idx = i;
        float r = hypot(points[3*idx+0], points[3*idx+1]);
        float theta = atan2(points[3*idx+1], points[3*idx+0]);
        r = min(r/maxR, 1.0f);
        theta = max(0.0, (theta+pi)/(2*pi));
        
        assert(r <= 1.0);
        assert(theta <= 1.0);
        
        int x = trunc(r*(h_width-1));
        int y = trunc(theta*(h_height-1));
        x = max(0, min(x, h_width-1));
        y = max(0, min(y, h_height-1));
        
        texCordinates.push_back(heightMap[x][y]);
        //texCordinates.push_back((float)distribution(generator)/50.0);
    }

    cout<<"Done Generating Texture Coordinates"<<endl;
*/
/*    color.resize(points.size(), 0);

    for(int i = 0; i < (int)points.size()/3; i++) { 
        color[i*3] = 1.0;
        color[i*3+1] = 0.0;
        color[i*3+2] = 0.0;
    }
*/

	// Texture Mapping of Rusted Surface
	/*
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data  = stbi_load("texture/skullvman.png", &x, &y, &n, force_channels);
	if (!image_data) {
		cerr<<"ERROR: could not load the texture file"<<endl;
	}
	
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		cerr<<"The image size is not power of two"<<endl;
	}
	
	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
	  top = image_data + row * width_in_bytes;
	  bottom = image_data + (y - row - 1) * width_in_bytes;
	  for (int col = 0; col < width_in_bytes; col++) {
		temp = *top;
		*top = *bottom;
		*bottom = temp;
		top++;
		bottom++;
	  }
	}
	
	unsigned int tex = 0;
	glGenTextures (1, &tex);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, tex);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	*/

    cout<<"Number of triangles to be rendered "<<points.size()/9<<" "<<shapes[0].mesh.indices.size()/3<<endl;

    // start GL context and O/S window using GLFW helper library
    if (glfwInit() != GL_TRUE)
        shutDown(1);
    if (glfwOpenWindow(window_width, window_height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW) != GL_TRUE)
        shutDown(2);
    // start GLEW extension handler
    glewInit();
    // register for resize callback
    glfwSetWindowSizeCallback(resize);
    glfwSetMousePosCallback(updateCameraPosMouse);

    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION);
    printf("Rendered by %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    vbo_vertex = 0;
    glGenBuffers (1, &vbo_vertex);
    glBindBuffer (GL_ARRAY_BUFFER, vbo_vertex);
    glBufferData (GL_ARRAY_BUFFER, points.size() * sizeof (GLfloat), &points[0], GL_DYNAMIC_DRAW);

    vbo_color = 0;
    glGenBuffers (1, &vbo_color);
    glBindBuffer (GL_ARRAY_BUFFER, vbo_color);
    glBufferData (GL_ARRAY_BUFFER, color.size() * sizeof (GLfloat), &color[0], GL_DYNAMIC_DRAW);

    vbo_normal = 0;
    glGenBuffers (1, &vbo_normal);
    glBindBuffer (GL_ARRAY_BUFFER, vbo_normal);
    glBufferData (GL_ARRAY_BUFFER, vnormals.size() * sizeof (GLfloat), &vnormals[0], GL_DYNAMIC_DRAW);

    vbo_tex = 0;
    glGenBuffers (1, &vbo_tex);
    glBindBuffer (GL_ARRAY_BUFFER, vbo_tex);
    glBufferData (GL_ARRAY_BUFFER, texCordinates.size() * sizeof (GLfloat), &texCordinates[0], GL_DYNAMIC_DRAW);    


    vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);

    glBindBuffer (GL_ARRAY_BUFFER, vbo_vertex);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*) NULL);
    glBindBuffer (GL_ARRAY_BUFFER, vbo_color);
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*) NULL);
    glBindBuffer (GL_ARRAY_BUFFER, vbo_normal);
    glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*) NULL);
    glBindBuffer (GL_ARRAY_BUFFER, vbo_tex);
    glVertexAttribPointer (3, 1, GL_FLOAT, GL_FALSE, 0, (GLubyte*) NULL);


    glEnableVertexAttribArray (0);
    glEnableVertexAttribArray (1);
    glEnableVertexAttribArray (2);
    glEnableVertexAttribArray (3);

    createShaderPrograms();
    useShaderProgram();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    buildView();
    buildProj();
    buildModel();
    bool running = true;

    draw_mode = GL_TRIANGLES;

    while (running) {
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         _update_fps_counter ();

        updateCameraPosKeyboard();

        buildView();
        glUniformMatrix4fv (view_mat_location[currentProgram], 1, GL_FALSE, &view_mat[0][0]);
        glUniformMatrix4fv (model_mat_location[currentProgram], 1, GL_FALSE, &model_mat[0][0]);

        glBindVertexArray (vao);
        glDrawArrays (draw_mode, 0, (GLint)points.size()/3);
        glfwSwapBuffers();
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam (GLFW_OPENED);
    }

    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}

//Assignment 02: Trandformations, viewing and projection

#include "utils.h"
#include <glm/gtx/string_cast.hpp>
#include <math.h>    


#define PI 3.14159265

int width = 640, height=640;

void createCubeObject(unsigned int &, unsigned int &);
void setupModelTransformation(unsigned int &);
void setupViewTransformation(unsigned int &);
void setupProjectionTransformation(unsigned int &, int, int);

int main(int, char**)
{
    // Setup window
    GLFWwindow *window = setupWindow(width, height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
    glUseProgram(shaderProgram);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    setupModelTransformation(shaderProgram);
    setupViewTransformation(shaderProgram);
    setupProjectionTransformation(shaderProgram, width , height);

    createCubeObject(shaderProgram, VAO);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glUseProgram(shaderProgram);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Information");                          
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO); 
        
        glDrawArrays(GL_TRIANGLES, 0, 6*2*3);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    // Cleanup
    cleanup(window);

    return 0;
}

void createCubeObject(unsigned int &program, unsigned int &cube_VAO)
{
    glUseProgram(program);

    //Bind shader variables
    int vVertex_attrib = glGetAttribLocation(program, "vVertex");
    if(vVertex_attrib == -1) {
        fprintf(stderr, "Could not bind location: vVertex\n");
        exit(0);
    }
    int vColor_attrib = glGetAttribLocation(program, "vColor");
    if(vColor_attrib == -1) {
        fprintf(stderr, "Could not bind location: vColor\n");
        exit(0);
    }

    //Cube data
    GLfloat cube_vertices[] = {10, 10, 10, -10, 10, 10, -10, -10, 10, 10, -10, 10, //Front
                   10, 10, -10, -10, 10, -10, -10, -10, -10, 10, -10, -10}; //Back
    GLushort cube_indices[] = {0, 2, 3, 0, 1, 2, //Front
                4, 7, 6, 4, 6, 5, //Back
                5, 2, 1, 5, 6, 2, //Left
                4, 3, 7, 4, 0, 3, //Right
                1, 0, 4, 1, 4, 5, //Top
                2, 7, 3, 2, 6, 7}; //Bottom
    GLfloat cube_colors[] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1}; //Unique face colors

    //Generate VAO object
    glGenVertexArrays(1, &cube_VAO);
    glBindVertexArray(cube_VAO);

    //Create VBOs for the VAO
    //Position information (data + format)
    int nVertices = 6*2*3; //(6 faces) * (2 triangles each) * (3 vertices each)
    GLfloat *expanded_vertices = new GLfloat[nVertices*3];
    for(int i=0; i<nVertices; i++) {
        expanded_vertices[i*3] = cube_vertices[cube_indices[i]*3];
        expanded_vertices[i*3 + 1] = cube_vertices[cube_indices[i]*3+1];
        expanded_vertices[i*3 + 2] = cube_vertices[cube_indices[i]*3+2];
    }
    GLuint vertex_VBO;
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vVertex_attrib);
    glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete []expanded_vertices;

    //Color - one for each face
    GLfloat *expanded_colors = new GLfloat[nVertices*3];
    for(int i=0; i<nVertices; i++) {
        int color_index = i / 6;
        expanded_colors[i*3] = cube_colors[color_index*3];
        expanded_colors[i*3+1] = cube_colors[color_index*3+1];
        expanded_colors[i*3+2] = cube_colors[color_index*3+2];
    }
    GLuint color_VBO;
    glGenBuffers(1, &color_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vColor_attrib);
    glVertexAttribPointer(vColor_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete []expanded_colors;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); //Unbind the VAO to disable changes outside this function.
}

void setupModelTransformation(unsigned int &program)
{
    //Modelling transformations (Model -> World coordinates)
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates
    
    //TODO: Q1 - Apply modelling transformations here.
    
    //Step 1 - Constructing a Basis Vector
    
    //Make the vector a unit vector
    glm::vec3 original_vec = glm::vec3(1.0, 2.0, 2.0);
    glm:: vec3 w = glm::normalize(original_vec);
   	

    //Choose any vector not collinear to w
	glm::vec3 t = glm::vec3(1.0, 3.0, 2.0);

	//Building u perpendicular to both
	glm::vec3 u = glm::cross(t, w);
	u = glm::normalize(u);
	
	//Building the final basis v
	glm::vec3 v = glm::cross(w, u); 
	v = glm::normalize(v);
	
	//Constructing updated vectors
	glm::vec4 updU = glm::vec4(u, 0.0);
	glm::vec4 updV = glm::vec4(v, 0.0);
	glm::vec4 updW = glm::vec4(w, 0.0);
	glm::vec4 temp = glm::vec4(0.0, 0.0, 0.0, 1.0);


	//Constructing rotation matrix from these vectors
	glm::mat4 rotMat = glm::mat4(updU,updV,updW,temp);

	//Constructing matrix for rotation along required axis (z) by 30 degrees
	double angle = 30;
	double cosAngle = cos ( angle * PI / 180.0 );
	double sinAngle = sin ( angle * PI / 180.0 );

	glm::mat4 rotMatZ = glm::mat4(cosAngle, sinAngle, 0.0, 0.0, -sinAngle, cosAngle, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	
	//Calculating the final matrix, matrices are column major
	model = rotMat*rotMatZ*(glm::transpose(rotMat));
	std::cout<<glm::to_string(model)<<std::endl;

	//OpenGL implementation
	model = glm::rotate(glm::mat4(1.0f), (float)(angle * PI / 180.0), w);//Model coordinates are the world coordinates
	// std::cout<<glm::to_string(model)<<std::endl;



    //TODO: Reset modelling transformations to Identity. Uncomment line below before attempting Q4!
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates

    //Pass on the modelling matrix to the vertex shader
    glUseProgram(program);
    int vModel_uniform = glGetUniformLocation(program, "vModel");
    if(vModel_uniform == -1){
        fprintf(stderr, "Could not bind location: vModel\n");
        exit(0);
    }
    glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(model));
}

//My version of openGL lookAt() function
glm::mat4 myLookAt(glm::vec3 &e, glm::vec3 &g, glm::vec3 &t){
	//calculating w, u, v

	glm::vec3 w = -glm::normalize(glm::vec3((g[0] - e[0]), (g[1] - e[1]), (g[2] - e[2])));	
	glm::vec3 u = glm::normalize(glm::cross(t, w));
	glm::vec3 v = glm::normalize(glm::cross(w, u));

	//Updating vectors
	glm::vec4 updU = glm::vec4(u, 0.0);
	glm::vec4 updV = glm::vec4(v, 0.0);
	glm::vec4 updW = glm::vec4(w, 0.0);
	glm::vec4 temp = glm::vec4(0.0, 0.0, 0.0, 1.0);
	
	//Assigning vectors to make matrix
	glm::vec4 idCol1 = glm::vec4(1.0, 0.0, 0.0, 0.0);
	glm::vec4 idCol2 = glm::vec4(0.0, 1.0, 0.0, 0.0);
	glm::vec4 idCol3 = glm::vec4(0.0, 0.0, 1.0, 0.0);
	glm::vec4 idCol4 = glm::vec4(-e[0], -e[1], -e[2], 1.0);
		
	
	//Form matrix
	glm::mat4 camMat1 = glm::mat4(updU, updV, updW, temp);
	glm::mat4 camMat2 = glm::mat4(idCol1, idCol2, idCol3, idCol4);
	glm::mat4 camMat = glm::transpose(camMat1)*camMat2;

	return camMat;

}


void setupViewTransformation(unsigned int &program)
{
    //Viewing transformations (World -> Camera coordinates
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 100.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    //Camera at (0, 0, 100) looking down the negative Z-axis in a right handed coordinate system

    //TODO: Q3 - Apply viewing transformations here.
    
    //Setting up e, g, t
	glm::vec3 e = glm::vec3(50.0, 100.0, 20.0);
	glm::vec3 g = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 t = glm::vec3(0.0, 0.0, 1.0);

	//Assigning
	view = myLookAt(e, g, t);

	//OpenGL implementation of lookAt
	//view = glm::lookAt(e,g,t);
	std::cout<<glm::to_string(view)<<std::endl;
	

    //TODO: Q4 - Apply appropriate viewing transformations here to generate 1-point, 2-point, and 3-point perspective views.
	
	g = glm::vec3(0.0, 0.0, 0.0);
	t = glm::vec3(0.0, 0.0, 1.0);

	//1 pointAssignment
	e = glm::vec3(0.0, 40.0, 0.0);

	//2 point
	e = glm::vec3(50.0, 50.0, 0.0);

	//3 point worm eye
	e = glm::vec3(30.0, 30.0, -10.0);

	//3 point bird eye
	e = glm::vec3(25.0, 25.0, 30.0);

	view = myLookAt(e, g, t);    
    
	//Resetting
    view = glm::lookAt(glm::vec3(0.0, 0.0, 100.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    //Pass-on the viewing matrix to the vertex shader
    glUseProgram(program);
    int vView_uniform = glGetUniformLocation(program, "vView");
    if(vView_uniform == -1){
        fprintf(stderr, "Could not bind location: vView\n");
        exit(0);
    }
    glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(view));
}

void setupProjectionTransformation(unsigned int &program, int screen_width, int screen_height)
{
    //Projection transformation
    float aspect = (float)screen_width/(float)screen_height;

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)screen_width/(GLfloat)screen_height, 0.1f, 1000.0f);

    //Pass on the projection matrix to the vertex shader
    glUseProgram(program);
    int vProjection_uniform = glGetUniformLocation(program, "vProjection");
    if(vProjection_uniform == -1){
        fprintf(stderr, "Could not bind location: vProjection\n");
        exit(0);
    }
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projection));
}

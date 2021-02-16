/*
 *	Code with copious notes following learnopengl.com
 */

#include <glad/glad.h>		// OpenGL library for providing driver functions for specific system, because OpenGL is a specification not a library.
							// Glad must be included before GLFW https://gamedev.stackexchange.com/questions/148453/getting-error-when-following-learnopengl-com-hello-window-tutorial-how-can-i
#include <GLFW/glfw3.h>		// OpenGL library for providing a simple API for creating windows, contexts and surfaces, receiving input and events.

#include <iostream>

/*
 * NOTES:
 * OpenGL is by itself a large state machine: a collection of variables that define how OpenGL should currently operate. 
 * The state of OpenGL is commonly referred to as the OpenGL context. When using OpenGL, we often change its state by setting
 * some options, manipulating some buffers and then render using the current context. Whenever we tell OpenGL that we now want
 * to draw lines instead of triangles for example, we change the state of OpenGL by changing some context variable that sets how
 * OpenGL should draw. As soon as we change the context by telling OpenGL it should draw lines, the next drawing commands will now
 * draw lines instead of triangles. When working in OpenGL we will come across several state-changing functions that change the context
 * and several state-using functions that perform some operations based on the current state of OpenGL. As long as you keep in mind that 
 * OpenGL is basically one large state machine, most of its functionality will make more sense.
 * 
 * The OpenGL libraries are written in C and allows for many derivations in other languages, but in its core it remains a C-library. 
 * Since many of C's language-constructs do not translate that well to other higher-level languages, OpenGL was developed with several abstractions 
 * in mind. One of those abstractions are objects in OpenGL. An object in OpenGL is a collection of options that represents a subset of OpenGL's state. 
 * For example, we could have an object that represents the settings of the drawing window; we could then set its size, how many colors it supports and 
 * so on. One could visualize an object as a C-like struct:
 * 
 * struct object_name {
 *		float  option1;
 *		int    option2;
 *		char[] name;
 * };
 * 
 * Whenever we want to use objects it generally looks something like this (with OpenGL's context visualized as a large struct):
 * 
 * // The State of OpenGL
 * struct OpenGL_Context {
 * 	...
 * 	object_name* object_Window_Target;
 * 	...  	
 * };
 * 
 * // create object
 * unsigned int objectId = 0;
 * glGenObject(1, &objectId);
 * // bind/assign object to context
 * glBindObject(GL_WINDOW_TARGET, objectId);
 * // set options of object currently bound to GL_WINDOW_TARGET
 * glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH,  800);
 * glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);
 * // set context target back to default
 * glBindObject(GL_WINDOW_TARGET, 0);
 * 
 * This little piece of code is a workflow you'll frequently see when working with OpenGL. We first create an object and store a reference to it as an 
 * id (the real object's data is stored behind the scenes). Then we bind the object (using its id) to the target location of the context (the location 
 * of the example window object target is defined as GL_WINDOW_TARGET). Next we set the window options and finally we un-bind the object by setting the 
 * current object id of the window target to 0. The options we set are stored in the object referenced by objectId and restored as soon as we bind the 
 * object back to GL_WINDOW_TARGET. 
 * 
 * 
 * A shader is just a small program run on the GPU to process information to be used for graphics
 * 
 */

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  // callback function used to resize viewport when window is resized
void processInput(GLFWwindow* window); // used to process input

// basic vertex shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// basic fragment shader
const char* fragmentShaderSource =  "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

int main()
{
	glfwInit(); // Initialises GLFW library

	// configure GLFW for OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // don't use backward compatible features
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // only for mac


	// create window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// register viewport resize callback function on window. When the window is first displayed the callback function is called
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Initialise glad with required function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// SETUP
	// graphics pipeline

	// vertex shader (process 3D data, typically transforms it into normalised device coordinates)
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);			// generate vertex shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // attach vertex shader source code to shader object
	glCompileShader(vertexShader);								// compile vertex shader

	// check for any issues with compilation of shader
	int  success;												// store state
	char infoLog[512];											// storage container for error messages
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) // if error with compilation of shader
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);	// get error message
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl; // output error message
	}

	// fragment shader (colours the pixels after they have been rasterised)
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);				// generate fragment shader object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);		// attach fragment shader source code to shader object
	glCompileShader(fragmentShader);									// complies fragment shader

	// check for any issues with compilation of shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) // if error with compilation of shader
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);	// get error message
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl; // output error message
	}

	// link compiled shaders to a shader program that is activated when rendering objects
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();				// generate shader program object
	glAttachShader(shaderProgram, vertexShader);	// attached compiled vertex shader
	glAttachShader(shaderProgram, fragmentShader);	// attach compiled fragment shader
	glLinkProgram(shaderProgram);					// link shader program together
	glUseProgram(shaderProgram);					// activate the shader program
													// Every shader and rendering call after glUseProgram will now use this program object (and thus the shaders). 

	// delete shader programs now that they have been copied and linked in the shader program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// check for any issues with the shader program
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);	// get error message
		std::cout << "ERROR::SHADER::PROGRAM:: Linking failed\n" << infoLog << std::endl; // output error message
	}
	

	// Initialise TRIANGLE object
	// vertex data, current defined within normalized device coordinates, -1.0 and 1.0 on all 3 axes (x, y and z)
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};


	// create memory on the GPU to store vertex information
	// memory managed by vertex buffer objects (VBO), batch send information from CPU to GPU slow, want to send as much data as possible at once
	// OpenGL object...
	unsigned int VBO;
	glGenBuffers(1, &VBO);	// generate buffer id

	/* PROVIDED AS INFORMATION
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind that buffer object by its id to the GL_ARRAY_BUFFER type target
										// all calls to GL_ARRAY_BUFFER affect the currently bound buffer VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// copies vertex data to the buffer object
																				// GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
																				// GL_STATIC_DRAW: the data is set only once and used many times.
																				// GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	// OpenGL does not yet know how it should interpret the vertex data in memory and how it should connect the vertex data to the vertex shader's attributes
	// Linking vertex attributes
	
	 * Our vertex buffer data is formatted as follows:
	 *	*Vertex attribte pointer setup of OpenGL VBO
	 *	*The position data is stored as 32-bit (4 byte) floating point values.
	 *	*Each position is composed of 3 of those values.
	 *	*There is no space (or other values) between each set of 3 values. The values are tightly packed in the array.
	 *	*The first value in the data is at the beginning of the buffer.
	 
	 // tell OpenGL how to interpret our vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // for more information https://learnopengl.com/Getting-started/Hello-Triangle
	glEnableVertexAttribArray(0);	//enable the bound vertex attribute, disabled by default
	// Each vertex attribute takes its data from memory managed by a VBO and which VBO it takes its data from (you can have multiple VBOs) 
	// is determined by the VBO currently bound to GL_ARRAY_BUFFER when calling glVertexAttribPointer. Since the previously defined VBO is 
	// still bound before calling glVertexAttribPointer vertex attribute 0 is now associated with its vertex data. 

	// Issue that when ever a object needs to be rendered vertex attributes need to be set, solution VAO
	// A vertex array object (also known as VAO) can be bound just like a vertex buffer object and any subsequent vertex attribute calls 
	// from that point on will be stored inside the VAO. This has the advantage that when configuring vertex attribute pointers you only 
	// have to make those calls once and whenever we want to draw the object, we can just bind the corresponding VAO. This makes switching 
	// between different vertex data and attribute configurations as easy as binding a different VAO. All the state we just set is stored inside the VAO.
	// OpenGL core **requires** a VAO

	// A vertex array object stores the following:
	//	*Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
	//	*Vertex attribute configurations via glVertexAttribPointer.
	//	*Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.
	
	*/

	// typical -> VAO -> VBO -> vertex data -> define/enable vertex attributes 

	// VAO initialisation code
	unsigned int VAO;
	glGenVertexArrays(1, &VAO); // generate voa
	glBindVertexArray(VAO); // bind voa
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		// bind and copy vbo
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);													// set vertex attributes
	
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);	// unbind buffer
	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so 
	//afterwards we can safely unbind
	glBindVertexArray(0);				//unbind vertex array

	// of note can also set element buffer object (EBO) to define incides to draw a combination of object from the same vertices
	// look up if required

	// render loop, keep running until told to stop, keeps window open
	// each iteration of the render loop is a "frame"
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);		// process input (keyboard, mouse, etc)

		// rendering commands here

		// start of frame you want to clear the screen previous rendering would still be visable
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);		// state setting function, colour blueish green
		glClear(GL_COLOR_BUFFER_BIT);				// state using function
													// clear entire framebuffer	of the current framebuffer, GL_COLOR_BUFFER_BIT clear to color as specificed in glClearColor
													// possible GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT

		// draw triangle
		glUseProgram(shaderProgram);		// set active shader program
		glBindVertexArray(VAO);				// bind active vao (VBO and Vertex attributes)
		glDrawArrays(GL_TRIANGLES, 0, 3);	// draw!


		// check and call events and swap the buffers
		glfwSwapBuffers(window);	// swap the color buffer (a large 2D buffer that contains color values for each pixel in GLFW's window) that
									// is used to render to during this render iteration and show it as output to the screen/
									// This is because a double buffer is being used, one that should be drawn on screen (front) and one for 
									// rendering (back), then back buffer is swaped to the front when it is done to prevent artifacts (flickering) while rendering
		glfwPollEvents();			// checks if any events are triggered (like keyboard input or mouse movement events), updates the window state, 
									// and calls the corresponding functions (which we can register via callback methods)
	}

	glfwTerminate(); // clean up any GLFW resources before terminating. Good practice
	return 0; // successful run
}

// callback function used to resize viewport when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// set opengl viewport size, for now same as GLFW window, but could be smaller to have other elements
	glViewport(0, 0, width, height);
	/*
	Behind the scenes OpenGL uses the data specified via glViewport to transform the 2D coordinates it processed to coordinates on your screen.
	For example, a processed point of location (-0.5,0.5) would (as its final transformation) be mapped to (200,450) in screen coordinates.
	Note that processed coordinates in OpenGL are between -1 and 1 so we effectively map from the range (-1 to 1) to (0, 800) and (0, 600).
	*/
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	// set state of GLFW window to close if 'escape' key is pressed. if it's not pressed, glfwGetKey returns GLFW_RELEASE
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}
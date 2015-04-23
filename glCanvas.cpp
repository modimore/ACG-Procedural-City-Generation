#include "glCanvas.h"
#include "grid.h"
#include "argparser.h"
#include "camera.h"

// ========================================================
// static variables of GLCanvas class

ArgParser* GLCanvas::args = NULL;
Grid* GLCanvas::grid = NULL;
Camera* GLCanvas::camera = NULL;
BoundingBox GLCanvas::bbox;
GLFWwindow* GLCanvas::window = NULL;

// mouse position
int GLCanvas::mouseX = 0;
int GLCanvas::mouseY = 0;
// which mouse button
bool GLCanvas::leftMousePressed = false;
bool GLCanvas::middleMousePressed = false;
bool GLCanvas::rightMousePressed = false;
// current state of modifier keys
bool GLCanvas::shiftKeyPressed = false;
bool GLCanvas::controlKeyPressed = false;
bool GLCanvas::altKeyPressed = false;
bool GLCanvas::superKeyPressed = false;

GLuint GLCanvas::simulation_VAO;

GLuint GLCanvas::ViewMatrixID;
GLuint GLCanvas::ModelMatrixID;
GLuint GLCanvas::LightID;
GLuint GLCanvas::MatrixID;
GLuint GLCanvas::programID;
GLuint GLCanvas::wireframeID;
GLuint GLCanvas::colormodeID;

// ========================================================
// Initialize all appropriate OpenGL variables, set
// callback functions, and start the main event loop.
// This function will not return but can be terminated
// by calling 'exit(0)'
// ========================================================

void GLCanvas::initialize(ArgParser *_args) {

  args = _args;
  grid = NULL;
  
  glfwSetErrorCallback(error_callback);
  
  // Initialize GLFW
  if( !glfwInit() ) {
    std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
    exit(1);
  }
  
  // We will ask it to specifically open an OpenGL 3.2 context
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a GLFW window
  window = glfwCreateWindow(args->width,args->height, "OpenGL viewer", NULL, NULL);
  if (!window) {
    std::cerr << "ERROR: Failed to open GLFW window" << std::endl;
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  HandleGLError("in glcanvas first");

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    std::cerr << "ERROR: Failed to initialize GLEW" << std::endl;
    glfwTerminate();
    exit(1);
  }

  // there seems to be a "GL_INVALID_ENUM" error in glewInit that is a
  // know issue, but can safely be ignored
  HandleGLError("after glewInit()",true);

  std::cout << "-------------------------------------------------------" << std::endl;
  std::cout << "OpenGL Version: " << (char*)glGetString(GL_VERSION) << '\n';
  std::cout << "-------------------------------------------------------" << std::endl;

  // Initialize callback functions
  glfwSetCursorPosCallback(GLCanvas::window,GLCanvas::mousemotionCB);
  glfwSetMouseButtonCallback(GLCanvas::window,GLCanvas::mousebuttonCB);
  glfwSetKeyCallback(GLCanvas::window,GLCanvas::keyboardCB);

  programID = LoadShaders( args->path+"/simulation.vertexshader",
                           args->path+"/simulation.fragmentshader" );

  // Load grid and building models
  Load();

  bbox.initializeVBOs();
  GLCanvas::initializeVBOs();
  GLCanvas::setupVBOs();

  // ===========================
  // initial placement of camera 
  // look at an object scaled & positioned to just fit in the box (-1,-1,-1)->(1,1,1)
  glm::vec3 min, max;
  grid->getBoundingBox().Get(min,max);
  glm::vec3 camera_position = max + float(2.0)*glm::vec3(1.0,1.0,1.0);
  glm::vec3 point_of_interest = float(0.5) * (min + max);
  //glm::vec3 camera_position = glm::vec3(1,3,8);
  //glm::vec3 point_of_interest = glm::vec3(0,0,0);
  glm::vec3 up = glm::vec3(0,1,0);
  float angle = 20.0;
  camera = new PerspectiveCamera(camera_position, point_of_interest, up, angle);
  camera->glPlaceCamera(); 

  HandleGLError("finished glcanvas initialize");
}


void GLCanvas::Load(){
  delete grid;
  grid = new Grid(args);
}

void GLCanvas::initializeVBOs(){
  HandleGLError("enter initilizeVBOs()");
  glGenVertexArrays(1, &simulation_VAO);
  glBindVertexArray(simulation_VAO);


  GLCanvas::MatrixID = glGetUniformLocation(GLCanvas::programID, "MVP");
  GLCanvas::LightID = glGetUniformLocation(GLCanvas::programID, "LightPosition_worldspace");
  GLCanvas::ViewMatrixID = glGetUniformLocation(GLCanvas::programID, "V");
  GLCanvas::ModelMatrixID = glGetUniformLocation(GLCanvas::programID, "M");
  GLCanvas::wireframeID = glGetUniformLocation(GLCanvas::programID, "wireframe");
  GLCanvas::colormodeID = glGetUniformLocation(GLCanvas::programID, "colormode");


  grid->initializeVBOs();
  if ( args->bounding_box ) bbox.initializeVBOs();
  HandleGLError("leaving initilizeVBOs()");
}

void GLCanvas::setupVBOs(){
  bbox.Set(grid->getBoundingBox());
  if ( args->bounding_box ) bbox.setupVBOs();
  grid->setupVBOs();
  HandleGLError("leaving setupVBOs()");
}

void GLCanvas::drawVBOs(const glm::mat4 &ProjectionMatrix,const glm::mat4 &ViewMatrix,const glm::mat4 &ModelMatrix){

  HandleGLError("enter GlCanvas::drawVBOs()");

  // prepare data to send to the shaders
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  HandleGLError("mid1 GlCanvas::drawVBOs()");

  //glm::vec3 lightPos = glm::vec3(4,4,4);
  glm::vec3 lightPos = bbox.getMax() + glm::vec3(1.0,1.0,1.0);
  glUniform3f(GLCanvas::LightID, lightPos.x, lightPos.y, lightPos.z);
  HandleGLError("mid1 GlCanvas::drawVBOs()");
  glUniformMatrix4fv(GLCanvas::MatrixID, 1, GL_FALSE, &MVP[0][0]);
  HandleGLError("mid2 GlCanvas::drawVBOs()");
  glUniformMatrix4fv(GLCanvas::ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
  HandleGLError("mid3 GlCanvas::drawVBOs()");
  glUniformMatrix4fv(GLCanvas::ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
  HandleGLError("mid4 GlCanvas::drawVBOs()");
  glUniform1i(GLCanvas::colormodeID, 1);

  HandleGLError("mid5 GlCanvas::drawVBOs()");
  if (args->bounding_box) {
    bbox.drawVBOs();
  }
  grid->drawVBOs();
  HandleGLError("leaving GlCanvas::drawVBOs()");
}

void GLCanvas::cleanupVBOs(){
  if ( args->bounding_box ) bbox.cleanupVBOs();
  grid->cleanupVBOs();
}


// ========================================================
// Callback function for mouse click or release
// ========================================================

void GLCanvas::mousebuttonCB(GLFWwindow *window, int which_button, int action, int mods) {
  // store the current state of the mouse buttons
  if (which_button == GLFW_MOUSE_BUTTON_1) {
    if (action == GLFW_PRESS) {
      leftMousePressed = true;
    } else {
      assert (action == GLFW_RELEASE);
      leftMousePressed = false;
    }
  } else if (which_button == GLFW_MOUSE_BUTTON_2) {
    if (action == GLFW_PRESS) {
      rightMousePressed = true;
    } else {
      assert (action == GLFW_RELEASE);
      rightMousePressed = false;
    }
  } else if (which_button == GLFW_MOUSE_BUTTON_3) {
    if (action == GLFW_PRESS) {
      middleMousePressed = true;
    } else {
      assert (action == GLFW_RELEASE);
      middleMousePressed = false;
    }
  }
}	

// ========================================================
// Callback function for mouse drag
// ========================================================

void GLCanvas::mousemotionCB(GLFWwindow *window, double x, double y) {
  // camera controls that work well for a 3 button mouse
  if (!shiftKeyPressed && !controlKeyPressed && !altKeyPressed) {
    if (leftMousePressed) {
      camera->rotateCamera(mouseX-x,mouseY-y);
    } else if (middleMousePressed)  {
      camera->truckCamera(mouseX-x, y-mouseY);
    } else if (rightMousePressed) {
      camera->dollyCamera(mouseY-y);
    }
  }

  if (leftMousePressed || middleMousePressed || rightMousePressed) {
    if (shiftKeyPressed) {
      camera->zoomCamera(mouseY-y);
    }
    // allow reasonable control for a non-3 button mouse
    if (controlKeyPressed) {
      camera->truckCamera(mouseX-x, y-mouseY);    
    }
    if (altKeyPressed) {
      camera->dollyCamera(y-mouseY);    
    }
  }
  mouseX = x;
  mouseY = y;
}

// ========================================================
// Callback function for keyboard events
// ========================================================

void GLCanvas::keyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
 // store the modifier keys
  //shiftKeyPressed = (GLFW_MOD_SHIFT & mods);
  //controlKeyPressed = (GLFW_MOD_CONTROL & mods);
  //altKeyPressed = (GLFW_MOD_ALT & mods);
  //superKeyPressed = (GLFW_MOD_SUPER & mods);
  if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)
    shiftKeyPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
  if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
    controlKeyPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
  if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT)
    altKeyPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
  if (key == GLFW_KEY_LEFT_SUPER || key == GLFW_KEY_RIGHT_SUPER)
    superKeyPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);

  // non modifier key actions

  if (key == GLFW_KEY_ESCAPE || key == 'q' || key == 'Q') {
    glfwSetWindowShouldClose(GLCanvas::window, GL_TRUE);
  }

  // other normal ascii keys...
  if ( (action == GLFW_PRESS || action == GLFW_REPEAT) && key < 256) {
    switch (key) {
    case 'b':  case 'B':
      args->bounding_box = !args->bounding_box;
      break; 
    case 'r':  case 'R': 
      // reset system
      cleanupVBOs();
      Load();
      initializeVBOs();
      setupVBOs();
      break; 
    case 'q':  case 'Q':
      exit(0);
      break;
    default:
      std::cout << "UNKNOWN KEYBOARD INPUT  '" << (char)key << "'" << std::endl;
    }
    setupVBOs();
  }
}


// ========================================================
// Load the vertex & fragment shaders
// ========================================================

GLuint LoadShaders(const std::string &vertex_file_path,const std::string &fragment_file_path){

  std::cout << "load shaders" << std::endl;

  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  
  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path.c_str(), std::ios::in);
  if (VertexShaderStream.is_open()){
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  } else {
    std::cerr << "ERROR: cannot open " << vertex_file_path << std::endl;
    exit(0);
  }
  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path.c_str(), std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
      FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  } else {
    std::cerr << "ERROR: cannot open " << vertex_file_path << std::endl;
    exit(0);
  }
  
    GLint Result = GL_FALSE;
 
    // Compile Vertex Shader
    std::cout << "Compiling shader : " << vertex_file_path << std::endl;
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
 
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    if (Result != GL_TRUE) {  
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(VertexShaderID, 1024, &log_length, message);
        std::cout << "VERTEX ERROR " << message << std::endl;
        exit(1);
    }

    // Compile Fragment Shader
    std::cout << "Compiling shader : " << fragment_file_path << std::endl;
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
 
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    if (Result != GL_TRUE) {  
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(FragmentShaderID, 1024, &log_length, message);
        std::cout << "FRAGMENT ERROR " << message << std::endl;
        exit(1);
    }

    // Link the program
    std::cout << "Linking shader program" << std::endl;
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    if (Result != GL_TRUE) {  
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(ProgramID, 1024, &log_length, message);
        std::cout << "SHADER PROGRAM ERROR " << message << std::endl;
        exit(1);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}

// ========================================================
// Functions related to error handling
// ========================================================

void GLCanvas::error_callback(int error, const char* description) {
  std::cerr << "ERROR CALLBACK: " << description << std::endl;
}

std::string WhichGLError(GLenum &error) {
  switch (error) {
  case GL_NO_ERROR:
    return "NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  case GL_STACK_UNDERFLOW:
    return "GL_STACK_UNDERFLOW";
  case GL_STACK_OVERFLOW:
    return "GL_STACK_OVERFLOW";
  default:
    return "OTHER GL ERROR";
  }
}

int HandleGLError(const std::string &message, bool ignore) {
  GLenum error;
  int i = 0;
  while ((error = glGetError()) != GL_NO_ERROR) {
    if (!ignore) {
      if (message != "") {
	std::cerr << "[" << message << "] ";
      }
      std::cerr << "GL ERROR(" << i << ") " << WhichGLError(error) << std::endl;
    }
    i++;
  }
  if (i == 0) return 1;
  return 0;
}

// ========================================================
// ========================================================



#include <stdio.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "generic_types.h"
#include "shader.h"
#include "utl_assert.h"
#include "utl_glfw.h"

#define VERSION_MAJOR       3
#define VERSION_MINOR       3
#define TOGGLE_BLUE_ON      1.0f
#define TOGGGLE_BLUE_OFF    0.0f

/* procedures */
static void processInput(GLFWwindow *window);
static void key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void toggle_background_color();

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"    FragColor = vec4(1.1f, 1.5f, 0.2f, 1.0f);\n"
"}\0";

/*
 variables
 */
static float s_background_blue;

int main()
{
/*
 * Local variables
 */
int major, minor, revision;
unsigned int VBO;
unsigned int VAO;
// unsigned int EBO;
unsigned int vertex_shader;
unsigned int fragment_shader;
unsigned int shader_program;
int  success;
char infoLog[512];
int shdr_ret;
char * fname;
/*
 * Vertices for a triangle
 */
float tri_vertices[] = {
   -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f
};  

/*
 * Vertices for a rectangle
 */
// float rect_vertices[] = {
//      0.5f,  0.5f, 0.0f,  // top right
//      0.5f, -0.5f, 0.0f,  // bottom right
//     -0.5f, -0.5f, 0.0f,  // bottom left
//     -0.5f,  0.5f, 0.0f   // top left 
// };

// unsigned int rect_indices[] = {
//     0, 1, 3,   // first triangle
//     1, 2, 3    // second triangle
// };

/*
 * Validate the GLFW version. We should be using OpenGL 3.3
 */
glfwGetVersion(&major, &minor, &revision);
utl_assert((major = VERSION_MAJOR) && (minor = VERSION_MINOR), "Supported OpenGL version is 3.3, prefered revision is 8");

/*
 * Initialize GLFW and tell it that we want to use OpenGL version 3.3
 * using core profile functionality means that we do not care about
 * backwards compatibility with older versions of OpenGL. This is okay
 * because legacy OpenGL is not useful for us anyway.
 */
glfwInit();
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

/*
 * Set default background color blue value for key press
 * validation
 */
s_background_blue = TOGGLE_BLUE_ON;

/*
 * Create a window and then make the context current. This means that 
 * all future OpenGL commands will be issued to the context of this
 * window.
 */
GLFWwindow * window = glfwCreateWindow(800, 600, "Validate GLFW", NULL, NULL);
if (window == NULL)
    {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
    }
glfwMakeContextCurrent(window);

/*
 * Initialize the function pointers for OpenGL
 */
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
    printf("Failed to initialize GLAD\n");
    return -1;
    }  

/*
 * Set up the inital viewport and ensure that the integrity of the viewport
 * is maintained when the window is resized.
 */
glViewport(0, 0, 800, 600);
glfwSetFramebufferSizeCallback(window, utl_glfw_fb_sz_maintain_vp);  

printf("OpenGL version: %s\n", glGetString(GL_VERSION));

/*
 * Create a Vertex Array Object (VAO) and bind it
 */
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

/*
 * Bind the Vertex Buffer Object (VBO) first, then bind and set vertex
 * buffer(s), and then configure vertex attributes(s).
 */
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);  
glBufferData(GL_ARRAY_BUFFER, sizeof(tri_vertices), tri_vertices, GL_DYNAMIC_DRAW);

// glGenBuffers(1, &EBO);
// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rect_indices), rect_indices, GL_STATIC_DRAW);

/*
 * Create a vertex shader object and compile it using the source code
 */
vertex_shader = glCreateShader(GL_VERTEX_SHADER);

/*
 * Read in the shader source code from the file and then compile it
*/
fname = "../../shader/simple_vertex.shader";
shdr_ret = shader_compile(&vertex_shader, fname);
if (shdr_ret != 0)
    {
    printf("Failed to compile vertex shader\n");
    return -1;
    }


glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

if(!success)
    {
    glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

/*
 * Create a fragment shader object and compile it using the source code
 */
fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

glShaderSource(fragment_shader, 1, &fragmentShaderSource, NULL);
glCompileShader(fragment_shader);

glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

if(!success)
    {
    glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
    printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }
/*
 * Create a shader program object and link the vertex and fragment shaders
 */
shader_program = glCreateProgram();
glAttachShader(shader_program, vertex_shader);
glAttachShader(shader_program, fragment_shader);
glLinkProgram(shader_program);

glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
if(!success)
    {
    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

/*
 * Delete the shader objects once they have been linked into the shader program
 */
glDeleteShader(vertex_shader);
glDeleteShader(fragment_shader);

/*
 * Link the vertex attributes
 */
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);   

/*
 * Set up callbacks
 */
glfwSetKeyCallback(window, key_press_callback);

/*
 * Main render loop
 */
while(!glfwWindowShouldClose(window))
    {
    processInput(window);

    /*
     * Render commands go here
     */
    glClearColor(0.1f, 0.1f, s_background_blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    /*
     * Swap the front and back buffers and poll new events
     */
    glfwSwapBuffers(window);
    glfwPollEvents();    
    }

/*
 * Clean up resources
 */
glDeleteVertexArrays(1, &VAO);
glDeleteBuffers(1, &VBO);
glDeleteProgram(shader_program);

glfwTerminate();
return 0;

}

static void processInput(GLFWwindow *window)
{
/*
    * Close the window if the escape key is pressed
    */
if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
    glfwSetWindowShouldClose(window, TRUE);    
    }

}

/*
 * key_press_callback
 * Key press callback registered to test window. Used to validate
 * key press registrations.
 */
static void key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
/*
 Determine key pressed by the user
 Note: there is no debounce here
 */
switch (key)
    {
    case GLFW_KEY_PAGE_UP:
        toggle_background_color();
        break;

    default:
        break;
    }
}

/*
 * toggle_background_color
 * toggle the background color
 */
static void toggle_background_color()
{
if(s_background_blue == TOGGLE_BLUE_ON)
    {
    s_background_blue = TOGGGLE_BLUE_OFF;
    }
else
    {
    s_background_blue = TOGGLE_BLUE_ON;
    }
}
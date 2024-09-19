#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

static std::string ParseShader(const std::string& filePath )
{
    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1,
    };

    std::ifstream stream(filePath);

    std::string line;
    std::stringstream stringsStream[2];
    ShaderType type = ShaderType::NONE;

    while(getline(stream, line))
    { 
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else 
        {
            stringsStream[(int)type] << line << "\n";
        }
    }
    std::cout << stringsStream[(int)type].str() << std::endl;
    return stringsStream[(int)type].str();
}

static unsigned int CompileShader(unsigned int type, const std::string& source )
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    
    if (result == GL_FALSE)
    {   
        int lenght = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)malloc(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "FAIL - " << message << std::endl;
        
        //if shader do not compile, delete beforehand
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static int CreateShader(const std::string vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vertShader = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fragShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glDetachShader(program, vertShader);
    glDetachShader(program, fragShader);

    glValidateProgram(program);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

float points[8] = {
    0.5f, 0.5f,
    0.5f, -0.5f,
    -0.5f, -0.5f,
    -0.5f, 0.5f,
};

unsigned int indices[6] = {
    0, 1, 2,
    2, 3, 0, 
};

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "GLWE ERROR" << std::endl;
   
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    unsigned int buffer_ptr;
    glGenBuffers(1, &buffer_ptr);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_ptr);
    //6 xy pairs
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), points, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);

    unsigned int indexBuffer_ptr;
    glGenBuffers(1, &indexBuffer_ptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_ptr);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
   
    
    //insert this in a struct to receive a full "blend" of shaders
    std::string vertexShader = ParseShader("res/Shaders/VertexBasic.shader");
    std::string fragmentShader = ParseShader("res/Shaders/FragBasic.shader");
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    /* Loop until the user,closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        //glDrawArrays(GL_TRIANGLES,0, 4);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}
// mandelbrot.cpp  — OpenGL 3.3 Core, Ubuntu + GLEW/GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

GLuint compileShader(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << "Shader error\n" << log << '\n';
    }
    return s;
}

GLuint makeProgram(const char* vs, const char* fs)
{
    GLuint p = glCreateProgram();
    glAttachShader(p, compileShader(GL_VERTEX_SHADER,   vs));
    glAttachShader(p, compileShader(GL_FRAGMENT_SHADER, fs));
    glLinkProgram(p);
    GLint ok; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetProgramInfoLog(p, 512, nullptr, log);
        std::cerr << "Link error\n" << log << '\n';
    }
    return p;
}

const char* vsrc = R"(
#version 330 core
out vec2 uv;               // coordonnée normalisée (-1..1)
void main()
{
    // gl_VertexID: 0,1,2,3  →  positions (-1,-1) ( 1,-1) (-1, 1) ( 1, 1)
    vec2 pos = vec2( (gl_VertexID & 1) * 2.0 - 1.0,
                     (gl_VertexID & 2)       - 1.0 );
    uv = pos;
    gl_Position = vec4(pos, 0.0, 1.0);
}
)";

const char* fsrc = R"(
#version 330 core
in  vec2 uv;
out vec4 FragColor;

uniform vec2  uCenter;   // centre du zoom
uniform float uScale;    // échelle (zoom)
uniform int   uMaxIter;  // itérations max

void main()
{
    // coordonnées complexe c = x + i y
    vec2 c = uCenter + uv * uScale;
    vec2 z = vec2(0.0);
    int i;
    for(i = 0; i < uMaxIter; ++i)
    {
        // z = z² + c
        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
        if(dot(z,z) > 4.0) break;
    }
    float t = float(i) / float(uMaxIter);     // normalisation 0..1
    // coloration simple : dégradé RGB
    FragColor = vec4(t, t*t, t*t*t, 1.0);
}
)";

int main()
{
    if (!glfwInit()) { std::cerr << "GLFW init fail\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* w = glfwCreateWindow(800, 600, "Mandelbrot GPU", nullptr, nullptr);
    if (!w) { std::cerr << "Window fail\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(w);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "GLEW init fail\n"; return -1; }

    GLuint prog = makeProgram(vsrc, fsrc);
    GLint  locCenter  = glGetUniformLocation(prog, "uCenter");
    GLint  locScale   = glGetUniformLocation(prog, "uScale");
    GLint  locMaxIter = glGetUniformLocation(prog, "uMaxIter");

    // VAO requis par le Core Profile même s’il reste vide
    GLuint vao; glGenVertexArrays(1, &vao); glBindVertexArray(vao);

    while (!glfwWindowShouldClose(w))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);

        glUniform2f(locCenter, -0.5f, 0.0f);          // centre classique
        glUniform1f(locScale,  1.5f);                 // « zoom » (plus petit = zoomer)
        glUniform1i(locMaxIter, 200);                 // itérations

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

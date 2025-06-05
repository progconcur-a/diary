#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

GLuint compileShader(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){ char log[512]; glGetShaderInfoLog(s,512,nullptr,log); std::cerr<<"Shader error\n"<<log<<'\n'; }
    return s;
}

GLuint makeProgram(const char* vs, const char* fs)
{
    GLuint p = glCreateProgram();
    glAttachShader(p, compileShader(GL_VERTEX_SHADER  , vs));
    glAttachShader(p, compileShader(GL_FRAGMENT_SHADER, fs));
    glLinkProgram(p);
    GLint ok; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){ char log[512]; glGetProgramInfoLog(p,512,nullptr,log); std::cerr<<"Link error\n"<<log<<'\n'; }
    return p;
}

const char* vsrc = R"(
#version 330 core
out vec2 uv;
void main(){
    vec2 pos = vec2( (gl_VertexID & 1) * 2.0 - 1.0,
                      (gl_VertexID & 2)       - 1.0 );
    uv = pos;
    gl_Position = vec4(pos,0.0,1.0);
})";

const char* fsrc = R"(
#version 330 core
in  vec2 uv;
out vec4 FragColor;

uniform vec2  uCenter;   // centre complexe (référence)
uniform float uScale;    // zoom courant (taille du pixel complex)
uniform float uAspect;   // largeur/hauteur
uniform int   uMaxIter;  // iterations max (<= 6000 conseillé)

vec2 cmul(in vec2 a, in vec2 b) {
    return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

float mandelbrotPerturb(vec2 c, vec2 dc, int maxIter)
{
    vec2 z  = vec2(0.0);
    vec2 dz = vec2(0.0);
    for(int i = 0; i < maxIter; ++i)
    {
        dz = cmul(2.0*z + dz, dz) + dc;   // delta orbit
        z  = cmul(z, z) + c;              // reference orbit
        if(dot(dz, dz) > 4.0) return float(i);
    }
    return -1.0; // n'a pas divergé
}

void main()
{
    // position dans le plan complexe (corrigé ratio)
    vec2 p = vec2(uv.x * uAspect, uv.y);

    vec2 dc = p * uScale;                // delta C autour du centre
    float iter = mandelbrotPerturb(uCenter, dc, uMaxIter);

    // coloration inspirée d'Iñigo Quílez
    vec3 col = (iter < 0.0) ? vec3(0.0)
               : 0.5 + 0.5 * cos( pow(uScale, 0.22) * iter * 0.05 + vec3(3.0, 3.5, 4.0) );

    FragColor = vec4(col, 1.0);
})";

int main(){
    if(!glfwInit()){ std::cerr<<"GLFW init fail\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    const int W=800,H=600;
    GLFWwindow* win = glfwCreateWindow(W,H,"Mandelbrot GPU",nullptr,nullptr);
    if(!win){ std::cerr<<"Window fail\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);

    glewExperimental = GL_TRUE;
    if(glewInit()!=GLEW_OK){ std::cerr<<"GLEW init fail\n"; return -1; }

    GLuint prog = makeProgram(vsrc,fsrc);
    GLint locCenter  = glGetUniformLocation(prog,"uCenter");
    GLint locScale   = glGetUniformLocation(prog,"uScale");
    GLint locAspect  = glGetUniformLocation(prog,"uAspect");
    GLint locMaxIter = glGetUniformLocation(prog,"uMaxIter");

    GLuint vao; glGenVertexArrays(1,&vao); glBindVertexArray(vao);

    const float CENTER_X = -0.74364388703f;
    const float CENTER_Y =  0.13182590421f;
    const int   ITER     = 500;
    const float BASE     = 2.0f;

    while(!glfwWindowShouldClose(win)){
        float t = glfwGetTime();
        float scale = BASE * std::pow(0.7f, 6.0f*std::sin(t*3.0f)+5.0f); // zoom oscillant
        float aspect = float(W)/float(H);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(prog);
        glUniform2f(locCenter , CENTER_X, CENTER_Y);
        glUniform1f(locScale  , scale);
        glUniform1f(locAspect , aspect);
        glUniform1i(locMaxIter, ITER);

        glDrawArrays(GL_TRIANGLE_STRIP,0,4);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

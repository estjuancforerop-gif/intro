#include<iostream>
#include<vector>
#include<cmath>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

// ------------------------------------------------------------------
// Vertex Shader
// Recibe la posicion del vertice (ya en coordenadas NDC, -1 a 1)
// ------------------------------------------------------------------
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// ------------------------------------------------------------------
// Fragment Shader
// Usa un "uniform" de color (uColor) en vez de un color fijo, para
// poder reutilizar el mismo shader con distintos colores mas adelante.
// ------------------------------------------------------------------
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 uColor;\n"
"void main()\n"
"{\n"
"    FragColor = uColor;\n"
"}\n\0";

// Tamaño actual de la ventana (se actualiza en el callback de resize)
int windowWidth = 800;
int windowHeight = 800;

// Cada cuantos pixeles queremos una linea de la grilla.
// Al cambiar el tamaño de la ventana, como este valor es fijo en
// pixeles, la CANTIDAD de lineas que entran en pantalla cambia
// (se ve mas densa o mas dispersa), que es el efecto pedido.
const float gridSpacingPixels = 40.0f;

// Grosor (en pixeles) de cada linea de la grilla. Cada linea ya NO es
// un segmento (GL_LINES); es un rectangulo angosto armado con DOS
// TRIANGULOS. Visualmente se sigue viendo como una linea fina, pero
// por dentro son puros triangulos.
const float gridThicknessPixels = 2.0f;

// ------------------------------------------------------------------
// Agrega al vector los 6 vertices (2 triangulos) de un rectangulo
// definido por sus esquinas en NDC: (x0,y0) a (x1,y1).
// Orden: A,D,C  y  A,C,B  (dos triangulos que forman el quad)
// ------------------------------------------------------------------
void agregarQuad(std::vector<GLfloat>& out, float x0, float y0, float x1, float y1)
{
    // A --- B
    // |   / |
    // |  /  |
    // | /   |
    // D --- C
    float ax = x0, ay = y1; // A: arriba-izquierda
    float bx = x1, by = y1; // B: arriba-derecha
    float cx = x1, cy = y0; // C: abajo-derecha
    float dx = x0, dy = y0; // D: abajo-izquierda

    // Triangulo 1: A, D, C
    out.push_back(ax); out.push_back(ay); out.push_back(0.0f);
    out.push_back(dx); out.push_back(dy); out.push_back(0.0f);
    out.push_back(cx); out.push_back(cy); out.push_back(0.0f);

    // Triangulo 2: A, C, B
    out.push_back(ax); out.push_back(ay); out.push_back(0.0f);
    out.push_back(cx); out.push_back(cy); out.push_back(0.0f);
    out.push_back(bx); out.push_back(by); out.push_back(0.0f);
}

// ------------------------------------------------------------------
// Genera los vertices de la grilla como TRIANGULOS (dos por cada
// linea vertical y dos por cada linea horizontal), en funcion del
// tamaño actual de la ventana.
// ------------------------------------------------------------------
void generarGrilla(int width, int height, std::vector<GLfloat>& outVertices)
{
    outVertices.clear();

    // Numero de divisiones segun el ancho/alto actuales
    int columnas = static_cast<int>(width / gridSpacingPixels);
    int filas = static_cast<int>(height / gridSpacingPixels);

    // Medio grosor de linea, convertido a NDC en cada eje
    float halfThicknessX = gridThicknessPixels / (float)width;
    float halfThicknessY = gridThicknessPixels / (float)height;

    // Lineas verticales -> quads angostos y altos (recorren y=-1 a y=1)
    for (int i = 0; i <= columnas; i++)
    {
        float x = -1.0f + (2.0f * i * gridSpacingPixels) / width;
        agregarQuad(outVertices, x - halfThicknessX, -1.0f, x + halfThicknessX, 1.0f);
    }

    // Lineas horizontales -> quads angostos y anchos (recorren x=-1 a x=1)
    for (int j = 0; j <= filas; j++)
    {
        float y = -1.0f + (2.0f * j * gridSpacingPixels) / height;
        agregarQuad(outVertices, -1.0f, y - halfThicknessY, 1.0f, y + halfThicknessY);
    }
}

// ------------------------------------------------------------------
// Callback que GLFW llama automaticamente cada vez que la ventana
// cambia de tamaño (maximizar, arrastrar el borde, etc).
// Aqui es donde "reajustamos" el viewport y guardamos el nuevo
// ancho/alto para que la grilla se regenere en el loop principal.
// ------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    if (width == 0 || height == 0) return; // evita division por cero al minimizar
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

int main()
{
    glfwInit();//se inicializa GLFW
    //contexto para el programa
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//numero principal de la version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//numero secundario de la version
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//perfil de opengl

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Grilla", NULL, NULL);//creacion de la ventana

    if (window == NULL) //Muestra de error si la ventana falla al crearse
    {
        std::cout << "Fallo en la creacion de la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);//añade la ventana creada al contexto actual

    gladLoadGL();//Carga GLAD para que configure OpenGL

    glViewport(0, 0, windowWidth, windowHeight);//area visible inicial

    // Registramos el callback de resize: sin esto, GLFW jamas nos
    // avisaria que la ventana cambio de tamaño.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //crea el objeto Vertex Shader y obtiene la referencia
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    //crea el objeto Fragment Shader y obtiene la referencia
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //crea el objeto Shader program y obtiene la referencia
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //borra los shader objects que ya no sirven
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Ubicacion del uniform de color dentro del shader
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");

    // ----------------------------------------------------------------
    // NOTA: el codigo original dibujaba un triangulo fijo (vertices,
    // indices, EBO). Se deja comentado porque no corresponde al
    // pedido de la grilla.
    // ----------------------------------------------------------------
    /*
    GLfloat vertices[] =
    {
        -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,//esquina inferior izquierda
        0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,//esquina inferior derecha
        0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f,//esquina superior
        -0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,//interior izquierda
        0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,//interior derecha
        0.0f , -0.5f * float(sqrt(3)) / 3, 0.0f,//interior abajo
    };

    GLuint indices[] =
    {
        0, 3, 5,//triangulo abajo a la iquierda
        3, 2, 4,//triangulo abajo a la derecha
        5, 4, 1//triangulo de arriba
    };
    */

    // ----------------------------------------------------------------
    // VAO/VBO de la GRILLA. Se usa GL_DYNAMIC_DRAW porque los vertices
    // se van a reescribir cada vez que la ventana cambie de tamaño.
    // ----------------------------------------------------------------
    GLuint gridVAO, gridVBO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);

    std::vector<GLfloat> gridVertices;
    generarGrilla(windowWidth, windowHeight, gridVertices);

    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(GLfloat), gridVertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);//color del fondo

    // Guardamos el ancho/alto con el que se genero la grilla actual,
    // para saber cuando hace falta regenerarla.
    int lastWidth = windowWidth;
    int lastHeight = windowHeight;

    while (!glfwWindowShouldClose(window))
    {
        // Si la ventana cambio de tamaño desde el ultimo frame,
        // regeneramos los vertices de la grilla y los volvemos a
        // subir al VBO (esto es lo que hace que la grilla "se
        // modifique" al ampliar o achicar la ventana).
        if (windowWidth != lastWidth || windowHeight != lastHeight)
        {
            generarGrilla(windowWidth, windowHeight, gridVertices);

            glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
            glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(GLfloat), gridVertices.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            lastWidth = windowWidth;
            lastHeight = windowHeight;
        }

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // --- Dibuja la grilla (color gris tenue). Son triangulos,
        // pero al ser rectangulos angostos se ven como lineas finas ---
        glUniform4f(colorLoc, 0.4f, 0.4f, 0.45f, 1.0f);
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(gridVertices.size() / 3));

        glfwSwapBuffers(window);
        glfwPollEvents();//se encarga de todos los eventos de GLFW
    }

    //borra todos los objetos creados
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);//Borra la ventana antes de cerrar el programa
    glfwTerminate();
    return 0;
}

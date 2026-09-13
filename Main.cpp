#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

//codigo fuente del Vertex Shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//codigo fuente del Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";


int main() 
{
	glfwInit();//se inicializa GLFW
	//contexto para el programa
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//numero principal de la version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//numero secundario de la version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//perfil de opengl

	//coordenadas de los vertices



	GLFWwindow* window = glfwCreateWindow(800, 800, "Grilla", NULL, NULL);//creacion de la ventana (Ancho,Alto,Nombre,...,...)
	
	if (window == NULL) //Muestra de error si la ventana falla al crearse
	{
		std::cout << "Fallo en la creacion de la ventana" << std::endl;
		glfwTerminate();
			return -1;
	}

	glfwMakeContextCurrent(window);//añade la ventana creada al contexto actual

	gladLoadGL();//Carga GLAD para que configure OpenGL

		glViewport(0, 0, 800, 800);//especifica el area visible de OpenGl en la ventana
		//en este caso va de x=0, y=0 a x=800, y=800

		//crea el objeto Vertex Shader y obtiene la referencia
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		//Adjunta el Vertex Shader fuente al objeto Vertex Shader
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		//Compila El vertex shader para la maquina
		glCompileShader(vertexShader);

		//crea el objeto Fragment Shader y obtiene la referencia
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		//Adjunta el Fragment Shader fuente al objeto Fragment Shader
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		//Compila El fragment shader para la maquina
		glCompileShader(fragmentShader);

		//crea el objeto Shader program y obtiene la referencia
		GLuint shaderProgram = glCreateProgram();
		//Adjunta el Shader program fuente al objeto Shader program
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		//vincula todos los Shaders en el Shader Program
		glLinkProgram(shaderProgram);

		//borra los shader objects de vertices y fragmentos que no sirven ahora
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		//coordenadas de los vertices
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

		//Crea los contenedores de referencia de vertex array objects y vertex buffer object
		GLuint VAO, VBO, EBO;

		//genera el VAO y el VBO con un solo objeto cada uno
		glGenVertexArrays(1, &VAO);//!!!!!!!!!!!!!!GENERAR VAO siempre antes del VBO!!!!!!
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		//hace que el VAO sea el actual Vertex Array Object Haciendo el "binding"
		glBindVertexArray(VAO);

		//
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//introduce los vertices en el VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//configura el VErtex Atrribute para que OpenGL sepa como leer el VBO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		//Activa el Vertex Attribute para que OpenGL sepa usarlo
		glEnableVertexAttribArray(0);

		//deja ambos en 0 para que no se modifiquen accidentalmente el VAO y el VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);//color del fondo, (r,g,b,transparencia) float (#f) y de 0 a 1
		glClear(GL_COLOR_BUFFER_BIT);//limpia y asigna el nuevo color
		glfwSwapBuffers(window);//cambia el back buffer por el front buffer

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//le dice a OpenGL cual Shader Program queremos usar
		glUseProgram(shaderProgram);
		//
		glBindVertexArray(VAO);
		//dibuja el triangulo que es el primitivo que queremos
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);

		glfwPollEvents();//se encarga de todos los eventos de GLFW
	}

	//borra todos los objetos creados
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);//Borra la ventana antes de cerrar el programa
	glfwTerminate();
	return 0;
}
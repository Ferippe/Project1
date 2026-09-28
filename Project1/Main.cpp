#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<cstddef>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// Vertex shader: ahora recibe posicion Y color, y aplica la matriz MVP
// para poder rotar/mover/proyectar el objeto en 3D
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 vColor;\n"
"uniform mat4 uMVP;\n"
"void main()\n"
"{\n"
"   gl_Position = uMVP * vec4(aPos, 1.0);\n"
"   vColor = aColor;\n"
"}\0";

// Fragment shader: ya no tiene el color quemado, usa el color
// que le llega interpolado desde el vertex shader
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 vColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(vColor, 1.0);\n"
"}\n\0";

// Estructura de vertice: posicion (3 floats) + color (3 floats)
struct Vertex {
	GLfloat pos[3];
	GLfloat color[3]; // RGB
};

int main()
{
	//Inicializar GLFW
	glfwInit();

	//Le dice a GLFW que version de OpenGL estamos usando
	//En este caso es la 3.3 lol
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Le dice a GLFW que usamos el perfil CORE
	//Asi que solo tenemos las funciones modernas de OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Pedimos buffer de profundidad para que el 3D se vea bien
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	// 3 triangulos IDENTICOS en forma y escala, superpuestos en X/Y
	// pero separados en Z, uno detras del otro (como en fila hacia el fondo)
	const float zOffset = 0.6f; // separacion en profundidad entre triangulos
	Vertex vertices[] = {
		// Triangulo rojo (mas cerca de la camara)
		{ {-0.35f, -0.35f, zOffset}, {1.0f, 0.0f, 0.0f} },
		{ { 0.35f, -0.35f, zOffset}, {1.0f, 0.0f, 0.0f} },
		{ { 0.0f,   0.45f, zOffset}, {1.0f, 0.0f, 0.0f} },

		// Triangulo verde (en medio)
		{ {-0.35f, -0.35f, 0.0f}, {0.0f, 1.0f, 0.0f} },
		{ { 0.35f, -0.35f, 0.0f}, {0.0f, 1.0f, 0.0f} },
		{ { 0.0f,   0.45f, 0.0f}, {0.0f, 1.0f, 0.0f} },

		// Triangulo azul (mas lejos de la camara)
		{ {-0.35f, -0.35f, -zOffset}, {0.0f, 0.0f, 1.0f} },
		{ { 0.35f, -0.35f, -zOffset}, {0.0f, 0.0f, 1.0f} },
		{ { 0.0f,   0.45f, -zOffset}, {0.0f, 0.0f, 1.0f} },
	};

	//Crea la ventana con una resolucion de 800x800 y le pone un nombre
	GLFWwindow* window = glfwCreateWindow(800, 800, "Ventanita OpenGL", NULL, NULL);
	//Check de errores si la ventana no se pudo crear
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//introduce la ventana en el contexto de OpenGL
	glfwMakeContextCurrent(window);

	//Carga GLAD para que configure OpenGL
	gladLoadGL();

	//Especifica el viewport de OpenGL
	glViewport(0, 0, 800, 800);

	//Activa el test de profundidad para que el orden en 3D se calcule bien
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//crea el objeto del Vertex Shader, y obtiene su referencia
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//crea el objeto del Fragment Shader, y obtiene su referencia
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//Crea el objeto shader program y obtiene su referencia
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//Elimina los shader objects que ya no necesitamos
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Ubicacion del uniform de la matriz MVP (Model-View-Projection)
	GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");

	//Crea contenedores de referencia para el VAO y el VBO
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//vuelve al VAO el vertex array object actual
	glBindVertexArray(VAO);

	//unimos el VBO y le pasamos los vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Atributo de posicion (location = 0), usando offsetof para que
	//tome el campo pos de la estructura Vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glEnableVertexAttribArray(0);

	//Atributo de color (location = 1), tomando el campo color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	//Vincula el VBO y el VAO a 0 para que no se modifiquen accidentalmente
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Loop hasta que el usuario cierre la ventana
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Le dice a OpenGL que Shader Program queremos que use para renderizar
		glUseProgram(shaderProgram);

		// Matriz de proyeccion (perspectiva 3D)
		glm::mat4 projection = glm::perspective(glm::radians(60.0f), 800.0f / 800.0f, 0.1f, 100.0f);

		// Matriz de vista: camara fija mirando hacia el origen
		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		// Matriz de modelo: rota con el tiempo para que gire solo
		float t = (float)glfwGetTime();
		glm::mat4 model = glm::rotate(glm::mat4(1.0f), t * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Combina las tres matrices y la envia al shader
		glm::mat4 mvp = projection * view * model;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

		//Unimos el VAO asi OpenGL sabe que lo debe usar
		glBindVertexArray(VAO);
		//Dibuja los 3 triangulos (9 vertices en total)
		glDrawArrays(GL_TRIANGLES, 0, 9);

		glfwSwapBuffers(window);
		//Se encarga de los eventos de GLFW
		glfwPollEvents();
	}

	//Borra todos los objetos que hemos creado muejejeje
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
} 
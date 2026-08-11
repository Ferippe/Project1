#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

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

	GLfloat vertices[] = {
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Abajo izquierda
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Abajo derecha
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f // Arriba
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
	//En este caso, el viewport va a ser del tamano de la ventana
	//o sea de x=0, y=0, a x=800, y=800
	//pensemoslo como un rectangulo que va desde la esquina inferior izquierda (0,0) 
	// hasta la esquina superior derecha (800,800)
    glViewport(0, 0, 800, 800);


	//crea el objeto del Vertex Shader, y obtiene su referencia
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//Une el vertex shader source con el vertex shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//Compila el shader a codigo de maquina pq la maquina es imbecil y no entiende
	glCompileShader(vertexShader);

	//crea el objeto del Fragment Shader, y obtiene su referencia
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//Une el fragment shader source con el fragment shader object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//Compila el shader a codigo de maquina pq la maquina no sabe leer xd
	glCompileShader(fragmentShader);

	//Crea el objeto shader program y obtiene su referencia
	GLuint shaderProgram = glCreateProgram();
	//Une los shaders compilados con el shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	//Linkea/conecta todos los shaders juntos al shader program
	glLinkProgram(shaderProgram);

	//Elimina los shader objects que ya no necesitamos
	// pq ya estan linkeados al shader program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);




	//Crea contenedores de referencia para el array de vertices y el array del vertex buffer
	GLuint VAO, VBO;
	//Genera el VAO y el VBO con un solo objeto cada uno
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//vuelve al VAO el vertex array object actual
	glBindVertexArray(VAO);

	//unimos el VBO especificando que es un GL_ARRAY_BUFFER y le pasamos la referencia del VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//introduce los vertices en el VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Configura los atributos de los vertices para que OpenGL sepa como interpretar los datos del VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//Activa el atributo de los vertices asi OpenGL sabe como usarlos
	glEnableVertexAttribArray(0);

	//Vincula el VBO y el VAO a 0 para que no se modifiquen accidentalmente
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//especifica el color de fondo (r,g,b,a) y limpia el buffer de color
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// Intercambia los buffers de la ventana, mostrando el color de fondo
	glfwSwapBuffers(window);

	// Loop hasta que el usuario cierre la ventana
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//Le dice a OpenGL que Shader Program queremos que use para renderizar
		glUseProgram(shaderProgram);
		//Unimos el VAO asi OpenGL sabe que lo debe usar
		glBindVertexArray(VAO);
		//Ahora si, dibuja el triangulo usando los GL_TRIANGLES primitive
		// Desde el vertice 0 hasta el vertice 3 (3 vertices)
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
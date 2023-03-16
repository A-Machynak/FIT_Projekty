#include "gui.h"

#include "img_export.h"
#include <algorithm>

namespace Gui
{
	PixelWindow::PixelWindow(int width, int height, const std::string & windowName,
		int textureWidth, int textureHeight, bool openglOff)
		: _width(width), _height(height), _textureWidth(textureWidth), _textureHeight(textureHeight),
		_glOff(openglOff), _windowBytes(WindowData(_textureWidth, _textureHeight))
	{
		if (!_glOff) {
			InitOpengl(windowName);
		}
	}

	PixelWindow::~PixelWindow()
	{
		if (!_glOff) {
			glDeleteTextures(1, &_textureId);
			glDeleteProgram(_shaderProgram);
			glDeleteVertexArrays(1, &_dummyVao);
		}
	}

	void PixelWindow::InitOpengl(const std::string & windowName)
	{
		if (!glfwInit()) {
			std::cerr << "[GUI]: glfwInit error - couldn't be initialized\n";
			std::cerr << "[GUI]: Switching opengl off...\n";
			_glOff = false;
			return;
		}

		// vytvoření okna
		_window = glfwCreateWindow(_width, _height, windowName.c_str(), nullptr, nullptr);
		if (!_window) {
			std::cerr << "[GUI]: glfwCreateWindow error - couldn't create window\n";
			std::cerr << "[GUI]: Switching opengl off...\n";
			_glOff = false;
			return;
		}

		glfwMakeContextCurrent(_window);
		gladLoadGL();

		// callbacky
		glfwSetWindowUserPointer(_window, this);
		glfwSetScrollCallback(_window, ScrollCallback);
		glfwSetCursorPosCallback(_window, CursorPosCallback);
		glfwSetMouseButtonCallback(_window, ClickCallback);
		glfwSetKeyCallback(_window, KeyCallback);

		_viewport = { 0, 0, _width, _height };

		// inicializace textur, framebufferu a shaderu
		_initialized = InitOpenglResources();
	}

	bool PixelWindow::IsInitialized() const
	{
		return _initialized;
	}

	void PixelWindow::OutputToFile()
	{
		ExportPng(_outputFile, _textureWidth, _textureHeight, _windowBytes.Bytes);
		std::cout << "[GUI]: Screenshot saved to " << _outputFile << "\n";
	}

	void PixelWindow::RunWithoutOpengl()
	{
		do {
			_stepper->Step(_windowBytes);
		} while (!_stepper->EndCondition(_windowBytes));
		std::cout << "[GUI]: Simulation finished\n";
		OutputToFile();
	}

	bool PixelWindow::Run(IStepper * stepper)
	{
		_stepper = stepper;
		_stepper->Init(_windowBytes);

		if (_glOff) {
			RunWithoutOpengl();
			return true;
		}

		if (!_initialized) {
			return false;
		}
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		while (!glfwWindowShouldClose(_window)) {
			// vyčištění color bufferu
			glClear(GL_COLOR_BUFFER_BIT);
			glViewport(_viewport.X, _viewport.Y, _viewport.Width, _viewport.Height);

			// render
			RenderLoop();

			// přehození front/back bufferu
			glfwSwapBuffers(_window);

			// procesy a eventy
			glfwPollEvents();
		}

		glfwTerminate();
		return true;
	}

	void PixelWindow::RenderLoop()
	{
		_currentTime = glfwGetTime();
		if (_end || _pause) {
			_lastTime = _currentTime;
			glDrawArrays(GL_TRIANGLES, 0, 3);
			return;
		}

		bool shouldUpdate = false;
		double dt = _currentTime - _lastTime;
		_cumulativeTime += dt;
		while (_cumulativeTime >= _timestep) {
			_cumulativeTime -= _timestep;

			// voláme krokovou funkci pro každý časový krok
			// ideálně proběhne pouze jednou, ale pokud je časový krok příliš malý
			// a renderování je pozadu, tak je nutné jej vykonat několikrát
			_stepper->Step(_windowBytes);
			_stepCount++;
			shouldUpdate = true;

			if (_stepper->EndCondition(_windowBytes)) {
				_end = true;
				std::cout << "[GUI]: Simulation finished\n";
				if (_glOff) {
					OutputToFile();
					glfwSetWindowShouldClose(_window, 1);
				}
				break;
			}
		}

		_lastTime = _currentTime;

		if (shouldUpdate) {
			// update textury
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _textureWidth, _textureHeight, GL_RGB, GL_UNSIGNED_BYTE, _windowBytes.Bytes);
		}
		// vykreslíme
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void PixelWindow::SetTimestep(double dt)
	{
		_timestep = dt;
	}

	void PixelWindow::SetOutputFile(const std::string & fileName)
	{
		_outputFile = fileName;
	}

	bool PixelWindow::InitOpenglResources()
	{
		// dummy VAO - i pro "bufferless" rendering je potřeba mít bindnutý VAO (https://www.khronos.org/opengl/wiki/Vertex_Rendering#Prerequisites)
		// - "A non-zero Vertex Array Object must be bound (though no arrays have to be enabled, so it can be a freshly-created vertex array object)."
		glGenVertexArrays(1, &_dummyVao);
		glBindVertexArray(_dummyVao);

		// textura
		glGenTextures(1, &_textureId);
		glBindTexture(GL_TEXTURE_2D, _textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _textureWidth, _textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, _windowBytes.Bytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if(!InitShader()) {
			return false;
		}

		// (!) bind textur a programu se provede pouze zde a je předpokládáno,
		// že zůstane bindnut pořád
		glUseProgram(_shaderProgram);

		glDisable(GL_DEPTH_TEST);

		return true;
	}

	bool PixelWindow::InitShader()
	{
		static constexpr GLchar const * VertexShader = 
			"#version 330 core\n"
			"out vec2 UV;\n"
			"\n"
			"void main() {\n"
			"	const vec2 positions[3] = vec2[](\n"
			"		vec2(-1.0, -1.0),\n"
			"		vec2(3.0, -1.0),\n"
			"		vec2(-1.0, 3.0)\n"
			"	);\n"
			"	const vec2 coords[3] = vec2[](\n"
			"		vec2(0.0, 0.0),\n"
			"		vec2(2.0, 0.0),\n"
			"		vec2(0.0, 2.0)\n"
			"	);\n"
			"	UV = coords[gl_VertexID];\n"
			"	gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);\n"
			"}"
		;

		static constexpr GLchar const * FragmentShader = 
			"#version 330 core\n"
			"in vec2 UV;\n"
			"out vec4 FragColor;\n"
			"\n"
			"uniform sampler2D screenTexture;\n"
			"\n"
			"void main() {\n"
			"	FragColor = texture(screenTexture, UV);\n"
			"}"
		;

		// vytvoření vertex a fragment shaderů
		GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertShaderId, 1, &VertexShader, NULL);
		glCompileShader(vertShaderId);
		GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragShaderId, 1, &FragmentShader, NULL);
		glCompileShader(fragShaderId);

		int ok;
		glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &ok);
		if (!ok) {
			char infoLog[512];
			glGetShaderInfoLog(vertShaderId, 512, NULL, infoLog);
			std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
			return false;
		}
		glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &ok);
		if (!ok) {
			char infoLog[512];
			glGetShaderInfoLog(fragShaderId, 512, NULL, infoLog);
			std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
			return false;
		}

		// link shader programu
		_shaderProgram = glCreateProgram();
		glAttachShader(_shaderProgram, vertShaderId);
		glAttachShader(_shaderProgram, fragShaderId);
		glLinkProgram(_shaderProgram);

		glDeleteShader(vertShaderId);
		glDeleteShader(fragShaderId);

		return true;
	}

	void PixelWindow::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		constexpr int ScrollScale = 4;
		constexpr int XBound = 12000;
		constexpr int YBound = 12000;

		PixelWindow * pw = reinterpret_cast<PixelWindow*>(glfwGetWindowUserPointer(window));

		const int xScale = (pw->_viewport.Width / ScrollScale) * static_cast<int>(-yoffset);
		const int yScale = (pw->_viewport.Height / ScrollScale) * static_cast<int>(-yoffset);

		// spočítáme velikost nového obdélníku
		const double newWidth = std::clamp(pw->_viewport.Width - xScale, pw->_width, XBound);
		const double newHeight = std::clamp(pw->_viewport.Height - yScale, pw->_height, YBound);

		// trochu posuneme začátek viewport renderu obdélníku dle nové velikosti
		const double newX = (newWidth / static_cast<double>(pw->_viewport.Width)) * static_cast<double>(pw->_viewport.X);
		const double newY = (newHeight / static_cast<double>(pw->_viewport.Height)) * static_cast<double>(pw->_viewport.Y);

		pw->_viewport.X = static_cast<int>(newX);
		pw->_viewport.Y = static_cast<int>(newY);
		pw->_viewport.Width = static_cast<int>(newWidth);
		pw->_viewport.Height = static_cast<int>(newHeight);
	}

	void PixelWindow::ClickCallback(GLFWwindow* window, int button, int action, int mods)
	{
		PixelWindow * pw = reinterpret_cast<PixelWindow*>(glfwGetWindowUserPointer(window));

		if (button == GLFW_MOUSE_BUTTON_1) {
			if (action == GLFW_PRESS) {
				// posouvání viewportu (pro CursorPosCallback)
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);

				pw->_mouse.IsPressed = true;
				pw->_mouse.X = static_cast<int>(xpos);
				pw->_mouse.Y = static_cast<int>(ypos);
			}
			else if (action == GLFW_RELEASE) {
				pw->_mouse.IsPressed = false;
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
			// reset viewportu
			pw->_viewport.X = 0;
			pw->_viewport.Y = 0;
			pw->_viewport.Width = pw->_width;
			pw->_viewport.Height = pw->_height;
		}
	}

	void PixelWindow::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		PixelWindow * pw = reinterpret_cast<PixelWindow*>(glfwGetWindowUserPointer(window));

		if (pw->_mouse.IsPressed) {
			// posouvání viewportu
			pw->_viewport.X -= pw->_mouse.X - static_cast<int>(xpos);
			pw->_viewport.Y += pw->_mouse.Y - static_cast<int>(ypos);

			pw->_mouse.X = static_cast<int>(xpos);
			pw->_mouse.Y = static_cast<int>(ypos);
		}
	}

	void PixelWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		PixelWindow * pw = reinterpret_cast<PixelWindow*>(glfwGetWindowUserPointer(window));

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			pw->_pause = !pw->_pause;
		}
		else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			pw->_stepper->Reset(pw->_windowBytes);
			pw->_stepper->Init(pw->_windowBytes);
			pw->_end = false;
		}
		else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
			if (pw->_outputFile != "") {
				pw->OutputToFile();
			}
		}
	}
} // namespace Gui

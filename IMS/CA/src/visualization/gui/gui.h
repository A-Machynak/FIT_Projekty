#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <cstdint>
#include <iostream>
#include <functional>

#include "rgb.h"
#include "window_data.h"
#include "stepper_i.h"

namespace Gui
{
	struct Viewport
	{
		int X;
		int Y;
		int Width;
		int Height;
	};

	struct Mouse
	{
		bool IsPressed {false};
		int X;
		int Y;
	};

	/**
	 * @brief Třída pro vytváření nového opengl kontextu s oknem umožňujícím nastavování barev pro konkrétní pixely.
	 */
	class PixelWindow
	{
	public:
		/**
		 * @brief Konstruktor. Vytvoří nový opengl kontext a okno. Okno je neresponzivní, dokud není zavolána metoda Run().
		 * 
		 * @param windowWidth šířka okna
		 * @param windowHeight výška okna
		 * @param windowName název okna
		 * @param textureWidth šířka textury
		 * @param textureHeight výška textury
		 * @param openglOff vypnutí opengl (finální stav bude zapsán do souboru)
		 */
		PixelWindow(
			int windowWidth,
			int windowHeight,
			const std::string & windowName,
			int textureWidth,
			int textureHeight,
			bool openglOff = false);

		/**
		 * @brief Destruktor
		 */
		~PixelWindow();

		/**
		 * @brief Kontrola inicializace
		 * 
		 * @return true inicializace proběhla v pořádku
		 * @return false chyba při inicializaci (výstup na std::cerr)
		 */
		bool IsInitialized() const;

		/**
		 * @brief Spustí hlavní loop.
		 * 
		 * @param stepper třída implementující krokovou funkci.
		 * Kroková funkce je volána každý časový krok ( SetTimestep() ).
		 * @return true vše proběhlo v pořádku
		 * @return false nastala chyba (popis chyby na stderr)
		 */
		bool Run(IStepper * stepper);

		/**
		 * @brief Nastaví hodnotu časového kroku - jak často bude volána kroková funkce třídy zadané v Run()
		 * 
		 * @param dt časový krok - pokud je tato hodnota příliš malá a vizualizace nestíhá,
		 * tak může proběhnout více časových kroků za 1 frame
		 */
		void SetTimestep(double dt);

		/**
		 * @brief Nastavení výstupního souboru (výchozí: žádný)
		 * 
		 * @param fileName cesta/název souboru (.png formát)
		 */
		void SetOutputFile(const std::string & fileName);
	private:
		/// @brief Flag pro inicializaci
		bool _initialized = false;

		/// @brief Flag pro konec
		bool _end = false;

		/// @brief Flag pro pauzu
		bool _pause = false;

		/// @brief Šířka okna
		const int _width;

		/// @brief Výška okna
		const int _height;

		/// @brief Šířka textury
		const int _textureWidth;

		/// @brief Výška textury
		const int _textureHeight;

		/// @brief Offscreen rendering
		bool _glOff;

		/// @brief Název výstupního souboru
		std::string _outputFile = "";

		/// @brief Velikost časového kroku
		double _timestep = 0.0;

		/// @brief Data okna
		WindowData _windowBytes;

		/// @brief Současný čas (poslední hodnota vrácena glfw)
		double _currentTime = 0.0;

		/// @brief Poslední zaznamenaný čas
		double _lastTime = 0.0;

		/// @brief Součet rozdílů _currentTime a _lastTime
		/// (při přesáhnutí hodnoty `_timestep` je nutné volat krokovou funkci)
		double _cumulativeTime = 0.0;

		/// @brief Počet provedených časových kroků
		std::uint64_t _stepCount = 0;

		/// @brief Třída implementující krokovou funkci
		IStepper * _stepper;

		/// @brief Okno
		GLFWwindow * _window;

		/// @brief Viewport okna
		Viewport _viewport;

		/// @brief Informace o myši
		Mouse _mouse;

		/// @brief Identifikator pro texturu
		GLuint _textureId;

		/// @brief Identifikator pro shader
		GLuint _shaderProgram;

		/// @brief Identifikator pro VAO
		GLuint _dummyVao;

		/**
		 * @brief Inicializace opengl
		 */
		void InitOpengl(const std::string & windowName);

		/**
		 * @brief Inicializace textury a shaderu
		 */
		bool InitOpenglResources();

		/**
		 * @brief Inicializace shaderu
		 */
		bool InitShader();
		
		/**
		 * @brief Hlavní render loop
		 */
		void RenderLoop();

		/**
		 * @brief Hlavní loop bez vizualizace (výsledek zapsán do souboru)
		 */
		void RunWithoutOpengl();

		/**
		 * @brief Zapíše současný stav okna do souboru
		 */
		void OutputToFile();

		/**
		 * @brief GLFW scroll callback (pro zoom)
		 * 
		 * @param window okno
		 * @param xoffset xoffset
		 * @param yoffset yoffset
		 */
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		/**
		 * @brief GLFW mouse callback (pro přesouvání)
		 * 
		 * @param window okno
		 * @param button tlačítko
		 * @param action glfw action
		 * @param mods mod
		 */
		static void ClickCallback(GLFWwindow* window, int button, int action, int mods);

		/**
		 * @brief GLFW cursor pos callback
		 * 
		 * @param window okno
		 * @param xpos x
		 * @param ypos y
		 */
		static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

		/**
		 * @brief GLFW key callback
		 * 
		 * @param window okno
		 * @param key key
		 * @param scancode code
		 * @param action glfw action
		 * @param mods mod
		 */
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	};
} // namespace Gui


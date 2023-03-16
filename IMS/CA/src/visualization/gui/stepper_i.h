
#pragma once

#include "window_data.h"

namespace Gui
{
	/**
	 * @brief Interface pro třídu implementující krokovou funkci,
	 * která mění stav okna na základě vnitřního stavu.
	 */
	class IStepper
	{
	public:
		virtual ~IStepper() { };

		/**
		 * @brief Inicializuje okno. Voláno pouze 1x a to před 1. voláním Step()
		 * 
		 * @param windowData data
		 */
		virtual void Init(WindowData & windowData) { };

		/**
		 * @brief Kroková funkce.
		 * 
		 * @param windowData data
		 */
		virtual void Step(WindowData & windowData) { };

		/**
		 * @brief Kontrola konce simulace - voláno po každém volání Step()
		 * 
		 * @param windowData data
		 * @return true ukončit simulaci
		 * @return false pokračovat v simulaci
		 */
		virtual bool EndCondition(WindowData & windowData) { return false; };

		/**
		 * @brief Resetuje stav do původního stavu - voláno při stisku klávesy;
		 * sémantika této funkce se může měnit v závislosti na konkrétním modelu
		 * (návrat do původního stavu / reset hodnot a nastavení náhodného stavu / ...)
		 * 
		 * @param windowData data
		 */
		virtual void Reset(WindowData & windowData) { };
	protected:
		IStepper() = default;

		IStepper & operator=(const IStepper & other) = default;

		IStepper(IStepper && other) = default;

		IStepper & operator=(IStepper && other) = default;
	};
} // namespace Gui

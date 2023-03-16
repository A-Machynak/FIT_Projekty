#pragma once

#include <cstdint>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "model_i.h"

namespace CA
{
	/**
	 * @brief Určuje jakým způsobem po sobě budou uplatňována pravidla.
	 */
	enum class RuleStrategy
	{
		Random,         ///< Každý krok je vybráno (pseudo)náhodné pravidlo
		DatasetRandom,  ///< Každý krok je vybráno (pseudo)náhodné pravidlo ze zadaných pravidel
		DatasetRing,    ///< Každý krok je vybráno následující pravidlo ze zadaných pravidel; v kruhu, tj. po posledním pravidle následuje první
		DatasetIterate, ///< Každý krok je vybráno následující pravidlo ze zadaných pravidel; poslední pravidlo je uplatněno ve všech ostatních krocích
	};

	using Cell = std::uint8_t;
	using Rule = std::uint8_t;

	/**
	 * @brief Elementární celulární automat s možností zvolení posloupnosti pravidel.
	 * Pokud je zvoleno pravidlo RuleStrategy::Random, tak je po zavolání GetRules().
	 * Jednotlivé generace jsou uloženy v 2d poli s tím, že nižší index "výšky" představuje
	 * starší generace.
	 */
	class Elementary : public IModel<Cell>
	{
	public:
		/**
		 * @brief Konstruktor. RuleStrategy == Random.
		 * 
		 * @param width šířka plochy
		 * @param height výška plochy
		 */
		Elementary(Dimension2D planeSize);

		/**
		 * @brief Konstruktor.
		 * 
		 * @param width šířka plochy
		 * @param height výška plochy
		 * @param strategy jak uplatňovat pravidla
		 * @param rules pravidla
		 */
		Elementary(Dimension2D planeSize, RuleStrategy strategy, const std::vector<Rule> & rules);

		/**
		 * @brief Provede jeden krok, čímž aktualizuje hodnoty na následujícím řádku
		 * - aktualizuje výstup následující generace
		 */
		void Step() override;

		/**
		 * @brief Nastavení stavu 1. generace
		 * 
		 * @param state stav; pokud je počet hodnot ve vektoru menší, než šířka plochy,
		 * tak jsou doplněny chybějící hodnoty předchozíma (tedy kruhový buffer v podstatě)
		 * - state = { 0, 1 } -> { 0, 1, 0, 1, 0, 1, ... }
		 */
		void SetInitialState(const std::vector<Cell> & state);

		/**
		 * @brief Nastaví náhodný počáteční stav
		 */
		void SetInitialStateRandom();

		/**
		 * @brief Nastaví počáteční stav jedné buňky na pozici
		 * 
		 * @param x souřadnice buňky
		 */
		void SetInitialStateSingle(std::uint32_t x, bool set);

		/**
		 * @brief Nastaví počáteční stav jedné náhodné buňky do 1
		 */
		void SetInitialStateSingleRandom();

		/**
		 * @brief Nastaví všechny buňky v počátečním stavu (1. generaci) do 0
		 */
		void ResetInitialState();

		/**
		 * @brief Getter stavu plochy
		 * 
		 * @return const Board<Cell>& stav plochy
		 */
		const Board<Cell> & GetBoardState() const override;

		/**
		 * @brief Getter pravidel
		 * 
		 * @return const std::vector<Rule>& pravidla
		 */
		const std::vector<Rule> & GetRules() const;

		/**
		 * @brief Getter pravidla aplikovaného při dalším volání Step()
		 * 
		 * @return Rule pravidlo
		 */
		Rule GetCurrentRule() const;

		/**
		 * @brief Getter čísla generace
		 * 
		 * @return std::uint32_t generace
		 */
		std::uint32_t GetGeneration() const;

		/**
		 * @brief Getter čísla posledního aktualizovaného řádku
		 * 
		 * @return std::uint32_t generace
		 */
		std::uint32_t GetRowIdx() const;

		/**
		 * @brief Resetování stavu - pokud je zadán RuleStrategy:
		 * - Random, tak budou vygenerovány nové pravidla
		 * - DatasetRandom, tak bude použita jiná posloupnost pravidel
		 * - DatasetRing/DatasetIterate bude pouze resetován stav, ale výsledek bude stejný
		 */
		void Reset();
	private:
		/// @brief Pravidla
		std::vector<Rule> _rules;

		/// @brief Jak se uplatňují pravidla
		RuleStrategy _strategy;

		/// @brief Řádek zpracovaný při zavolání Step() (generace)
		std::uint32_t _currentGen {0};

		/// @brief Právě zpracované pravidlo
		std::uint32_t _currentRuleIdx {0};

		/**
		 * @brief Vrátí stav okolních buněk
		 * 
		 * @param centerX prostřední buňka (x souřadnice)
		 * @return std::uint8_t spodní 3b reprezentují stav okolních buněk
		 * 1.[idx 0 - LSB] je levá buňka
		 * 2.[idx 1] je prostřední buňka
		 * 3.[idx 2] je pravá buňka
		 */
		std::uint8_t GetNeighbourhood(std::uint32_t centerX, const std::vector<Cell> & cellRow);

		/**
		 * @brief Inicializace srand()
		 */
		void InitRand();

		/**
		 * @brief Inicializace náhodných pravidel
		 */
		void InitRandomRules();

		/**
		 * @brief Inicializace počátečního stavu
		 */
		void InitInitialState();

		/**
		 * @brief Aktualizace pravidel dle typu RuleStrategy
		 */
		void UpdateRuleIdxFromStrategy();

		/**
		 * @brief Aktualizace pravidla pomocí RuleStrategy::Random
		 */
		void UpdateRuleIdxRandom();

		/**
		 * @brief Aktualizace pravidla pomocí RuleStrategy::DatasetRandom
		 */
		void UpdateRuleIdxDatasetRandom();

		/**
		 * @brief Aktualizace pravidla pomocí RuleStrategy::DatasetRing
		 */
		void UpdateRuleIdxDatasetRing();

		/**
		 * @brief Aktualizace pravidla pomocí RuleStrategy::DatasetIterate
		 */
		void UpdateRuleIdxDatasetIterate();
	};
} // namespace Model

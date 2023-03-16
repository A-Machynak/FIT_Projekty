Cell[Width] NextGeneration(Cell[Width] cells, int rule) {
	Cell[Width] nextGeneration;
	for (int i = 0; i < Width; i++) {
		int state =
			  (cell[i-1 % Width] == 1) << 0 // levá buňka
			| (cell[i] == 1)           << 1 // současná buňka
			| (cell[i+1 % Width] == 1) << 2 // pravá buňka
		nextGeneration[i] = ((1 << state) & rule) ? 1 : 0;
	}
	return nextGeneration;
}
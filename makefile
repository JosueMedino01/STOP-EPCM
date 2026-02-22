run: 
	g++ app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
	time ./app.out -f Artificial_POP_Instance_01.dat -MIN_PRIZE 12000 -MIN_PROB 0.9 -K 2 -T 10 -maxNotImproviment 3000 -seed 13 -TMAX 10
	rm -f *.o app.out

bug: 
	g++ -g app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
I01: 
	g++ app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
	time ./app.out -f Artificial_POP_Instance_01.dat -MIN_PRIZE 3000 -MIN_PROB 0.5 -K 2 -T 10 -maxNotImproviment 3000 -seed 13 -TMAX 1800
	rm -f *.o app.out

I02: 
	g++ app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
	time ./app.out -f Artificial_POP_Instance_02.dat -MIN_PRIZE 3000 -MIN_PROB 0.5 -K 2 -T 10 -maxNotImproviment 3000 -seed 13 -TMAX 1800
	rm -f *.o app.out


I03: 
	g++ app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
	time ./app.out -f Artificial_POP_Instance_03.dat -MIN_PRIZE 3000 -MIN_PROB 0.5 -K 2 -T 10 -maxNotImproviment 3000 -seed 13 -TMAX 1800
	rm -f *.o app.out


I04: 
	g++ app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
	time ./app.out -f Artificial_POP_Instance_04.dat -MIN_PRIZE 3000 -MIN_PROB 0.5 -K 2 -T 10 -maxNotImproviment 3000 -seed 13 -TMAX 1800
	rm -f *.o app.out


bug: 
	g++ -g app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
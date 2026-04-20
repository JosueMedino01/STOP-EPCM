compile: 
	g++ app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
run:
	time ./app.out -f Artificial_POP_Instance_01.dat -K 2 -T 10 -maxNotImproviment 3000 -seed 13 -TMAX 1800 -output xx
clear:
	rm -f *.o app.out

bug: 
	g++ -g app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
run: 
	g++ app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
	time ./app.out -f Artificial_POP_Instance_03.txt -MIN_PRIZE 2000 -MIN_PROB 0.6 -K 2 -T 10 -maxNotImproviment 3000 -seed 13
	rm -f *.o app.out

bug: 
	g++ -g app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out

refactor: 
	g++ src/app.cpp -o bin/app.out
	./bin/app.out

unit:

e2e: 
CONVERTER:
	g++ -o converter converter.cpp -Wall
clean:
	rm converter *.txt
test:
	./converter sample1 out.txt
test2:
	./converter sample2 out.txt 
debug:
	./converter sample2 out.txt > debug_out.txt
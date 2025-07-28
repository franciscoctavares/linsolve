model_reader:
	g++ -c src/model_reader.cpp -o build/model_reader.o
constraint:
	g++ -c src/constraint.cpp -o build/constraint.o
matrix:
	g++ -c src/matrix.cpp -o build/matrix.o
lp:
	g++ -c src/lp.cpp -o build/lp.o
bb_node:
	g++ -c src/bb_node.cpp -o build/bb_node.o
bb_tree:
	g++ -c src/bb_tree.cpp -o build/bb_tree.o
main:
	g++ -c src/main.cpp -o build/main.o
build: main lp matrix constraint model_reader bb_node bb_tree
	g++ -o bin/main build/main.o build/lp.o build/matrix.o build/constraint.o build/model_reader.o build/bb_node.o build/bb_tree.o
run: build
	clear
	./bin/main
test: matrix lp constraint model_reader bb_node bb_tree ip
	g++ -c src/test.cpp -o build/test.o
	g++ -o bin/test build/test.o build/matrix.o build/constraint.o build/model_reader.o build/bb_node.o build/bb_tree.o build/lp.o build/ip.o
	clear
	./bin/test
clean:
	rm -f build/* bin/*
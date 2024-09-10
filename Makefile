ifeq ($(OS), Windows_NT)
	EXECUTABLE_EXTENSION=.exe
else
	EXECUTABLE_EXTENSION=
endif

install:
	@echo 'Installing dependencies...'
	curl -s https://raw.githubusercontent.com/mity/acutest/31751b4089c93b46a9fd8a8183a695f772de66de/include/acutest.h > libs/acutest.h
	curl -s https://raw.githubusercontent.com/fmenozzi/argparser/b44924e228bd983fe12916c22b5b3e5e5b9573d4/argparser.hpp > libs/argparse.hpp
	curl -s https://raw.githubusercontent.com/laserpants/dotenv-cpp/432def2fb9c2a5e51e6d491f35f2e8f35fa900b0/include/laserpants/dotenv/dotenv.h > libs/dotenv.h
	@echo '... done.'
test:
	@echo 'Compiling tests...'
	g++ --std=gnu++20 -DTEST -x c++ src/reader.hpp -o bin/reader.test$(EXECUTABLE_EXTENSION)
	g++ --std=gnu++20 -DTEST -x c++ src/simplification.hpp -o bin/simplification.test$(EXECUTABLE_EXTENSION)
	@echo '... running test ...'
	./bin/reader.test$(EXECUTABLE_EXTENSION)
	./bin/simplification.test$(EXECUTABLE_EXTENSION)
	@echo '... done.'
build:
	g++ --std=gnu++20 src/maxmodels.cpp -o bin/maxmodels$(EXECUTABLE_EXTENSION)

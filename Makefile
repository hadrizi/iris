cc         = g++
cc_version = 20
cc_flags   = -Wextra -Wall -pedantic -O3 -std=c++$(cc_version)

project      = iris
build_folder = build/

$(project):
	mkdir $(build_folder)
	$(cc) $(cc_flags) -o $(build_folder)$(project) main.cpp

all: $(project)

.PHONY: clear

clear:
	rm -rf $(build_folder)
	rm -rf *.ppm
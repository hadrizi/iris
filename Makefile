cc               = g++
cc_version       = 20
cc_flags_debug   = -g -fsanitize=address -fsanitize=leak -Wextra -Wall -pedantic -std=c++$(cc_version)
cc_flags_release = -O3 -Wextra -Wall -pedantic -std=c++$(cc_version)
cc_libs          = -lX11

build_mode = debug

project       = iris
source_folder = src/
build_folder  = build/$(build_mode)/


$(build_folder):
	mkdir $(build_folder)

$(build_folder)graphics.o: $(build_folder)
	$(cc) $(cc_flags_$(build_mode)) -c -o $(build_folder)graphics.o $(source_folder)graphics/graphics.cpp

$(build_folder)math.o: $(build_folder)
	$(cc) $(cc_flags_$(build_mode)) -c -o $(build_folder)math.o $(source_folder)math/math.cpp

$(build_folder)mesh.o: $(build_folder)
	$(cc) $(cc_flags_$(build_mode)) -c -o $(build_folder)mesh.o $(source_folder)mesh/mesh.cpp

$(build_folder)window.o: $(build_folder)
	$(cc) $(cc_flags_$(build_mode)) -c -o $(build_folder)window.o $(source_folder)window/window.cpp

$(build_folder)$(project).o: $(build_folder)
	$(cc) $(cc_flags_$(build_mode)) -c -o $(build_folder)$(project).o $(source_folder)$(project).cpp

$(project): $(build_folder)$(project).o $(build_folder)graphics.o $(build_folder)math.o $(build_folder)mesh.o $(build_folder)window.o
	$(cc) $(cc_flags_$(build_mode)) $(build_folder)$(project).o $(build_folder)graphics.o $(build_folder)math.o $(build_folder)mesh.o $(build_folder)window.o $(cc_libs) -o $(build_folder)$(project) 

all: $(project)

.PHONY: gif
gif:
	ffmpeg -framerate 10 -i out/out%d.ppm -vf "setpts=0.5*PTS,fps=10,scale=800:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" out/out.gif

.PHONY: clear
clear:
	rm -rf $(build_folder)
	rm -rf log.txt
	rm -rf out/*

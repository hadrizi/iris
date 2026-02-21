cc         = g++
cc_version = 20
cc_flags   = -g -fsanitize=address -fsanitize=leak -Wextra -Wall -pedantic -std=c++$(cc_version)

project       = iris
build_folder  = build/

$(project):
	mkdir $(build_folder)
	$(cc) $(cc_flags) -o $(build_folder)$(project) main.cpp

all: $(project)

.PHONY: gif
gif:
	ffmpeg -framerate 10 -i out/out%d.ppm -vf "setpts=0.5*PTS,fps=10,scale=800:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" out/out.gif

.PHONY: clear
clear:
	rm -rf $(build_folder)
	rm -rf log.txt
	rm -rf out/*

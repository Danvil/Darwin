#/bin/sh

export LD_LIBRARY_PATH=../Candy/LinuxProfile

valgrind --tool=callgrind LinuxProfile/Tangerin /home/david/Documents/Darwin/assets/

~/Programs/jrfonseca/gprof2dot.py -f callgrind callgrind.out.* | dot -Tpng -o output.png

eog output.png &


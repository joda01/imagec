cd build/build/output
./imagec
#valgrind --tool=callgrind --trace-children=yes ./imagec
#gprof ./imagec /workspaces/imagec/build/CMakeFiles/3.25.2/CompilerIdCXX/a.out > profile.txt


#perf record -g ./imagec

#perf report

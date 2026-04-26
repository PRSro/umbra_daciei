# CMake generated Testfile for 
# Source directory: /home/prsro/Projects/GitHub/Dacia Libera+Fractal CCP/umbra_daciei
# Build directory: /home/prsro/Projects/GitHub/Dacia Libera+Fractal CCP/umbra_daciei/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ConspiracyWebTest "/home/prsro/Projects/GitHub/Dacia Libera+Fractal CCP/umbra_daciei/build/test_conspiracy_web")
set_tests_properties(ConspiracyWebTest PROPERTIES  _BACKTRACE_TRIPLES "/home/prsro/Projects/GitHub/Dacia Libera+Fractal CCP/umbra_daciei/CMakeLists.txt;135;add_test;/home/prsro/Projects/GitHub/Dacia Libera+Fractal CCP/umbra_daciei/CMakeLists.txt;0;")
subdirs("_deps/sdl2-build")
subdirs("_deps/sdl2_ttf-build")
subdirs("_deps/sdl2_image-build")
subdirs("_deps/sdl2_mixer-build")
subdirs("_deps/nlohmann_json-build")

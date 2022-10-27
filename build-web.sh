source "/usr/lib/emsdk/emsdk_env.sh"
emcc \
	-o builds/web/index.html main.c \
	--shell-file shell_minimal.html \
	--preload-file res \
	-Os -Wall -DPLATFORM_WEB \
	../raylib/src/libraylib.a \
	-I. -I../raylib/src \
	-L. -L..../raylib/src \
	-s USE_GLFW=3 -s ASYNCIFY

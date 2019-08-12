# Makefile for Thunder3d

# The library and Demo names
LIB_NAME=thunder3d
DEMO_NAME=demo

# MinGW include
MINGW_INCLUDE=c:/MinGW/include

################### No need to change #############################

C_FILES=stb_image.c \
stb_vorbis.c \
t3d_ivec2.c \
t3d_vec2.c \
t3d_vec3.c \
t3d_vec4.c \
t3d_quat.c \
t3d_mat2.c \
t3d_mat3.c \
t3d_mat4.c \
t3d_bezier.c \
t3d_util.c \
t3d_timer.c \
t3d_geomath.c \
t3d_vector.c \
t3d_hashtable.c \
t3d_slist.c \
t3d_ogl.c \
t3d_texture.c \
t3d_shader.c \
t3d_object.c \
t3d_frustum.c \
t3d_camera.c \
t3d_light.c \
t3d_control.c \
t3d_heightmap.c \
t3d_geopatch.c \
t3d_patch.c \
t3d_patchmap.c \
t3d_ms3dmesh.c \
t3d_ms3d.c \
t3d_mesh.c \
t3d_decal.c \
t3d_aamesh.c \
t3d_unit.c \
t3d_font.c \
t3d_bbox.c \
t3d_pick.c \
t3d_astar.c \
t3d_sys.c \
t3d_thpool.c \
t3d_wave.c \
t3d_vorbis.c \
t3d_minimap.c \
t3d_scene.c

DEMO_C_FILES=engine.c


#C_SOURCE=$(addprefix src/,$(C_FILES)) $(addprefix src/constraints/,$(CONSTRAINSTS_C_FILES))
LIB_OBJECTS=$(addprefix obj/,$(C_FILES:.c=.o))
#DEMO_SOURCE=$(addprefix demo/,$(DEMO_C_FILES))
DEMO_OBJECTS=$(addprefix obj/,$(DEMO_C_FILES:.c=.o))

LIBRARY=lib$(LIB_NAME).a
DEMO_EXE=$(addsuffix .exe, $(DEMO_NAME))

# Compilers and librarian
CC=gcc
AR=ar

# Compiling FLAGS
COMPILER_FLAG=-Wall -std=gnu99 -O2 -fomit-frame-pointer -ffast-math
DEFINES=-DPTW32_STATIC_LIB -DGLFW_INCLUDE_GLCOREARB
INCLUDES=-I$(MINGW_INCLUDE) -Iinclude

CFLAGS=$(COMPILER_FLAG) $(DEFINES) $(INCLUDES)

# Linker FLAGS
LDFLAGS=-Lobj -Llib/win32
LIBS=$(LDFLAGS) -l$(LIB_NAME)

# OpenGL libs
OPENGL_LIBS=-lglfw3 -lopengl32

# OpenAL Lib
OPENAL_LIB=-lopenal32.dll

# pthread lib
PTHREAD_LIB=-lpthread

# CRT lib
MS_LIBS=-lgdi32 -lwinmm -lmsvcrt

.PHONY: all
all: demo

.PHONY: clean
clean:
	del obj\*.o
	del obj\*.a
	del demo\$(DEMO_EXE)

demo: obj/$(LIBRARY) $(DEMO_OBJECTS)
	$(CC) -o demo/$(DEMO_EXE) $(DEMO_OBJECTS) $(LIBS) $(OPENGL_LIBS) $(MS_LIBS) $(PTHREAD_LIB) $(OPENAL_LIB) -static -s

obj/%.o: demo/%.c
	$(CC) -c $(CFLAGS) $< -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

lib: obj/$(LIBRARY)

obj/$(LIBRARY): $(LIB_OBJECTS)
	$(AR) cr obj/$(LIBRARY) $(LIB_OBJECTS)
	ranlib obj/$(LIBRARY)
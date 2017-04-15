CC = g++
DEFINES = 

ifdef CGLX
	ifdef LINUX
		DEFINES = -DCGLX -DLINUX
		CGLX_OPT = -I/usr/local/cglX/include
		CGLX_LIBOPT = -lcglX
	else
		DEFINES = -DCGLX -DMAC
		CGLX_LIBOPT = -framework cglX
	endif
endif

ifdef LINUX
	DEFINES = -DLINUX
	LIBOPT = -lGL -lGLU -L/usr/local/cglX/lib -lGLEW $(CGLX_LIBOPT)
else
	DEFINES = -DMAC
	LIBOPT = -framework GLUT -framework OpenGL -lGLEW $(CGLX_LIBOPT)
endif

CCOPT = -g -O3 -Wall $(CGLX_OPT)

# LIBOPT for compiling on cglx machines


# LIBOPT = -L/usr/X11R6/lib -lglut -lGLU -lGL -lXmu -lXi -lX11 -lm

# LIBOPT for compiling on Mac
# LIBOPT = -framework GLUT -framework OpenGL -lGLEW

DEPENDENCIES = surface.o grapher.o shader_primitive.o texture.o ray.o point.o stopwatch.o p_surface.o p_curve.o flow.o primitive.o

EXECUTABLE = driver

all : $(EXECUTABLE)

$(EXECUTABLE) : driver.o $(DEPENDENCIES)
	$(CC) $(CCOPT) -o $(EXECUTABLE) driver.o $(DEPENDENCIES) $(LIBOPT)
	
driver.o : driver.cpp
	$(CC) $(CCOPT) -c driver.cpp
	
%.o : %.cpp %.h
	$(CC) $(CCOPT) $(DEFINES) -c $<

project : project3.c $(DEPENDENCIES)
	$(CC) $(CCOPT) -c ray.cpp
	$(CC) $(CCOPT) -c point.cpp
	$(CC) $(CCOPT) -c project3.c
	$(CC) -o project3 project3.o $(DEPENDENCIES) ray.o point.o $(LIBOPT)

clean : 
	find . -type f -name '*.o' -o -name '*.dSYM' -o -name '*.dylib' | xargs rm -rf

#include <iostream>
#include <string>
#include <cmath>
#include <getopt.h>

#include "grapher.h"
#include "color.h"

using namespace glot;
using namespace std;

typedef p_surface my_prim;

void usage(const string& name) {
    // Convenient surfaces
    cout << "Examples: " << endl << endl;
    cout << "  Surface:" << endl;
    cout << "    " << name << " --surface 'sin(3.0 * sqrt(x * x + y * y) - 2.0 * t) * exp(-sqrt(x * x + y * y)/5.0) * cos(5.0 * sqrt((x - 1.5) * (x - 1.5) + (y - 0.75) * (y - 0.75)) - t)'" << endl << endl;
    cout << "  Parametric surface:" << endl;
    cout << "    " << name << " --p-surface 'pow(abs(sin(t)), u * 18.0) * (1 + cos(v * 6.0)) * cos(u * 18.0), pow(abs(sin(t)), u * 18.0) * (1 + cos(v * 6.0)) * sin(u * 18.0), pow(abs(sin(t)), u * 18.0) * sin(v * 6.0) - 1.5 * pow(abs(sin(t)), u * 18.0)'" << endl << endl;
    cout << "  Toroid:" << endl;
    cout << "    " << name << " --cylindrical --p-surface '2 * 6.3 * v, 3 + sin(6.3 * v) + cos(6.3 * u), sin(6.3 * u) + 2 * cos(6.3 * v)'" << endl << endl;
    cout << "  Curve:" << endl;
    cout << "    " << name << " --cylindrical --p-curve 'u * 19.0, (2.0 + cos(7.0 * u * 19.0 / 3.0)) * (1 +  abs(sin(t))), sin(7.0 * u * 19.0 / 3.0)'" << endl << endl;
}

int main(int argc, char ** argv) {

    enum primitive_type {
        FLOW      = 0,
        P_CURVE   = 1,
        P_SURFACE = 2,
        SURFACE   = 3
    };

    const string name = argv[0];
    int grid = GRID_OFF;
    int axes = AXES_ON;
    int x_axis = X_LIN;
    int y_axis = Y_LIN;
    int coordinates = CARTESIAN;
    int type = P_CURVE;

    while (true) {
        static struct option long_options[] = {
            {"grid",        no_argument, &grid,        GRID_ON     },
            {"no-grid",     no_argument, &grid,        GRID_OFF    },
            {"axes",        no_argument, &axes,        AXES_ON     },
            {"no-axes",     no_argument, &grid,        AXES_OFF    },
            {"x-lin",       no_argument, &x_axis,      X_LIN       },
            {"x-log",       no_argument, &x_axis,      X_LOG       },
            {"y-lin",       no_argument, &x_axis,      Y_LIN       },
            {"y-log",       no_argument, &y_axis,      Y_LOG       },
            {"cartesian",   no_argument, &coordinates, CARTESIAN   },
            {"cylindrical", no_argument, &coordinates, CYLINDRICAL },
            {"spherical",   no_argument, &coordinates, SPHERICAL   },
            {"flow",        no_argument, &type,        FLOW        },
            {"p-curve",     no_argument, &type,        P_CURVE     },
            {"p-surface",   no_argument, &type,        P_SURFACE   },
            {"surface",     no_argument, &type,        SURFACE     },
            {"help",        no_argument, 0,            'h'         },
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int index = getopt_long (argc, argv, "ht:", long_options, &option_index);

        /* Detect the end of the options. */
        if (index == -1) {
            break;
        }

        switch (index) {
        case 0:
            if ("help" == string(long_options[option_index].name)) {
                usage(name);
                return 0;
            }
            break;

        case 'h':
            usage(name);
            return 0;
            break;

        case '?':
            usage(name);
            return 1;
            break;

        default:
            usage(name);
            return 2;
        }
    }

    short int options = static_cast<short int>(grid | axes | x_axis | y_axis | coordinates);

    grapher::initialize(argc, argv, ZOOM_KEYS_ON);
    color c(1.0, 0, 0);
    
    for (int i = optind; i < argc; ++i) {
        shader_primitive * p = NULL;
        const string function = string(argv[i]);
        const string texture = "textures/tilable.ppm";
        cout << "Looking at " << function << endl;
        switch(type) {
        case FLOW:
            cout << "Making flow: " << function << endl;
            p = new flow(function);
            break;
        case P_CURVE:
            cout << "Making curve: " << function << endl;
            p = new p_curve(function, c, texture, options);
            break;
        case P_SURFACE:
            cout << "Making parametric surface: " << function << endl;
            p = new p_surface(function, c, texture, options);
            break;
        case SURFACE:
            cout << "Making surface: " << function << endl;
            p = new surface(function, c, texture);
            break;
        default:
            usage(name);
            return 3;
        }
        p->load_texture(texture);
        grapher::add(*p);
    }

    grapher::set_idle_function(grapher::redraw);
    grapher::run();
    
    return 0;
}

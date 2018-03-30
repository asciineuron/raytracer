#include <iostream>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

struct point
{
    float x,y,z;
};

struct vector
{
    float x,y,z;
    point tail; //optional, where tail is located (for shadow rays reflected off objects)
};


struct color
{
    int r,g,b;
};


struct sphere
{
    vector center;
    float radius;
    color scolor;
};

//empty vector, at origin
const vector EMPTY =
{
    0.0,
    0.0,
    0.0
};

const color BLUE =
{
    0, 255, 0
};

const color PRETTY =
{
    244,
    66,206
};

float dot_prod(vector r1, vector r2)
{
    float out;
    out = r1.x * r2.x + r1.y * r2.y + r1.z * r2.z;
    return out;
}

vector v_sub(vector v1, vector v2)
{
    vector out;
    out.x = v1.x - v2.x;
    out.y = v1.y - v2.y;
    out.z = v1.z - v2.z;
    return out;
}

vector v_scale(vector v, float s) //multiplies vector v by scalar s
{
    vector out;
    out.x = v.x * s;
    out.y = v.y * s;
    out.z = v.z * s;
    return out;
}

float v_len(vector v)
{
    float length;
    length = sqrt(pow(v.x, 2.0) + pow(v.y, 2.0) + pow(v.z, 2.0));
    return length;
}

vector v_unit(vector v) //returns a vector in same direction but of length 1
{
    vector out;
    float length = v_len(v);
    out = v_scale(v, (1.0/length));
    return out;
}

vector point_to_vect(point p)
{
    vector v;
    v.x = p.x;
    v.y = p.y;
    v.z = p.z;
    return v;
}

point vect_to_point(vector v)
{
    point p;
    p.x = v.x;
    p.y = v.y;
    p.z = v.z;
    return p;
}

bool is_intersect_shifted(vector v, sphere s)
{
    v = v_unit(v);
    float calc = (pow(dot_prod(v, (v_sub(s.center, point_to_vect(v.tail)))), 2.0) - pow(v_len(v_sub(point_to_vect(v.tail), s.center)), 2.0) + pow(s.radius, 2.0));
    return (calc >= 0.0);
}

bool is_intersect(vector v, sphere s) //calculates using quadratic discriminant, simplifying to: (v * o-c)^2 - ||o-c||^2 + r^2, true if >=0
// as long as v is unit vector!!
{
    v = v_unit(v);
    float calc = (pow(dot_prod(v, (v_sub(s.center, EMPTY))), 2.0) - pow(v_len(v_sub(EMPTY, s.center)), 2.0) + pow(s.radius, 2.0));
    return (calc >= 0.0);
}

vector find_hit(vector v, sphere s)
{
    vector out;
    v = v_unit(v);
    out = v;
    float d1;
    float d2;
    if ((pow(dot_prod(v, (v_sub(s.center, point_to_vect(v.tail)))), 2.0) - pow(v_len(v_sub(point_to_vect(v.tail), s.center)), 2.0) + pow(s.radius, 2.0)) >= 0.0)
    {
        if ((pow(dot_prod(v, (v_sub(s.center, point_to_vect(v.tail)))), 2.0) - pow(v_len(v_sub(point_to_vect(v.tail), s.center)), 2.0) + pow(s.radius, 2.0)) > 0.0)
        {
            d1 = -1.0*dot_prod(v, v_sub(s.center, point_to_vect(v.tail))) + sqrt((pow(dot_prod(v, (v_sub(s.center, point_to_vect(v.tail)))), 2.0) - pow(v_len(v_sub(point_to_vect(v.tail), s.center)), 2.0) + pow(s.radius, 2.0)));
            d2 = -1.0*dot_prod(v, v_sub(s.center, point_to_vect(v.tail))) - sqrt((pow(dot_prod(v, (v_sub(s.center, point_to_vect(v.tail)))), 2.0) - pow(v_len(v_sub(point_to_vect(v.tail), s.center)), 2.0) + pow(s.radius, 2.0)));
            if (d1 < d2) //d1 closer to origin
            {
                out = v_scale(v, d1);
            } else
            {
                out = v_scale(v, d2);
            }
        }
    }
    return out;
}

/* 
ppm format:
P3 (indicated ppm)
width height
max pixel value
r g b  r g b
r g b  r g b
etc...
*/

string col_to_string(color in)
{
    string out = "";
    out += to_string(in.r);
    out += " ";
    out += to_string(in.g);
    out += " ";
    out += to_string(in.b);
    return out;
}

void ppm_write(color *in[1000])
{
    //set up file
    string color = "255 0 0"; //red pixel to write
    string empty = "0 0 0"; //black pixel to write
    ofstream ppm;
    ppm.open("image.ppm");
    int width = 1000;
    int height = 1000;
    ppm << "P3\n";
    ppm << width << " " << height << "\n";
    ppm << "255\n";
    //now add actual pixels
    for (int row = 0; row != height; row++)
    {
        for (int col = 0; col != width; col++)
        {
            ppm << col_to_string(in[row][col]) << "    ";
        }
        ppm << "\n";
    }
    ppm.close();
}

int main()
{
    vector vect;
    vect.x = 1;
    vector cent =
    {
        14.0,
        0.0,
        0.0
    };
    sphere sp =
    {
        cent,
        5.0,
        PRETTY
    };

    vector light = //light source
    {
        0.0,
        0.0,
        4.0
    };

    vector hit; //place of contact
    vector shadow; //will draw shadow ray
    
    sphere shapes[1];
    shapes[0] = sp;

    color **screen = new color*[1000];
    for (int i = 0; i < 1000; i++)
    {
        screen[i] = new color[1000];
    } 

    for (int row = 0; row != 1000; row++)
    {
        for (int col = 0; col != 1000; col++)
        {
            vect.y = (2.0/1000)*(row - 500); //centers grid at y=0
            vect.z = (2.0/1000)*(col - 500);
            for (auto shape : shapes)
            {
                if (is_intersect(vect, shape))
                {
                    //must find d! actual vector
                    hit = find_hit(vect, shape);
                    shadow = v_sub(hit, light);
                    shadow.tail = vect_to_point(hit);
                    if (!is_intersect_shifted(shadow, shape))
                    {
                        screen[row][col] = sp.scolor;
                    } else
                    {
                        screen[row][col] = BLUE;
                    }
                }
            }
            /*
            if (is_intersect(vect, sp))
            {
                screen[row][col] = sp.scolor;
            }
            */
        }
    }

    /*
    for (int row = 0; row != 1000; row++)
    {
        for (int col = 0; col != 1000; col++)
        {
            vect.y = (2.0/1000)*(row - 500); //centers grid at y=0
            vect.z = (2.0/1000)*(col - 500);
            //vect.x = sqrt(1.0 - pow(vect.y, 2.0) - pow(vect.z, 2.0));
            //screen[row][col] = is_intersect(vect, sp);
            if (is_intersect(vect, sp))
            {
                screen[row][col] = sp.scolor;
            }
        }
    }
    */
    ppm_write(screen);
    return 0;
}
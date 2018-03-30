#include <iostream>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

struct vector
{
    float x,y,z; 
};

struct sphere
{
    vector center;
    float radius;
};

//empty vector, at origin
const vector EMPTY =
{
    0.0,
    0.0,
    0.0
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

bool is_intersect(vector v, sphere s) //calculates using quadratic discriminant, simplifying to: (v * o-c)^2 - ||o-c||^2 + r^2, true if >=0
// as long as v is unit vector!!
{
    v = v_unit(v);
    float calc = (pow(dot_prod(v, (v_sub(s.center, EMPTY))), 2.0) - pow(v_len(v_sub(EMPTY, s.center)), 2.0) + pow(s.radius, 2.0));
    return (calc >= 0.0);
}

/*
bool is_intersect_2(vector v, sphere s)
{
    v = v_unit(v);
    float r_2 = pow(s.radius, 2.0);
    vector L = v_sub(s.center, EMPTY);
    float t_ca = dot_prod(L, v);
    if (t_ca < 0.0) return false;
    //if d^2 < radius^2 then false
    float d_2 = dot_prod(L, L) - pow(t_ca, 2.0);
    if (d_2 > r_2)
    {
        return false;
    } else
    {
        return true;
    }
}
*/

/* 
ppm format:
P3 (indicated ppm)
width height
max pixel value
r g b  r g b
r g b  r g b
etc...
*/

void ppm_write(bool in[][1000])
{
    //set up file
    string color = "255 0 0"; //red pixel to write
    string empty = "0 0 0"; //black pixel to write
    ofstream ppm;
    ppm.open("image.ppm");
    //int width = sizeof(in[0])/sizeof(*in[0]);
    //int height = sizeof(in)/sizeof(*in);
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
            if (in[row][col])
            {
                ppm << color << "   ";
            } else
            {
                ppm << empty << "   ";
            }
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
        6.0,
        2.0
    };
    sphere sp =
    {
        cent,
        4.0
    };
    bool screen[1000][1000];
    for (int row = 0; row != 1000; row++)
    {
        for (int col = 0; col != 1000; col++)
        {
            vect.y = (2.0/1000)*(row - 500); //centers grid at y=0
            vect.z = (2.0/1000)*(col - 500);
            //vect.x = sqrt(1.0 - pow(vect.y, 2.0) - pow(vect.z, 2.0));
            screen[row][col] = is_intersect(vect, sp);
        }
    }
    for (int row = 0; row != sizeof(screen)/sizeof(*screen); row++)
    {
        for (int col = 0; col != sizeof(screen[row])/sizeof(*screen[row]); col++)
        {
            if (screen[row][col])
            {
                cout << "#";
            } else
            {
                cout << ".";
            }
        }
        cout << "\n";
    }
    ppm_write(screen);
    return 0;
}
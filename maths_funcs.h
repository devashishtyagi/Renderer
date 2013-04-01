#ifndef MATHS_FUNCS_H
#define MATHS_FUNCS_H

struct vec2;
struct vec3;
struct vec4;

struct vec2 {
    vec2 ();
    vec2 (const float& x, const float& y);
    float v[2];
};

struct vec3 {
    vec3 ();
    vec3 (const float& x, const float& y, const float& z);
    vec3 (const vec2& vv, const float& z);
    vec3 (const vec4& vv);
    vec3 operator+ (const vec3& rhs);
    vec3 operator- (const vec3& rhs);
    float v[3];
};

struct vec4 {
    vec4 ();
    vec4 (const float& x, const float& y, const float& z, const float& w);
    vec4 (const vec2& vv, const float& z, const float& w);
    vec4 (const vec3& vv, const float& w);
    float v[4];
};

/* stored like this:
0 3 6
1 4 7
2 5 8 */
struct mat3 {
    mat3 ();
    mat3 (const float& a, const float& b, const float& c,
                const float& d, const float& e, const float& f,
                const float& g, const float& h, const float& i);
    float m[9];
};

/* stored like this:
0 4 8  12
1 5 9  13
2 6 10 14
3 7 11 15*/
struct mat4 {
    mat4 ();
    mat4 (const float& a, const float& b, const float& c, const float& d,
                const float& e, const float& f, const float& g, const float& h,
                const float& i, const float& j, const float& k, const float& l,
                const float& mm, const float& n, const float& o, const float& p);
    vec4 operator* (const vec4& rhs);
    mat4 operator* (const mat4& rhs);
    mat4& operator= (const mat4& rhs);
    float m[16];
};

struct versor {
    versor ();
    versor (const float& w, const float& x, const float& y, const float& z);
    float q[4];
};

void print (const vec2& v);
void print (const vec3& v);
void print (const vec4& v);
void print (const mat3& m);
void print (const mat4& m);
void print (const versor& q);
// vector functions
float length (const vec3& v);
float length2 (const vec3& v);
vec3 normalise (const vec3& v);
float dot (const vec3& a, const vec3& b);
vec3 cross (const vec3& a, const vec3& b);
// matrix functions
mat3 zero_mat3 ();
mat3 identity_mat3 ();
mat4 zero_mat4 ();
mat4 identity_mat4 ();
float determinant (const mat4& mm);
mat4 inverse (const mat4& mm);
mat4 transpose (const mat4& mm);
// affine functions
mat4 translate (const mat4& m, const vec3& v);
mat4 rotate_x_deg (const mat4& m, const float& deg);
mat4 rotate_y_deg (const mat4& m, const float& deg);
mat4 rotate_z_deg (const mat4& m, const float& deg);
mat4 scale (const mat4& m, const vec3& v);
// camera functions
mat4 look_at (const vec3& cam_pos, vec3 targ_pos, const vec3& up);
mat4 perspective (const float& fovy, const float& aspect, const float& near, const float& far);
// quaternion functions
versor normalise (const versor& q);
mat4 versor_to_mat4 (const versor& q);

#endif // MATHS_FUNCS_H

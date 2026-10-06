#pragma once
#include <d3d8.h>
#include <cstring>

#define D3DX_PI 3.141592654f

struct D3DXVECTOR3 : D3DVECTOR {
    D3DXVECTOR3() {}
    D3DXVECTOR3(float xx, float yy, float zz) { x=xx; y=yy; z=zz; }
    D3DXVECTOR3(const float* v) { x=v[0]; y=v[1]; z=v[2]; }
    D3DXVECTOR3(const D3DVECTOR& v) { x=v.x; y=v.y; z=v.z; }
    operator float*() { return &x; }
    operator const float*() const { return &x; }
    D3DXVECTOR3 operator+() const { return *this; }
    D3DXVECTOR3 operator-() const { return {-x,-y,-z}; }
    D3DXVECTOR3 operator+(const D3DXVECTOR3& v) const { return {x+v.x,y+v.y,z+v.z}; }
    D3DXVECTOR3 operator-(const D3DXVECTOR3& v) const { return {x-v.x,y-v.y,z-v.z}; }
    D3DXVECTOR3 operator*(float f) const { return {x*f,y*f,z*f}; }
    D3DXVECTOR3 operator/(float f) const { return *this*(1.0f/f); }
    D3DXVECTOR3& operator+=(const D3DXVECTOR3& v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
    D3DXVECTOR3& operator-=(const D3DXVECTOR3& v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
    D3DXVECTOR3& operator*=(float f) { x*=f; y*=f; z*=f; return *this; }
    D3DXVECTOR3& operator/=(float f) { return *this*=1.0f/f; }
    bool operator==(const D3DXVECTOR3& v) const { return x==v.x && y==v.y && z==v.z; }
    bool operator!=(const D3DXVECTOR3& v) const { return !(*this==v); }
};
inline D3DXVECTOR3 operator*(float f,const D3DXVECTOR3& v) { return v*f; }

struct D3DXVECTOR4 {
    float x,y,z,w;
    D3DXVECTOR4() {}
    D3DXVECTOR4(float xx,float yy,float zz,float ww):x(xx),y(yy),z(zz),w(ww) {}
    D3DXVECTOR4(const float* v):x(v[0]),y(v[1]),z(v[2]),w(v[3]) {}
    operator float*() { return &x; }
    operator const float*() const { return &x; }
    D3DXVECTOR4 operator+() const { return *this; }
    D3DXVECTOR4 operator-() const { return {-x,-y,-z,-w}; }
    D3DXVECTOR4 operator+(const D3DXVECTOR4& v) const { return {x+v.x,y+v.y,z+v.z,w+v.w}; }
    D3DXVECTOR4 operator-(const D3DXVECTOR4& v) const { return {x-v.x,y-v.y,z-v.z,w-v.w}; }
    D3DXVECTOR4 operator*(float f) const { return {x*f,y*f,z*f,w*f}; }
    D3DXVECTOR4 operator/(float f) const { return *this*(1.0f/f); }
    D3DXVECTOR4& operator+=(const D3DXVECTOR4& v) { x+=v.x; y+=v.y; z+=v.z; w+=v.w; return *this; }
    D3DXVECTOR4& operator-=(const D3DXVECTOR4& v) { x-=v.x; y-=v.y; z-=v.z; w-=v.w; return *this; }
    D3DXVECTOR4& operator*=(float f) { x*=f; y*=f; z*=f; w*=f; return *this; }
    D3DXVECTOR4& operator/=(float f) { return *this*=1.0f/f; }
    bool operator==(const D3DXVECTOR4& v) const { return x==v.x && y==v.y && z==v.z && w==v.w; }
    bool operator!=(const D3DXVECTOR4& v) const { return !(*this==v); }
};
inline D3DXVECTOR4 operator*(float f,const D3DXVECTOR4& v) { return v*f; }

struct D3DXMATRIX : D3DMATRIX {
    D3DXMATRIX() {}
    D3DXMATRIX(const D3DMATRIX& matrix) { std::memcpy(m,matrix.m,sizeof(m)); }
    D3DXMATRIX(const float* values) { std::memcpy(m,values,sizeof(m)); }
    D3DXMATRIX(float a,float b,float c,float d,float e,float f,float g,float h,
        float i,float j,float k,float l,float n,float o,float p,float q) {
        const float values[16]{a,b,c,d,e,f,g,h,i,j,k,l,n,o,p,q};
        std::memcpy(m,values,sizeof(m));
    }
    operator float*() { return &m[0][0]; }
    operator const float*() const { return &m[0][0]; }
    float& operator()(UINT row,UINT column) { return m[row][column]; }
    float operator()(UINT row,UINT column) const { return m[row][column]; }
    D3DXMATRIX operator*(const D3DXMATRIX&) const;
    D3DXMATRIX& operator*=(const D3DXMATRIX&);
};

extern "C" {
D3DXMATRIX* WINAPI D3DXMatrixMultiply(D3DXMATRIX*,const D3DXMATRIX*,const D3DXMATRIX*);
D3DXMATRIX* WINAPI D3DXMatrixInverse(D3DXMATRIX*,float*,const D3DXMATRIX*);
D3DXMATRIX* WINAPI D3DXMatrixTranspose(D3DXMATRIX*,const D3DXMATRIX*);
D3DXMATRIX* WINAPI D3DXMatrixScaling(D3DXMATRIX*,float,float,float);
D3DXMATRIX* WINAPI D3DXMatrixTranslation(D3DXMATRIX*,float,float,float);
D3DXMATRIX* WINAPI D3DXMatrixRotationZ(D3DXMATRIX*,float);
D3DXVECTOR4* WINAPI D3DXVec3Transform(D3DXVECTOR4*,const D3DXVECTOR3*,const D3DXMATRIX*);
D3DXVECTOR4* WINAPI D3DXVec4Transform(D3DXVECTOR4*,const D3DXVECTOR4*,const D3DXMATRIX*);
}
inline D3DXMATRIX D3DXMATRIX::operator*(const D3DXMATRIX& right) const {
    D3DXMATRIX result; D3DXMatrixMultiply(&result,this,&right); return result;
}
inline D3DXMATRIX& D3DXMATRIX::operator*=(const D3DXMATRIX& right) {
    D3DXMatrixMultiply(this,this,&right); return *this;
}
inline D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* matrix) {
    if (!matrix) return nullptr;
    matrix->_11=matrix->_22=matrix->_33=matrix->_44=1.0f;
    matrix->_12=matrix->_13=matrix->_14=matrix->_21=matrix->_23=matrix->_24=
        matrix->_31=matrix->_32=matrix->_34=matrix->_41=matrix->_42=matrix->_43=0.0f;
    return matrix;
}
inline float D3DXVec4Dot(const D3DXVECTOR4* a,const D3DXVECTOR4* b) {
    return a->x*b->x+a->y*b->y+a->z*b->z+a->w*b->w;
}
static_assert(sizeof(D3DXVECTOR3)==12 && sizeof(D3DXVECTOR4)==16 && sizeof(D3DXMATRIX)==64);

#include "d3dx8.h"

/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

void PointLight::shade( Ray3D& ray) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  
	Vector3D n = ray.intersection.normal;
	n.normalize();
	Point3D point = ray.intersection.point;
	Vector3D s = _pos - point;
	s.normalize();
	Vector3D m = 2*(n.dot(s)) * n - s;
	m.normalize();
	Vector3D d = -ray.dir;
	d.normalize();

	Colour output;
	Colour RaIa = ray.intersection.mat->ambient * _col_ambient;
	Colour RsIsmax = pow((fmax(0, m.dot(d))), (*ray.intersection.mat).specular_exp) * ray.intersection.mat->specular * _col_specular;
	Colour RdIdmax = fmax(0, s.dot(n)) * (ray.intersection.mat->diffuse) * _col_diffuse ;
	
	// Render type: Scene signature
	//output = ray.col + ray.intersection.mat->diffuse;
	
	// Render type: Diffuse and ambient
	//output = RaIa + RdIdmax;
	
	// Render type: Phong
	output = RaIa + RsIsmax + RdIdmax;
	
	output.clamp();
	ray.col = output;



}


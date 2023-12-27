#pragma once
#include <tuple>
#include <vector>
#include "cvdface.h"
#include <numeric>
using  ovimgsize = std::tuple<int, int>;
int DetectObjectsExtOV(uint8_t* red, uint8_t* blue, uint8_t* green, int size, float kof, int add_y, FRECT* rect, int& rsz);
void initOpenVinoModel(std::string wpath);
void CloseOpenVinoModel();
ovimgsize GetInferSize();
std::string GetOVString();
template<class T>
inline T GetShapeSize(std::vector<T> shape) {
	T size = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<T>());
	return size;
}

using CnnShape = std::tuple<int, std::vector<int>>;
typedef struct _CNNINFOSTRUCT {
	enum CNN_TYPE {
	  UNDEFINED,
	  FACE_DETECTION,	 // image
	  AGE_DETECTION,
	  FACE_PARAMETERS,		// data
	  CLASS_SELECTION,		// input
	  CLASS_PARAMETERS
	};
	int Type = CNN_TYPE::UNDEFINED;
	std::vector< std::vector<int> > inShapes{ {} };
	std::vector< std::vector<int> > outShapes{ {} };
	std::vector<std::string> inNames;
	std::vector<std::string> outNames;
	std::vector<int> inTypes;
	std::vector<int> outTypes;
	int GetTypeCNN() {
		Type = CNN_TYPE::UNDEFINED;
		if (inShapes.size() != 1) return Type;
		if (inShapes[0].size() == 4) {
			if (inNames[0] == "image") { Type = CNN_TYPE::FACE_DETECTION; }
			if (inNames[0] == "input") { Type = CNN_TYPE::CLASS_SELECTION; }
			if (outShapes.size() == 1) {
				if (outNames[0] == "descriptor"){
					Type = CNN_TYPE::CLASS_PARAMETERS; return Type;
				} 
			}
			if (GetShapeSize(outShapes[0]) == 256) Type = CNN_TYPE::FACE_PARAMETERS;
			if (outShapes.size() == 2) {
				if (GetShapeSize(outShapes[0]) == 2 && GetShapeSize(outShapes[1]) == 1)
					Type = CNN_TYPE::AGE_DETECTION;
			}

		}
		return Type;
	};
	void Clear() {
		Type = CNN_TYPE::UNDEFINED;
		inShapes.clear();
		outShapes.clear();
		inNames.clear();
		outNames.clear();
		inTypes.clear();
		outTypes.clear();
	}
}CNNINFOSTRUCT;

std::string GetModelInformation(std::string path);
void FillModelInformation(std::string path, CNNINFOSTRUCT& minfo);
std::string sModelParameters(CNNINFOSTRUCT & minfo);




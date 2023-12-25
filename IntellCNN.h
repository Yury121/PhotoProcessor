#pragma once
#include <tuple>
#include <vector>
#include "cvdface.h"
using  ovimgsize = std::tuple<int, int>;
int DetectObjectsExtOV(uint8_t* red, uint8_t* blue, uint8_t* green, int size, float kof, int add_y, FRECT* rect, int& rsz);
void initOpenVinoModel(std::string wpath);
void CloseOpenVinoModel();
ovimgsize GetInferSize();
std::string GetOVString();

using CnnShape = std::tuple<int, std::vector<int>>;

std::string GetModelInformation(std::string path);




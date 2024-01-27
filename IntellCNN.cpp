#pragma once
#define NOMINMAX
#include <cstdio>
#include "cvdface.h"
#include "umodels.h"

////https://docs.openvino.ai/2023.2/omz_demos_smart_classroom_demo_cpp.html#doxid-omz-demos-smart-classroom-demo-cpp

#ifdef _DEBUG
#pragma comment(lib, "./lib/openvinod.lib")
#else
#pragma comment(lib, "./lib/openvino.lib")
#endif
//#pragma comment(lib, "./lib/openvino_onnx_frontend.lib")
//#pragma comment(lib, "./lib/tbb.lib")


std::string GetModelInformation(std::string path)
{
	std::string info;
	COVModel model(path);
	if (model.ov_err.empty()) {
		info = model.GetInputInfo();
		info += "\n" + model.GetOutputInfo();
	}
	else info = model.ov_err;
	return info;
}

void FillModelInformation(std::string path, CNNINFOSTRUCT& minfo)
{
	minfo.Clear();
	COVModel model(path);
	if (model.ov_err.empty()) {
		minfo.inNames = model.GetInputNames();
		minfo.outNames = model.GetOutputNames();
		std::vector<CnnShape> shapes = model.GetInputShape();
		for (auto&& ii : shapes) {
			minfo.inShapes.push_back(std::get<1>(ii));
			minfo.inTypes.push_back(std::get<0>(ii));
		}
		shapes = model.GetOutputShape();
		for (auto&& ii : shapes) {
			minfo.outShapes.push_back(std::get<1>(ii));
			minfo.outTypes.push_back(std::get<0>(ii));
		}
		minfo.Type = minfo.GetTypeCNN();
	}
}

std::string sModelParameters(CNNINFOSTRUCT& minfo)
{
	std::string sinfo = "";
	switch (minfo.Type) {
	case 1:
		sinfo = "Face recognition model\r\n";
		break;
	case 2:
		sinfo = "Age detection model\r\n";
		break;
	case 3:
		sinfo = "Model for determining facial parameters\r\n";
		break;
	case 4:
		sinfo = "Object(person) recognition model\r\n";
		break;
	case 5:
		sinfo = "Model for determining object(person) parameters\r\n";
		break;
	default:
		sinfo = "Undefined model\r\n";
		break;
	}
	sinfo += "Input nodes = " + std::to_string(minfo.inShapes.size()) + "\r\n  Node:\r\n";
	for (int i = 0; i < minfo.inShapes.size(); i++) {
		sinfo += "\t" + minfo.inNames[i] + ";  Type: ";
		sinfo += (minfo.inTypes[i] == int(ov::element::Type_t::f32)) ? " float; shape: { " :
			(minfo.inTypes[i] == int(ov::element::Type_t::u8)) ? "uint_8; shape: { " : " not suported; shape: { ";
		for (auto&& cc : minfo.inShapes[i]) {
			sinfo += std::to_string(cc) + " ";
		}
		sinfo += "};\r\n";
	}
	sinfo += "Output nodes = " + std::to_string(minfo.outShapes.size());
	sinfo += "\r\n Nodes:\r\n";
	for (int i = 0; i < minfo.outShapes.size(); i++) {
		sinfo += "\t" + minfo.outNames[i] + ";  Type: ";
		sinfo += (minfo.outTypes[i] == int(ov::element::Type_t::f32)) ? " float; shape: { " :
			(minfo.outTypes[i] == int(ov::element::Type_t::u8)) ? "uint_8; shape: { " :
			(minfo.outTypes[i] == int(ov::element::Type_t::i32)) ? "int; shape: { " : " not suported; shape: { ";
		for (auto&& cc : minfo.outShapes[i]) {
			sinfo += std::to_string(cc) + " ";
		}
		sinfo += "};\r\n";
	}
	return sinfo;
}



static COpenVinoModel* m_ov = nullptr;


void initOpenVinoModel(std::string wpath)
{
	std::string path_to_model = wpath ;
	if (m_ov) {
		if (m_ov->GetModel() != nullptr) return;
	}
	m_ov = new COpenVinoModel(path_to_model);
}

void CloseOpenVinoModel()
{
	if (m_ov) delete m_ov;
	m_ov = nullptr;
}

ovimgsize GetInferSize()
{
	if (m_ov) return m_ov->GetSizes();
	return { 0,0 };
}

std::string GetOVString() {
	std::string sout = "Memory error";
	if (m_ov) {
		return m_ov->ov_err;
	}
	return sout;
}


int DetectObjectsExtOV(uint8_t* red, uint8_t* blue, uint8_t* green, int size, float kof, int add_y, FRECT* rect, int& rsz)
{
	std::vector< ovector> out_v;
	int count = out_v.size();
	void* data = nullptr;
#ifdef _M_X64
	if (m_ov) {
		ov::Shape shape = m_ov->GetInputShape();
		int height = int(shape[2]);
		int sz = shape[2] * shape[3];
		if (m_ov->GetType() == ov::element::Type_t::u8) {
			data =  malloc(sz * 3);// *sizeof(float));
			uint8_t* data_t = (uint8_t*)data;	   
			if (data) {
				//			memcpy(data, blue, size);
				//			memcpy(data+size, green, size);
				//			memcpy(data+2*size, red, size);
				for (int i = height - 1; i >= 0; i--) {
					memcpy(data_t + i * shape[2], blue + sz - (i + 1) * shape[2], shape[2]);
					memcpy(data_t + sz + i * shape[2], green + sz - (i + 1) * shape[2], shape[2]);
					memcpy(data_t + 2 * size + i * shape[2], red + sz - (i + 1) * shape[2], shape[2]);
				}
				out_v = m_ov->RunInfer((uint8_t*)data);
			}
		}
		if (m_ov->GetType() == ov::element::Type_t::f32) {
			data = malloc(sz * 3 * sizeof(float));// *sizeof(float));
			float * data_t = (float*) data;
			if (data) {
				//			memcpy(data, blue, size);
				//			memcpy(data+size, green, size);
				//			memcpy(data+2*size, red, size);
				for (int i = height - 1; i >= 0; i--) {
					for (int j = 0; j < shape[2]; j++) {
						data_t[i * shape[2] + j] = 1.f*blue[sz - (i + 1) * shape[2] + j];
						data_t[sz + i * shape[2] + j] =1.f* green[sz - (i + 1) * shape[2] + j];
						data_t[2 * sz + i * shape[2] + j] =1.f* red[sz - (i + 1) * shape[2] + j];

					}
				}
				out_v = m_ov->RunInfer((float*)data);
			}
		}
		if (data){
			free(data);
			count = (int(out_v.size()) > rsz) ? rsz : out_v.size();
			for (int i = 0; i < count; i++) {
				rect[i].x = int(kof * std::get<0>(out_v[i]));
				rect[i].y = int(kof *(shape[3]- std::get<1>(out_v[i])));// +add_y;
				rect[i].width = int(kof * (std::get<2>(out_v[i]) - std::get<0>(out_v[i])) );
				rect[i].height = int(kof * (std::get<3>(out_v[i]) - std::get<1>(out_v[i])));
			}
		}
	}


#endif
	rsz = count;

	return count;
}

int IncreaseResalution(std::vector<uint8_t*>& vImage, std::vector<uint8_t*> vOutput)
{
	return 0;
}
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

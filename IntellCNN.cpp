#pragma once
#define NOMINMAX
#include <cstdio>
#include "cvdface.h"
#include <time.h>
#include <openvino/openvino.hpp>
#include <fstream>
#include "IntellCNN.h"

#ifdef DEBUG
#pragma comment(lib, "./lib/openvinod.lib")
#else
#pragma comment(lib, "./lib/openvino.lib")
#endif
//#pragma comment(lib, "./lib/openvino_onnx_frontend.lib")
//#pragma comment(lib, "./lib/tbb.lib")

double timedif(timespec& start, timespec& end) {
	double dt1 = 1.0 * double(start.tv_sec) + 0.000000001 * double(start.tv_nsec);
	double dt2 = 1.0 * double(end.tv_sec) + 0.000000001 * double(end.tv_nsec);
	return (dt2 - dt1);	   //sec
}//end diff

using ovector = std::tuple<float, float, float, float>;




class COpenVinoModel {
public:
	COpenVinoModel() {};
	COpenVinoModel(std::string path) {
		try {
			model = core.read_model(path);

			compiledModel = core.compile_model(model, "CPU");
			infer_request = compiledModel.create_infer_request();
			auto input_port = compiledModel.input();
			ov::element::Type tt = input_port.get_element_type();
			input_port.get_tensor().set_element_type(ov::element::u8);
			shape = input_port.get_shape();
		}
		catch (ov::Exception& wcp) {
			ov_err = "\nException "; 	ov_err += (char*)wcp.what();
			model.reset();
		}

	};
	~COpenVinoModel() { model.reset(); };
	inline ov::Model* GetModel() { return model.get(); };
	inline ovimgsize GetSizes() {
		ov_err = "";
		if (shape.size() == 4) {
			return { shape[2], shape[3] };
		}
		ov_err = "shape is not image";
		return { 0,0 };
	};
	inline ov::Shape GetInputShape() { return shape; };
	inline std::vector<ovector> RunInfer(uint8_t* data) {
		std::vector<ovector> out_v;
		
		if (model.get() == nullptr) {
			ov_err = "model_not set";
			return out_v;
		}
		ov_err = "";
		timespec t_start, t_stop;
		try {
			
			ov::Tensor input_tensor(ov::element::u8, shape, (uint8_t*)data);
			infer_request.set_input_tensor(input_tensor);
			timespec_get(&t_start, TIME_UTC);
			infer_request.infer();
			timespec_get(&t_stop, TIME_UTC);

			ov::Tensor output_tensor = infer_request.get_output_tensor();
			ov::Shape o_shape = output_tensor.get_shape();
			if (o_shape.size() == 4) {
				auto out_data = output_tensor.data<float>();
				for (int i = 0; i < o_shape[2]; i++) {
					float* ptr = &out_data[7 * i];
					if (ptr[2] >= 0.5f) {
						out_v.push_back({ ptr[3]*shape[2], ptr[4]*shape[3], ptr[5]*shape[2],ptr[6]*shape[3]});
					}

				}

			}
			
			std::cout << "\nModel inference execution time " << timedif(t_start, t_stop) * 1000. << " ms\n";
		}
		catch (ov::Exception& wcp) {
			ov_err = "\nException "; 	ov_err += (char*)wcp.what();
//			model.reset();
		}

		return out_v;
	}

private :
	std::string ov_err = "";
	ov::Core core;
	std::shared_ptr<ov::Model> model;
	ov::CompiledModel compiledModel;
	ov::Shape shape;
	ov::InferRequest infer_request;

};

static COpenVinoModel* m_ov = nullptr;


void initOpenVinoModel(std::string wpath)
{
	std::string path_to_model = wpath + "/face-detection-0202.xml";
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


int DetectObjectsExtOV(uint8_t* red, uint8_t* blue, uint8_t* green, int size, float kof, int add_y, FRECT* rect, int& rsz)
{
	std::vector< ovector> out_v;
	int count = out_v.size();
#ifdef _M_X64
	if (m_ov) {
		uint8_t* data = (uint8_t*)malloc(size * 3);// *sizeof(float));
		ov::Shape shape = m_ov->GetInputShape();
		
		int height = int(shape[3]) ;

		if (data) {
//			memcpy(data, blue, size);
//			memcpy(data+size, green, size);
//			memcpy(data+2*size, red, size);
			for (int i = height-1; i >= 0; i--) {
				memcpy(data + i * shape[2], blue + size - (i + 1) * shape[2], shape[2]);
				memcpy(data + size + i * shape[2], green + size - (i + 1) * shape[2], shape[2]);
				memcpy(data + 2*size + i * shape[2], red + size - (i + 1) * shape[2], shape[2]);
			}
			out_v = m_ov->RunInfer((uint8_t*)data);
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

#pragma once
#include <time.h>
#include <openvino/openvino.hpp>
#include <fstream>
#include "IntellCNN.h"
inline double timedif(timespec& start, timespec& end) {
	double dt1 = 1.0 * double(start.tv_sec) + 0.000000001 * double(start.tv_nsec);
	double dt2 = 1.0 * double(end.tv_sec) + 0.000000001 * double(end.tv_nsec);
	return (dt2 - dt1);	   //sec
}//end diff

using ovector = std::tuple<float, float, float, float>;

class COVModel {
public:

	COVModel() {};
	COVModel(std::string path) {
		try {
			ov::CompiledModel cmodel = core.compile_model(path);

//			Model = core.read_model(path);
			//int cnt =  Model->get_output_size();
			auto outN = cmodel.outputs();
			//for (int i = 0; i < cnt; i++) {
			for (int i = 0; i < outN.size(); i++) {
				CnnShape oshape;
				ov::element::Type_t el = outN[i].get_element_type();// Model->get_output_element_type(i);
				std::get<0>(oshape) = int(el);
				std::get<1>(oshape).clear();
				//ov::Shape pshape = Model->get_output_partial_shape(i).get_shape();
				ov::Shape pshape = outN[i].get_partial_shape().get_max_shape();
				for (int j = 0; j < pshape.size(); j++) {
					std::get<1>(oshape).push_back(int(pshape[j]));
				}
				if (std::get<1>(oshape).size() > 0) outShape.push_back(oshape);
			}
			//ov::CompiledModel cmodel = core.compile_model(path);
			auto inN = cmodel.inputs();
			for (int i = 0; i < inN.size(); i++) {
				CnnShape oshape;
				ov::element::Type_t el = inN[i].get_element_type();
				std::get<0>(oshape) = int(el);
				std::get<1>(oshape).clear();
				ov::Shape pshape = inN[i].get_partial_shape().get_max_shape();
				for (int j = 0; j < pshape.size(); j++) { std::get<1>(oshape).push_back(int(pshape[j])); }
				if (std::get<1>(oshape).size() > 0) inShape.push_back(oshape);
			}
		}
		catch (ov::Exception& wcp) {
			ov_err = "\nException "; 	ov_err += (char*)wcp.what();
			Model.reset();
		}
	};
	~COVModel() { Model.reset(); };
	inline int GetInputSize() { return inShape.size(); };
	inline std::string GetInputInfo() {
		std::string info = "Input nodes = " + std::to_string(inShape.size());
		info += "\r\n Node : ";
		for (int i = 0; i < inShape.size(); i++) {
			info += std::to_string(i + 1) + ";  Type: ";
			info += (std::get<0>(inShape[i]) == int(ov::element::Type_t::f32)) ? " float; shape: { " :
				(std::get<0>(inShape[i]) == int(ov::element::Type_t::u8)) ? "uint_8; shape: { " : " not suported; shape: { ";
			for (auto&& cc : std::get<1>(inShape[i])) {
				info += std::to_string(cc) + " ";
			}
			info += "};\r\n";
		}
		return info;
	};
	inline std::string GetOutputInfo() {
		std::string info = "Output nodes = " + std::to_string(outShape.size());
		info += "\r\n Node : ";
		for (int i = 0; i < outShape.size(); i++) {
			info += std::to_string(i + 1) + ";  Type: ";
			info += (std::get<0>(outShape[i]) == int(ov::element::Type_t::f32)) ? " float; shape: { " :
				(std::get<0>(outShape[i]) == int(ov::element::Type_t::u8)) ? "uint_8; shape: { " :
				(std::get<0>(outShape[i]) == int(ov::element::Type_t::i32)) ? "int; shape: { " : " not suported; shape: { ";
			for (auto&& cc : std::get<1>(outShape[i])) {
				info += std::to_string(cc) + " ";
			}
			info += "};\r\n";
		}
		return info;
	};


	std::string ov_err = "";

private:
	ov::Core core;
	std::shared_ptr<ov::Model> Model;
	std::vector<CnnShape> inShape;
	std::vector<CnnShape> outShape;


};

class COpenVinoModel {
public:
	std::string ainet_name = "DLL/FP32/face-detection-0204.xml";//"DLL/FP32/person-detection-0200.xml";// "DLL/face-detection-0204.xml";// ";// "Bin / 0200.xml";  face-detection-retail-0004.xml

	COpenVinoModel() {};
	COpenVinoModel(std::string path) {
		try {
			std::string mpath = path + ainet_name;

			model = core.read_model(mpath);

			m_szout = model->get_output_size();
			ov::PartialShape lshape;
			int osize = 0;
			std::string oname;


			compiledModel = core.compile_model(model);
			infer_request = compiledModel.create_infer_request();
			auto input_port = compiledModel.input();
			/*
						auto a = compiledModel.outputs();
						osize = a.size();
						for (size_t i = 0; i < m_szout; i++) {
							lshape = a[i].get_partial_shape();
							osize = lshape.size();
							oname = a[i].get_any_name();


						}
			*/

			m_tt = input_port.get_element_type();
			//input_port.get_tensor().set_element_type(ov::element::u8);
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
	template <typename T>
	inline std::vector<ovector> RunInfer(T* data) {
		std::vector<ovector> out_v;

		if (model.get() == nullptr) {
			ov_err = "model_not set";
			return out_v;
		}
		ov_err = "";
		timespec t_start, t_stop;
		try {
			ov::Tensor input_tensor(m_tt, shape, (T*)data);
			infer_request.set_input_tensor(input_tensor);
			timespec_get(&t_start, TIME_UTC);
			infer_request.infer();
			timespec_get(&t_stop, TIME_UTC);
			ov::Tensor output_tensor;
			ov::Shape o_shape;
			if (m_szout == 1) {
				output_tensor = infer_request.get_output_tensor();
				o_shape = output_tensor.get_shape();
			}
			else {
				output_tensor = infer_request.get_output_tensor(1);
				o_shape = output_tensor.get_shape();
			}
			if (o_shape.size() == 4) {
				auto out_data = output_tensor.data<float>();
				for (int i = 0; i < o_shape[2]; i++) {
					float* ptr = &out_data[7 * i];
					if (ptr[2] >= 0.5f) {
						out_v.push_back({ ptr[3] * shape[2], ptr[4] * shape[3], ptr[5] * shape[2],ptr[6] * shape[3] });
					}

				}
			}
			if (o_shape.size() == 2) {
				auto out_data = output_tensor.data<float>();
				for (int i = 0; i < o_shape[0]; i++) {
					float* ptr = &out_data[5 * i];
					//if (ptr[4] >= 0.5f) {
					out_v.push_back({ ptr[0], ptr[1], ptr[2],ptr[3] });
					//}

				}

			}

			//std::cout << "\nModel inference execution time " << timedif(t_start, t_stop) * 1000. << " ms\n";
		}
		catch (ov::Exception& wcp) {
			ov_err = "\nException "; 	ov_err += (char*)wcp.what();
			//			model.reset();
		}

		return out_v;
	}
	std::string ov_err = "";
	inline ov::element::Type GetType() { return m_tt; };
private:

	ov::Core core;
	std::shared_ptr<ov::Model> model;
	ov::CompiledModel compiledModel;
	ov::Shape shape;
	ov::InferRequest infer_request;
	ov::element::Type m_tt;
	size_t m_szout = 0;

};

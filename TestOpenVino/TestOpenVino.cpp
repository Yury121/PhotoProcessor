// TestOpenVino.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdio>
#include <time.h>
#include <openvino/openvino.hpp>
#pragma comment(lib, "../lib/openvino.lib")

double timedif(timespec& start, timespec& end) {
	double dt1 = 1.0 * double(start.tv_sec) + 0.000000001 * double(start.tv_nsec);
	double dt2 = 1.0 * double(end.tv_sec) + 0.000000001 * double(end.tv_nsec);
	return (dt2 - dt1);	   //sec
}//end diff

static std::string ov_err = "";


int main()
{
    std::cout << "Hello World!\n";
	timespec t_start, t_stop;
	std::string path = "D:/Worker/Free2023/PhotoProcessor/Bin/face-detection-0202.xml"; // 
	std::string dir = "D:/Worker/Free2023/PhotoProcessor/Bin/face-detection-0202.bin";

	ov::Core core;
	uint8_t* data = nullptr;
	//std::string path = "D:\\Worker\\Free2023\\PhotoProcessor\DLL\\FP32\\mlp2_conv.onnx"; // 
	//std::string path = "face-detection-0202.xml"; // 
//	std::fstream ifs;
//	ifs.open(dir+path, std::ios::in | std::ios_base::binary);
//	if (ifs.is_open()) 		ifs.close();

	//ov::CompiledModel compiled_model = core.compile_model(path, "AUTO");
	try {
		std::shared_ptr<ov::Model> model = core.read_model("face-detection-0202.xml");
		std::cout << "Loaded!\n";
		ov::CompiledModel compiled_model = core.compile_model(model, "CPU");
		ov::InferRequest infer_request = compiled_model.create_infer_request();
		auto input_port = compiled_model.input();
		// Create tensor from external memory
		ov::Tensor input_tensor(input_port.get_element_type(), input_port.get_shape(), data);
		// Set input tensor for model with one input
		infer_request.set_input_tensor(input_tensor);
	}
	catch (ov::Exception& exc) {
		ov_err = "\nException ";
		ov_err += (char*)exc.what();
		std::cout << ov_err << "\n";
	}

#if 0	

	auto input_tensor1 = ov::Tensor(model->input().get_element_type(), { 10000000, 16 } /* { 1000, 16 }  {100, 1, 28, 28}*/);


	infer_request.set_input_tensor(input_tensor1);
	auto data1 = input_tensor1.data<float>();
	std::cout << "TInput size: " << input_tensor1.get_size() << "\n";
	for (size_t i = 0; i < input_tensor1.get_size(); i++) {
		data1[i] = 1.f * i;
	}
	infer_request.infer();
	timespec_get(&t_start, TIME_UTC);
	ov::Tensor output_tensor = infer_request.get_output_tensor();
	timespec_get(&t_stop, TIME_UTC);
	std::cout << "\nModel inference execution time " << timedif(t_start, t_stop) * 1000. << " ms\n";
#endif

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

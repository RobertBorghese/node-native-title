#include <cmath>
#include <stdio.h>
#include <string>

#include <nan.h>

#include "windows.h"

const char* ToCString(v8::Local<v8::String> str) {
	v8::String::Utf8Value value(str);
	return *value ? *value : nullptr;
}

std::string ConvertHWNDToString(HWND winId) {
	return std::to_string((long)winId);
}

NAN_METHOD(TestFunction) {
	v8::Local<v8::Function> cb = info[0].As<v8::Function>();

	Nan::Callback callback(cb);

	LPCSTR str = TEXT("Test");
	auto windowId = FindWindowA(NULL, str);

	v8::Local<v8::Value> argv[1] = {
		Nan::New(std::to_string((long)windowId)).ToLocalChecked()
	};

	Nan::AsyncResource resource("MyAsyncCall");
	callback.Call(Nan::GetCurrentContext()->Global(), 1, argv, &resource);

	/*
	const unsigned argc = 1;
	v8::Local<v8::Value> argv[argc] = {Nan::New("hello world").ToLocalChecked()};
	Nan::AsyncResource resource("nan:makeCallback");
	resource.runInAsyncScope(Nan::GetCurrentContext()->Global(), cb, argc, argv);
	*/

	SetWindowTextA(windowId, TEXT("HELLO!!!"));

	info.GetReturnValue().Set(Nan::New("You have grabbed a string.").ToLocalChecked());
}

NAN_METHOD(FindHWNDByTitle) {
	if(info.Length() >= 1) {
		v8::Local<v8::String> title = info[0].As<v8::String>();

		LPCSTR className = NULL;
		if(info.Length() >= 2) {
			v8::Local<v8::String> name = info[1].As<v8::String>();
			className = (LPCSTR)ToCString(name);
		}

		auto windowId = FindWindowA(className, (LPCSTR)ToCString(title));
		if(windowId) {
			info.GetReturnValue().Set(Nan::New(ConvertHWNDToString(windowId)).ToLocalChecked());
		} else {
			info.GetReturnValue().SetNull();
		}
	} else {
		info.GetReturnValue().SetNull();
	}
}

struct handle_data {
	unsigned long process_id;
	HWND window_handle;
};

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam) {
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id)
	    return TRUE;
	data.window_handle = handle;
	return FALSE;   
}

HWND find_main_window(unsigned long process_id) {
	handle_data data;
	data.process_id = process_id;
	data.window_handle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return data.window_handle;
}

NAN_METHOD(FindHWNDByPID) {
	v8::Local<v8::Int32> id = info[0].As<v8::Int32>();

	auto windowId = find_main_window(id->Value());
	if(windowId) {
		info.GetReturnValue().Set(Nan::New(ConvertHWNDToString(windowId)).ToLocalChecked());
	} else {
		info.GetReturnValue().SetNull();
	}
}

NAN_MODULE_INIT(Init) {
	Nan::Set(
		target,
		Nan::New("TestFunction").ToLocalChecked(),
		Nan::GetFunction(Nan::New<v8::FunctionTemplate>(TestFunction)).ToLocalChecked()
	);

	Nan::Set(
		target,
		Nan::New("FindHWNDByTitle").ToLocalChecked(),
		Nan::GetFunction(Nan::New<v8::FunctionTemplate>(FindHWNDByTitle)).ToLocalChecked()
	);

	Nan::Set(
		target,
		Nan::New("FindHWNDByPID").ToLocalChecked(),
		Nan::GetFunction(Nan::New<v8::FunctionTemplate>(FindHWNDByPID)).ToLocalChecked()
	);
}

NODE_MODULE(NodeNativeTitle, Init)

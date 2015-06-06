// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "dllmain.h"

FILE *f;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


BOOL __cdecl load(HGLOBAL h, long len){
	char* buf = new char[len + 1];
	memcpy(buf, h, len);
	buf[len] = 0;
	string param((char*)buf);
	GlobalFree(h);
	delete[] buf;

	SetCurrentDirectory(s2w(param).c_str());

	interp = Tcl_CreateInterp();
	Tcl_Eval(interp, "encoding system utf-8");
	Tcl_Eval(interp, "source index.tcl");

	cmd = Tcl_NewStringObj("_request", -1);
	arg = Tcl_NewStringObj(param.c_str(), -1);
	Tcl_Obj* objv[] = { cmd, arg };
	Tcl_EvalObjv(interp, 2, objv, TCL_EVAL_DIRECT);
	string ret=string(Tcl_GetStringResult(interp));


	if (ret.compare("true") == 0)return true;
	return false;
}


BOOL __cdecl unload(){
	Tcl_Eval(interp, "_unload");
	string ret = string(Tcl_GetStringResult(interp));
	Tcl_DeleteInterp(interp);

	if (ret.compare("true") == 0)return true;
	return false;
}
HGLOBAL __cdecl request(HGLOBAL h, long *len){

	char* buf = new char[*len + 1];
	memcpy(buf, h, *len);
	buf[*len] = 0;
	string param((char*)buf);
	GlobalFree(h);
	delete[] buf;
	string ret;




	cmd=Tcl_NewStringObj("_request", -1);
	arg=Tcl_NewStringObj(param.c_str(), -1);
	Tcl_Obj* objv[] = { cmd, arg };
	Tcl_EvalObjv(interp, 2, objv, TCL_EVAL_DIRECT);
	ret = string(Tcl_GetStringResult(interp));



	*len = ret.size() + 1;
	buf = (char*)GlobalAlloc(GMEM_FIXED, *len);
	memcpy(buf, ret.c_str(), *len);
//	((char*)buf)[*len - 1] = 0;
	return buf;
}


wstring s2w(string in, string cs){
	UINT csint;
	if (cs == "Shift_JIS"){
		csint = 932;
	}
	else if (cs == "ISO-2022-JP"){
		csint = 50221;
	}
	else if (cs == "EUC-JP"){
		csint = 51932;
	}
	else if (cs == "UTF-8"){
		csint = 65001;
	}
	else{
		csint = CP_ACP;
	}
	const int nSize = MultiByteToWideChar(csint, 0, (LPCSTR)in.c_str(), -1, NULL, 0);
	TCHAR* buff = new TCHAR[nSize + 1];
	MultiByteToWideChar(csint, 0, (LPCSTR)in.c_str(), -1, (LPWSTR)buff, nSize);
	wstring ret(buff);
	delete[] buff;
	return ret;
}

string w2s(wstring in, string cs){
	UINT csint;
	if (cs == "Shift_JIS"){
		csint = 932;
	}
	else if (cs == "ISO-2022-JP"){
		csint = 50221;
	}
	else if (cs == "EUC-JP"){
		csint = 51932;
	}
	else if (cs == "UTF-8"){
		csint = 65001;
	}
	else{
		csint = CP_ACP;
	}
	const int nSize = WideCharToMultiByte(csint, 0, (LPCWSTR)in.c_str(), -1, NULL, 0, NULL, NULL);
	BYTE* buff = new BYTE[nSize + 1];
	WideCharToMultiByte(csint, 0, (LPCWSTR)in.c_str(), -1, (LPSTR)buff, nSize, NULL, NULL);
	string ret = string((char*)buff);
	delete[] buff;
	return ret;
}
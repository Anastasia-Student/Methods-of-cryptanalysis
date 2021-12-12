#pragma once
#include "criterions.h"
#include <msclr\marshal_cppstd.h>
#include <string>

using namespace std;
using namespace msclr::interop;
using namespace System;
using namespace System::Text;
using namespace System::IO;
using namespace System::IO::Compression;

ComressionResult gzip(const string& text)
{
	ASCIIEncoding^ encoding = gcnew ASCIIEncoding();
	auto bytes = encoding->GetBytes(gcnew String(text.c_str()));
	MemoryStream^ compressStream = gcnew MemoryStream();
	GZipStream^ gzip = gcnew GZipStream(compressStream, CompressionMode::Compress);
	gzip->Write(bytes, 0, bytes->Length);
	gzip->Close();
	ComressionResult result;
	result.complited = true;
	result.size = text.size() * 8;
	result.compressedSize = compressStream->ToArray()->Length * 8;
	return result;
}

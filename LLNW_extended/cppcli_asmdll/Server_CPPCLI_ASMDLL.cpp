/********************************************************************
* N.Kozak // Lviv'2019 // example server .NET(LLNW)-Asm for pkt6 SP *
*                         file: Server_CPPCLI_ASMDLL.cpp            *
*                                                            files: *
*                                                           calc.so *
*                                          Server_CPPCLI_ASMDLL.cpp *
*                                                                   *
*                                                 *extended version *
*********************************************************************/

#include "stdafx.h"

//#define LOAD_DLL_BY_METHOD_1
using namespace System;
using namespace System::Collections::Generic;
//using namespace System::Linq;
using namespace System::Text;
using namespace System::Text::RegularExpressions;
using namespace System::Threading::Tasks;

using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Globalization;

//using namespace System::Runtime::InteropServices;

namespace Server_CPPCLI_ASMDLL {

//#pragma managed
	public ref class Server_CPPCLI_ASMDLL{
private: 
#ifdef LOAD_DLL_BY_METHOD_1
		[System::Runtime::InteropServices::DllImport(L"calc.so", CallingConvention = System::Runtime::InteropServices::CallingConvention::Cdecl, EntryPoint = "calc")]
		static double calc(double b2, float c1, double d2, float e1, double f2);
#else
		//System::Runtime::InteropServices::Dll
		[System::Runtime::InteropServices::DllImport(L"kernel32.dll")]
	    static IntPtr LoadLibrary(String^ dllname);

		[System::Runtime::InteropServices::DllImport(L"kernel32.dll")]
	    static IntPtr GetProcAddress(IntPtr hModule, String^ procname);

		[System::Runtime::InteropServices::UnmanagedFunctionPointer(System::Runtime::InteropServices::CallingConvention::Cdecl)]
		delegate double calcDelegate(double b2, float c1, double d2, float e1, double f2);
		static calcDelegate^ calc = nullptr;
#endif

		static const String^ HOST = "127.0.0.1";
		static const int PORT = 80;

		static bool usePostSubmit = true; // defaault value

		static const int K = 0x00025630; // (153136) const
		static double b2 = 10;          // defaault value
		static float  c1 = 20;          // defaault value
		static double d2 = 30;          // defaault value
		static float  e1 = 40;          // defaault value
		static double f2 = 50;          // defaault value

		static String^ http_response_fmt =
			"HTTP / 1.1 200 OK\r\n" +
			"Date : Mon, 27 Jul 2009 12 : 28 : 53 GMT\r\n" +
			"Server : Apache / 2.2.14 (Win32)\r\n" +
			"Last - Modified : Wed, 22 Jul 2009 19 : 15 : 56 GMT\r\n" +
			"Content - Length : {0}\r\n" +
			"Content - Type : text / html\r\n" +
			"Connection : Closed\r\n" +
			"\r\n" +
			"{1}"
			;

		static String^ http_response_server_stopped =
			"HTTP / 1.1 200 OK\r\n" +
			"Date : Mon, 27 Jul 2009 12 : 28 : 53 GMT\r\n" +
			"Server : Apache / 2.2.14 (Win32)\r\n" +
			"Last - Modified : Wed, 22 Jul 2009 19 : 15 : 56 GMT\r\n" +
			"Content - Length : 57\r\n" +
			"Content - Type : text / html\r\n" +
			"Connection : Closed\r\n" +
			"\r\n" +
			"<html>\r\n" +
			"<body>\r\n" +
			"<h1>" + "Server stopped" + "</h1>\r\n" +
			"</body>\r\n" +
			"</html>"
			;

		static String^ html_code_fmt__withGetSubmit =
			"<html>\r\n" +
			"<head>\r\n" +
			"<link rel = \"icon\" href = \"data:;base64,=\">\r\n" +
			"</head>\r\n" +
			"<body>\r\n" +
			"\r\n" +
			"<form action = \"/setSettings\" method=\"get\">\r\n" +
			"<h1>Settings:</h1>\r\n" +
			"<p>Select mode:</p>\r\n" +
			"<input type = \"radio\" name = \"mode\" value = \"1\"> mode 1<br>\r\n" +
			"<input type = \"radio\" name = \"mode\" value = \"2\"> mode 2<br>\r\n" +
			"<input type = \"radio\" name = \"mode\" value = \"3\"> mode 3<br>\r\n" +
			"<p>Change used http - method : </p>\r\n" +
			"<input type = \"radio\" name = \"http_method\" value = \"0\" checked = \"checked\"> GET<br>\r\n" +
			"<input type = \"radio\" name = \"http_method\" value = \"1\"> POST<br>\r\n" +
			"\r\n" +
			"<input type = \"submit\" value = \"Submit parameters and reload page\" text = \"\">\r\n" +
			"</form>\r\n" +
			"<h1>Compute board:</h1>\r\n" +
			"<button type = \"submit\" form = \"calcData\">Send values by GET and compute result</button>\r\n" +
			"\r\n" +
			"<form id = \"calcData\"  method = \"get\" action = \"callCalc\">\r\n" +
			"\r\n" +
			"<h2>X = K + B2 - D2/C1 + E1*F2</h2>\r\n" +
			"<h2>--------------------------</h2>\r\n" +
			"<h2>K = {0}</h2>\r\n" +
			"<h2>B = <input name = \"B\" type = \"text\" value = \"{1}\"></h2>\r\n" +
			"<h2>C = <input name = \"C\" type = \"text\" value = \"{2}\"></h2>\r\n" +
			"<h2>D = <input name = \"D\" type = \"text\" value = \"{3}\"></h2>\r\n" +
			"<h2>E = <input name = \"E\" type = \"text\" value = \"{4}\"></h2>\r\n" +
			"<h2>F = <input name = \"F\" type = \"text\" value = \"{5}\"></h2>\r\n" +
			"<h2>-------</h2>\r\n" +
			"<h2>X(Assembly) = {6}</h2>\r\n" +
			"<h2>X(Java) = {7}</h2>\r\n" +
			"<h2>--------------------------</h2>\r\n" +
			"<input type = \"submit\" value = \"Send values by GET and compute result\">\r\n" +
			"\r\n" +
			"</form>\r\n" +
			"</body>\r\n" +
			"</html>\r\n"
			;

		static String^ html_code_fmt__withPostSubmit =
			"<html>\r\n" +
			"<head>\r\n" +
			"<link rel = \"icon\" href = \"data:;base64,=\">\r\n" +
			"</head>\r\n" +
			"<body>\r\n" +
			"\r\n" +
			"<form action = \"/setSettings\" method=\"post\">\r\n" +
			"<h1>Settings:</h1>\r\n" +
			"<p>Select mode:</p>\r\n" +
			"<input type = \"radio\" name = \"mode\" value = \"1\"> mode 1<br>\r\n" +
			"<input type = \"radio\" name = \"mode\" value = \"2\"> mode 2<br>\r\n" +
			"<input type = \"radio\" name = \"mode\" value = \"3\"> mode 3<br>\r\n" +
			"<p>Change used http - method : </p>\r\n" +
			"<input type = \"radio\" name = \"http_method\" value = \"0\"> GET<br>\r\n" +
			"<input type = \"radio\" name = \"http_method\" value = \"1\" checked = \"checked\"> POST<br>\r\n" +
			"\r\n" +
			"<input type = \"submit\" value = \"Submit parameters and reload page\" text = \"\">\r\n" +
			"</form>\r\n" +
			"<h1>Compute board:</h1>\r\n" +
			"<button type = \"submit\" form = \"calcData\">Send values by POST and compute result</button>\r\n" +
			"\r\n" +
			"<form id = \"calcData\"  method = \"post\" action = \"callCalc\">\r\n" +
			"\r\n" +
			"<h2>X = K + B2 - D2/C1 + E1*F2</h2>\r\n" +
			"<h2>--------------------------</h2>\r\n" +
			"<h2>K = {0}</h2>\r\n" +
			"<h2>B = <input name = \"B\" type = \"text\" value = \"{1}\"></h2>\r\n" +
			"<h2>C = <input name = \"C\" type = \"text\" value = \"{2}\"></h2>\r\n" +
			"<h2>D = <input name = \"D\" type = \"text\" value = \"{3}\"></h2>\r\n" +
			"<h2>E = <input name = \"E\" type = \"text\" value = \"{4}\"></h2>\r\n" +
			"<h2>F = <input name = \"F\" type = \"text\" value = \"{5}\"></h2>\r\n" +
			"<h2>-------</h2>\r\n" +
			"<h2>X(Assembly) = {6}</h2>\r\n" +
			"<h2>X(Java) = {7}</h2>\r\n" +
			"<h2>--------------------------</h2>\r\n" +
			"<input type = \"submit\" value = \"Send values by POST and compute result\">\r\n" +
			"\r\n" +
			"</form>\r\n" +
			"</body>\r\n" +
			"</html>\r\n"
			;

		static String^ buildResponse() {
			double x_AssemblyResult = calc(b2, c1, d2, e1, f2);
			double x_CResult = ((double)K + b2 - d2 / (double)c1 + (double)e1 * f2);

			String^ html_code_fmt = html_code_fmt__withGetSubmit;

			if (usePostSubmit) {
				html_code_fmt = html_code_fmt__withPostSubmit;
			}

			String^ html_code = String::Format(html_code_fmt, K.ToString(CultureInfo::InvariantCulture), b2.ToString(CultureInfo::InvariantCulture), c1.ToString(CultureInfo::InvariantCulture), d2.ToString(CultureInfo::InvariantCulture), e1.ToString(CultureInfo::InvariantCulture), f2.ToString(CultureInfo::InvariantCulture), x_AssemblyResult.ToString(CultureInfo::InvariantCulture), x_CResult.ToString(CultureInfo::InvariantCulture));
			String^ str = String::Format(http_response_fmt, html_code->Length, html_code);
			Console::WriteLine("Build response: " + str);

			return str;
		}

		static String^ read(Socket^ client) {
			int BUFFER_SIZE = 1024;
			array<Byte>^ buffer = gcnew array<Byte>(BUFFER_SIZE);
			int bytesRec = client->Receive(buffer);
			String^ sDatasReceived = Encoding::ASCII->GetString(buffer, 0, bytesRec);
			Console::WriteLine("Text received : {0}", sDatasReceived);

			return sDatasReceived;
		}

		static int handleClient(Socket^ connectionSocket)  {
			String^ message = read(connectionSocket);

			Console::WriteLine("Received: " + message);
			if (message == nullptr || message->Length <= 0) {
				connectionSocket->Send(Encoding::ASCII->GetBytes(buildResponse()));
				Console::WriteLine("\r\n>> response sent\r\n");
				connectionSocket->Close();
				return 0;
			}

			if (message->IndexOf("close") > -1) {
				connectionSocket->Send(Encoding::ASCII->GetBytes(http_response_server_stopped));
				Console::WriteLine("\r\n>> response sent\r\n");
				connectionSocket->Close();
				return -1;// break;
			}

			String^ http_method_key = "http_method=";
			String^ B_key = "B=";
			String^ C_key = "C=";
			String^ D_key = "D=";
			String^ E_key = "E=";
			String^ F_key = "F=";

			Match^ match;
			Regex^ integerValueRegex = gcnew Regex("[-+]?(0[xX][\\dA-Fa-f]+|0[0-7]*|\\d+)");
			Regex^ realValueRegex = gcnew Regex("[-+]?(\\d+([.,]\\d*)?|[.,]\\d+)([eE][-+]?\\d+)?");

			int indexPOSTValues;
			if (message->IndexOf("POST") > -1 && (indexPOSTValues = message->IndexOf("\r\n\r\n")) > -1) {
				usePostSubmit = true;

				int index;
				if ((index = message->Substring(indexPOSTValues)->IndexOf(http_method_key)) > -1) {
					int usePostSubmitValue = 0;
					if (usePostSubmit) {
						usePostSubmitValue = 1;
					}
					if ((match = integerValueRegex->Match(message->Substring(indexPOSTValues + index + http_method_key->Length)))->Success) {
						usePostSubmitValue = Int32::Parse(match->Value, CultureInfo::InvariantCulture);
					}
					usePostSubmit = true;
					if (usePostSubmitValue == 0) {
						usePostSubmit = false;
					}
				}

				if ((index = message->Substring(indexPOSTValues)->IndexOf(B_key)) > -1) {
					if ((match = realValueRegex->Match(message->Substring(indexPOSTValues + index + B_key->Length)))->Success) {
						b2 = Double::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
					}
					else {
						b2 = 0;
					}
				}
				if ((index = message->Substring(indexPOSTValues)->IndexOf(C_key)) > -1) {
					if ((match = realValueRegex->Match(message->Substring(indexPOSTValues + index + C_key->Length)))->Success) {
						c1 = Single::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
					}
					else {
						c1 = 0;
					}
				}
				if ((index = message->Substring(indexPOSTValues)->IndexOf(D_key)) > -1) {
					if ((match = realValueRegex->Match(message->Substring(indexPOSTValues + index + D_key->Length)))->Success) {
						d2 = Double::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
					}
					else {
						d2 = 0;
					}
				}
				if ((index = message->Substring(indexPOSTValues)->IndexOf(E_key)) > -1) {
					if ((match = realValueRegex->Match(message->Substring(indexPOSTValues + index + E_key->Length)))->Success) {
						e1 = Single::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
					}
					else {
						e1 = 0;
					}
				}
				if ((index = message->Substring(indexPOSTValues)->IndexOf(F_key)) > -1) {
					if ((match = realValueRegex->Match(message->Substring(indexPOSTValues + index + F_key->Length)))->Success) {
						f2 = Double::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
					}
					else {
						f2 = 0;
					}
				}
			}
			else {

				int index;
				if ((index = message->IndexOf(http_method_key)) > -1) {
					int usePostSubmitValue = 0;
					if (usePostSubmit) {
						usePostSubmitValue = 1;
					}
					if ((match = integerValueRegex->Match(message->Substring(index + http_method_key->Length)))->Success) {
						usePostSubmitValue = Int32::Parse(match->Value, CultureInfo::InvariantCulture);
					}
					usePostSubmit = true;
					if (usePostSubmitValue == 0) {
						usePostSubmit = false;
					}
				}
				else {

					if ((index = message->IndexOf(B_key)) > -1) {
						usePostSubmit = false;
						if ((match = realValueRegex->Match(message->Substring(index + B_key->Length)))->Success) {
							b2 = Double::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
						}
						else {
							b2 = 0;
						}
					}
					if ((index = message->IndexOf(C_key)) > -1) {
						usePostSubmit = false;
						if ((match = realValueRegex->Match(message->Substring(index + C_key->Length)))->Success) {
							c1 = Single::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
						}
						else {
							c1 = 0;
						}
					}
					if ((index = message->IndexOf(D_key)) > -1) {
						usePostSubmit = false;
						if ((match = realValueRegex->Match(message->Substring(index + D_key->Length)))->Success) {
							d2 = Double::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
						}
						else {
							d2 = 0;
						}
					}
					if ((index = message->IndexOf(E_key)) > -1) {
						usePostSubmit = false;
						if ((match = realValueRegex->Match(message->Substring(index + E_key->Length)))->Success) {
							e1 = Single::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
						}
						else {
							e1 = 0;
						}
					}
					if ((index = message->IndexOf(F_key)) > -1) {
						usePostSubmit = false;
						if ((match = realValueRegex->Match(message->Substring(index + F_key->Length)))->Success) {
							f2 = Double::Parse(match->Value->Replace(',', '.'), CultureInfo::InvariantCulture);
						}
						else {
							f2 = 0;
						}
					}
				}
			}

			connectionSocket->Send(Encoding::ASCII->GetBytes(buildResponse()));
			Console::WriteLine("\r\n>> response sent\r\n");
			connectionSocket->Close();

			return 0;
		}

		public: 
			static void Main(array<System::String ^> ^args){
#ifndef LOAD_DLL_BY_METHOD_1
			IntPtr ptrDll = LoadLibrary(L"calc.so");
			IntPtr procAddr = GetProcAddress(ptrDll, "calc");
			calc = (calcDelegate^)System::Runtime::InteropServices::Marshal::GetDelegateForFunctionPointer(procAddr, calcDelegate::typeid);
#endif

			Socket^ listener = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);
			try
			{
				listener->Bind(gcnew System::Net::IPEndPoint(IPAddress::Parse((String^)HOST), PORT));
				listener->Listen(1);
				int toExit = 0;
				while (!toExit){
					Socket^ client = listener->Accept();
					toExit = handleClient(client);
				}
			}
			catch (Exception^ e)
			{
				//System::Console::WriteLine(e->ToString());
				Console::WriteLine(e->ToString());
			}

			listener->Close();
			return;
		}

	};
};


int main(array<System::String ^> ^args)
{
	Server_CPPCLI_ASMDLL::Server_CPPCLI_ASMDLL::Main(args);
    return 0;
}

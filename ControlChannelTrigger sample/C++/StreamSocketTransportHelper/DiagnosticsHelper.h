#include "pch.h"
#include<Windows.h>
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Platform;
using namespace Windows::Foundation;

namespace StreamSocketTransportHelper
{
    namespace DiagnosticsHelper
    {
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Diag sealed
        {
            static CoreDispatcher^ coreDispatcher;
            static TextBlock^ debugOutputTextBlock;
			static TextBlock^ debugBytesTextBlock;
			static TextBlock^ debugCpuTextBlock;
        public:
            static void SetCoreDispatcher(CoreDispatcher^ dispatcher)
            {
                coreDispatcher=dispatcher;
            }
            static void SetDebugTextBlock(TextBlock^ debug)
            {
                debugOutputTextBlock=debug;
            }
            static void SetBytesTextBlock(TextBlock^ debug)
            {
                debugBytesTextBlock=debug;
            }
            static void SetCpuTextBlock(TextBlock^ debug)
            {
                debugCpuTextBlock=debug;
            }
            static void DebugPrint(String^ msg)
            {
                if (coreDispatcher != nullptr)
                {
					ShowCpu();
                    coreDispatcher->RunAsync(CoreDispatcherPriority::Normal,ref new DispatchedHandler([msg](){
                        SYSTEMTIME systime;
                        GetLocalTime(&systime);
						String^ newText;
                        if(debugOutputTextBlock->Text!=nullptr)
                        {
							if(debugOutputTextBlock->Text->Length() > 5000)
								newText = ref new String(debugOutputTextBlock->Text->Begin() + 4000);
							else
								newText = debugOutputTextBlock->Text;
                        }
						else
							newText = ref new String();

                        debugOutputTextBlock->Text = newText + systime.wMonth + "/" + 
                            systime.wDay + "/" + systime.wYear + " " + systime.wHour + ":" + systime.wMinute + ":" + " " + msg + "\r\n";						
                    }));
                }
            }
            static void ShowBytes(int bytes)
            {
                if (coreDispatcher != nullptr)
                {
					Object^ bytesObj = PropertyValue::CreateInt32(bytes);
                    coreDispatcher->RunAsync(CoreDispatcherPriority::Normal,ref new DispatchedHandler([bytesObj](){
                        debugBytesTextBlock->Text = "Bytes: " + bytesObj->ToString();
                    }));
                }
			}
			static BOOL GetCPUStats(
					Object^ kernelCpuObj,
					Object^ userCpuObj){
				FILETIME CreationFileTime = {0, 0};
				FILETIME ExitFileTime = {0, 0};
				FILETIME KernelFileTime = {0, 0};
				FILETIME UserFileTime = {0, 0};
				typedef BOOL (__cdecl *GetProcessTimesProc)(
					HANDLE hProcess,
					LPFILETIME lpCreationTime,
					LPFILETIME lpExitTime,
					LPFILETIME lpKernelTime,
					LPFILETIME lpUserTime
				);
				WINBASEAPI
				_Ret_maybenull_
				HMODULE
				WINAPI
				LoadLibraryW(
					_In_ LPCWSTR lpLibFileName
					);

				static GetProcessTimesProc pGetProcessTimes = NULL;
				if(! pGetProcessTimes){
					HMODULE hLib = LoadPackagedLibrary(TEXT("kernel32.dll"), 0);
					if(! hLib)
						return false;
					pGetProcessTimes = (GetProcessTimesProc) GetProcAddress(hLib, "GetProcessTimes");
					if(! pGetProcessTimes)
						return false;
				}

				if (! pGetProcessTimes(
					GetCurrentProcess(),
					&CreationFileTime,
					&ExitFileTime,
					&KernelFileTime,
					&UserFileTime))
					return false;

				ULARGE_INTEGER KernelTime;
				ULARGE_INTEGER UserTime;
				KernelTime.HighPart = KernelFileTime.dwHighDateTime;
				KernelTime.LowPart = KernelFileTime.dwLowDateTime;
				UserTime.HighPart = UserFileTime.dwHighDateTime;
				UserTime.LowPart = UserFileTime.dwLowDateTime;

				//Process kernel mode and user mode times are amounts of time. 
				//For example, if a process has spent one second in kernel mode, 
				//this function will fill the FILETIME structure specified by lpKernelTime 
				//with a 64-bit value of ten million. That is the number of 100-nanosecond 
				//units in one second.
				kernelCpuObj = PropertyValue::CreateSingle(((float) KernelTime.QuadPart) / 10000000);
				userCpuObj = PropertyValue::CreateSingle(((float) UserTime.QuadPart) / 10000000);

				return true;
			}
            static void ShowCpu()
            {
                if (coreDispatcher != nullptr)
                {
					Object^ kernelCpuObj;
					Object^ userCpuObj;
					if(! GetCPUStats(kernelCpuObj, userCpuObj))
						return;
                    coreDispatcher->RunAsync(CoreDispatcherPriority::Normal,ref new DispatchedHandler([kernelCpuObj, userCpuObj](){
						debugCpuTextBlock->Text = "Kernel CPU: " + kernelCpuObj->ToString() +  "  User CPU: " + userCpuObj->ToString();
                    }));
                }
			}

		};
    }
}
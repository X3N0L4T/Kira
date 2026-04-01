#include "MainForm.h"
#include <opencv2/opencv.hpp>

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew Kira::MainForm());
    return 0;
}
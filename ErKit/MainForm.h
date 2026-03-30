#pragma once

namespace ErKit {
	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class MainForm : public Form {
	public:
		MainForm() {
			InitializeComponent();
		}

	private:
		Panel^ pnlSidebar;
		Panel^ pnlCamera;
		Panel^ pnlStatus;
		Label^ lblCameraTitle;
		Label^ lblCamStatus;
		Label^ lblStatusSystem;
		Label^ lblStatusCamera;
		Label^ lblStatusAI;
		Label^ lblStatusSpeech;
		Label^ lblVersion;

		void pnlSidebar_Paint(Object^ sender, PaintEventArgs^ e) {
			Graphics^ g = e->Graphics;
			g->SmoothingMode = Drawing2D::SmoothingMode::AntiAlias;

			//cam icon
			Drawing::Pen^ activePen = gcnew Drawing::Pen(Color::FromArgb(55, 138, 221), 1.5f);
			Drawing::Pen^ inactivePen = gcnew Drawing::Pen(Color::FromArgb(74, 74, 85), 1.5f);

			//Active highlight bg
			g->FillRectangle(gcnew SolidBrush(Color::FromArgb(30, 42, 58)), 8, 12, 36, 32);
	}

		void InitializeComponent() {

			//--- Form
			this->Text = L"ErKit";
			this->Size = Drawing::Size(1100, 700);
			this->BackColor = Color::FromArgb(18, 18, 20);
			this->ForeColor = Color::FromArgb(200, 200, 210);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->StartPosition = FormStartPosition::CenterScreen;
			this->Font = gcnew Drawing::Font("Segoe UI", 9.0f);

			//--- Sidebar
			pnlSidebar = gcnew Panel();
			pnlSidebar->BackColor = Color::FromArgb(17, 17, 19);
			pnlSidebar->Size = Drawing::Size(52, 662);
			pnlSidebar->Location = Point(0, 0);
			pnlSidebar->Paint += gcnew PaintEventHandler(this, &MainForm::pnlSidebar_Paint);

			//--- Camera Panel
			pnlCamera = gcnew Panel();
			pnlCamera->BackColor = Color::FromArgb(13, 13, 16);
			pnlCamera->Size = Drawing::Size(1032, 560);
			pnlCamera->Location = Point(58, 10);
			pnlCamera->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;

			//Camera Panel Header
			lblCameraTitle = gcnew Label();
			lblCameraTitle->Text = L"CAMERA FEED";
			lblCameraTitle->ForeColor = Color::FromArgb(138, 138, 150);
			lblCameraTitle->Font = gcnew Drawing::Font("Segoe UI", 8.0f, FontStyle::Regular);
			lblCameraTitle->Location = Point(14, 10);
			lblCameraTitle->AutoSize = true;

			//Live/idle dot
			lblCamStatus = gcnew Label();
			lblCamStatus->Text = L"0";
			lblCamStatus->ForeColor = Color::FromArgb(239, 159, 39); //amber = idle
			lblCamStatus->Font = gcnew Drawing::Font("Segoe UI", 7.0f);
			lblCamStatus->Location = Point(990, 10);
			lblCamStatus->AutoSize = true;

			pnlCamera->Controls->Add(lblCameraTitle);
			pnlCamera->Controls->Add(lblCamStatus);

			//Status bar
			pnlStatus = gcnew Panel();
			pnlStatus->BackColor = Color::FromArgb(17, 17, 19);
			pnlStatus->Size = Drawing::Size(1032, 42);
			pnlStatus->Location = Point(58, 578);
			pnlStatus->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;

			//System
			lblStatusSystem = gcnew Label();
			lblStatusSystem->Text = L"System Online";
			lblStatusSystem->ForeColor = Color::FromArgb(76, 175, 130);
			lblStatusSystem->Location = Point(14, 12);
			lblStatusSystem->AutoSize = true;
			lblStatusSystem->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

			//Camera
			lblStatusCamera = gcnew Label();
			lblStatusCamera->Text = L"Camera Idle";
			lblStatusCamera->ForeColor = Color::FromArgb(239, 159, 39);
			lblStatusCamera->Location = Point(160, 12);
			lblStatusCamera->AutoSize = true;
			lblStatusCamera->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

			//AI
			lblStatusAI = gcnew Label();
			lblStatusAI->Text = L"AI Standby";
			lblStatusAI->ForeColor = Color::FromArgb(74, 74, 85);
			lblStatusAI->Location = Point(300, 12);
			lblStatusAI->AutoSize = true;
			lblStatusAI->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

			// Speech
			lblStatusSpeech = gcnew Label();
			lblStatusSpeech->Text = L"Speech Off";
			lblStatusSpeech->ForeColor = Color::FromArgb(74, 74, 85);
			lblStatusSpeech->Location = Point(420, 12);
			lblStatusSpeech->AutoSize = true;
			lblStatusSpeech->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

			//Version
			lblVersion = gcnew Label();
			lblVersion->Text = L"ErKit © 2026";
			lblVersion->ForeColor = Color::FromArgb(74, 74, 85);
			lblVersion->Location = Point(920, 12);
			lblVersion->AutoSize = true;
			lblVersion->Font = gcnew Drawing::Font("Segoe UI", 8.0f);

			pnlStatus->Controls->Add(lblStatusSystem);
			pnlStatus->Controls->Add(lblStatusCamera);
			pnlStatus->Controls->Add(lblStatusAI);
			pnlStatus->Controls->Add(lblStatusSpeech);
			pnlStatus->Controls->Add(lblVersion);

			//Add to form
			this->Controls->Add(pnlSidebar);
			this->Controls->Add(pnlCamera);
			this->Controls->Add(pnlStatus);
		}

	};
}
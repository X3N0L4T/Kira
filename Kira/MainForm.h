#pragma once
#include <opencv2/opencv.hpp>

namespace Kira {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Threading;

    public ref class MainForm : public Form {
    public:
        MainForm() {
            InitializeComponent();
            cameraRunning = false;
            activeTab = 0;
            highlightAlpha = 40;
            animTimer = gcnew System::Windows::Forms::Timer();
            animTimer->Interval = 16;
            animTimer->Tick += gcnew EventHandler(this, &MainForm::AnimTick);
        }

        ~MainForm() {
            StopCamera();
        }

    private:
        //Panels
        Panel^ pnlSidebar;
        Panel^ pnlCamera;
        Panel^ pnlLogs;
        Panel^ pnlSettings;
        Panel^ pnlProfile;
        Panel^ pnlStatus;

        //Camera controls
        Label^ lblCameraTitle;
        Label^ lblCamStatus;
        PictureBox^ picFeed;

        //Log controls
        ListBox^ lstLogs;

        //Settings controls
        ComboBox^ cmbCameraDevice;
        ComboBox^ cmbResolution;
        Button^ btnApplySettings;
        Label^ lblLoadingDevices;

        //Profile controls
        Label^ lblAppName;
        Label^ lblAppVersion;
        Label^ lblAppBuild;
        Label^ lblAppCredits;

        //Status bar
        Label^ lblStatusSystem;
        Label^ lblStatusCamera;
        Label^ lblStatusAI;
        Label^ lblStatusSpeech;
        Label^ lblVersion;

        //State
        cv::VideoCapture* cap;
        Thread^ cameraThread;
        bool cameraRunning;
        int activeTab;

        //Animation
        System::Windows::Forms::Timer^ animTimer;
        int highlightAlpha;
        int targetAlpha;

        //Anim tick - fade highlight in/out
        void AnimTick(Object^ sender, EventArgs^ e) {
            int diff = targetAlpha - highlightAlpha;
            if (Math::Abs(diff) <= 2) {
                highlightAlpha = targetAlpha;
                animTimer->Stop();
            }
            else {
                highlightAlpha += diff / 4;
            }
            pnlSidebar->Invalidate();
        }

        //Logging
        delegate void AddLogDelegate(String^ msg);
        void AddLog(String^ msg) {
            if (lstLogs->InvokeRequired) {
                lstLogs->Invoke(gcnew AddLogDelegate(this, &MainForm::AddLog), msg);
                return;
            }
            String^ timestamp = DateTime::Now.ToString("[HH:mm:ss] ");
            lstLogs->Items->Insert(0, timestamp + msg);
        }

        //Camera thread
        void CameraLoop() {
            cap = new cv::VideoCapture(0);
            if (!cap->isOpened()) {
                UpdateCameraStatus(false);
                AddLog("Camera failed to open");
                return;
            }

            UpdateCameraStatus(true);
            AddLog("Camera connected successfully");

            while (cameraRunning) {
                cv::Mat frame;
                *cap >> frame;
                if (frame.empty()) continue;

                //Convert BGR to RGB
                cv::Mat rgb;
                cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);

                //Resize to fit
                cv::Mat resized;
                cv::resize(rgb, resized, cv::Size(
                    picFeed->Width,
                    picFeed->Height
                ));

                //Convert to bitmap and display
                Drawing::Bitmap^ bmp = MatToBitmap(resized);
                UpdateFeed(bmp);

                cv::waitKey(30);
            }

            cap->release();
            delete cap;
            AddLog("Camera disconnected");
        }

        //Mat to Bitmap
        Drawing::Bitmap^ MatToBitmap(cv::Mat& mat) {
            Drawing::Bitmap^ bmp = gcnew Drawing::Bitmap(
                mat.cols, mat.rows,
                Drawing::Imaging::PixelFormat::Format24bppRgb
            );
            Drawing::Rectangle rect(0, 0, mat.cols, mat.rows);
            Drawing::Imaging::BitmapData^ bmpData =
                bmp->LockBits(rect,
                    Drawing::Imaging::ImageLockMode::WriteOnly,
                    Drawing::Imaging::PixelFormat::Format24bppRgb);

            //Copy row by row
            for (int i = 0; i < mat.rows; i++) {
                IntPtr dst = IntPtr(bmpData->Scan0.ToInt64() + i * bmpData->Stride);
                memcpy(dst.ToPointer(), mat.data + i * mat.step, mat.cols * 3);
            }

            bmp->UnlockBits(bmpData);
            return bmp;
        }

        //Thread-safe UI updates
        delegate void UpdateFeedDelegate(Drawing::Bitmap^ bmp);
        void UpdateFeed(Drawing::Bitmap^ bmp) {
            if (picFeed->InvokeRequired) {
                picFeed->Invoke(gcnew UpdateFeedDelegate(this, &MainForm::UpdateFeed), bmp);
                return;
            }
            if (picFeed->Image != nullptr) delete picFeed->Image;
            picFeed->Image = bmp;
        }

        delegate void UpdateCameraStatusDelegate(bool online);
        void UpdateCameraStatus(bool online) {
            if (lblStatusCamera->InvokeRequired) {
                lblStatusCamera->Invoke(
                    gcnew UpdateCameraStatusDelegate(this, &MainForm::UpdateCameraStatus), online);
                return;
            }
            if (online) {
                lblStatusCamera->Text = L"Camera  Live";
                lblStatusCamera->ForeColor = Color::FromArgb(76, 175, 130);
                lblCamStatus->ForeColor = Color::FromArgb(76, 175, 130);
            }
            else {
                lblStatusCamera->Text = L"Camera  No Device";
                lblStatusCamera->ForeColor = Color::FromArgb(226, 75, 74);
                lblCamStatus->ForeColor = Color::FromArgb(226, 75, 74);
            }
        }

        //Start / Stop
        void StartCamera() {
            cameraRunning = true;
            cameraThread = gcnew Thread(
                gcnew ThreadStart(this, &MainForm::CameraLoop));
            cameraThread->IsBackground = true;
            cameraThread->Start();
        }

        void StopCamera() {
            cameraRunning = false;
            if (cameraThread != nullptr && cameraThread->IsAlive)
                cameraThread->Join(2000);
        }

        //Tab switching with fade animation
        void ShowTab(int tab) {
            activeTab = tab;
            pnlCamera->Visible = (tab == 0);
            pnlLogs->Visible = (tab == 1);
            pnlSettings->Visible = (tab == 2);
            pnlProfile->Visible = (tab == 3);

            //Trigger fade in
            highlightAlpha = 10;
            targetAlpha = 40;
            animTimer->Start();

            if (tab == 1) AddLog("Logs viewed");
            if (tab == 2) PopulateCameraDevicesAsync();
        }

        //Populate camera devices on background thread - no UI freeze
        delegate void PopulateDevicesDoneDelegate(System::Collections::Generic::List<String^>^ devices);
        void PopulateCameraDevicesAsync() {
            cmbCameraDevice->Items->Clear();
            cmbCameraDevice->Enabled = false;
            lblLoadingDevices->Visible = true;
            Thread^ scanThread = gcnew Thread(gcnew ThreadStart(this, &MainForm::ScanDevices));
            scanThread->IsBackground = true;
            scanThread->Start();
        }

        void ScanDevices() {
            System::Collections::Generic::List<String^>^ found =
                gcnew System::Collections::Generic::List<String^>();
            for (int i = 0; i < 5; i++) {
                cv::VideoCapture test(i);
                if (test.isOpened()) {
                    found->Add("Camera " + i.ToString());
                    test.release();
                }
            }
            this->Invoke(gcnew PopulateDevicesDoneDelegate(
                this, &MainForm::PopulateDevicesDone), found);
        }

        void PopulateDevicesDone(System::Collections::Generic::List<String^>^ devices) {
            lblLoadingDevices->Visible = false;
            cmbCameraDevice->Enabled = true;
            if (devices->Count > 0) {
                for each (String ^ d in devices)
                    cmbCameraDevice->Items->Add(d);
                cmbCameraDevice->SelectedIndex = 0;
            }
            else {
                cmbCameraDevice->Items->Add("No devices found");
            }
            AddLog("Device scan complete - " + devices->Count.ToString() + " found");
        }

        //Sidebar clicks
        void btnCam_Click(Object^ sender, EventArgs^ e) { ShowTab(0); }
        void btnLogs_Click(Object^ sender, EventArgs^ e) { ShowTab(1); }
        void btnSettings_Click(Object^ sender, EventArgs^ e) { ShowTab(2); }
        void btnProfile_Click(Object^ sender, EventArgs^ e) { ShowTab(3); }

        //Apply settings
        void btnApplySettings_Click(Object^ sender, EventArgs^ e) {
            AddLog("Settings applied - " + cmbResolution->Text + " / " + cmbCameraDevice->Text);
            ShowTab(0);
        }

        //Form load
        void MainForm_Load(Object^ sender, EventArgs^ e) {
            targetAlpha = 40;
            highlightAlpha = 40;
            AddLog("Kira started");
            StartCamera();
        }

        //Sidebar paint
        void pnlSidebar_Paint(Object^ sender, PaintEventArgs^ e) {
            Graphics^ g = e->Graphics;
            g->SmoothingMode = Drawing2D::SmoothingMode::AntiAlias;

            //Toned down blue - not full brightness
            Color activeColor = Color::FromArgb(45, 110, 180);
            Color inactiveColor = Color::FromArgb(65, 65, 75);

            Drawing::Pen^ activePen = gcnew Drawing::Pen(activeColor, 1.5f);
            Drawing::Pen^ inactivePen = gcnew Drawing::Pen(inactiveColor, 1.5f);
            SolidBrush^ activeBg = gcnew SolidBrush(Color::FromArgb(highlightAlpha, 35, 80, 140));
            SolidBrush^ activeFill = gcnew SolidBrush(activeColor);
            SolidBrush^ inactiveFill = gcnew SolidBrush(inactiveColor);

            //Highlight active tab with animated alpha
            if (activeTab == 0) g->FillRectangle(activeBg, 6, 12, 40, 32);
            if (activeTab == 1) g->FillRectangle(activeBg, 6, 56, 40, 32);
            if (activeTab == 2) g->FillRectangle(activeBg, 6, 106, 40, 32);
            if (activeTab == 3) g->FillRectangle(activeBg, 6, 572, 40, 32);

            Drawing::Pen^ p0 = (activeTab == 0) ? activePen : inactivePen;
            Drawing::Pen^ p1 = (activeTab == 1) ? activePen : inactivePen;
            Drawing::Pen^ p2 = (activeTab == 2) ? activePen : inactivePen;
            Drawing::Pen^ p3 = (activeTab == 3) ? activePen : inactivePen;

            //Camera icon
            g->DrawRectangle(p0, 12, 20, 22, 16);
            g->DrawEllipse(p0, 19, 24, 8, 8);
            g->FillEllipse((activeTab == 0) ? activeFill : inactiveFill, 32, 21, 4, 4);

            //Clock icon
            g->DrawEllipse(p1, 12, 62, 22, 22);
            g->DrawLine(p1, 23, 68, 23, 74);
            g->DrawLine(p1, 23, 74, 27, 78);

            //Settings icon
            g->DrawEllipse(p2, 16, 112, 14, 14);
            g->DrawLine(p2, 23, 108, 23, 112);
            g->DrawLine(p2, 23, 126, 23, 130);
            g->DrawLine(p2, 12, 119, 16, 119);
            g->DrawLine(p2, 30, 119, 34, 119);

            //Profile icon
            g->DrawEllipse(p3, 16, 580, 14, 14);
            g->DrawArc(p3, 10, 596, 26, 14, 180, 180);

            delete activePen;
            delete inactivePen;
            delete activeBg;
            delete activeFill;
            delete inactiveFill;
        }

        void InitializeComponent() {

            //Form
            this->Text = L"Kira";
            this->Size = Drawing::Size(1100, 700);
            this->BackColor = Color::FromArgb(18, 18, 20);
            this->ForeColor = Color::FromArgb(200, 200, 210);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
            this->StartPosition = FormStartPosition::CenterScreen;
            this->Font = gcnew Drawing::Font("Segoe UI", 9.0f);
            this->Load += gcnew EventHandler(this, &MainForm::MainForm_Load);

            //Sidebar
            pnlSidebar = gcnew Panel();
            pnlSidebar->BackColor = Color::FromArgb(17, 17, 19);
            pnlSidebar->Size = Drawing::Size(52, 662);
            pnlSidebar->Location = Point(0, 0);
            pnlSidebar->Paint += gcnew PaintEventHandler(this, &MainForm::pnlSidebar_Paint);

            //Invisible click buttons over icons
            Button^ btnCam = gcnew Button();
            btnCam->Size = Drawing::Size(52, 52);
            btnCam->Location = Point(0, 8);
            btnCam->FlatStyle = FlatStyle::Flat;
            btnCam->FlatAppearance->BorderSize = 0;
            btnCam->BackColor = Color::Transparent;
            btnCam->Click += gcnew EventHandler(this, &MainForm::btnCam_Click);

            Button^ btnLogs = gcnew Button();
            btnLogs->Size = Drawing::Size(52, 52);
            btnLogs->Location = Point(0, 52);
            btnLogs->FlatStyle = FlatStyle::Flat;
            btnLogs->FlatAppearance->BorderSize = 0;
            btnLogs->BackColor = Color::Transparent;
            btnLogs->Click += gcnew EventHandler(this, &MainForm::btnLogs_Click);

            Button^ btnSettings = gcnew Button();
            btnSettings->Size = Drawing::Size(52, 52);
            btnSettings->Location = Point(0, 102);
            btnSettings->FlatStyle = FlatStyle::Flat;
            btnSettings->FlatAppearance->BorderSize = 0;
            btnSettings->BackColor = Color::Transparent;
            btnSettings->Click += gcnew EventHandler(this, &MainForm::btnSettings_Click);

            Button^ btnProfile = gcnew Button();
            btnProfile->Size = Drawing::Size(52, 52);
            btnProfile->Location = Point(0, 568);
            btnProfile->FlatStyle = FlatStyle::Flat;
            btnProfile->FlatAppearance->BorderSize = 0;
            btnProfile->BackColor = Color::Transparent;
            btnProfile->Click += gcnew EventHandler(this, &MainForm::btnProfile_Click);

            pnlSidebar->Controls->Add(btnCam);
            pnlSidebar->Controls->Add(btnLogs);
            pnlSidebar->Controls->Add(btnSettings);
            pnlSidebar->Controls->Add(btnProfile);

            //Camera panel
            pnlCamera = gcnew Panel();
            pnlCamera->BackColor = Color::FromArgb(13, 13, 16);
            pnlCamera->Size = Drawing::Size(1032, 560);
            pnlCamera->Location = Point(58, 10);
            pnlCamera->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            pnlCamera->Visible = true;

            lblCameraTitle = gcnew Label();
            lblCameraTitle->Text = L"CAMERA FEED";
            lblCameraTitle->ForeColor = Color::FromArgb(138, 138, 150);
            lblCameraTitle->Font = gcnew Drawing::Font("Segoe UI", 8.0f);
            lblCameraTitle->Location = Point(14, 10);
            lblCameraTitle->AutoSize = true;

            lblCamStatus = gcnew Label();
            lblCamStatus->Text = L"Rec";
            lblCamStatus->ForeColor = Color::FromArgb(239, 159, 39);
            lblCamStatus->Font = gcnew Drawing::Font("Segoe UI", 7.0f);
            lblCamStatus->Location = Point(990, 10);
            lblCamStatus->AutoSize = true;

            picFeed = gcnew PictureBox();
            picFeed->Size = Drawing::Size(1030, 530);
            picFeed->Location = Point(0, 28);
            picFeed->BackColor = Color::FromArgb(10, 10, 13);
            picFeed->SizeMode = PictureBoxSizeMode::StretchImage;

            pnlCamera->Controls->Add(picFeed);
            pnlCamera->Controls->Add(lblCameraTitle);
            pnlCamera->Controls->Add(lblCamStatus);

            //Logs panel
            pnlLogs = gcnew Panel();
            pnlLogs->BackColor = Color::FromArgb(13, 13, 16);
            pnlLogs->Size = Drawing::Size(1032, 560);
            pnlLogs->Location = Point(58, 10);
            pnlLogs->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            pnlLogs->Visible = false;

            Label^ lblLogsTitle = gcnew Label();
            lblLogsTitle->Text = L"ACTIVITY LOG";
            lblLogsTitle->ForeColor = Color::FromArgb(138, 138, 150);
            lblLogsTitle->Font = gcnew Drawing::Font("Segoe UI", 8.0f);
            lblLogsTitle->Location = Point(14, 10);
            lblLogsTitle->AutoSize = true;

            lstLogs = gcnew ListBox();
            lstLogs->Size = Drawing::Size(1010, 520);
            lstLogs->Location = Point(10, 30);
            lstLogs->BackColor = Color::FromArgb(13, 13, 16);
            lstLogs->ForeColor = Color::FromArgb(138, 138, 150);
            lstLogs->Font = gcnew Drawing::Font("Consolas", 9.0f);
            lstLogs->BorderStyle = System::Windows::Forms::BorderStyle::None;
            lstLogs->SelectionMode = SelectionMode::None;

            pnlLogs->Controls->Add(lblLogsTitle);
            pnlLogs->Controls->Add(lstLogs);

            //Settings panel
            pnlSettings = gcnew Panel();
            pnlSettings->BackColor = Color::FromArgb(13, 13, 16);
            pnlSettings->Size = Drawing::Size(1032, 560);
            pnlSettings->Location = Point(58, 10);
            pnlSettings->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            pnlSettings->Visible = false;

            Label^ lblSettingsTitle = gcnew Label();
            lblSettingsTitle->Text = L"SETTINGS";
            lblSettingsTitle->ForeColor = Color::FromArgb(138, 138, 150);
            lblSettingsTitle->Font = gcnew Drawing::Font("Segoe UI", 8.0f);
            lblSettingsTitle->Location = Point(14, 10);
            lblSettingsTitle->AutoSize = true;

            Label^ lblCamDevice = gcnew Label();
            lblCamDevice->Text = L"Camera Device";
            lblCamDevice->ForeColor = Color::FromArgb(138, 138, 150);
            lblCamDevice->Font = gcnew Drawing::Font("Segoe UI", 9.0f);
            lblCamDevice->Location = Point(40, 60);
            lblCamDevice->AutoSize = true;

            //Loading indicator shown during device scan
            lblLoadingDevices = gcnew Label();
            lblLoadingDevices->Text = L"Scanning devices...";
            lblLoadingDevices->ForeColor = Color::FromArgb(74, 74, 85);
            lblLoadingDevices->Font = gcnew Drawing::Font("Segoe UI", 8.5f);
            lblLoadingDevices->Location = Point(40, 86);
            lblLoadingDevices->AutoSize = true;
            lblLoadingDevices->Visible = false;

            cmbCameraDevice = gcnew ComboBox();
            cmbCameraDevice->Size = Drawing::Size(300, 28);
            cmbCameraDevice->Location = Point(40, 82);
            cmbCameraDevice->BackColor = Color::FromArgb(22, 22, 26);
            cmbCameraDevice->ForeColor = Color::FromArgb(200, 200, 210);
            cmbCameraDevice->FlatStyle = FlatStyle::Flat;
            cmbCameraDevice->DropDownStyle = ComboBoxStyle::DropDownList;

            Label^ lblRes = gcnew Label();
            lblRes->Text = L"Resolution";
            lblRes->ForeColor = Color::FromArgb(138, 138, 150);
            lblRes->Font = gcnew Drawing::Font("Segoe UI", 9.0f);
            lblRes->Location = Point(40, 124);
            lblRes->AutoSize = true;

            cmbResolution = gcnew ComboBox();
            cmbResolution->Size = Drawing::Size(300, 28);
            cmbResolution->Location = Point(40, 146);
            cmbResolution->BackColor = Color::FromArgb(22, 22, 26);
            cmbResolution->ForeColor = Color::FromArgb(200, 200, 210);
            cmbResolution->FlatStyle = FlatStyle::Flat;
            cmbResolution->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbResolution->Items->Add("1920 x 1080");
            cmbResolution->Items->Add("1280 x 720");
            cmbResolution->Items->Add("640 x 480");
            cmbResolution->SelectedIndex = 1;

            btnApplySettings = gcnew Button();
            btnApplySettings->Text = L"Apply & Return to Feed";
            btnApplySettings->Size = Drawing::Size(200, 36);
            btnApplySettings->Location = Point(40, 200);
            btnApplySettings->FlatStyle = FlatStyle::Flat;
            btnApplySettings->BackColor = Color::FromArgb(22, 34, 50);
            btnApplySettings->ForeColor = Color::FromArgb(45, 110, 180);
            btnApplySettings->FlatAppearance->BorderColor = Color::FromArgb(45, 110, 180);
            btnApplySettings->Click += gcnew EventHandler(this, &MainForm::btnApplySettings_Click);

            pnlSettings->Controls->Add(lblSettingsTitle);
            pnlSettings->Controls->Add(lblCamDevice);
            pnlSettings->Controls->Add(lblLoadingDevices);
            pnlSettings->Controls->Add(cmbCameraDevice);
            pnlSettings->Controls->Add(lblRes);
            pnlSettings->Controls->Add(cmbResolution);
            pnlSettings->Controls->Add(btnApplySettings);

            //Profile panel
            pnlProfile = gcnew Panel();
            pnlProfile->BackColor = Color::FromArgb(13, 13, 16);
            pnlProfile->Size = Drawing::Size(1032, 560);
            pnlProfile->Location = Point(58, 10);
            pnlProfile->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            pnlProfile->Visible = false;

            Label^ lblProfileTitle = gcnew Label();
            lblProfileTitle->Text = L"ABOUT";
            lblProfileTitle->ForeColor = Color::FromArgb(138, 138, 150);
            lblProfileTitle->Font = gcnew Drawing::Font("Segoe UI", 8.0f);
            lblProfileTitle->Location = Point(14, 10);
            lblProfileTitle->AutoSize = true;

            lblAppName = gcnew Label();
            lblAppName->Text = L"Kira";
            lblAppName->ForeColor = Color::FromArgb(200, 200, 210);
            lblAppName->Font = gcnew Drawing::Font("Segoe UI", 28.0f, FontStyle::Regular);
            lblAppName->Location = Point(40, 60);
            lblAppName->AutoSize = true;

            lblAppVersion = gcnew Label();
            lblAppVersion->Text = L"Version 0.1.0";
            lblAppVersion->ForeColor = Color::FromArgb(45, 110, 180);
            lblAppVersion->Font = gcnew Drawing::Font("Segoe UI", 11.0f);
            lblAppVersion->Location = Point(40, 110);
            lblAppVersion->AutoSize = true;

            lblAppBuild = gcnew Label();
            lblAppBuild->Text = L"Built with C++ / CLR  OpenCV 4.12.0  Visual Studio 2026";
            lblAppBuild->ForeColor = Color::FromArgb(74, 74, 85);
            lblAppBuild->Font = gcnew Drawing::Font("Segoe UI", 9.0f);
            lblAppBuild->Location = Point(40, 140);
            lblAppBuild->AutoSize = true;

            lblAppCredits = gcnew Label();
            lblAppCredits->Text = L"Real-time AI vision and audio processing.\nBuilt from scratch as a learning project.\n\nGitHub: github.com/X3N0L4T/Kira";
            lblAppCredits->ForeColor = Color::FromArgb(100, 100, 112);
            lblAppCredits->Font = gcnew Drawing::Font("Segoe UI", 9.0f);
            lblAppCredits->Location = Point(40, 200);
            lblAppCredits->AutoSize = true;

            pnlProfile->Controls->Add(lblProfileTitle);
            pnlProfile->Controls->Add(lblAppName);
            pnlProfile->Controls->Add(lblAppVersion);
            pnlProfile->Controls->Add(lblAppBuild);
            pnlProfile->Controls->Add(lblAppCredits);

            //Status bar
            pnlStatus = gcnew Panel();
            pnlStatus->BackColor = Color::FromArgb(17, 17, 19);
            pnlStatus->Size = Drawing::Size(1032, 42);
            pnlStatus->Location = Point(58, 578);
            pnlStatus->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;

            lblStatusSystem = gcnew Label();
            lblStatusSystem->Text = L"System  Online";
            lblStatusSystem->ForeColor = Color::FromArgb(76, 175, 130);
            lblStatusSystem->Location = Point(14, 12);
            lblStatusSystem->AutoSize = true;
            lblStatusSystem->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

            lblStatusCamera = gcnew Label();
            lblStatusCamera->Text = L"Camera  Idle";
            lblStatusCamera->ForeColor = Color::FromArgb(239, 159, 39);
            lblStatusCamera->Location = Point(160, 12);
            lblStatusCamera->AutoSize = true;
            lblStatusCamera->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

            lblStatusAI = gcnew Label();
            lblStatusAI->Text = L"AI  Standby";
            lblStatusAI->ForeColor = Color::FromArgb(74, 74, 85);
            lblStatusAI->Location = Point(300, 12);
            lblStatusAI->AutoSize = true;
            lblStatusAI->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

            lblStatusSpeech = gcnew Label();
            lblStatusSpeech->Text = L"Speech  Off";
            lblStatusSpeech->ForeColor = Color::FromArgb(74, 74, 85);
            lblStatusSpeech->Location = Point(420, 12);
            lblStatusSpeech->AutoSize = true;
            lblStatusSpeech->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

            lblVersion = gcnew Label();
            lblVersion->Text = L"Kira©2026";
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
            this->Controls->Add(pnlLogs);
            this->Controls->Add(pnlSettings);
            this->Controls->Add(pnlProfile);
            this->Controls->Add(pnlStatus);
        }
    };
}
#pragma once
#include <opencv2/opencv.hpp>

namespace ErKit {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Threading;

    public ref class MainForm : public Form {
    public:
        MainForm() {
            InitializeComponent();
            cameraRunning = false;
        }

        ~MainForm() {
            StopCamera();
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
        PictureBox^ picFeed;

        cv::VideoCapture* cap;
        Thread^ cameraThread;
        bool cameraRunning;

        //Cam thread
        void CameraLoop() {
            cap = new cv::VideoCapture(0);
            if (!cap->isOpened()) {
                UpdateCameraStatus(false);
                return;
            }

            UpdateCameraStatus(true);

            while (cameraRunning) {
                cv::Mat frame;
                *cap >> frame;
                if (frame.empty()) continue;

                // Convert BGR to RGB
                cv::Mat rgb;
                cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);

                //resize to fit
                cv::Mat resized;
                cv::resize(rgb, resized, cv::Size(
                    picFeed->Width,
                    picFeed->Height
                ));

                //convert to bitmap and display
                Drawing::Bitmap^ bmp = MatToBitmap(resized);
                UpdateFeed(bmp);

                cv::waitKey(30);
            }

            cap->release();
            delete cap;
        }
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

            IntPtr ptr = bmpData->Scan0;
            int bytes = mat.step * mat.rows;
            System::Runtime::InteropServices::Marshal::Copy(
                IntPtr(mat.data), gcnew array<Byte>(bytes), 0, bytes);

            //Copy row by row
            for (int i = 0; i < mat.rows; i++) {
                IntPtr dst = IntPtr(bmpData->Scan0.ToInt64() + i * bmpData->Stride);
                IntPtr src = IntPtr(mat.data + i * mat.step);
                System::Runtime::InteropServices::Marshal::Copy(
                    gcnew array<Byte>(mat.step), 0, dst, mat.cols * 3);
                memcpy(dst.ToPointer(), mat.data + i * mat.step, mat.cols * 3);
            }

            bmp->UnlockBits(bmpData);
            return bmp;
        }

        //Thread-safe UI update
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

        //Form Load
        void MainForm_Load(Object^ sender, EventArgs^ e) {
            StartCamera();
        }

        //Sidebar Paint
        void pnlSidebar_Paint(Object^ sender, PaintEventArgs^ e) {
            Graphics^ g = e->Graphics;
            g->SmoothingMode = Drawing2D::SmoothingMode::AntiAlias;

            Drawing::Pen^ activePen = gcnew Drawing::Pen(Color::FromArgb(55, 138, 221), 1.5f);
            Drawing::Pen^ inactivePen = gcnew Drawing::Pen(Color::FromArgb(74, 74, 85), 1.5f);

            g->FillRectangle(gcnew SolidBrush(Color::FromArgb(30, 42, 58)), 8, 12, 36, 32);
            g->DrawRectangle(activePen, 12, 20, 22, 16);
            g->DrawEllipse(activePen, 19, 24, 8, 8);
            g->FillEllipse(gcnew SolidBrush(Color::FromArgb(55, 138, 221)), 32, 21, 4, 4);

            g->DrawEllipse(inactivePen, 12, 62, 22, 22);
            g->DrawLine(inactivePen, 23, 68, 23, 74);
            g->DrawLine(inactivePen, 23, 74, 27, 78);

            g->DrawEllipse(inactivePen, 16, 112, 14, 14);
            g->DrawLine(inactivePen, 23, 108, 23, 112);
            g->DrawLine(inactivePen, 23, 126, 23, 130);
            g->DrawLine(inactivePen, 12, 119, 16, 119);
            g->DrawLine(inactivePen, 30, 119, 34, 119);

            g->DrawEllipse(inactivePen, 16, 580, 14, 14);
            g->DrawArc(inactivePen, 10, 596, 26, 14, 180, 180);

            delete activePen;
            delete inactivePen;
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
            this->Load += gcnew EventHandler(this, &MainForm::MainForm_Load);

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

            // PictureBox for live feed
            picFeed = gcnew PictureBox();
            picFeed->Size = Drawing::Size(1030, 530);
            picFeed->Location = Point(0, 28);
            picFeed->BackColor = Color::FromArgb(10, 10, 13);
            picFeed->SizeMode = PictureBoxSizeMode::StretchImage;

            pnlCamera->Controls->Add(picFeed);
            pnlCamera->Controls->Add(lblCameraTitle);
            pnlCamera->Controls->Add(lblCamStatus);

            //System
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

            //Camera
            lblStatusCamera = gcnew Label();
            lblStatusCamera->Text = L"Camera  Idle";
            lblStatusCamera->ForeColor = Color::FromArgb(239, 159, 39);
            lblStatusCamera->Location = Point(160, 12);
            lblStatusCamera->AutoSize = true;
            lblStatusCamera->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

            //AI
            lblStatusAI = gcnew Label();
            lblStatusAI->Text = L"AI  Standby";
            lblStatusAI->ForeColor = Color::FromArgb(74, 74, 85);
            lblStatusAI->Location = Point(300, 12);
            lblStatusAI->AutoSize = true;
            lblStatusAI->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

            // Speech
            lblStatusSpeech = gcnew Label();
            lblStatusSpeech->Text = L"Speech  Off";
            lblStatusSpeech->ForeColor = Color::FromArgb(74, 74, 85);
            lblStatusSpeech->Location = Point(420, 12);
            lblStatusSpeech->AutoSize = true;
            lblStatusSpeech->Font = gcnew Drawing::Font("Segoe UI", 8.5f);

            //Version
            lblVersion = gcnew Label();
            lblVersion->Text = L"ErKit©2026";
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
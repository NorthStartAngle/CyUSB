#pragma once
#include "CyAPI.h"
#include <dbt.h>
#include <string> 

namespace CppCLRWinFormsProject {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::Threading;
	using namespace System::Text::RegularExpressions;

	using namespace std;

	using System::Runtime::InteropServices::Marshal;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();

			bPnP_Arrival = false;
			bPnP_Removal = false;
			bPnP_DevNodeChange = false;
			bAppQuiting = false;
			bDeviceRefreshNeeded = false;
		}

	protected:
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		System::ComponentModel::Container ^components;
		System::Windows::Forms::TextBox^ TimeOutBox;
		System::Windows::Forms::CheckBox^ ShowDataBox;


		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::Label^ label2;
		System::Windows::Forms::ComboBox^ PacketsPerXferBox;
		System::Windows::Forms::Label^ label4;
		System::Windows::Forms::Label^ label5;
		System::Windows::Forms::Button^ StartBtn;
		System::Windows::Forms::ComboBox^ EndPointsBox;

		System::Windows::Forms::ComboBox^ QueueLenBox;
		System::Windows::Forms::TextBox^ DataTextBox;
		System::Windows::Forms::Label^ RateLabel;
		System::Windows::Forms::ProgressBar^ RateProgressBar;
		System::Windows::Forms::TextBox^ SuccessesBox;
		System::Windows::Forms::TextBox^ FailuresBox;
		System::Windows::Forms::Label^ SuccessLabel;
		System::Windows::Forms::GroupBox^ RateGroupBox;
		System::Windows::Forms::Label^ label15;
		System::Windows::Forms::ComboBox^ DeviceComboBox;
		System::Windows::Forms::Label^ FailureLabel;

		Thread^ XferThread;

		static CCyUSBDevice* USBDevice;

		static const int				MAX_QUEUE_SZ = 64;
		static const int				VENDOR_ID = 0x04B4;
		static const int				PRODUCT_ID = 0x00F1;

		// These declared static because accessed from the static XferLoop method
		// XferLoop is static because it is used as a separate thread.

		static CCyUSBEndPoint* EndPt;
		static int					PPX;
		static int					QueueSize;
		static int					TimeOut;
		static bool					bShowData;
		static bool					bStreaming;
		static bool                 bDeviceRefreshNeeded;
		static bool                 bAppQuiting;
		static bool					bHighSpeedDevice;
		static bool					bSuperSpeedDevice;

		static ProgressBar^ XferRateBar;
		static Label^ XferRateLabel;
		static TextBox^ DataBox;
		static TextBox^ SuccessBox;
		static TextBox^ FailureBox;

		static ComboBox^ EptsBox;
		static ComboBox^ PpxBox;
		static ComboBox^ QueueBox;
		static Button^ StartButton;
		static TextBox^ TimeoutBox;
		static CheckBox^ ShowBox;

		bool						bPnP_Arrival;
		bool						bPnP_Removal;
		private: System::Windows::Forms::PictureBox^ picCamera;

	    bool						bPnP_DevNodeChange;

		//-!
			//Vendor Commands
			static int fps = 0;
			static int frame = 0;
			static DateTime t1;
			static DateTime 	t2;

			              
			String^ configFile;
			System::Windows::Forms::Application^ app;
			
			static CCyControlEndPoint* CtrlEndPt = NULL;

			Byte *gbl_buffer;                      //Frame Buffer = Ht*Wd
			static int Ht = 480;                    //int Ht = 480;
			static int Wd = 752;                    //int Wd = 752;
			static int bytes = Wd * Ht;
			int BufSz;
			static int QueueSz;
			static PUCHAR* buffers;
			Byte* sample = new Byte[360960];
			Bitmap^ bmp;
			BitmapData ^bmpData;
			delegate void UpdateUICallback();
			static UpdateUICallback^ updateUI;

			int IsoPktBlockSize;
			Thread^ inThread;
		//-!!

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->EndPointsBox = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->PacketsPerXferBox = (gcnew System::Windows::Forms::ComboBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->TimeOutBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->StartBtn = (gcnew System::Windows::Forms::Button());
			this->DataTextBox = (gcnew System::Windows::Forms::TextBox());
			this->ShowDataBox = (gcnew System::Windows::Forms::CheckBox());
			this->RateGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->RateLabel = (gcnew System::Windows::Forms::Label());
			this->RateProgressBar = (gcnew System::Windows::Forms::ProgressBar());
			this->QueueLenBox = (gcnew System::Windows::Forms::ComboBox());
			this->SuccessesBox = (gcnew System::Windows::Forms::TextBox());
			this->FailuresBox = (gcnew System::Windows::Forms::TextBox());
			this->SuccessLabel = (gcnew System::Windows::Forms::Label());
			this->FailureLabel = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->DeviceComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->picCamera = (gcnew System::Windows::Forms::PictureBox());
			this->RateGroupBox->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picCamera))->BeginInit();
			this->SuspendLayout();
			// 
			// EndPointsBox
			// 
			this->EndPointsBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->EndPointsBox->Location = System::Drawing::Point(136, 53);
			this->EndPointsBox->MaxDropDownItems = 16;
			this->EndPointsBox->Name = L"EndPointsBox";
			this->EndPointsBox->Size = System::Drawing::Size(270, 21);
			this->EndPointsBox->Sorted = true;
			this->EndPointsBox->TabIndex = 3;
			this->EndPointsBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::EndPointsBox_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(17, 58);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(100, 16);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Endpoint . . . . . ";
			this->label1->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(17, 91);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(100, 16);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Packets per Xfer";
			this->label2->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// PacketsPerXferBox
			// 
			this->PacketsPerXferBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->PacketsPerXferBox->Items->AddRange(gcnew cli::array< System::Object^  >(10) {
				L"1", L"2", L"4", L"8", L"16", L"32", L"64",
					L"128", L"256", L"512"
			});
			this->PacketsPerXferBox->Location = System::Drawing::Point(136, 86);
			this->PacketsPerXferBox->Name = L"PacketsPerXferBox";
			this->PacketsPerXferBox->Size = System::Drawing::Size(76, 21);
			this->PacketsPerXferBox->TabIndex = 5;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(17, 123);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(88, 16);
			this->label4->TabIndex = 6;
			this->label4->Text = L"Xfers to Queue";
			this->label4->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// TimeOutBox
			// 
			this->TimeOutBox->Location = System::Drawing::Point(136, 154);
			this->TimeOutBox->Name = L"TimeOutBox";
			this->TimeOutBox->Size = System::Drawing::Size(76, 20);
			this->TimeOutBox->TabIndex = 9;
			this->TimeOutBox->Text = L"1500";
			this->TimeOutBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(17, 154);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(115, 16);
			this->label5->TabIndex = 8;
			this->label5->Text = L"Timeout Per Xfer (ms)";
			this->label5->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// StartBtn
			// 
			this->StartBtn->BackColor = System::Drawing::Color::Aquamarine;
			this->StartBtn->Location = System::Drawing::Point(226, 149);
			this->StartBtn->Name = L"StartBtn";
			this->StartBtn->Size = System::Drawing::Size(180, 28);
			this->StartBtn->TabIndex = 10;
			this->StartBtn->Text = L"Start";
			this->StartBtn->UseVisualStyleBackColor = false;
			this->StartBtn->Click += gcnew System::EventHandler(this, &Form1::StartBtn_Click);
			// 
			// DataTextBox
			// 
			this->DataTextBox->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->DataTextBox->Location = System::Drawing::Point(17, 295);
			this->DataTextBox->Multiline = true;
			this->DataTextBox->Name = L"DataTextBox";
			this->DataTextBox->Size = System::Drawing::Size(389, 112);
			this->DataTextBox->TabIndex = 18;
			this->DataTextBox->TabStop = false;
			// 
			// ShowDataBox
			// 
			this->ShowDataBox->Location = System::Drawing::Point(17, 271);
			this->ShowDataBox->Name = L"ShowDataBox";
			this->ShowDataBox->Size = System::Drawing::Size(144, 16);
			this->ShowDataBox->TabIndex = 17;
			this->ShowDataBox->Text = L"Show Transfered Data";
			// 
			// RateGroupBox
			// 
			this->RateGroupBox->Controls->Add(this->RateLabel);
			this->RateGroupBox->Controls->Add(this->RateProgressBar);
			this->RateGroupBox->Location = System::Drawing::Point(17, 182);
			this->RateGroupBox->Name = L"RateGroupBox";
			this->RateGroupBox->Size = System::Drawing::Size(389, 72);
			this->RateGroupBox->TabIndex = 15;
			this->RateGroupBox->TabStop = false;
			this->RateGroupBox->Text = L" Transfer Rate (KBps) ";
			// 
			// RateLabel
			// 
			this->RateLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->RateLabel->Location = System::Drawing::Point(155, 48);
			this->RateLabel->Name = L"RateLabel";
			this->RateLabel->Size = System::Drawing::Size(74, 16);
			this->RateLabel->TabIndex = 1;
			this->RateLabel->Text = L"0";
			this->RateLabel->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// RateProgressBar
			// 
			this->RateProgressBar->Location = System::Drawing::Point(18, 24);
			this->RateProgressBar->Maximum = 800000;
			this->RateProgressBar->Name = L"RateProgressBar";
			this->RateProgressBar->Size = System::Drawing::Size(350, 16);
			this->RateProgressBar->TabIndex = 0;
			// 
			// QueueLenBox
			// 
			this->QueueLenBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->QueueLenBox->Items->AddRange(gcnew cli::array< System::Object^  >(7) { L"1", L"2", L"4", L"8", L"16", L"32", L"64" });
			this->QueueLenBox->Location = System::Drawing::Point(136, 118);
			this->QueueLenBox->Name = L"QueueLenBox";
			this->QueueLenBox->Size = System::Drawing::Size(76, 21);
			this->QueueLenBox->TabIndex = 7;
			// 
			// SuccessesBox
			// 
			this->SuccessesBox->Location = System::Drawing::Point(298, 87);
			this->SuccessesBox->Name = L"SuccessesBox";
			this->SuccessesBox->Size = System::Drawing::Size(108, 20);
			this->SuccessesBox->TabIndex = 12;
			this->SuccessesBox->Text = L"0";
			this->SuccessesBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// FailuresBox
			// 
			this->FailuresBox->Location = System::Drawing::Point(298, 119);
			this->FailuresBox->Name = L"FailuresBox";
			this->FailuresBox->Size = System::Drawing::Size(108, 20);
			this->FailuresBox->TabIndex = 14;
			this->FailuresBox->Text = L"0";
			this->FailuresBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// SuccessLabel
			// 
			this->SuccessLabel->Location = System::Drawing::Point(223, 90);
			this->SuccessLabel->Name = L"SuccessLabel";
			this->SuccessLabel->Size = System::Drawing::Size(64, 16);
			this->SuccessLabel->TabIndex = 11;
			this->SuccessLabel->Text = L"Successes";
			this->SuccessLabel->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// FailureLabel
			// 
			this->FailureLabel->Location = System::Drawing::Point(223, 123);
			this->FailureLabel->Name = L"FailureLabel";
			this->FailureLabel->Size = System::Drawing::Size(64, 16);
			this->FailureLabel->TabIndex = 13;
			this->FailureLabel->Text = L"Failures";
			this->FailureLabel->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(17, 24);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(101, 13);
			this->label15->TabIndex = 0;
			this->label15->Text = L"Connected Devices";
			// 
			// DeviceComboBox
			// 
			this->DeviceComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->DeviceComboBox->FormattingEnabled = true;
			this->DeviceComboBox->Location = System::Drawing::Point(136, 19);
			this->DeviceComboBox->Name = L"DeviceComboBox";
			this->DeviceComboBox->Size = System::Drawing::Size(270, 21);
			this->DeviceComboBox->TabIndex = 1;
			this->DeviceComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::DeviceComboBox_SelectedIndexChanged);
			// 
			// picCamera
			// 
			this->picCamera->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->picCamera->Location = System::Drawing::Point(428, 19);
			this->picCamera->Name = L"picCamera";
			this->picCamera->Size = System::Drawing::Size(573, 388);
			this->picCamera->TabIndex = 19;
			this->picCamera->TabStop = false;
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(1013, 426);
			this->Controls->Add(this->picCamera);
			this->Controls->Add(this->DeviceComboBox);
			this->Controls->Add(this->label15);
			this->Controls->Add(this->FailureLabel);
			this->Controls->Add(this->SuccessLabel);
			this->Controls->Add(this->FailuresBox);
			this->Controls->Add(this->SuccessesBox);
			this->Controls->Add(this->QueueLenBox);
			this->Controls->Add(this->RateGroupBox);
			this->Controls->Add(this->DataTextBox);
			this->Controls->Add(this->ShowDataBox);
			this->Controls->Add(this->StartBtn);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->TimeOutBox);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->PacketsPerXferBox);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->EndPointsBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Streamer Camera";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Form1::Form_Closed);
			this->Load += gcnew System::EventHandler(this, &Form1::Form_Load);
			this->RateGroupBox->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picCamera))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Form_Load(System::Object^ sender, System::EventArgs^ e) {
		//-!
		configFile = String::Concat(app->StartupPath, "\\config\\sensor.conf", "");
		updateUI = gcnew UpdateUICallback(this,&Form1::StatusUpdate);
		//-!!
		XferRateBar = RateProgressBar;
		XferRateLabel = RateLabel;
		DataBox = DataTextBox;
		SuccessBox = SuccessesBox;
		FailureBox = FailuresBox;

		EptsBox = EndPointsBox;
		PpxBox = PacketsPerXferBox;
		QueueBox = QueueLenBox;
		StartButton = StartBtn;
		TimeoutBox = TimeOutBox;
		ShowBox = ShowDataBox;
		bDeviceRefreshNeeded = false;

		if (PacketsPerXferBox->SelectedIndex == -1) PacketsPerXferBox->SelectedIndex = 5;
		if (QueueLenBox->SelectedIndex == -1) QueueLenBox->SelectedIndex = 4;

		bmp = gcnew Bitmap(Wd, Ht, PixelFormat::Format8bppIndexed);

		GetStreamerDevice();
		XferThread = gcnew Thread(gcnew ThreadStart(&XferLoop));
	}
	
	private: System::Void Form_Closed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) {
		//if (XferThread->ThreadState == System::Threading::ThreadState::Suspended)
				//XferThread->Resume();

		bAppQuiting = true;
		bStreaming = false;  // Stop the thread's xfer loop     
		bDeviceRefreshNeeded = false;

		if (XferThread->ThreadState == System::Threading::ThreadState::Running)
			XferThread->Join(10);
	}

	void GetStreamerDevice();

	static void XferLoop();

	static void AbortXferLoop(int pending, PUCHAR* buffers, CCyIsoPktInfo** isoPktInfos, PUCHAR* contexts, OVERLAPPED inOvLap[]);

	static void ShowStats(DateTime t, long bytesXferred, unsigned long successes, unsigned long failures);

	static void Display16Bytes(PUCHAR data);

	static void Display(String^ s);

	static UInt64 HexToInt(String^ hexString);

	void EnforceValidPPX();

	void StatusUpdate();

	Void StartBtn_Click(System::Object^ sender, System::EventArgs^ e);

	Void DeviceComboBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
	
	Void EndPointsBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);

	void sendData();

	static void inImageData();

	static void setVBlanking(int vB);
};
}

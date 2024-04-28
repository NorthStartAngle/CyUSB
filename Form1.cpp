#include "pch.h"
#include "Form1.h"
#include <fstream>
#include <iostream>

namespace CppCLRWinFormsProject
{
	void Form1::GetStreamerDevice()
	{
        StartBtn->Enabled = false;

        EndPointsBox->Items->Clear();
        EndPointsBox->Text = "";

        if (USBDevice)
        {
            USBDevice->Close();
            delete USBDevice;
            USBDevice = NULL;
        }

        USBDevice = new CCyUSBDevice((HANDLE)this->Handle, CYUSBDRV_GUID, true);
        
        if (USBDevice == NULL) return;

        int n = USBDevice->DeviceCount();
        DeviceComboBox->Items->Clear();

        /////////////////////////////////////////////////////////////////
        // Walk through all devices looking for VENDOR_ID/PRODUCT_ID
        // We No longer got restricted with vendor ID and Product ID.
        // Check for vendor ID and product ID is discontinued.
        ///////////////////////////////////////////////////////////////////
        for (int i = 0; i < n; i++)
        {
            //if ((USBDevice->VendorID == VENDOR_ID) && (USBDevice->ProductID == PRODUCT_ID)) 
            //    break;
            USBDevice->Open(i);
            String^ strDeviceData = "";
            String^ strTemp = gcnew String(USBDevice->FriendlyName);
            strDeviceData = String::Concat(strDeviceData, "(0x");
            strDeviceData = String::Concat(strDeviceData, USBDevice->VendorID.ToString("X4"));
            strDeviceData = String::Concat(strDeviceData, " - 0x");
            strDeviceData = String::Concat(strDeviceData, USBDevice->ProductID.ToString("X4"));
            strDeviceData = String::Concat(strDeviceData, ") ");
            strDeviceData = String::Concat(strDeviceData, strTemp);

            DeviceComboBox->Items->Add(strDeviceData);
            DeviceComboBox->Enabled = true;
        }
        if (n > 0) {
            DeviceComboBox->SelectedIndex = 0;
            USBDevice->Open(0);
        }


        //if ((USBDevice->VendorID == VENDOR_ID) && (USBDevice->ProductID == PRODUCT_ID)) 
        {
            StartBtn->Enabled = true;

            int interfaces = USBDevice->AltIntfcCount() + 1;

            bHighSpeedDevice = USBDevice->bHighSpeed;
            bSuperSpeedDevice = USBDevice->bSuperSpeed;

            for (int i = 0; i < interfaces; i++)
            {
                if (USBDevice->SetAltIntfc(i) == true)
                {

                    int eptCnt = USBDevice->EndPointCount();

                    // Fill the EndPointsBox
                    for (int e = 1; e < eptCnt; e++)
                    {
                        CCyUSBEndPoint* ept = USBDevice->EndPoints[e];
                        // INTR, BULK and ISO endpoints are supported.
                        if ((ept->Attributes >= 1) && (ept->Attributes <= 3))
                        {
                            String^ s = "";
                            s = String::Concat(s, ((ept->Attributes == 1) ? "ISOC " :
                                ((ept->Attributes == 2) ? "BULK " : "INTR ")));
                            s = String::Concat(s, ept->bIn ? "IN,       " : "OUT,   ");
                            s = String::Concat(s, ept->MaxPktSize.ToString(), " Bytes,");
                            if (USBDevice->BcdUSB == USB30MAJORVER)
                                s = String::Concat(s, ept->ssmaxburst.ToString(), " MaxBurst,");

                            s = String::Concat(s, "   (", i.ToString(), " - ");
                            s = String::Concat(s, "0x", ept->Address.ToString("X02"), ")");
                            EndPointsBox->Items->Add(s);
                        }
                    }
                }
            }
            if (EndPointsBox->Items->Count > 0)
                EndPointsBox->SelectedIndex = 0;
            else
                StartBtn->Enabled = false;
        }
	}

	void Form1::XferLoop()
	{
        long BytesXferred = 0;
        unsigned long Successes = 0;
        unsigned long Failures = 0;
        int i = 0;
        
        // Allocate the arrays needed for queueing
        PUCHAR* buffers = new PUCHAR[QueueSize];
        CCyIsoPktInfo** isoPktInfos = new CCyIsoPktInfo * [QueueSize];
        PUCHAR* contexts = new PUCHAR[QueueSize];
        OVERLAPPED		inOvLap[MAX_QUEUE_SZ];

        long len = EndPt->MaxPktSize * PPX; // Each xfer request will get PPX isoc packets

        EndPt->SetXferSize(len);

        // Allocate all the buffers for the queues
        for (i = 0; i < QueueSize; i++)
        {
            buffers[i] = new UCHAR[len];
            isoPktInfos[i] = new CCyIsoPktInfo[PPX];
            inOvLap[i].hEvent = CreateEvent(NULL, false, false, NULL);

            memset(buffers[i], 0xEF, len);
        }

        DateTime t1 = DateTime::Now;	// For calculating xfer rate

        // Queue-up the first batch of transfer requests
        for (i = 0; i < QueueSize; i++)
        {
            contexts[i] = EndPt->BeginDataXfer(buffers[i], len, &inOvLap[i]);
            if (EndPt->NtStatus || EndPt->UsbdStatus) // BeginDataXfer failed
            {
                Display(String::Concat("Xfer request rejected. NTSTATUS = ", EndPt->NtStatus.ToString("x")));
                AbortXferLoop(i + 1, buffers, isoPktInfos, contexts, inOvLap);
                return;
            }
        }

        i = 0;

        // The infinite xfer loop.
        for (; bStreaming;)
        {
            long rLen = len;	// Reset this each time through because
            // FinishDataXfer may modify it

            if (!EndPt->WaitForXfer(&inOvLap[i], TimeOut))
            {
                EndPt->Abort();
                if (EndPt->LastError == ERROR_IO_PENDING)
                    WaitForSingleObject(inOvLap[i].hEvent, 2000);
            }

            if (EndPt->Attributes == 1) // ISOC Endpoint
            {
                if (EndPt->FinishDataXfer(buffers[i], rLen, &inOvLap[i], contexts[i], isoPktInfos[i]))
                {
                    CCyIsoPktInfo* pkts = isoPktInfos[i];
                    for (int j = 0; j < PPX; j++)
                    {
                        if ((pkts[j].Status == 0) && (pkts[j].Length <= EndPt->MaxPktSize))
                        {
                            BytesXferred += pkts[j].Length;

                            if (bShowData)
                                Display16Bytes(buffers[i]);

                            Successes++;
                        }
                        else
                            Failures++;

                        pkts[j].Length = 0;	// Reset to zero for re-use.
                        pkts[j].Status = 0;
                    }

                }
                else
                    Failures++;

            }

            else // BULK Endpoint
            {
                if (EndPt->FinishDataXfer(buffers[i], rLen, &inOvLap[i], contexts[i]))
                {
                    Successes++;
                    BytesXferred += rLen;

                    if (bShowData)
                        Display16Bytes(buffers[i]);
                }
                else
                    Failures++;
            }


            if (BytesXferred < 0) // Rollover - reset counters
            {
                BytesXferred = 0;
                t1 = DateTime::Now;
            }

            // Re-submit this queue element to keep the queue full
            contexts[i] = EndPt->BeginDataXfer(buffers[i], len, &inOvLap[i]);
            if (EndPt->NtStatus || EndPt->UsbdStatus) // BeginDataXfer failed
            {
                Display(String::Concat("Xfer request rejected. NTSTATUS = ", EndPt->NtStatus.ToString("x")));
                AbortXferLoop(QueueSize, buffers, isoPktInfos, contexts, inOvLap);
                return;
            }

            i++;

            if (i == QueueSize) //Only update the display once each time through the Queue
            {
                i = 0;
                ShowStats(t1, BytesXferred, Successes, Failures);
            }

        }  // End of the infinite loop

        // Memory clean-up
        AbortXferLoop(QueueSize, buffers, isoPktInfos, contexts, inOvLap);
	}

    void Form1::AbortXferLoop(int pending, PUCHAR* buffers, CCyIsoPktInfo** isoPktInfos, PUCHAR* contexts, OVERLAPPED inOvLap[])
    {
        //EndPt->Abort(); - This is disabled to make sure that while application is doing IO and user unplug the device, this function hang the app.
        long len = EndPt->MaxPktSize * PPX;
        EndPt->Abort();

        for (int j = 0; j < QueueSize; j++)
        {
            if (j < pending)
            {
                EndPt->WaitForXfer(&inOvLap[j], TimeOut);
                /*{
                    EndPt->Abort();
                    if (EndPt->LastError == ERROR_IO_PENDING)
                        WaitForSingleObject(inOvLap[j].hEvent,2000);
                }*/
                EndPt->FinishDataXfer(buffers[j], len, &inOvLap[j], contexts[j]);
            }

            CloseHandle(inOvLap[j].hEvent);

            delete[] buffers[j];
            delete[] isoPktInfos[j];
        }

        delete[] buffers;
        delete[] isoPktInfos;
        delete[] contexts;


        bStreaming = false;

        if (bAppQuiting == false)
        {
            StartButton->Text = "Start";
            StartButton->BackColor = Color::Aquamarine;
            StartButton->Refresh();

            EptsBox->Enabled = true;
            PpxBox->Enabled = true;
            QueueBox->Enabled = true;
            TimeoutBox->Enabled = true;
            ShowBox->Enabled = true;
        }
    }

    void Form1::ShowStats(DateTime t, long bytesXferred, unsigned long successes, unsigned long failures)
    {
        TimeSpan elapsed = DateTime::Now.Subtract(t);

        long totMS = (long)elapsed.TotalMilliseconds;
        if (totMS <= 0)	return;

        unsigned long XferRate = bytesXferred / totMS;

        // Convert to KBps
        XferRate = XferRate * 1000 / 1024;

        // Truncate last 1 or 2 digits
        int rounder = (XferRate > 2000) ? 100 : 10;
        XferRate = XferRate / rounder * rounder;

        if (XferRate > 625000)
            XferRate = 625000;
        //thread safe-commented
        CheckForIllegalCrossThreadCalls = false;

        XferRateBar->Value = XferRate;
        XferRateLabel->Text = XferRate.ToString();

        SuccessBox->Text = successes.ToString();
        FailureBox->Text = failures.ToString();
    }

    void Form1::Display16Bytes(PUCHAR data)
    {
        String^ xData = "";

        for (int i = 0; i < 16; i++)
            xData = String::Concat(xData, data[i].ToString("X02"), " ");

        Display(xData);
    }

    void Form1::Display(String^ s)
    {
        DataBox->Text = String::Concat(DataBox->Text, s, "\r\n");
        DataBox->SelectionStart = DataBox->Text->Length;
        DataBox->ScrollToCaret();
    }

    UInt64 Form1::HexToInt(String^ hexString)
    {
        String^ HexChars = "0123456789abcdef";

        String^ s = hexString->ToLower();

        // Trim off the 0x prefix
        if (s->Length > 2)
            if (s->Substring(0, 2)->Equals("0x"))
                s = s->Substring(2, s->Length - 2);


        String^ _s = "";
        int len = s->Length;

        // Reverse the digits
        for (int i = len - 1; i >= 0; i--) _s = String::Concat(_s, s->Substring(i, 1));

        UInt64 sum = 0;
        UInt64 pwrF = 1;
        for (int i = 0; i < len; i++)
        {
            UInt32 ordinal = (UInt32)HexChars->IndexOf(_s->Substring(i, 1));
            sum += (i == 0) ? ordinal : pwrF * ordinal;
            pwrF *= 16;
        }


        return sum;
    }

    void Form1::EnforceValidPPX()
    {
        if (PacketsPerXferBox->SelectedIndex == -1) PacketsPerXferBox->SelectedIndex = 5;
        if (QueueLenBox->SelectedIndex == -1) QueueLenBox->SelectedIndex = 4;
        PPX = Convert::ToInt32(PacketsPerXferBox->Text);

        if (EndPt->MaxPktSize == 0)
            return;

        // Limit total transfer length to 4MByte
        int len = ((EndPt->MaxPktSize) * PPX);

        int maxLen = 0x400000;  //4MByte
        if (len > maxLen)
        {

            PPX = maxLen / (EndPt->MaxPktSize);
            if ((PPX % 8) != 0)
                PPX -= (PPX % 8);

            //int iIndex = PacketsPerXferBox->SelectedIndex;
            //PacketsPerXferBox->Items->Remove(PacketsPerXferBox->Text);
            //PacketsPerXferBox->Items->Insert(iIndex,PPX.ToString());
            //PacketsPerXferBox->SelectedIndex = iIndex;
            PacketsPerXferBox->SelectedIndex = PacketsPerXferBox->Items->IndexOf(PPX.ToString());

            DataBox->Text = String::Concat(DataBox->Text, "Total Xfer length limited to 4Mbyte.\r\n");
            DataBox->Text = String::Concat(DataBox->Text, "Xfer Length = Packets_Per_Xfer * Ep_MaxPktSize\r\n");
            DataBox->Text = String::Concat(DataBox->Text, "Packets per Xfer has been adjusted.\r\n");
            DataBox->SelectionStart = DataBox->Text->Length;
            DataBox->ScrollToCaret();
        }

        if ((bSuperSpeedDevice || bHighSpeedDevice) && (EndPt->Attributes == 1))  // HS/SS ISOC Xfers must use PPX >= 8
        {
            if (PPX < 8)
            {
                PPX = 8;
                Display("ISOC xfers require at least 8 Packets per Xfer.");
                Display("Packets per Xfer has been adjusted.");
            }

            PPX = (PPX / 8) * 8;

            if (bHighSpeedDevice)
            {
                if (PPX > 128)
                {
                    PPX = 128;
                    Display("Hish Speed ISOC xfers does not support more than 128 Packets per transfer");
                }
            }
        }

        PacketsPerXferBox->Text = PPX.ToString();
    }

    void Form1::StartBtn_Click(System::Object^ sender, System::EventArgs^ e)
    {

        Decimal db;
        int ab;

        if (!Decimal::TryParse(this->TimeOutBox->Text, db))
        {
            ::MessageBox(NULL, L"Invalid Input : TimeOut Per Xfer(ms)", L"Streamer", 0);
            this->TimeOutBox->Text = "";
            return;
        }
        if (!Int32::TryParse(this->TimeOutBox->Text, ab))
        {
            ::MessageBox(NULL, L"Invalid Input : TimeOut Per Xfer(ms)", L"Streamer", 0);
            this->TimeOutBox->Text = "";
            return;
        }

        if (StartBtn->Text->Equals("Start"))
        {
           sendData();
            //EptsBox->Enabled = false;
            StartBtn->Text = "Stop";

            BufSz = bytes;
            int QueueSz = Convert::ToInt32(QueueBox->Text);
            PPX = Convert::ToInt32(PacketsPerXferBox->Text);
            EndPt->SetXferSize(Convert::ToInt32(QueueBox->Text));
            CCyIsocEndPoint* isoPt =(CCyIsocEndPoint*)EndPt;
            if (isoPt != NULL)
            {
                if (isoPt->MaxPktSize == 0)
                {
                    IsoPktBlockSize = 0;
                }
                else {
                    int num = BufSz / isoPt->MaxPktSize;
                    if (BufSz % isoPt->MaxPktSize > 0)
                    {
                        num++;
                    }
                    if (num == 0)
                    {
                        IsoPktBlockSize = 0;
                    }
                    else {
                        IsoPktBlockSize = num * sizeof(UInt16)*2;
                    }                    
                }
            }
            else {
                IsoPktBlockSize = 0;
            }

            XferThread = gcnew Thread(gcnew ThreadStart(&XferLoop));
            XferThread->IsBackground = true;
            XferThread->Priority = ThreadPriority::Highest;
            XferThread->Start();
        }
        else {
            if (XferThread->IsAlive)
            {
                if (XferThread->Join(10)) XferThread->Abort();

            }
        }

        return;
        if (XferThread) {
            switch (XferThread->ThreadState)
            {
            case System::Threading::ThreadState::Stopped:
            case System::Threading::ThreadState::Unstarted:

                if (EndPt == NULL) EndPointsBox_SelectedIndexChanged(nullptr, nullptr);
                else EnforceValidPPX();

                StartBtn->Text = "Stop";
                SuccessBox->Text = "";
                FailureBox->Text = "";
                StartBtn->BackColor = Color::MistyRose;
                StartBtn->Refresh();

                bStreaming = true;

                // Start-over, initializing counters, etc.
                if ((XferThread->ThreadState) == System::Threading::ThreadState::Stopped)
                    XferThread = gcnew Thread(gcnew ThreadStart(&XferLoop));

                PPX = Convert::ToInt32(PacketsPerXferBox->Text);

                QueueSize = Convert::ToInt32(QueueLenBox->Text);
                TimeOut = Convert::ToInt32(TimeOutBox->Text);
                bShowData = ShowDataBox->Checked;

                EndPointsBox->Enabled = false;
                PacketsPerXferBox->Enabled = false;
                QueueLenBox->Enabled = false;
                TimeOutBox->Enabled = false;
                ShowDataBox->Enabled = false;
                DeviceComboBox->Enabled = false;

                XferThread->Start();
                break;
            case System::Threading::ThreadState::Running:
                StartBtn->Text = "Start";
                StartBtn->BackColor = Color::Aquamarine;
                StartBtn->Refresh();

                bStreaming = false;  // Stop the thread's xfer loop
                XferThread->Join(10);

                EndPointsBox->Enabled = true;
                PacketsPerXferBox->Enabled = true;
                QueueLenBox->Enabled = true;
                TimeOutBox->Enabled = true;
                ShowDataBox->Enabled = true;
                DeviceComboBox->Enabled = true;

                if (bDeviceRefreshNeeded == true)
                {
                    bDeviceRefreshNeeded = false;
                    GetStreamerDevice();
                }

                break;
            }
        }
    }

    void Form1::sendData()
    {
        if (USBDevice == NULL) return;
        CtrlEndPt = USBDevice->ControlEndPt;
        LONG len = 0;
        unsigned char* bufBegin = new unsigned char[3];
        bufBegin[0] = 0x00;
        bufBegin[1] = 0x00;
        bufBegin[2] = 0x00;

        //Vendor Command Format : 0xAA to configure the Image Sensor and add the Header
        CtrlEndPt->Target = CTL_XFER_TGT_TYPE::TGT_DEVICE;
        CtrlEndPt->ReqType = CTL_XFER_REQ_TYPE::REQ_VENDOR;
        CtrlEndPt->Direction = CTL_XFER_DIR_TYPE::DIR_TO_DEVICE;
        CtrlEndPt->ReqCode = VX_AA;                               // to configure Image sensor
        CtrlEndPt->Value = 0;
        CtrlEndPt->Index = 0;
        CtrlEndPt->XferData(bufBegin, len);

        
        IntPtr p = Marshal::StringToHGlobalAnsi(configFile);
        char* pNewCharStr = static_cast<char*>(p.ToPointer());

        std::ifstream infile(pNewCharStr);
        if (!infile.is_open()) {
            return ;
        }

        string line;
        while (getline(infile, line)) {
            
        }

        // Close the file 
        infile.close();
    }

    Void Form1::DeviceComboBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
    {
        if (DeviceComboBox->SelectedIndex == -1) return;

        if (USBDevice->IsOpen() == true) USBDevice->Close();
        USBDevice->Open(DeviceComboBox->SelectedIndex);

        int interfaces = USBDevice->AltIntfcCount() + 1;

        bHighSpeedDevice = USBDevice->bHighSpeed;
        bSuperSpeedDevice = USBDevice->bSuperSpeed;

        EndPointsBox->Items->Clear();

        for (int i = 0; i < interfaces; i++)
        {
            if (USBDevice->SetAltIntfc(i) == true)
            {

                int eptCnt = USBDevice->EndPointCount();

                // Fill the EndPointsBox
                for (int e = 1; e < eptCnt; e++)
                {

                    CCyUSBEndPoint* ept = USBDevice->EndPoints[e];
                    // INTR, BULK and ISO endpoints are supported.
                    if ((ept->Attributes >= 1) && (ept->Attributes <= 3))
                    {
                        String^ s = "";
                        s = String::Concat(s, ((ept->Attributes == 1) ? "ISOC " :
                            ((ept->Attributes == 2) ? "BULK " : "INTR ")));
                        s = String::Concat(s, ept->bIn ? "IN,       " : "OUT,   ");
                        s = String::Concat(s, ept->MaxPktSize.ToString(), " Bytes,");
                        if (USBDevice->BcdUSB == USB30MAJORVER)
                            s = String::Concat(s, ept->ssmaxburst.ToString(), " MaxBurst,");

                        s = String::Concat(s, "   (", i.ToString(), " - ");
                        s = String::Concat(s, "0x", ept->Address.ToString("X02"), ")");
                        EndPointsBox->Items->Add(s);
                    }
                }
            }
        }
        if (EndPointsBox->Items->Count > 0)
        {
            EndPointsBox->SelectedIndex = 0;
            EndPointsBox_SelectedIndexChanged(nullptr, nullptr);
            StartBtn->Enabled = true;
        }
        else
            StartBtn->Enabled = false;
    }

    Void Form1::EndPointsBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
    {
        // Parse the alt setting and endpoint address from the EndPointsBox->Text
        String^ tmp = EndPointsBox->Text->Substring(EndPointsBox->Text->IndexOf("("), 10);
        int  alt = Convert::ToInt32(tmp->Substring(1, 1));

        String^ addr = tmp->Substring(7, 2);
        //changed int to __int64 to avoid data loss
        __int64 eptAddr = HexToInt(addr);

        int clrAlt = (USBDevice->AltIntfc() == 0) ? 1 : 0;

        // Attempt to set the selected Alt setting and get the endpoint
        if (!USBDevice->SetAltIntfc(alt))
        {
            MessageBox::Show("Alt interface could not be selected.", "USB Exception", MessageBoxButtons::OK, MessageBoxIcon::Hand);
            StartBtn->Enabled = false;
            USBDevice->SetAltIntfc(clrAlt); // Cleans-up
            return;
        }


        EndPt = USBDevice->EndPointOf((UCHAR)eptAddr);

        StartBtn->Enabled = true;


        if (EndPt->Attributes == 1)
        {
            SuccessLabel->Text = "Good Pkts";
            FailureLabel->Text = "Bad Pkts";
        }
        else
        {
            SuccessLabel->Text = "Successes";
            FailureLabel->Text = "Failures";
        }

        EnforceValidPPX();
    }
    
    void Form1::StatusUpdate()
    {

    }
}
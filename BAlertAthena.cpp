#include <string>
#include <iostream>
#include <tchar.h>
#include <filesystem>

// BAlert API
#include <Windows.h>
#include "AbmSdkInclude.h"
#pragma comment(lib, "BAlert.lib")
// LSL API
#include <lsl_cpp.h>

// some constant device properties
const char* const name_by_type[] = { "None","X10","X24","X4" };
int channels_by_type[] = { 0,10,24,4 };
const int nSamples = 8;
const int sRate = 256;


int main(int argc, char* argv[])
{
	try {

		std::cout << "checking configuration ..." << std::endl;

		std::filesystem::path cfgfoldername = ".\\Config";
		std::filesystem::path cfgpath = std::filesystem::absolute(cfgfoldername);
		std::string fullPathChar = cfgpath.string();

		bool configExists = std::filesystem::exists(cfgpath);
		if (!configExists) {
			throw std::runtime_error("Config folder is missing.");
		}

		TCHAR* fullPathT = new TCHAR[fullPathChar.size() + 1];
		fullPathT[fullPathChar.size()] = 0;
		std::copy(fullPathChar.begin(), fullPathChar.end(), fullPathT);
		bool configPath = SetConfigPath(fullPathT);

        std::cout << "querying device info..." << std::endl;

		_DEVICE_INFO* devInfo = GetDeviceInfoKeepConnection(NULL);
		if (devInfo == NULL) {
			CloseCurrentConnection();
			throw std::runtime_error("No device found. Is it plugged in?");
		}


		#ifdef  UNICODE
			std::wstring device_name(devInfo->chDeviceName);
		#else
			std::string device_name(devInfo->chDeviceName);
		#endif

		bool bFlexX24 = device_name.find(TEXT("X24t")) != std::string::npos;
		bool bFlexX10 = device_name.find(TEXT("X10t")) != std::string::npos;



		int device_id;
		if (devInfo->nNumberOfChannel == 24) {
			if (bFlexX24)
				device_id = ABM_DEVICE_X24Flex_10_20;
			else
				device_id = ABM_DEVICE_X24Standard;
		}
		else if (devInfo->nNumberOfChannel == 10) {
			if (bFlexX10)
				device_id = ABM_DEVICE_X10Flex_Standard;
			else if (bFlexX24)
				device_id = ABM_DEVICE_X24Flex_Reduced;
			else
				device_id = ABM_DEVICE_X10Standard;
		}
		else {
			throw std::runtime_error("This device is not currently supported ");
		}

		int init_res = InitSessionForCurrentConnection(device_id, ABM_SESSION_RAW, -1, false);
		if (init_res != INIT_SESSION_OK) {
			throw std::runtime_error("Could not initialize ABM session.");
		}

		int nChannels, nDeconPacketChannelsNmb, nPSDPacketChannelsNmb,
			nRawPSDPacketChannelNmb, nQualityPacketChannelNmb;
		int pkt_chan_res = GetPacketChannelNmbInfo(nChannels, nDeconPacketChannelsNmb,
			nPSDPacketChannelsNmb, nRawPSDPacketChannelNmb, nQualityPacketChannelNmb);

		// create streaminfo
		std::string modelname = std::string("BAlert");  // TOOD: Get device name from devInfo
		lsl::stream_info info(
			modelname, "EEG", nChannels + 6, sRate, lsl::cf_float32, modelname + "_sdetsff");
		// append some meta-data
		info.desc()
			.append_child("acquisition")
			.append_child_value("manufacturer", "Advanced Brain Monitoring")
			.append_child_value("model", modelname.c_str());
		lsl::xml_element chans_info = info.desc().append_child("channels");
		chans_info.append_child("channel").append_child_value("label", "Epoch");
		chans_info.append_child("channel").append_child_value("label", "Offset");
		chans_info.append_child("channel").append_child_value("label", "Hour");
		chans_info.append_child("channel").append_child_value("label", "Min");
		chans_info.append_child("channel").append_child_value("label", "Sec");
		chans_info.append_child("channel").append_child_value("label", "MilliSec");
		// TODO: Handle channel names from other device types.

		_CHANNELMAP_INFO stChannelMapInfo;
		int chan_map_res = GetChannelMapInfo(stChannelMapInfo);

		// std::vector<std::string> chan_names = {"Fp1", "F7", "F8", "T4", "T6", "T5", "T3", "Fp2", "O1", "P3", "Pz", "F3", "Fz", "F4", "C4", "P4", "POz", "C3", "Cz", "O2", "EKG", "AUX1", "AUX2", "AUX3"};
		for (size_t i = 0; i < nChannels; i++) {
			chans_info.append_child("channel").append_child_value(
				"label", stChannelMapInfo.stEEGChannels.cChName[i]);
			// chans_info.append_child("channel").append_child_value("label", chan_names[i]);
		}

		// make a new outlet
		lsl::stream_outlet outlet(info);

		std::cout << "LSL Initialized.\n";

		// try to connect
		int start_res = StartAcquisitionForCurrentConnection();
		if (start_res != ACQ_STARTED_OK) {
			throw std::runtime_error("Could not start ABM acquisition.");
		}

		std::cout << "Acquisition Started.\n";


		// enter transmission loop
		int nReceived;
		while (true) {
			float* pData;
			pData = GetRawData(nReceived);
			if (pData) {
				// pData size is (nChannels+6)*nReceived
				for (size_t k = 0; k < nReceived; k++) {
					outlet.push_sample(&pData[k * (nChannels + 6)]);
				}
			}
		}
		int stop_res = StopAcquisition();
		if (stop_res != ACQ_STOPPED_OK) throw std::runtime_error("Failed to stop acquisition.");

	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}


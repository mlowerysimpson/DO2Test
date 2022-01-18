//DO2Test.cpp
//this program reads dissolved oxygen data from an Atlas Scientific EZO-DO (TM) Embedded Dissolved Oxygen Circuit 

#include <stdio.h>
#include <wiringPi.h>
#include "../RemoteControlTest/AtlasDO2Sensor.h"
#include "../RemoteControlTest/Util.h"

#define NUM_DO2_SAMPLES 10

int CheckForFlag(int argc, char** argv, char* szFlag) {//check to see if szFlag can be found in any of the argv elements
	for (int i = 0; i < argc; i++) {
		if (strstr(argv[i], szFlag)) {
			return i;
		}
	}
	return -1;
}

void ShowUsage() {
	int nNumDO2Samples = NUM_DO2_SAMPLES;
	printf("DO2Test\n");
	printf("Usage:\n");
	printf("DO2Test     --> without any arguments, the program collects and displays %d samples of dissolved oxygen data.\n",nNumDO2Samples);
	printf("DO2Test -cal  --> perform a one-point calibration of the DO2 sensor in air.\n");
	printf("DO2Test -plock <0 or 1> --> turn the I2C protocol lock feature on (1) or off (0).\n");
	printf("DO2Test -h --> displays this help message.\n");
}

int DoCalibration(AtlasDO2Sensor *do2Sensor) {
	printf("Make sure that the DO2 sensor is connected and exposed to ambient air and then press any key to get started. Once the readings have stabilized, press the 'C' key to do the actual calibration in air.\n");

	int nKeyboardChar = -1;
	while (nKeyboardChar < 0) {
		nKeyboardChar = Util::getch_noblock();
		usleep(10000);
	}
	nKeyboardChar = -1;
	double dDO2Conc = 0.0;
	while (nKeyboardChar != 'c' && nKeyboardChar != 'C') {
		nKeyboardChar = Util::getch_noblock();
		if (do2Sensor->GetDO2(dDO2Conc)) {
			printf("dissolved O2 = %.2f mg/L\n", dDO2Conc);
		}
		usleep(10000);
	}
	//do calibration
	if (do2Sensor->Calibrate()) {
		printf("Calibrated successfully.\n");
	}
	else {
		printf("Error, unable to calibrate.\n");
	}
	return 0;
}

void ProtocolLock(int nLock, AtlasDO2Sensor *do2Sensor) {
	if (!do2Sensor->ProtocolLock(nLock)) {
		printf("Error, unable to change protocol lock.\n");
	}
	else {
		if (nLock > 0) {
			printf("I2C protocol lock successfully enabled.\n");
		}
		else {
			printf("I2C protocol lock successfully removed.\n");
		}
	}
}


int main(int argc, char** argv)
{
    const int NUM_SAMPLES = NUM_DO2_SAMPLES;
	pthread_mutex_t i2cMutex = PTHREAD_MUTEX_INITIALIZER;
	AtlasDO2Sensor do2Sensor(&i2cMutex);

	double dDissolvedO2 = 0.0;//concentration of dissolved oxygen in mg/L
	
	// GPIO Initialization
	if (wiringPiSetupGpio() == -1)
	{
		printf("[x_x] GPIO Initialization FAILED.\n");
		return -1;
	}
	int nDoCal = CheckForFlag(argc, argv, (char*)"-cal");//check to see if calibration should be performed
	if (nDoCal >= 1) {
		return DoCalibration(&do2Sensor);
	}
	int nDoProtcolLock = CheckForFlag(argc, argv, (char*)"-plock");//check to see if the I2C protocol lock should be changed
	if (nDoProtcolLock >= 1) {
		if (argc < 3) {
			printf("Need to specify a third parameter (0 or 1) for the protocol lock flag.\n");
			return -2;
		}
		int nProtocolLock = 0;
		sscanf(argv[2], "%d", &nProtocolLock);
		ProtocolLock(nProtocolLock, &do2Sensor);
		return 0;
	}
	int nHelp1 = CheckForFlag(argc, argv, (char*)"-h");//check to see if help is requested 
	int nHelp2 = CheckForFlag(argc, argv, (char*)"-H");//check to see if help is requested 
	if (nHelp1 >= 1||nHelp2>=1) {
		ShowUsage();
		return 0;
	}
	for (int i = 0; i < NUM_SAMPLES; i++) {
		if (do2Sensor.GetDO2(dDissolvedO2)) {
			printf("dissolved O2 = %.2f mg/L\n", dDissolvedO2);
		}
	}
	return 0;
}
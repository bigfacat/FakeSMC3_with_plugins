/*
 *  SMIMonitor.h
 *  HWSensors
 *
 *  Copyright (C) 2001  Massimo Dal Zotto <dz@debian.org>
 *  http://www.debian.org/~dz/i8k/
 *  more work https://www.diefer.de/i8kfan/index.html , 2007
 *
 *  FakeSMC plugin created by Slice 2014.
 *
 */

#include <IOKit/IOService.h>
#include "IOKit/acpi/IOACPIPlatformDevice.h"
#include <IOKit/IOTimerEventSource.h>
#include <i386/proc_reg.h>

#define I8K_SMM_FN_STATUS				0x0025
#define I8K_SMM_POWER_STATUS			0x0069   /* 0x85*/ /* not confirmed*/
#define I8K_SMM_SET_FAN					  0x01a3
#define I8K_SMM_GET_FAN					  0x00a3
#define I8K_SMM_GET_SPEED				  0x02a3
#define I8K_SMM_GET_FAN_TYPE			0x03a3
#define I8K_SMM_GET_NOM_SPEED			0x04a3
#define I8K_SMM_GET_TOLERANCE			0x05a3
#define I8K_SMM_GET_TEMP				  0x10a3
#define I8K_SMM_GET_TEMP_TYPE			0x11a3
//0x12a3  arg 0x0003=NBSVC-Query
//	arg 0x0000 = NBSVC - Clear
//	arg 0x122 = NBSVC - Start Trend
//	arg 0x0100 = NBSVC - Stop Trend
//	arg 0x02 ? ? = NBSVC - Read
//0x21a3  ??? (2 args: 1 byte (oder 0x16) + 1 byte)
#define I8K_SMM_GET_POWER_TYPE			0x22a3
//0x23a3  ??? (4 args: 2x 1 byte, 1xword, 1xdword)
#define I8K_SMM_GET_POWER_STATUS		0x24a3
#define I8K_SMM_IO_DISABLE_FAN_CTL1     0x30a3 //No automatic speed control, arg = 0? fan?
#define I8K_SMM_IO_ENABLE_FAN_CTL1      0x31a3
#define I8K_SMM_ENABLE_FN				0x32a3
#define I8K_SMM_IO_DISABLE_FAN_CTL2     0x34a3 //A complete strike. no command.
#define I8K_SMM_IO_ENABLE_FAN_CTL2      0x35a3
//0x36a3  get hotkey scancode list (args see diags)
#define I8K_SMM_GET_DOCK_STATE			0x40a3
#define I8K_SMM_GET_DELL_SIG1			  0xfea3
#define I8K_SMM_GET_DELL_SIG2			  0xffa3
#define I8K_SMM_BIOS_VERSION			  0x00a6  /* not confirmed*/

// GET_TEMP_TYPE result codes
#define I8K_SMM_TEMP_CPU      0
#define I8K_SMM_TEMP_GPU      1
#define I8K_SMM_TEMP_MEMORY   2
#define I8K_SMM_TEMP_MISC     3
#define I8K_SMM_TEMP_AMBIENT  4
#define I8K_SMM_TEMP_OTHER    5

#define I8K_FAN_MULT            30
#define I8K_MAX_TEMP            127

#define I8K_FAN_PROCESSOR       0
#define I8K_FAN_SYSTEM          1
#define I8K_FAN_GPU             2
#define I8K_FAN_PSU             3
#define I8K_FAN_CHIPSET         4

#define I8K_FAN_OFF             0
#define I8K_FAN_LOW             1
#define I8K_FAN_HIGH            2
#define I8K_FAN_MAX             I8K_FAN_HIGH

#define I8K_POWER_AC            0x05
#define I8K_POWER_BATTERY       0x01
#define I8K_AC                  1
#define I8K_BATTERY             0

#define I8K_FN_NONE		0x00
#define I8K_FN_UP		  0x01
#define I8K_FN_DOWN		0x02
#define I8K_FN_MUTE		0x04
#define I8K_FN_MASK		0x07
#define I8K_FN_SHIFT		8

typedef struct {
  unsigned int eax;
  unsigned int ebx __attribute__ ((packed));
  unsigned int ecx __attribute__ ((packed));
  unsigned int edx __attribute__ ((packed));
  unsigned int esi __attribute__ ((packed));
  unsigned int edi __attribute__ ((packed));
} SMMRegisters;

#define INIT_REGS               SMMRegisters regs = { 0, 0, 0, 0, 0, 0 }

extern "C" {
  void mp_rendezvous_no_intrs(void (*action_func)(void *), void * arg);
  int cpu_number(void);
};


class SMIMonitor : public IOService
{
  OSDeclareDefaultStructors(SMIMonitor)
private:
	IOService*				      fakeSMC;
	IOACPIPlatformDevice *	acpiDevice;
	OSDictionary*			      sensors;
	int						fanMult;
	int						fanNum;
	UInt16				fansStatus;
  int           TempSensors[6];
	
	bool				addSensor(const char* key, const char* type, unsigned int size);
	bool				addTachometer(int index, const char* caption);
  
  int  i8k_smm(SMMRegisters *regs);
//  int  i8k_get_bios_version(void);
  bool i8k_get_dell_sig_aux(int fn);
  bool i8k_get_dell_signature(void);
  int  i8k_get_temp(int sensor);
  int  i8k_get_temp_type(int sensor);
  int  i8k_get_power_status(void);
  int  i8k_get_fan_speed(int fan);
  int  i8k_get_fan_status(int fan);
  int  i8k_get_fan_type(int fan);
  int  i8k_get_fan_nominal_speed(int fan, int speed);
  int  i8k_set_fan(int fan, int speed);
  int  i8k_set_fan_control_manual(int fan);
  int  i8k_set_fan_control_auto(int fan);
  const char * getKeyForTemp(int sensor);

	
public:
  virtual IOService*	probe(IOService *provider, SInt32 *score);
  virtual bool		start(IOService *provider);
  virtual bool		init(OSDictionary *properties=0);
  virtual void		free(void);
  virtual void		stop(IOService *provider);
	
  virtual IOReturn	callPlatformFunction(const OSSymbol *functionName,
                                         bool waitForFunction,
                                         void *param1,
                                         void *param2,
                                         void *param3,
                                         void *param4);
};

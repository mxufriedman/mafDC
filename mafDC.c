// Operation template: mafDC /VCh=number:VCh /iCh=number:iCh /Nafx[=number:fNa] /M1fx[=number:fM1] /M2fx[=number:fM2] /TEST[=number:t] /PROC=number:p /ZERO AMPA[={number:gAMPACh[,number:VrevAMPA]}], GABA[={number:gGABACh[,number:VrevGABA]}], LEAK={number:gLeak,number:VrevLEAK}, NMDA[={number:gNMDACh[,number:rate,number:vhlf]}], NaV={number:gNaMax,number:ENa,wave:ah,wave:bh,wave:am,wave:bm}, PULS={number:pulseCh}, MRK1={number:gMrk1Max,number:Vrev1,wave:trans1,wave:Mrk1gStates}, MRK2={number:gMrk2Max,number:Vrev2,wave:trans2,wave:Mrk2gStates}

/*	mafDC.c

	A generalized routine that handles various dynamic clamp conductances.

	It reads the membrane potential and requested NMDA, AMPA, GABA, or NaV conductances,
	and calculates the total current, based on the activation
	characteristics specified by flag values passed to the XOP.
	In addition, there are two testing modes to evaluate the speed of the clamp.
	To make running of the dynamic clamp uniform, an option is allowed to direct program flow
	through a specified processor.
*/

#include "XOPStandardHeaders.h"			// Include ANSI headers, Mac headers, IgorXOP.h, XOP.h and XOPSupport.h

// Prototypes
HOST_IMPORT void main(IORecHandle ioRecHandle);

// Runtime param structure for mafNMDAslave operation.
#include "XOPStandardHeaders.h"			// Include ANSI headers, Mac headers, IgorXOP.h, XOP.h and XOPSupport.h
#include "conio.h"
#include <NIDAQmx.h>
// Custom error codes
#define REQUIRES_IGOR_500 1 + FIRST_XOP_ERR
#define BAD_RATE 2 + FIRST_XOP_ERR
#define CREATE_TASK_ERROR 3 + FIRST_XOP_ERR
#define CREATE_AI_ERROR 4 + FIRST_XOP_ERR
#define CFG_AI_ERROR 5 + FIRST_XOP_ERR
#define CREATE_AO_ERROR 6 + FIRST_XOP_ERR
#define CFG_AO_ERROR 7 + FIRST_XOP_ERR
#define START_OUT_ERROR 8 + FIRST_XOP_ERR
#define START_IN_ERROR 9 + FIRST_XOP_ERR
#define READ_ERROR 10 + FIRST_XOP_ERR
#define WRITE_ERROR 11 + FIRST_XOP_ERR
#define OVERWRITE_ERROR 12 + FIRST_XOP_ERR
#define NO_ADGAIN_ERROR 13 + FIRST_XOP_ERR
#define BAD_ADGAIN_FORM_ERROR 14 + FIRST_XOP_ERR
#define CANT_CALL_ADGAIN_ERROR 15 + FIRST_XOP_ERR
#define NO_DAGAIN_ERROR 16 + FIRST_XOP_ERR
#define BAD_DAGAIN_FORM_ERROR 17 + FIRST_XOP_ERR
#define CANT_CALL_DAGAIN_ERROR 18 + FIRST_XOP_ERR
#define NO_HADGAIN_ERROR 19 + FIRST_XOP_ERR
#define BAD_HADGAIN_FORM_ERROR 20 + FIRST_XOP_ERR
#define CANT_CALL_HADGAIN_ERROR 21 + FIRST_XOP_ERR
#define REL_ERROR 22 + FIRST_XOP_ERR
#define OFFSET_ERROR 23 + FIRST_XOP_ERR
#define ALPHAH_NOT_VALID 24 + FIRST_XOP_ERR
#define BETAH_NOT_VALID 25 + FIRST_XOP_ERR
#define ALPHAM_NOT_VALID 26 + FIRST_XOP_ERR
#define BETAM_NOT_VALID 27 + FIRST_XOP_ERR
#define CHANNEL_ALREADY_CLAIMED 28 + FIRST_XOP_ERR
#define UNIDENTIFIED_CH_TYPE 29 + FIRST_XOP_ERR
#define NO_HIRES_COUNTER_ERROR 30 + FIRST_XOP_ERR
#define MARKOV_TRANSITIONS_INVALID 31 + FIRST_XOP_ERR
#define MARKOV_CONDUCTANCES_INVALID 32 + FIRST_XOP_ERR
#define MARKOV_TRANSITIONS_NOT_2D 33 + FIRST_XOP_ERR
#define MARKOV_TRANSITIONS_NOT_SQUARE 34 + FIRST_XOP_ERR
#define MARKOV_BAD_MATCH 35 + FIRST_XOP_ERR
#define MARKOV_TRANSITION_INVALID 36 + FIRST_XOP_ERR
#define MARKOV_BAD_STATE 37 + FIRST_XOP_ERR
#define NO_ISDIFF_ERROR 38 + FIRST_XOP_ERR
#define BAD_ISDIFF_FORM_ERROR 39 + FIRST_XOP_ERR
#define CANT_CALL_ISDIFF_ERROR 40 + FIRST_XOP_ERR
#define MARKOV_TOO_MANY_STATES 41 + FIRST_XOP_ERR

#define screenDAQError(job,errnum) if (daqerr = job) {err = errnum; goto Error;}
#define screenIgorError(job,errnum) if (job) {err = errnum; goto Error;}
#define screenError(job) if (err = job) goto Error;
#define checkBounds(xval,thisScale,thisOffset,thisSize) (int)(__max (__min (floor ((xval - thisOffset) / thisScale), thisSize), 0))
#define screenDispose(testPtr) if (testPtr != NULL) DisposePtr ((Ptr)(testPtr));
#define screenMemError(job) screenIgorError (!(job), NOMEM)

#define MARKOVMAXSTATES	20

// Runtime param structure for mafDC operation.
#pragma pack(2)	// All structures passed to Igor are two-byte aligned.

struct mafDCRuntimeParams {
	// Flag parameters.

	// Parameters for /VCH flag group.
	int VCHFlagEncountered;
	double VCh;
	int VCHFlagParamsSet[1];

	// Parameters for /ICH flag group.
	int ICHFlagEncountered;
	double iCh;
	int ICHFlagParamsSet[1];

	// Parameters for /NAFX flag group.
	int NAFXFlagEncountered;
	double fNa;								// Optional parameter.
	int NAFXFlagParamsSet[1];

	// Parameters for /M1FX flag group.
	int M1FXFlagEncountered;
	double fM1;								// Optional parameter.
	int M1FXFlagParamsSet[1];

	// Parameters for /M2FX flag group.
	int M2FXFlagEncountered;
	double fM2;								// Optional parameter.
	int M2FXFlagParamsSet[1];

	// Parameters for /TEST flag group.
	int TESTFlagEncountered;
	double t;								// Optional parameter.
	int TESTFlagParamsSet[1];

	// Parameters for /PROC flag group.
	int PROCFlagEncountered;
	double p;
	int PROCFlagParamsSet[1];

	// Parameters for /ZERO flag group.
	int ZEROFlagEncountered;
	// There are no fields for this group because it has no parameters.

	// Main parameters.

	// Parameters for AMPA keyword group.
	int AMPAEncountered;
	double gAMPACh;							// Optional parameter.
	double VrevAMPA;						// Optional parameter.
	int AMPAParamsSet[2];

	// Parameters for GABA keyword group.
	int GABAEncountered;
	double gGABACh;							// Optional parameter.
	double VrevGABA;						// Optional parameter.
	int GABAParamsSet[2];

	// Parameters for LEAK keyword group.
	int LEAKEncountered;
	double gLeak;
	double VrevLEAK;
	int LEAKParamsSet[2];

	// Parameters for NMDA keyword group.
	int NMDAEncountered;
	double gNMDACh;							// Optional parameter.
	double rate;							// Optional parameter.
	double vhlf;							// Optional parameter.
	int NMDAParamsSet[3];

	// Parameters for NaV keyword group.
	int NaVEncountered;
	double gNaMax;
	double ENa;
	waveHndl ah;
	waveHndl bh;
	waveHndl am;
	waveHndl bm;
	int NaVParamsSet[6];

	// Parameters for PULS keyword group.
	int PULSEncountered;
	double pulseCh;
	int PULSParamsSet[1];

	// Parameters for MRK1 keyword group.
	int MRK1Encountered;
	double gMrk1Max;
	double Vrev1;
	waveHndl trans1;
	waveHndl Mrk1gStates;
	int MRK1ParamsSet[4];

	// Parameters for MRK2 keyword group.
	int MRK2Encountered;
	double gMrk2Max;
	double Vrev2;
	waveHndl trans2;
	waveHndl Mrk2gStates;
	int MRK2ParamsSet[4];

	// These are postamble fields that Igor sets.
	int calledFromFunction;					// 1 if called from a user function, 0 otherwise.
	int calledFromMacro;					// 1 if called from a macro, 0 otherwise.
};

typedef struct mafDCRuntimeParams mafDCRuntimeParams;
typedef struct mafDCRuntimeParams* mafDCRuntimeParamsPtr;

typedef struct GainParams
	{
	double ch;
	} GainParams;

#pragma pack()	// All structures passed to Igor are two-byte aligned.

static int
ExecutemafDC(mafDCRuntimeParamsPtr p)
	{
	int err = 0;
	int32 daqerr = 0;
	// defaults set here may be overridden by the flags
	double vgain, gAMPAgain, gNMDAgain, gGABAgain, gNaVgain, gPulsegain, igain, gM1gain, gM2gain;	// gain values for the different channels, set once we finalize the channels
	int VmIndex, AMPAIndex, NMDAIndex, GABAIndex, NaVIndex, PulseIndex, M1Index, M2Index;
	int testVal=0;
	int16 tempin[8], rawI;
	int32 sampsread[2], sampswritten[2], bytesPerSamp = 2;
	double thisVm, thisG;
	char	errBuff[2048]={'\0'};
	char	tempstr[80], inStr[80];
	int chCount=0;
	char kinetics = 0;
	LARGE_INTEGER performanceRate, lasttime, thistime;
	double NaVm = 0, NaVh = 1, dt;
	double ahOffset, ahScale, bhOffset, bhScale, amOffset, amScale, bmOffset, bmScale;
	double *amCopy=NULL, *ahCopy=NULL, *bmCopy=NULL, *bhCopy=NULL;
	int ahSize, bhSize, amSize, bmSize;
	int sortIndex, sortFlipped, i, j, k;
	float64	Bits;
	double Mrk1Now[MARKOVMAXSTATES], Mrk1Change[MARKOVMAXSTATES][MARKOVMAXSTATES+1], Mrk1gCopy[MARKOVMAXSTATES]; // to contain values of Markov states
	double Mrk1Scales[MARKOVMAXSTATES][MARKOVMAXSTATES], Mrk1Offsets[MARKOVMAXSTATES][MARKOVMAXSTATES];	// to contain wave indexing data for transition waves
	double *Mrk1TransCopies[MARKOVMAXSTATES][MARKOVMAXSTATES]={NULL};	// to contain copies of transition waves
	double Mrk2Now[MARKOVMAXSTATES], Mrk2Change[MARKOVMAXSTATES][MARKOVMAXSTATES+1], Mrk2gCopy[MARKOVMAXSTATES]; // to contain values of Markov states
	double Mrk2Scales[MARKOVMAXSTATES][MARKOVMAXSTATES], Mrk2Offsets[MARKOVMAXSTATES][MARKOVMAXSTATES];	// to contain wave indexing data for transition waves
	double *Mrk2TransCopies[MARKOVMAXSTATES][MARKOVMAXSTATES]={NULL};	// to contain copies of transition waves
	double i2jrate, GJ1, GJ2;	// for building and solving Markov transition matrix
	int Mrk1Sizes[MARKOVMAXSTATES][MARKOVMAXSTATES]={0}, Mrk1nStates=0;	// to contain sizes of transition waves, how many states
	int Mrk2Sizes[MARKOVMAXSTATES][MARKOVMAXSTATES]={0}, Mrk2nStates=0;	// to contain sizes of transition waves, how many states
	long numDimensions, dimensionSizes[MAX_DIMENSIONS+1], indices[MAX_DIMENSIONS];
	Handle transWaveName=NULL;
	waveHndl tempWave=NULL;
	long thistrans, debugAfter=0;

	FunctionInfo fi;	// used to check up on mafITC
	int badParameterNumber;	// used to check up on mafITC
	int requiredParameterTypes[1]; // used to check up on mafITC
	double result;	// to hold result from mafITC call
	char isDiff;
	GainParams thisGainParams;
	TaskHandle	intask=0, outtask=0;

	lasttime.QuadPart = 0;
	// Flag parameters.

	if (p->TESTFlagEncountered)
		{
		if (p->TESTFlagParamsSet[0])
			testVal = p->t;
		}

	if (p->NaVEncountered)
		{
		// Do a quick parsing of the waves to set up rapid look-up later
		// Parameter: p->ah
		screenIgorError (p->ah == NULL, ALPHAH_NOT_VALID);
		MDGetWaveScaling (p->ah, ROWS, &ahScale, &ahOffset);
		ahSize = WavePoints (p->ah);
		screenMemError (ahCopy = (double *)NewPtr (ahSize * sizeof (double)));
		screenError (MDGetDPDataFromNumericWave (p->ah, ahCopy));

		// Parameter: p->bh
		screenIgorError (p->bh == NULL, BETAH_NOT_VALID);
		MDGetWaveScaling (p->bh, ROWS, &bhScale, &bhOffset);
		bhSize = WavePoints (p->bh);
		screenMemError (bhCopy = (double *)NewPtr (bhSize * sizeof (double)));
		screenError (MDGetDPDataFromNumericWave (p->bh, bhCopy));

		// Parameter: p->am
		screenIgorError (p->am == NULL, ALPHAM_NOT_VALID);
		MDGetWaveScaling (p->am, ROWS, &amScale, &amOffset);
		amSize = WavePoints (p->am);
		screenMemError (amCopy = (double *)NewPtr (amSize * sizeof (double)));
		screenError (MDGetDPDataFromNumericWave (p->am, amCopy));

		// Parameter: p->bm
		screenIgorError (p->bm == NULL, BETAM_NOT_VALID);
		MDGetWaveScaling (p->bm, ROWS, &bmScale, &bmOffset);
		bmSize = WavePoints (p->bm);
		screenMemError (bmCopy = (double *)NewPtr (bmSize * sizeof (double)));
		screenError (MDGetDPDataFromNumericWave (p->bm, bmCopy));
		
		kinetics = -1;
		if (!p->NAFXFlagEncountered || !p->NAFXFlagParamsSet[0])
			p->fNa = 1;			
		}

	if (p->MRK1Encountered)
		{
		kinetics = -1;
		if (!p->M1FXFlagEncountered || !p->M1FXFlagParamsSet[0])
			p->fM1 = 1;

		// error checking - are all waves declared, correct sizes, etc.
		screenIgorError (p->trans1 == NULL, MARKOV_TRANSITIONS_INVALID);
		screenIgorError (p->Mrk1gStates == NULL, MARKOV_CONDUCTANCES_INVALID);
		Mrk1nStates = WavePoints (p->Mrk1gStates);
		screenIgorError (Mrk1nStates > MARKOVMAXSTATES, MARKOV_TOO_MANY_STATES);
		MDGetWaveDimensions (p->trans1, &numDimensions, dimensionSizes);
		screenIgorError (numDimensions != 2, MARKOV_TRANSITIONS_NOT_2D);
		screenIgorError (dimensionSizes[0] != dimensionSizes[1], MARKOV_TRANSITIONS_NOT_SQUARE);
		screenIgorError (Mrk1nStates != dimensionSizes[0], MARKOV_BAD_MATCH);
		screenIgorError (WaveType (p->trans1) != TEXT_WAVE_TYPE, MARKOV_TRANSITIONS_INVALID);
		
		// setup scratch waves
		MemClear (Mrk1Now, Mrk1nStates * sizeof (*Mrk1Now));// poor man's state initialization - just put all channels in first state
		Mrk1Now[0] = 1;
		screenError (MDGetDPDataFromNumericWave (p->Mrk1gStates, Mrk1gCopy));

		// check out transition waves and copy into local scratch
		screenMemError (transWaveName = NewHandle (0L));
		for (i = 0; i < Mrk1nStates; i++)
			{
			indices[0] = i;
			for (j = 0; j < Mrk1nStates; j++)
				{
				indices[1] = j;
				screenError (MDGetTextWavePointValue (p->trans1, indices, transWaveName));	// get name of wave in that position
				if (GetHandleSize (transWaveName) == 0)	// nothing specified, so no connection, so skip
					continue;
				screenError (GetCStringFromHandle (transWaveName, tempstr, 80)); // convert string handle to C string
				tempWave = FetchWaveFromDataFolder (NULL, tempstr); // convert string name to wave handle
				MDGetWaveScaling (tempWave, ROWS, &(Mrk1Scales[i][j]), &(Mrk1Offsets[i][j]));	// get details of wave
				Mrk1Sizes[i][j] = WavePoints (tempWave);
				screenMemError (Mrk1TransCopies[i][j] = (double *)NewPtr (Mrk1Sizes[i][j] * sizeof (double)));	// allocate space for wave
				screenError (MDGetDPDataFromNumericWave (tempWave, Mrk1TransCopies[i][j]));	// copy transition wave in
				}
			}
		DisposeHandle (transWaveName);
		}

	if (p->MRK2Encountered)
		{
		kinetics = -1;
		if (!p->M2FXFlagEncountered || !p->M2FXFlagParamsSet[0])
			p->fM2 = 1;

		// error checking - are all waves declared, correct sizes, etc.
		screenIgorError (p->trans2 == NULL, MARKOV_TRANSITIONS_INVALID);
		screenIgorError (p->Mrk2gStates == NULL, MARKOV_CONDUCTANCES_INVALID);
		Mrk2nStates = WavePoints (p->Mrk2gStates);
		screenIgorError (Mrk2nStates > MARKOVMAXSTATES, MARKOV_TOO_MANY_STATES);
		MDGetWaveDimensions (p->trans2, &numDimensions, dimensionSizes);
		screenIgorError (numDimensions != 2, MARKOV_TRANSITIONS_NOT_2D);
		screenIgorError (dimensionSizes[0] != dimensionSizes[1], MARKOV_TRANSITIONS_NOT_SQUARE);
		screenIgorError (Mrk2nStates != dimensionSizes[0], MARKOV_BAD_MATCH);
		screenIgorError (WaveType (p->trans2) != TEXT_WAVE_TYPE, MARKOV_TRANSITIONS_INVALID);
		
		// setup scratch waves
		MemClear (Mrk2Now, Mrk2nStates * sizeof (*Mrk2Now));// poor man's state initialization - just put all channels in first state
		Mrk2Now[0] = 1;
		screenError (MDGetDPDataFromNumericWave (p->Mrk2gStates, Mrk2gCopy));

		// check out transition waves and copy into local scratch
		screenMemError (transWaveName = NewHandle (0L));
		for (i = 0; i < Mrk2nStates; i++)
			{
			indices[0] = i;
			for (j = 0; j < Mrk2nStates; j++)
				{
				indices[1] = j;
				screenError (MDGetTextWavePointValue (p->trans2, indices, transWaveName));	// get name of wave in that position
				if (GetHandleSize (transWaveName) == 0)	// nothing specified, so no connection, so skip
					continue;
				screenError (GetCStringFromHandle (transWaveName, tempstr, 80)); // convert string handle to C string
				tempWave = FetchWaveFromDataFolder (NULL, tempstr); // convert string name to wave handle
				MDGetWaveScaling (tempWave, ROWS, &(Mrk2Scales[i][j]), &(Mrk2Offsets[i][j]));	// get details of wave
				Mrk2Sizes[i][j] = WavePoints (tempWave);
				screenMemError (Mrk2TransCopies[i][j] = (double *)NewPtr (Mrk2Sizes[i][j] * sizeof (double)));	// allocate space for wave
				screenError (MDGetDPDataFromNumericWave (tempWave, Mrk2TransCopies[i][j]));	// copy transition wave in
				}
			}
		DisposeHandle (transWaveName);
		}


	// call mafITC routines to get AD gains for the different channels
	screenIgorError (GetFunctionInfo ("mafITC_ADGain", &fi), NO_ADGAIN_ERROR);
	requiredParameterTypes[0] = NT_FP64;
	screenIgorError (CheckFunctionForm (&fi, 1, requiredParameterTypes, &badParameterNumber, NT_FP64), BAD_ADGAIN_FORM_ERROR);
	
	// set up channels that need to be read - configure reading order and gains
	// get gain on Vm channel
	if (!p->VCHFlagEncountered)
		p->VCh = 0;
	thisGainParams.ch = p->VCh;
	screenIgorError (CallFunction (&fi, (void *)&thisGainParams, &result), CANT_CALL_ADGAIN_ERROR);
	vgain = result;
	VmIndex = chCount;
	sprintf_s (inStr, 80, "Dev1/ai%g", p->VCh);
	chCount += 1;
	
	// setup conductance channels for reading from A/D
	if (p->NMDAEncountered)
		{
		if (!p->NMDAParamsSet[0])
			p->gNMDACh = 1;
		thisGainParams.ch = p->gNMDACh;
		screenIgorError (p->gNMDACh == p->VCh, CHANNEL_ALREADY_CLAIMED);
		NMDAIndex = chCount;
		sprintf_s (inStr, 80, "%s,Dev1/ai%g", inStr, p->gNMDACh);
		chCount += 1;
		screenIgorError (CallFunction (&fi, (void *)&thisGainParams, &result), CANT_CALL_ADGAIN_ERROR);
		gNMDAgain = result;
		if (!p->NMDAParamsSet[1])
			p->rate = 18.089;
		screenIgorError (p->rate < 0, BAD_RATE);
		if (!p->NMDAParamsSet[2])
			p->vhlf = -11.178;
		}

	if (p->AMPAEncountered)
		{
		if (!p->AMPAParamsSet[0])
			p->gAMPACh = 1;
		screenIgorError (p->gAMPACh == p->VCh, CHANNEL_ALREADY_CLAIMED);
		AMPAIndex = chCount;
		sprintf_s (inStr, 80, "%s,Dev1/ai%g", inStr, p->gAMPACh);
		chCount += 1;
		thisGainParams.ch = p->gAMPACh;
		screenIgorError (CallFunction (&fi, (void *)&thisGainParams, &result), CANT_CALL_ADGAIN_ERROR);
		gAMPAgain = result;
		if (!p->AMPAParamsSet[1])
			p->VrevAMPA = 0;
		}

	if (p->GABAEncountered)
		{
		if (!p->GABAParamsSet[0])
			p->gGABACh = 1;
		screenIgorError (p->gGABACh == p->VCh, CHANNEL_ALREADY_CLAIMED);
		GABAIndex = chCount;
		sprintf_s (inStr, 80, "%s,Dev1/ai%g", inStr, p->gGABACh);
		chCount += 1;
		thisGainParams.ch = p->gGABACh;
		screenIgorError (CallFunction (&fi, (void *)&thisGainParams, &result), CANT_CALL_ADGAIN_ERROR);
		gGABAgain = result;
		if (!p->GABAParamsSet[1])
			p->VrevGABA = -80;
		}

	if (p->PULSEncountered)
		{
		if (!p->PULSParamsSet[0])
			p->pulseCh = 1;
		screenIgorError (p->pulseCh == p->VCh, CHANNEL_ALREADY_CLAIMED);
		PulseIndex = chCount;
		sprintf_s (inStr, 80, "%s,Dev1/ai%g", inStr, p->pulseCh);
		chCount += 1;
		thisGainParams.ch = p->pulseCh;
		screenIgorError (CallFunction (&fi, (void *)&thisGainParams, &result), CANT_CALL_ADGAIN_ERROR);
		gPulsegain = result;
		}

	if (p->NaVEncountered && !p->fNa)
		{
		// NaV is not fixed, so need to configure its A/D channel
		screenIgorError (p->gNaMax == p->VCh, CHANNEL_ALREADY_CLAIMED);
		NaVIndex = chCount;
		sprintf_s (inStr, 80, "%s,Dev1/ai%g", inStr, p->gNaMax);
		chCount += 1;
		thisGainParams.ch = p->gNaMax;
		screenIgorError (CallFunction (&fi, (void *)&thisGainParams, &result), CANT_CALL_ADGAIN_ERROR);
		gNaVgain = result;
		}
		
	if (p->MRK1Encountered && !p->fM1)
		{
		// Markov1 is not fixed, so need to configure its A/D channel
		screenIgorError (p->gMrk1Max == p->VCh, CHANNEL_ALREADY_CLAIMED);
		M1Index = chCount;
		sprintf_s (inStr, 80, "%s,Dev1/ai%g", inStr, p->gMrk1Max);
		chCount += 1;
		thisGainParams.ch = p->gMrk1Max;
		screenIgorError (CallFunction (&fi, (void *)&thisGainParams, &result), CANT_CALL_ADGAIN_ERROR);
		gM1gain = result;
		}
	if (p->MRK2Encountered && !p->fM2)
		{
		// Markov2 is not fixed, so need to configure its A/D channel
		screenIgorError (p->gMrk2Max == p->VCh, CHANNEL_ALREADY_CLAIMED);
		M2Index = chCount;
		sprintf_s (inStr, 80, "%s,Dev1/ai%g", inStr, p->gMrk2Max);
		chCount += 1;
		thisGainParams.ch = p->gMrk2Max;
		screenIgorError (CallFunction (&fi, (void *)&thisGainParams, &result), CANT_CALL_ADGAIN_ERROR);
		gM2gain = result;
		}
	
	// call mafITC routines to get DA gains for the output
	screenIgorError (GetFunctionInfo ("mafITC_DAGain", &fi), NO_DAGAIN_ERROR);
	screenIgorError (CheckFunctionForm (&fi, 1, requiredParameterTypes, &badParameterNumber, NT_FP64), BAD_DAGAIN_FORM_ERROR)
	// get gain on i channel
	if (!p->ICHFlagEncountered)
		p->iCh = 0;
	thisGainParams.ch = p->iCh;
	screenIgorError (CallFunction (&fi, (void *)&thisGainParams, &result), CANT_CALL_DAGAIN_ERROR);
	igain = result / 1000;

	// get the hardware AD gain from mafITC
	screenIgorError (GetFunctionInfo ("mafITC_HardwareGainAD", &fi), NO_HADGAIN_ERROR);
	screenIgorError (CheckFunctionForm (&fi, 0, NULL, &badParameterNumber, NT_FP64), BAD_HADGAIN_FORM_ERROR);
	// get the gain
	screenIgorError (CallFunction (&fi, (void *)NULL, &result), CANT_CALL_HADGAIN_ERROR);
	vgain *= result;
	gAMPAgain *= result;
	gNMDAgain *= result;
	gNaVgain *= result;
	gGABAgain *= result;
	gPulsegain *= result;
	gM1gain *= result;
	gM2gain *= result;

	// find out if differential or single-ended inputs
	screenIgorError (GetFunctionInfo ("mafITC_isDIFF", &fi), NO_ISDIFF_ERROR);
	screenIgorError (CheckFunctionForm (&fi, 0, NULL, &badParameterNumber, NT_FP64), BAD_ISDIFF_FORM_ERROR);
	// get the value
	screenIgorError (CallFunction (&fi, (void *)NULL, &result), CANT_CALL_ISDIFF_ERROR);
	isDiff = (result != 0);

	if (kinetics)
		screenIgorError (!QueryPerformanceFrequency (&performanceRate), NO_HIRES_COUNTER_ERROR);

	// Initialize scan task - channels are read in the order requested (not sorted by channel number)
	screenDAQError (DAQmxCreateTask ("", &intask), CREATE_TASK_ERROR);
	screenDAQError (DAQmxCreateAIVoltageChan (intask, inStr, "", isDiff ? DAQmx_Val_Diff : DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL), CREATE_AI_ERROR);
//	screenDAQError (DAQmxGetAIResolution(intask, PChar(n), &Bits), GET_AI_RES_ERROR);
	vgain *= 65536 / 20;	//pow(2, round (Bits)) / 20
	gAMPAgain *= 65536 / 20;
	gNMDAgain *= 65536 / 20;
	gNaVgain *= 65536 / 20;
	gGABAgain *= 65536 / 20;
	gPulsegain *= 65536 / 20;
	gM1gain *= 65536/20;
	gM2gain *= 65536/20;

	// Initialize output task
	screenDAQError (DAQmxCreateTask ("", &outtask), CREATE_TASK_ERROR);
	sprintf_s (tempstr, 80, "Dev1/ao%g", p->iCh);	//***check that these don't have decimals
	screenDAQError (DAQmxCreateAOVoltageChan (outtask, tempstr, "",-10,10,DAQmx_Val_Volts,NULL), CREATE_AO_ERROR);
//	screenDAQError (DAQmxGetAOResolution(outtask, PChar(n), &Bits), GET_AO_RES_ERROR);
	igain *= 65536 / 20; 	//pow(2, round (Bits)) / 20

	//start tasks going
	screenDAQError (DAQmxStartTask (intask), START_IN_ERROR);
	screenDAQError (DAQmxStartTask (outtask), START_OUT_ERROR);
	
	if (p->PROCFlagEncountered)
		{
		// Parameter: p->p
		// Direct the main loop to a specific processor
		}

	while (!SpinProcess ())
		{
		if (kinetics)
			screenIgorError (!QueryPerformanceCounter (&thistime), NO_HIRES_COUNTER_ERROR);
		screenDAQError (DAQmxReadRaw (intask, 1, 1, tempin, sizeof (int16) * chCount, sampsread, &bytesPerSamp, NULL), READ_ERROR);
		// now unravel inputs and fix gains
		thisVm = tempin[VmIndex] / vgain;
		rawI = 0;
		if (p->AMPAEncountered)
			rawI += (p->VrevAMPA - thisVm) * tempin[AMPAIndex] / gAMPAgain * igain;
		if (p->GABAEncountered)
			rawI += (p->VrevGABA - thisVm) * tempin[GABAIndex] / gGABAgain * igain;
		if (p->NMDAEncountered)
			rawI += -thisVm * tempin[NMDAIndex] / gNMDAgain / (1 + exp ((p->vhlf - thisVm) / p->rate)) * igain;
		if (p->LEAKEncountered)
			rawI += (p->VrevLEAK - thisVm) * p->gLeak * igain;
		if (p->PULSEncountered)
			rawI += tempin[PulseIndex] / gPulsegain * igain;
		if (kinetics)	// i.e. time-dependent conductances
			{
			if (lasttime.QuadPart)	// wait until 2nd round, to make sure dt is valid
				{
				dt = (double)(thistime.QuadPart - lasttime.QuadPart) / performanceRate.QuadPart ;	// returns timing in s
				if (p->NaVEncountered)
					{
					NaVm += (amCopy[checkBounds (thisVm, amScale, amOffset, amSize)] * (1 - NaVm) - bmCopy[checkBounds (thisVm, bmScale, bmOffset, bmSize)] * NaVm) * dt;
					NaVh += (ahCopy[checkBounds (thisVm, ahScale, ahOffset, ahSize)] * (1 - NaVh) - bhCopy[checkBounds (thisVm, bhScale, bhOffset, bhSize)] * NaVh) * dt;
					rawI += (p->fNa ?  p->gNaMax : tempin[NaVIndex] / gNaVgain) * NaVm * NaVm * NaVm * NaVh * (p->ENa - thisVm) * igain;
					}
				if (p->MRK1Encountered)
					{
					// first build matrix: if M is the matrix of transition rates between states,
					// then we need (I - dt M) for our linear solve
					//MemClear (Mrk1Change, Mrk1nStates * (Mrk1nStates + 1) * sizeof (*Mrk1Change));
					for (i = 0; i < Mrk1nStates; ++i)// clear transition matrix
						for (j = 0; j < Mrk1nStates; ++j)
							Mrk1Change[i][j] = 0;	// ***can this be faster??? MemClear???
					for (i = 0; i < Mrk1nStates; ++i)
						for (j = 0; j < Mrk1nStates; ++j)
							{
							if (i == j)
								{
								Mrk1Change[i][j] += 1;	// put identity in diagonal
								continue;
								}
							if (Mrk1TransCopies[i][j] == NULL || Mrk1Sizes[i][j] == 0)	// states i and j don't connect
								continue;
							if (Mrk1Sizes[i][j] == 1)	// i.e. not voltage-dependent
								i2jrate = *(Mrk1TransCopies[i][j]) * dt;
							else	// i.e. voltage-dependent
								i2jrate = (Mrk1TransCopies[i][j])[checkBounds (thisVm, Mrk1Scales[i][j], Mrk1Offsets[i][j], Mrk1Sizes[i][j])] * dt;
							//dependencies have "from" in rows and "to" in columns ("row major")
							//but actually the other way around is more natural to matrix manipulation ("column major")
							//so, transpose the values here to save time for matrix operations
							Mrk1Change[j][i] -= i2jrate;	// j state gains these from i
							Mrk1Change[i][i] += i2jrate;	// i state loses these
							}
					// put current state in last column
					for (i = 0; i < Mrk1nStates; ++i)
						Mrk1Change[i][Mrk1nStates] = Mrk1Now[i];
					// now solve using Gauss-Jordan method
					for (k = 0; k < Mrk1nStates; k++)
						for (i = 0; i < Mrk1nStates; i++)
							{
							GJ2 = Mrk1Change[k][k];
							GJ1 = Mrk1Change[i][k] / GJ2;	// are these indices all backwards???
							for (j = k; j <= Mrk1nStates; j++)
								if (i == k)
									Mrk1Change[i][j] /= GJ2;
								else
									Mrk1Change[i][j] -= GJ1 * Mrk1Change[k][j];
							}
					for (i = 0; i < Mrk1nStates; ++i)	// now add up contribution of all states with non-zero conductance
						{
						Mrk1Now[i] = Mrk1Change[i][Mrk1nStates];	// copy out of solution matrix
						if (Mrk1gCopy[i] > 0)
							rawI += Mrk1Now[i] * Mrk1gCopy[i] * (p->fM1 ? p->gMrk1Max : tempin[M1Index] / gM1gain) * (p->Vrev1 - thisVm) * igain;
						}
					}	// end if MRK1Encounteres
				// I know, I know, these Markov channels should be one subroutine, but putting all here (probably) improves speed
				if (p->MRK2Encountered)
					{
					// first build matrix: if M is the matrix of transition rates between states,
					// then we need (I - dt M) for our linear solve
					for (i = 0; i < Mrk2nStates; ++i)// clear transition matrix
						for (j = 0; j < Mrk2nStates; ++j)
							Mrk2Change[i][j] = 0;
					for (i = 0; i < Mrk2nStates; ++i)
						for (j = 0; j < Mrk2nStates; ++j)
							{
							if (i == j)
								{
								Mrk2Change[i][j] += 1;	// put identity in diagonal
								continue;
								}
							if (Mrk2TransCopies[i][j] == NULL || Mrk2Sizes[i][j] == 0)	// states i and j don't connect
								continue;
							if (Mrk2Sizes[i][j] == 1)	// i.e. not voltage-dependent
								i2jrate = *(Mrk2TransCopies[i][j]) * dt;
							else	// i.e. voltage-dependent
								i2jrate = (Mrk2TransCopies[i][j])[checkBounds (thisVm, Mrk2Scales[i][j], Mrk2Offsets[i][j], Mrk2Sizes[i][j])] * dt;
							//dependencies have "from" in rows and "to" in columns ("row major")
							//but actually the other way around is more natural to matrix manipulation ("column major")
							//so, transpose the values here to save time for matrix operations
							Mrk2Change[j][i] -= i2jrate;	// j state gains these from i
							Mrk2Change[i][i] += i2jrate;	// i state loses these
							}
					// put current state in last column
					for (i = 0; i < Mrk2nStates; ++i)
						Mrk2Change[i][Mrk2nStates] = Mrk2Now[i];
					// now solve using Gauss-Jordan method
					for (k = 0; k < Mrk2nStates; k++)
						for (i = 0; i < Mrk2nStates; i++)
							{
							GJ2 = Mrk2Change[k][k];
							GJ1 = Mrk2Change[i][k] / GJ2;	// are these indices all backwards???
							for (j = k; j <= Mrk2nStates; j++)
								if (i == k)
									Mrk2Change[i][j] /= GJ2;
								else
									Mrk2Change[i][j] -= GJ1 * Mrk2Change[k][j];
							}
					for (i = 0; i < Mrk2nStates; ++i)	// now add up contribution of all states with non-zero conductance
						{
						Mrk2Now[i] = Mrk2Change[i][Mrk2nStates];	// copy out of solution matrix
						if (Mrk2gCopy[i] > 0)
							rawI += Mrk2Now[i] * Mrk2gCopy[i] * (p->fM2 ? p->gMrk2Max : tempin[M2Index] / gM2gain) * (p->Vrev2 - thisVm) * igain;
						}
					}
				}
			lasttime = thistime;
			if (debugAfter++ > 10000)
				debugAfter = 0;
			}	// end take care of time-dependent conductances

		// now check if we are actually just running a test
		if (p->TESTFlagEncountered && testVal)
			{
			rawI = testVal;
			testVal *= -1;
			}
		else if (p->TESTFlagEncountered)
			rawI = tempin[VmIndex];
		screenDAQError (DAQmxWriteRaw (outtask, 1, 10, 1, &rawI, sampswritten, NULL), WRITE_ERROR);
	}
	_getch();

	if (p->ZEROFlagEncountered) {
		rawI = 0;
		screenDAQError (DAQmxWriteRaw (outtask, 1, 10, 1, &rawI, sampswritten, NULL), WRITE_ERROR);
	}


Error:
	if (daqerr)
	{
		DAQmxGetExtendedErrorInfo (errBuff, 2048);
		XOPOKAlert ("NIDAQ Error", errBuff);
	}
	if (intask != 0)
	{
		DAQmxStopTask (intask);
		DAQmxClearTask (intask);
	}
	if (outtask != 0)
	{
		DAQmxStopTask (outtask);
		DAQmxClearTask (outtask);
	}
	screenDispose (ahCopy);
	screenDispose (amCopy);
	screenDispose (bhCopy);
	screenDispose (bmCopy);
	for (i = 0; i < Mrk1nStates; i++)
		for (j = 0; j < Mrk1nStates; j++)
			screenDispose (Mrk1TransCopies[i][j]);
	for (i = 0; i < Mrk2nStates; i++)
		for (j = 0; j < Mrk2nStates; j++)
			screenDispose (Mrk2TransCopies[i][j]);
	return err;
}

static int
RegistermafDC(void)
{
	const char* cmdTemplate;
	const char* runtimeNumVarList;
	const char* runtimeStrVarList;

	// NOTE: If you change this template, you must change the mafDCRuntimeParams structure as well.
	cmdTemplate = "mafDC /VCh=number:VCh /iCh=number:iCh /Nafx[=number:fNa] /M1fx[=number:fM1] /M2fx[=number:fM2] /TEST[=number:t] /PROC=number:p /ZERO AMPA[={number:gAMPACh[,number:VrevAMPA]}], GABA[={number:gGABACh[,number:VrevGABA]}], LEAK={number:gLeak,number:VrevLEAK}, NMDA[={number:gNMDACh[,number:rate,number:vhlf]}], NaV={number:gNaMax,number:ENa,wave:ah,wave:bh,wave:am,wave:bm}, PULS={number:pulseCh}, MRK1={number:gMrk1Max,number:Vrev1,wave:trans1,wave:Mrk1gStates}, MRK2={number:gMrk2Max,number:Vrev2,wave:trans2,wave:Mrk2gStates}";
	runtimeNumVarList = "";
	runtimeStrVarList = "";
	return RegisterOperation(cmdTemplate, runtimeNumVarList, runtimeStrVarList, sizeof(mafDCRuntimeParams), (void*)ExecutemafDC, 0);
}


//	XOPEntry() : Entry point from the host application to the XOP for all
static void
XOPEntry(void)
{	
	long result = 0;
	
	switch (GetXOPMessage()) {
		// We don't need to handle any messages for this XOP.
	}
	SetXOPResult(result);
}

//	main(ioRecHandle): Initial entry point at which the host application calls XOP.
//	Sets the XOPEntry field of ioRecHandle to the address to be called for future messages.
HOST_IMPORT void
main(IORecHandle ioRecHandle)
{
	int result;
	
	XOPInit(ioRecHandle);							// Do standard XOP initialization.
	SetXOPEntry(XOPEntry);							// Set entry point for future calls.
	if (result = RegistermafDC()) {
		SetXOPResult(result);
		return;
	}
	SetXOPResult(0);
}

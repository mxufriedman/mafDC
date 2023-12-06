#pragma rtGlobals=1		// Use modern global access method.
#include "mafutils"

menu "Macros"
	"mafDC Helper panel", mafdcpanel()
endmacro

Window mafdcpanel() : Panel
	PauseUpdate; Silent 1		// building window...
	NewPanel /W=(46,61,250,577)
	CheckBox AMPACheck,pos={5,67},size={48,14},proc=DCHelperRadioProc,title="AMPA"
	CheckBox AMPACheck,help={"Are you using an AMPA conductance?"},value= 0
	CheckBox ZeroCheck,pos={5,48},size={40,14},proc=DCHelperRadioProc,title="Zero"
	CheckBox ZeroCheck,help={"Zero current output after dynamic clamp terminated?"}
	CheckBox ZeroCheck,value= 0
	SetVariable VChsetvar,pos={5,3},size={70,20},proc=DCHelperBuildProc,title="V\\Bm\\M ADC"
	SetVariable VChsetvar,help={"What A/D channel carries the membrane potential?"}
	SetVariable VChsetvar,limits={0,inf,1},value= _NUM:0
	SetVariable IChsetvar,pos={17,27},size={58,16},proc=DCHelperBuildProc,title="I DAC"
	SetVariable IChsetvar,help={"What D/A channel will be used to drive current output?"}
	SetVariable IChsetvar,limits={0,inf,1},value= _NUM:0
	SetVariable AMPAChsetvar,pos={23,82},size={53,16},proc=DCHelperBuildProc,title="ADC"
	SetVariable AMPAChsetvar,help={"What A/D channel carries the AMPA conductance?"}
	SetVariable AMPAChsetvar,limits={0,inf,1},value= _NUM:1
	SetVariable AMPArevsetvar,pos={79,82},size={71,20},proc=DCHelperBuildProc,title="V\\Brev"
	SetVariable AMPArevsetvar,help={"What is the AMPA reversal potential?"}
	SetVariable AMPArevsetvar,format="%G mV",limits={-inf,inf,10},value= _NUM:0
	CheckBox GABACheck,pos={5,101},size={47,14},proc=DCHelperRadioProc,title="GABA"
	CheckBox GABACheck,help={"Are you using a GABA conductance?"},value= 0
	SetVariable GABAChsetvar,pos={23,115},size={53,16},proc=DCHelperBuildProc,title="ADC"
	SetVariable GABAChsetvar,help={"What A/D channel carries the GABA conductance?"}
	SetVariable GABAChsetvar,limits={0,inf,1},value= _NUM:3
	SetVariable GABArevsetvar,pos={79,115},size={71,20},proc=DCHelperBuildProc,title="V\\Brev"
	SetVariable GABArevsetvar,help={"What is the GABA reversal potential?"}
	SetVariable GABArevsetvar,format="%G mV",limits={-inf,inf,10},value= _NUM:-60
	CheckBox NMDACheck,pos={5,133},size={50,14},proc=DCHelperRadioProc,title="NMDA"
	CheckBox NMDACheck,help={"Are you using an NMDA conductance?"},value= 0
	SetVariable NMDAChsetvar,pos={23,147},size={53,16},proc=DCHelperBuildProc,title="ADC"
	SetVariable NMDAChsetvar,help={"What A/D channel carries the NMDA conductance?"}
	SetVariable NMDAChsetvar,limits={0,inf,1},value= _NUM:2
	SetVariable NMDAratesetvar,pos={79,147},size={71,16},proc=DCHelperBuildProc,title="rate"
	SetVariable NMDAratesetvar,help={"What is the NMDA reversal potential?"}
	SetVariable NMDAratesetvar,format="%G mV"
	SetVariable NMDAratesetvar,limits={-inf,inf,10},value= _NUM:18.089
	SetVariable NMDAVhalfsetvar,pos={73,164},size={77,20},proc=DCHelperBuildProc,title="V\\Bhalf"
	SetVariable NMDAVhalfsetvar,help={"What is the NMDA reversal potential?"}
	SetVariable NMDAVhalfsetvar,format="%G mV"
	SetVariable NMDAVhalfsetvar,limits={-inf,inf,10},value= _NUM:-11.178
	SetVariable Testsetvar,pos={23,229},size={57,16},proc=DCHelperBuildProc,title="val"
	SetVariable Testsetvar,help={"What output value will you use for the test function?"}
	SetVariable Testsetvar,limits={-inf,inf,100},value= _NUM:700
	CheckBox TestCheck,pos={5,215},size={39,14},proc=DCHelperRadioProc,title="Test"
	CheckBox TestCheck,help={"Are you using the DC test function?"},value= 0
	SetVariable PulseChsetvar,pos={23,198},size={53,16},proc=DCHelperBuildProc,title="ADC"
	SetVariable PulseChsetvar,help={"What A/D channel carries the current pulse?"}
	SetVariable PulseChsetvar,limits={0,inf,1},value= _NUM:2
	CheckBox PulseCheck,pos={5,183},size={81,14},proc=DCHelperRadioProc,title="Current Pulse"
	CheckBox PulseCheck,help={"Are you using a current pulse?"},value= 0
	CheckBox LeakCheck,pos={5,246},size={42,14},proc=DCHelperRadioProc,title="Leak"
	CheckBox LeakCheck,help={"Are you using a leak conductance?"},value= 0
	SetVariable GLeaksetvar,pos={21,260},size={84,20},proc=DCHelperBuildProc,title="g\\Bleak"
	SetVariable GLeaksetvar,help={"How big is the leak conductance?"},format="%G nS"
	SetVariable GLeaksetvar,limits={0,inf,1},value= _NUM:8
	SetVariable Leakrevsetvar,pos={109,260},size={80,20},proc=DCHelperBuildProc,title="V\\Brev"
	SetVariable Leakrevsetvar,help={"What is the leak reversal potential?"}
	SetVariable Leakrevsetvar,format="%G mV",limits={-inf,inf,10},value= _NUM:-71
	CheckBox NaCheck,pos={5,280},size={32,14},proc=DCHelperRadioProc,title="Na"
	CheckBox NaCheck,help={"Are you using a HH-Na conductance?"},value= 0
	SetVariable gNasetvar,pos={45,315},size={68,16},proc=DCHelperBuildProc,title=" "
	SetVariable gNasetvar,help={"What is the peak HH-Na conductance?"}
	SetVariable gNasetvar,format="%G nS",limits={0,inf,100},value= _NUM:400
	SetVariable Narevsetvar,pos={109,297},size={84,20},proc=DCHelperBuildProc,title="V\\Brev"
	SetVariable Narevsetvar,help={"What is the HH-Na reversal potential?"}
	SetVariable Narevsetvar,format="%G mV",limits={-inf,inf,10},value= _NUM:55
	CheckBox NaFixRadio,pos={26,316},size={16,14},proc=DCHelperRadioProc,title=""
	CheckBox NaFixRadio,help={"Is the peak HH-Na conductance fixed?"}
	CheckBox NaFixRadio,value= 1,mode=1
	CheckBox NaChRadio,pos={26,299},size={16,14},proc=DCHelperRadioProc,title=""
	CheckBox NaChRadio,help={"Is the peak HH-Na conductance set by a channel?"}
	CheckBox NaChRadio,value= 0,mode=1
	SetVariable NaChsetvar,pos={46,297},size={53,16},proc=DCHelperBuildProc,title="ADC"
	SetVariable NaChsetvar,help={"What A/D channel carries the peak HH-Na conductance?"}
	SetVariable NaChsetvar,limits={0,inf,1},value= _NUM:1
	SetVariable ahsetvar,pos={40,333},size={75,22},proc=DCHelperBuildProc,title="\\F'Symbol'a\\F'MS sans serif'\\Bh"
	SetVariable ahsetvar,help={"What Igor wave holds the activation for the h-gate for the HH-Na conductance?"}
	SetVariable ahsetvar,limits={0,inf,1},value= _STR:"ah"
	SetVariable bmsetvar,pos={119,357},size={74,22},proc=DCHelperBuildProc,title="\\F'Symbol'b\\F'MS sans serif'\\Bm"
	SetVariable bmsetvar,help={"What Igor wave holds the deactivation for the m-gate for the HH-Na conductance?"}
	SetVariable bmsetvar,limits={0,inf,1},value= _STR:"bm"
	SetVariable amsetvar,pos={118,333},size={75,22},proc=DCHelperBuildProc,title="\\F'Symbol'a\\F'MS sans serif'\\Bm"
	SetVariable amsetvar,help={"What Igor wave holds the activation for the m-gate for the HH-Na conductance?"}
	SetVariable amsetvar,limits={0,inf,1},value= _STR:"am"
	SetVariable bhsetvar,pos={41,357},size={74,22},proc=DCHelperBuildProc,title="\\F'Symbol'b\\F'MS sans serif'\\Bh"
	SetVariable bhsetvar,help={"What Igor wave holds the deactivation for the h-gate for the HH-Na conductance?"}
	SetVariable bhsetvar,limits={0,inf,1},value= _STR:"bh"
	CheckBox Mrk1Check,pos={5,382},size={70,14},proc=DCHelperRadioProc,title="Markov #1"
	CheckBox Mrk1Check,help={"Are you using a HH-Na conductance?"},value= 0
	SetVariable Mrk1Transitions,pos={29,458},size={55,16},proc=DCHelperBuildProc
	SetVariable Mrk1Transitions,help={"What Igor text wave holds the names of the microscopic transition waves?"}
	SetVariable Mrk1Transitions,limits={0,inf,1},value= _STR:"HHTransitions"
	SetVariable Mrk1revsetvar,pos={22,435},size={80,20},proc=DCHelperBuildProc,title="V\\Brev"
	SetVariable Mrk1revsetvar,help={"What is the Markov reversal potential?"}
	SetVariable Mrk1revsetvar,format="%G mV",limits={-inf,inf,10},value= _NUM:55
	SetVariable gMrk1setvar,pos={45,416},size={57,16},proc=DCHelperBuildProc,title=" "
	SetVariable gMrk1setvar,help={"What is the Markov peak conductance?"}
	SetVariable gMrk1setvar,format="%G nS",limits={0,inf,100},value= _NUM:400
	CheckBox Mrk1FixRadio,pos={26,416},size={16,14},proc=DCHelperRadioProc,title=""
	CheckBox Mrk1FixRadio,help={"Is the Markov peak conductance fixed?"}
	CheckBox Mrk1FixRadio,value= 1,mode=1
	CheckBox Mrk1ChRadio,pos={26,399},size={16,14},proc=DCHelperRadioProc,title=""
	CheckBox Mrk1ChRadio,help={"Is the Markov peak conductance set by a channel?"}
	CheckBox Mrk1ChRadio,value= 0,mode=1
	SetVariable Mrk1Chsetvar,pos={46,398},size={53,16},proc=DCHelperBuildProc,title="ADC"
	SetVariable Mrk1Chsetvar,help={"What A/D channel carries the Markov peak conductance?"}
	SetVariable Mrk1Chsetvar,limits={0,inf,1},value= _NUM:2
	SetVariable Mrk1Conductances,pos={29,477},size={55,16},proc=DCHelperBuildProc
	SetVariable Mrk1Conductances,help={"What Igor wave holds the relative conductances of each state?"}
	SetVariable Mrk1Conductances,limits={0,inf,1},value= _STR:"HHStates"
	Button button0,pos={83,4},size={64,37},proc=StartDCButtonProc,title="Start DC"
	CheckBox Mrk2Check,pos={106,382},size={70,14},proc=DCHelperRadioProc,title="Markov #2"
	CheckBox Mrk2Check,help={"Are you using a HH-Na conductance?"},value= 0
	SetVariable Mrk2Transitions,pos={85,458},size={110,16},proc=DCHelperBuildProc,title="Transitions"
	SetVariable Mrk2Transitions,help={"What Igor text wave holds the names of the microscopic transition waves?"}
	SetVariable Mrk2Transitions,limits={0,inf,1},value= _STR:"HHKTransitions"
	SetVariable Mrk2revsetvar,pos={116,435},size={80,20},proc=DCHelperBuildProc,title="V\\Brev"
	SetVariable Mrk2revsetvar,help={"What is the Markov reversal potential?"}
	SetVariable Mrk2revsetvar,format="%G mV",limits={-inf,inf,10},value= _NUM:-80
	SetVariable gMrk2setvar,pos={139,416},size={57,16},proc=DCHelperBuildProc,title=" "
	SetVariable gMrk2setvar,help={"What is the Markov peak conductance?"}
	SetVariable gMrk2setvar,format="%G nS",limits={0,inf,100},value= _NUM:50
	CheckBox Mrk2FixRadio,pos={120,416},size={16,14},proc=DCHelperRadioProc,title=""
	CheckBox Mrk2FixRadio,help={"Is the Markov peak conductance fixed?"}
	CheckBox Mrk2FixRadio,value= 1,mode=1
	CheckBox Mrk2ChRadio,pos={120,399},size={16,14},proc=DCHelperRadioProc,title=""
	CheckBox Mrk2ChRadio,help={"Is the Markov peak conductance set by a channel?"}
	CheckBox Mrk2ChRadio,value= 0,mode=1
	SetVariable Mrk2Chsetvar,pos={140,398},size={53,16},proc=DCHelperBuildProc,title="ADC"
	SetVariable Mrk2Chsetvar,help={"What A/D channel carries the Markov peak conductance?"}
	SetVariable Mrk2Chsetvar,limits={0,inf,1},value= _NUM:2
	SetVariable Mrk2Conductances,pos={96,477},size={99,16},proc=DCHelperBuildProc,title="Cond.    "
	SetVariable Mrk2Conductances,help={"What Igor wave holds the relative conductances of each state?"}
	SetVariable Mrk2Conductances,limits={0,inf,1},value= _STR:"HHKStates"
	SetVariable cmdsetvar,pos={0,496},size={45,16},value= _STR:"mafDC ",noedit= 1
EndMacro

Function DCHelperRadioProc(ctrlName,checked) : CheckBoxControl
	String ctrlName
	Variable checked

	strswitch (ctrlName)
		case "NaFixRadio":
		case "NaChRadio":
			checkbox NaFixRadio, value=0
			checkbox NaChRadio, value=0
			checkbox $ctrlName, value=1
			break
		case "Mrk1FixRadio":
		case "Mrk1ChRadio":
			checkbox Mrk1FixRadio, value=0
			checkbox Mrk1ChRadio, value=0
			checkbox $ctrlName, value=1
			break
		case "Mrk2FixRadio":
		case "Mrk2ChRadio":
			checkbox Mrk2FixRadio, value=0
			checkbox Mrk2ChRadio, value=0
			checkbox $ctrlName, value=1
			break
	endswitch
	DCHelperBuildProc ("", 0, "", "")
End

Function StartDCButtonProc(ctrlName) : ButtonControl
	String ctrlName
	controlinfo /w=mafdcpanel cmdSetVar
	execute s_value
End

Function DCHelperBuildProc(ctrlName,varNum,varStr,varName) : SetVariableControl
	String ctrlName, varStr, varName
	Variable varNum
	string buildflags = "", buildkeys="", tempstr
	
	if (checkVal ("vchsetvar", "mafDCPanel") != 0)
		buildflags += "/VCh=" + num2str (checkVal ("vchsetvar", "mafDCPanel"))
	endif
	if (checkVal ("ichsetvar", "mafDCPanel") != 0)
		buildflags += "/iCh=" + num2str (checkVal ("ichsetvar", "mafDCPanel"))
	endif
	if (checkval ("AMPACheck", "mafDCPanel"))
		buildkeys += ",AMPA={" + num2str (checkVal ("ampachsetvar", "mafDCPanel")) + "," + num2str (checkVal ("amparevsetvar", "mafDCPanel")) + "}"
	endif
	if (checkval ("GABACheck", "mafDCPanel"))
		buildkeys += ",GABA={" + num2str (checkVal ("gabachsetvar", "mafDCPanel")) + "," + num2str (checkVal ("gabarevsetvar", "mafDCPanel")) + "}"
	endif
	if (checkval ("LeakCheck", "mafDCPanel"))
		buildkeys += ",LEAK={" + num2str (checkVal ("gleaksetvar", "mafDCPanel")) + "," + num2str (checkVal ("leakrevsetvar", "mafDCPanel")) + "}"
	endif
	if (checkval ("NMDACheck", "mafDCPanel"))
		buildkeys += ",NMDA={" + num2str (checkVal ("nmdachsetvar", "mafDCPanel")) + "," + num2str (checkVal ("nmdaratesetvar", "mafDCPanel")) + "," + num2str (checkVal ("nmdavhalfsetvar", "mafDCPanel"))+ "}"
	endif
	if (checkval ("PulseCheck", "mafDCPanel"))
		buildkeys += ",PULS={" + num2str (checkVal ("pulsechsetvar", "mafDCPanel")) + "}"
	endif
	if (checkval ("TestCheck", "mafDCPanel"))
		buildflags += "/TEST"
		if (checkVal ("testsetvar", "mafDCPanel"))
			buildflags += "=" + num2str (checkVal ("testsetvar", "mafDCPanel"))
		endif
	endif
	if (checkval ("ZeroCheck", "mafDCPanel"))
		buildflags += "/ZERO"
	endif
	if (checkval ("NaCheck", "mafDCPanel"))
		if (checkval ("NaFixRadio", "mafDCPanel"))
			buildflags += "/Nafx=1"
		endif
		buildkeys += ",NaV={" + num2str (checkval (SelectString (checkval ("NaFixRadio", "mafDCPanel") , "NaChSetVar", "gNaSetVar"), "mafDCPanel"))
		buildkeys += "," + num2str (checkval ("NaRevSetVar", "mafDCPanel"))
		buildkeys += "," + popupval ("ahsetvar", "mafDCPanel") + "," + popupval ("bhsetvar", "mafDCPanel")
		buildkeys += "," + popupval ("amsetvar", "mafDCPanel") + "," + popupval ("bmsetvar", "mafDCPanel") + "}"
	endif
	if (checkval ("Mrk1Check", "mafDCPanel"))
		if (checkval ("Mrk1FixRadio", "mafDCPanel"))
			buildflags += "/M1fx=1"
		endif
		buildkeys += ",Mrk1={" + num2str (checkval (SelectString (checkval ("Mrk1FixRadio", "mafDCPanel") , "Mrk1ChSetVar", "gMrk1SetVar"), "mafDCPanel"))
		buildkeys += "," + num2str (checkval ("Mrk1RevSetVar", "mafDCPanel"))
		buildkeys += "," + popupval ("Mrk1Transitions", "mafDCPanel")
		buildkeys += "," + popupval ("Mrk1Conductances", "mafDCPanel") + "}"
	endif
	if (checkval ("Mrk2Check", "mafDCPanel"))
		if (checkval ("Mrk2FixRadio", "mafDCPanel"))
			buildflags += "/M2fx=1"
		endif
		buildkeys += ",Mrk2={" + num2str (checkval (SelectString (checkval ("Mrk2FixRadio", "mafDCPanel") , "Mrk2ChSetVar", "gMrk2SetVar"), "mafDCPanel"))
		buildkeys += "," + num2str (checkval ("Mrk2RevSetVar", "mafDCPanel"))
		buildkeys += "," + popupval ("Mrk2Transitions", "mafDCPanel")
		buildkeys += "," + popupval ("Mrk2Conductances", "mafDCPanel") + "}"
	endif

	tempstr = "mafDC"+buildflags+" " + buildkeys[1,strlen(buildkeys)-1]
	setvariable cmdsetvar, win=mafDCPanel, value=_STR:tempstr, size={fontsizestringwidth ("MS Sans Serif", 12, 0, tempstr)+10,16 }
End

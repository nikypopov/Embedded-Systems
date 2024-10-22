Video Demo: https://youtu.be/BFlAUsvJ1go

(^1) nikyp@bu.edu
(^2) bbrake01@bu.edu
Abstract —Thisprojectisaproofofconceptforadedicatedsensor
systemfor anRCcar.Itutilizes 3 sensors,thosebeing 2 I2S
microphonesandanultrasonicdistancesensor,allconnectedtoa
BeagleBoneBlack.Theoutputforthismodelissimply 2 LEDs, 1
correspondingtoeachmicrophone.Inafuturemodel,thisoutput
wouldbetranslatedtothemotorsoftheRCcarusingaconnected
navigationsystem.
Keywords — ALSA,BeagleBone Black,devicetree, embedded
systems,I2S,McASP,RCcar,soundlocalization.
GitHubLink— github.com/bbrake01/535-group/tree/main/Project
I. INTRODUCTION
This project presents a foundational exploration
into sensor-based navigation for remote-controlled
(RC)cars,leveragingsoundlocalizationandobstacle
detection. The primary objective is to develop a
proofofconceptforanembeddedsensorsystemthat
utilizes a combination of acoustic and distance
sensingtechnologiesto guideanRCcartowardsa
designatedsoundsourcewhileavoidingobstacles.
At the heart of this system is the BeagleBone
Black, which manages three sensors: two I2S
microphonesandanultrasonicdistancesensor.These
sensors are employed to discern the direction of
sound and detect physical obstacles, respectively.
The sensory information is visually represented
through twoLEDs,whichindicatethestatusofthe
sensors.
Theultimatevisionforthisinitiativeistointegrate
thissensorysystemwithanavigationalmodulethat
directs theRC car's motors, enabling autonomous
movementtowardssoundsources.Thegoalofthe
system is to demonstrate basic directional sound
localizationandimmediateobstacledetection.When
asoundisemitted,itisidentifiedasoriginatingfrom
either theleftorright,triggeringthecorresponding
LED. Movement of the sound source results in
real-timeswitchingoftheLEDs.Simultaneously,the
presence of an obstacle activates an interrupt,
disablingthemicrophoneindicatorsandlightingup
bothLEDstosignalobstacledetection.
II.TECHNOLOGYSTACK
This section details the essential hardware and
softwarecomponentsutilizedinthedevelopmentof
the sensorsystemfor theRCcar.See SectionIII.
TechnicalDetailsforanextendedexplanation.
A. Hardware
The core of the hardware assembly is the
BeagleBone Black board, which orchestrates the
operation of the sensorycomponents. The system
employs two Adafruit I2S MEMS Microphones
(SPH0645) for acoustic signal capture and one
HC-SR04 ultrasonic distance sensor for proximity
detection. Visual feedback from the sensors is
facilitated through two LEDs. The circuit also
integratesavoltagedividerusingone 10 kΩandone
20 kΩresistor,alongsideavarietyofjumperwiresto
establish the necessary connections between
components.
B. Software
Onthesoftwarefront,thesystem'sfunctionalityis
drivenbyacustomLinuxkernelconfiguration.This
involves a modification to the device tree viathe
inclusionofa.dtsifileintotheprimary.dtsdevice
tree source file, which issubsequentlyrecompiled
intoa .dtb(DeviceTreeBlob)file(seeSectionIV.
for more details). Additionally,a custom.ckernel
module was developed to handle the operational
logicofthesensorsystem,encapsulatingthecontrol
ofinputsignalsandresponsemechanisms.
III.TECHNICALDETAILS
A.SoftwareArchitecture
Thesoftwarearchitectureofthesensorsystemis
centeredaroundarobustkernelmoduledesignedto
manage the real-time operations of the embedded
sensors. Thismodule isconfiguredtoinitiateupon
system startup and functions continuously via a

recurring kernel timer, ensuring consistent
performanceandresponsiveness.
Themoduleutilizesakerneltimersetatadefined
interval, cateringto differentoperationaldemands.
Each timer expiry triggers the timer_callback
function, which is critical for initiating the
measurement cycle of the ultrasonic sensor and
soundlocalization.
Inthetimer_callbackfunction,theTRIG_PIN
of the HC-SR04 ultrasonic sensor is set high to
initiatethemeasurementprocess.TheTRIG_PINis
maintained high for exactly 10 microseconds,
facilitatedbytheudelay(10);function,toensurea
uniform ultrasonic burst is emitted. Immediately
afterwards,theTRIG_PINissetlow,concludingthe
pulseandemissionphase.
Echodetectionismanagedthroughtheecho_isr
(InterruptServiceRoutine),whichbeginswhenthe
ECHO_PINgoeshighasthesensorstartstoreceive
reflectedultrasonicwaves.Thismomentiscaptured
asecho_startbytheecho_isr.TheECHO_PIN
remainshighwhilethewavesarebeingreceived,and
oncethereceptionends,theECHO_PINissetlow,
marking echo_end. The time duration between
echo_startandecho_endiscalculated,corresponding
tothedistancetraveledbythewaves.
Thedistance toan objectis computedusing the
formula:

𝑑𝑖𝑠𝑡𝑎𝑛𝑐𝑒= (𝑡𝑖𝑚𝑒𝑒𝑙𝑎𝑝𝑠𝑒𝑑* 𝑠𝑝𝑒𝑒𝑑𝑜𝑓_𝑠𝑜𝑢𝑛𝑑 / 2 )

where time_elapsed is the duration for which the
ECHO_PINwashigh,representingthetimetakenfor
theultrasonicpulsetotraveltotheobjectandback,
and the speed of sound is considered to be
approximately 34000 cm/s. The division by two
accountsfortheroundtriptravelofthesoundwaves.
Upon identifying an obstacle (if the calculated
distance fallsbelow apresetthreshold),thesystem
triggersanobstacleinterrupt,lightingupbothLEDs
inshort,half-secondpulses.Itiswithinthisroutine
that the rover would initiate obstacle avoidance
procedures.Thesystemremains inthis modeuntil
theobstacleiscleared.Afterward,itreturnstosound
localization, lighting uptheLEDcorrespondingto
thedirectoutofthesource.Again,itisinthisroutine

the RC car would be directed towards thesound
source. This cycle of detection,obstacle handling,
andnavigationiscontinuouslydrivenbythekernel
timer, ensuring ongoing monitoring and adaptive
responsetoenvironmentalchanges.
Fig. 1 Diagramdetailingahighleveloverviewofthesoftwarearchitectureand
logicflow
Fig.2.DetaileddiagramofinterruptserviceroutineusedwiththeHC-SR04for
obstacledetection
B.HardwareArchitecture
EachLEDispluggedintoadedicatedGPIOpinin
series with a small resistor. These pins are not
typically able to drive anything, but produce
sufficient power for a smallLEDevenwith logic
voltage.
TheHC-SR04UltrasonicSensor,picturedbelow,
is used tomeasuredistance inorder to implement
obstacle avoidance.Fig. 2 showsthepinoutofthe
device, which are connected to the BeagleBone
Blackasdescribedbelow:

Vccconnectsto5V.GNDconnectstoGND.
Trigsendsapulsetotellthesensorwhento
transmitdata.Inordertofacilitatethissmall
pulse,itconnectstoGPIO_51.
EchoconnectstoGPIO_48.Sincethesensor
isa5VdeviceandtheBBBuses3.3Vlogic,
asimplevoltagedividerusinga10kΩanda
20kΩ resistor was used to step down the
voltageandprotecttheBBB.
Fig. 3 HC-SR04UltrasonicRangeSensorwithlabeledpins[6]
TheBeagleBoneBlacksupportsMcASPprotocol
(Multichannel Audio Serial Port).Inthis protocol,
I2Sisacertainmode,notablynotrequiringamaster
clock.I2Ssupports 2 devices onthesamebusfor
stereo sound. Fig. 3 shows the MEMS I2S
microphoneusedinthedesign.

Fig. 4 I2SMEMSmicrophonewithlabeledpins[5]
Belowisadescriptionishoweachpiniswiredto
theBeagleBoneBlackandthewidersystem:

SEL:SettoeitherVccorGNDtodistinguish
betweenright/leftmicrophone,respectively.
LRCL: Otherwise known as Word Select,
when high/low ittells right/left to transmit
data.ThisconnectstoFSR0.
- DOUT:Serialdataport.Thisconnectstothe
AXR0 port on the BBB. Only one
microphonewilltransmitdataatatime,thus
theycanusethesamebus.However,I2Sonly
supports stereo transmission, whereas the
broaderMcASPsupportsmoreconnections.
- BCLK:Bitclock,determinesbitrateofserial
datatransmission.ThisconnectstoACLKR.
- 3Vconnectsto3.3V,andGNDtoGND.
Fig. 4 shows a generalized McASP bus model,
with each microphone serving as an ADC clock
follower. This diagram was used to identify and
translatebetweenthemicrophoneandBBBpins.
Fig. 5 McASPprotocolwithADCasclockfollower[1]
C.FinalPinout
Device DevicePin PhysicalPin PinMode
MicrophoneL SEL P9_1 GND
MicrophoneR SEL P9_3 VDD_3V
Microphone
L/RCombined
Busses
LRCL P9_27 mcasp0_fsr
DOUT P9_41B mcasp0_axr
BCLK P9_42B mcasp0_aclkr
Ultrasonic
Sensor
Trig P9_16 GPIO_
Echo P9_15 GPIO_
LEDL + P9_23 GPIO_
LEDR + P9_25 GPIO_
Fig. 6 Systempinouttable,notincludingVcc/GND
IV.DISCUSSIONOFREVISIONS&SUCCESS
A. Challenges&Failures
Throughout our project, we encountered several
significantchallengesthatimpactedbothourplanned
processesandmethodologies.Thesechallengeswere
primarily centered around hardware configuration,
software limitations, and tedious protocol
implementationontheBeagleBoneBlack.
Onemajor issue was our inabilityto utilizethe
capemanagerutilityoftheBBB,whichsignificantly
hinderedourattemptsto configurethehardwareto
our specifications. Our initial strategy involved
convertingthedevicetreebinary(.dtb)toitssource
codeandmanuallyincorporatingouredits.However,
we faced persistent compilation errors, primarily
because we were not compiling directly on the
Beagle Bone's filesystem, which would have
providedaccesstoall necessaryincludefiles.This
complicationaroseduetotheabsenceofthedevice
treecompilerutilityontheBeagleBone,forcingusto
perform these operations on our machines.
Unfortunately, after making the necessary
adjustments tothesource code,wewereunableto
recompile it backinto a binaryformat compatible
withtheBeagleBone.
Withoutafunctionaldevicetree,thenexttechnical
hurdle wasimplementing the I2Sprotocol viathe
McASPpinsontheBBBequippedwithanAM335x
processor.OurgoalwastoconfiguretheMcASPfor
audio reception by setting the clock to 2 MHz,
enabling internal frame sync, and configuring the
serializerforI2Smode.Thissetupwasintendedto
detect audio signals above a certain threshold
without the need for audio storage or playback.
Despite careful setup, loading the kernel module
resultedinakernelpaniccharacterizedbyan"error
unhandledfault:impreciseexternalabort,"indicating
potential issues with accessing unmappedmemory
addresses or incorrect memory mapping. We
managed theconfigurationof theMcASPregisters
directly in the kernel module code, using
memory-mapped addresses and offsets such as
MCASP_BASE and MCASP0_AXR1_OFFSET.
Despite thesignificant effort put towardsderiving
safe andusableaddresses,weencounteredakernel
panicduringeachiterationtesting,suggestingfurther
issues with address mapping alignment or
uninitializedmemoryaccesses.
As an alternativeto theI2Simplementation, we
also explored using analogmicrophonesinterfaced
through auserspaceprogramviatheBBB'sAIN
and AIN1 pins. However, this approach also

stumbled due to similar device treechallenges,as
enablingtheBBB'sADCdriverrequiredloadingan
appropriatedevicetreeoverlay—ataskcomplicated
byourinabilitytousethecapemanager.Overall,a
more feature-rich BeagleBone Black image would
have saved us a significant amount of time and
resultedinamorepresentableproduct.
B. Adjustments&Changes
TheRCcarwasnotavailable,sowewereforced
toswitchovertoLEDdisplaysandfocussolelyon
thedetectionsystem.Thisshrinkingscopeultimately
allowedustomakeafunctionalsystem,albeitatthe
expenseofamorevisuallyandpracticallyappealing
finalproduct.
Thescope of themicrophone arrayscaled back
considerablyduetotechnologyandtimeconstraints,
andultimately
Whilewe initiallyintended touse anarrayof 4
microphones to performrobust soundlocalization,
themicrophonesweutilizedwereconfiguredtothe
I2S protocol, which only supports stereo sound,
meaning 2 inputsonly.
C. SuccessfulImplementations
Whileourproject’sscopescaledbackconsiderably
throughout our work cycle, we emerged with a
robust ultrasonicsensorimplementation.Giventhat
thecrutchoftheproject(thatbeingthemicrophone
array) was unsuccessfully implemented, we were
forced to demo what we did have,thatbeingour
control flow and obstacle detection. A button
replaces themicrophone input,whichturns onthe
associated LED. Then when theultrasonic sensor
detects an object too close, it will interrupt the
microphone routineand beginflashingbothLEDs.
Astheobjectgetscloser,theLEDswillpulsefaster
togetthedriver’sattention.
Wemanagedtocorrectlyprioritizeobjectdetection
over soundlocalization.This prioritizationensured
oursystem wouldreact appropriatelytoimmediate
environmental inputs while still performing its
primary function of sound localization when
conditionsallowed.
D.TechnicalSkillsLearned
Throughoutthedurationofthis project,weboth
acquiredseveralcriticaltechnicalskillsessentialfor
embedded systemsdevelopment,particularly inthe
contextofutilizingtheBeagleBoneBlackplatform.
Interfacing with GPIO Pins: We developed
proficiency in interfacing directly with GPIOpins
through a kernel module. This was crucial for
controllingtheultrasonicandmicrophonesensor,as
detailedinourkernelmodule.
Pin Multiplexing: We learned to set pin modes
through direct manipulation of hardware control
registers within kernel space, which allowed for
precise controlover hardware’sfunction.This was
donethroughtheset_pin_modefunction.
Interrupt Service Routines: We learned how to
registerandtriggerinterruptserviceroutines(ISRs)
via GPIO pins. This was integral in handling
real-timechangesinsensordata,allowingimmediate
responsestoenvironmentalinputs.
KernelTimers:Weeffectivelyimplementedkernel
timers to schedule future operations at specific
intervals. This functionality was applied to
periodicallypollthesensordataandmanagesystem
stateseffectivelyfromwithinkernelspace,avoiding
theoverheadofuserspaceprocesses.
Device Tree Modifications: Another significant
learningareawasinthemodificationofdevicetrees.
Although initially intending to use device tree
overlays with the BeagleBone’s cape manager
functionalities,differencesinourLinuxdistribution
ledustoa moredirectapproach.Wemodifiedthe

device tree(.dtb) byconvertingitbacktoadevice
treesource (.dts)file usingadevice treecompiler.
Wethencreatedandincludedadevicetreeinclude
(.dtsi) in the .dts file to reconfigure the hardware
settingsontheboard,whichwasthencompiledback
into binary format using thedevice treecompiler.
Thisprocessisalsocrucialforensuringhardwareis
configured correctly to support a specific sensor
system.
Theseskills helpeddeepenour understandingof
embedded systems, particularly in real-time data
handlinganddeviceconfigurationunderconstraints.
REFERENCES
[1] B.Tufino,“McASPDesignGuide-Tips,Tricks,andPracticalExamples
ApplicationReportMcASPDesignGuide-Tips,Tricks,andPractical
Examples,”2019.Accessed:Apr.30,2024.[Online].Available:
https://www.ti.com/lit/an/sprack0/sprack0.pdf?ts=
[2] “ALSAproject-theClibraryreference:Index,PreambleandLicense,”
http://www.alsa-project.org.https://www.alsa-project.org/alsa-doc/alsa-lib/
(accessedApr.30,2024).
[3] “SettingUptheBeagleBoneBlack’sGPIOPins,”vadl.github.io.
https://vadl.github.io/beagleboneblack/2016/07/29/setting-up-bbb-gpio
(accessedApr.30,2024).
[4] P.Sharma,“HowtointerfaceanI2SmicrophonewithBeaglebone
Black(BBB),”MantraLabs,Jul.12,2018.
https://www.mantralabsglobal.com/blog/how-to-interface-an-i2s-microph
one-with-beaglebone-blackbbb/(accessedApr.30,2024).
[5] “AdafruitI2SMEMSMicrophoneBreakout,”AdafruitLearningSystem.
https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/
[6] “Sensors—BeagleBoardDocumentation,”docs.beagleboard.org.
https://docs.beagleboard.org/latest/books/beaglebone-cookbook/02sensor
s/sensors.html(accessedApr.30,2024).
[7] “AM335xandAMIC110SitaraTMProcessorsTechnicalReference
Manual.”Available:https://www.ti.com/lit/ug/spruh73q/spruh73q.pdf
Githublink: https://github.com/bbrake01/535-group/tree/main/Project

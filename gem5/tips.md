# Gem5: A guide to understanding it better...

This guide serves as a guide mainly for me during my internship and working on Gem5 - particularly on adding custom instructions (**mainly ARM**), changing store-set in O3 CPU and testing...

I have tried to make this as coherent and succinct as possible.

_______________________________________________________________________________________
## Gem5 basics

### *TIPS for gem5* 

1. Compiling the binary for an arm/RISV/ etc --> use the following link: https://wiki.huawei.com/domains/1390/wiki/9608/WIKI2021122401759
	--> make sure you are in the same folder in gem5, when compiling.

2. Hidden typdefs are big in that you may not know where all the typdefs are declared. Consult http://doxygen.gem5.org/release/v22-0-0-0 (if new release obviously use that)

______________________________________________________________________________________________________________________________________________
### *GEM5 structure:*

GEM5 has a python wrapper where classes are defined and then the actual implementation is in C++ files. To find where the wrapping happens look at SimObject.py in m5 and then if you look at the imports there will be pybind.py
--> *double check this and look a bit deeper* (see below)
--> objects for each type of architecture are generated through these python files however, gem5 provides base classes which are inherited by each architecture's classes.



*Key wrapper docs* (all in gem5/src)
--> m5/simulate.py
--> m5/SimObject.py (NB. there are 2 SimObject.py files. The outermost is the one with more useful content)
--> m5/params.py
--> m5/main.py
--> m5/util/pybind.y
--> sim/root.py
--> SConscript the (the outermost one has the class defintions, the list of Pysources have all the directories which are processed)
--> scons/sources.py

Let's do a round trip of how the wrapping works...

1. Firstly, SimObject.py classes are inherited from SimObject.py
	--> As said in the comments of this file, "This class controls how new classes that derive from SimObject are instantiated ..."
	--> Attributes declarations set here.
	--> The *type* is the C++ class that you are wrapping with with the Python Object. 
	--> Objects get created in createCCObject (using self._ccObject = params.create()) (not sure where create() comes from *(see below in the API change bit)*)
	--> descendants() calls the moethod on the parent node before its children...
2. simulate.py instantiates the SimObject graph and initialises the system, calling on functions from root.py and in turn SimObject...

3. I think wrapping of the C++ happens compile time. This creates a Params folder which holds all the parameter settings... (*see sim_object.hh* in the commented section for some more explanarion)
    --> There has been **API Change** and so there are NO more create() functions which creates a C++ instance of one of the classes set up with the parameters in the Params struct. *see release notes* 
	--> Now the function is created automattically. (idk how/where lol) 
	--> **UPDATE**!!: its in the SConscript file...
_______________________________________________________________________________________________________________________________________________

1. Class is declared in python .py script. 
	a. This contains basic attributes for the base class
2. After creating python projects, the C++ equivalents are created to run the code. 
3. Header file for functions/varaible defintions/attributes/methods are declared. 
	a. declared as a C++ class along with other things like request response ports.
4. Normal c++ to give the actual functionality. 
	a. The attributes in here are given based on the parameters of the Python object
_______________________________________________________________________________________________________________________________________________
#### *Key stuff*

 There is some overlap in the way that the includes are done. 

 For example in the https://wiki.huawei.com/domains/1390/wiki/9608/WIKI2022031000013 "TLB changes" tutorial, you follow it but may wonder where the BaseTLB parent class comes from.
- This is found in the include "arch/generic/tlb.hh" where the generic BaseTLB is made and then inherited to make each TLB for each architecture
_______________________________________________________________________________________________________________________________________________
#### Wiki Notes:

**Steps to make changes in the microarchitecture:**
1. Change python script.
2. Add new attributes that you may use to .hh 
3. Use attributes in .cc 
4. See https://www.gem5.org/documentation/learning_gem5/part2/helloobject/ for making a simple SimObject
	--> This will give the overview for the syntax, proceedures to adhere to.
	--> *Key* The constructor for all SimObjects assumes it will take a parameter object.  
5. *Remember to rebuild gem5 after modifying files in the src/ directory.

To test against a specific configuration of a microarchitecture:
1. Make a config file --> generic ones are 
2. Use: http://learning.gem5.org/book/part1/simple_config.html as an example of how to make a simple config file.
3. Some configs and programs simulated do not report timing because gem5 uses the atomic CPU and uses atomic memory accesses, so there is no real timing data reported. You can specify a type of CPU when you build on the CLI using --cpu-type=TimingSimpleCPU 
	- these are cache configurations--l1d_size=64kB --l1i_size=16kB
4. Some common options are described here: http://learning.gem5.org/book/part1/example_configs.html
5. The output of the simulation is in m5out folder...
6. To run an example command is as such: 
	build/X86/gem5.opt configs/tutorial/part1/simple.py
	*build/X86/gem5.opt*  = the build option for the specific architecture you want to test with
	*configs/tutorial/part1/simple.py* = the config file that you wish to use. 
_______________________________________________________________________________________________________________________________________________
##### *Creating SimObjects*
1. Python class for new sim object
2. Implement SimObject in C++
3. REgister SimObject and C++ file in the SConscipt file. 
4. Rebuild gem5
---> scons build/X86/gem5.opt -j9
---> scons build/ARM/gem5.opt -j9
---> scons build/RISCV/gem5.opt -j9
5. Create a config script for your new SimObject
6. Run the script
_______________________________________________________________________________________________________________________________________________
##### *Events*
gem5 is an event-driven simulator --> gem5 provides a wrapper function for creating simple events.

Schedule events to be processes...
Can define them yourself or the latency parameters defined in the config can be use.


*Params*
Python parameters are modified using the Param type:
read: https://www.gem5.org/documentation/learning_gem5/part2/parameters/

**REMEMBER* --> once you have declared these parameters in the SimObject file, you need to copy their values to your C++ class in its constructor.
_______________________________________________________________________________________________________________________________________________
##### *SIMULATIONS*

SE (syscall emulation): does not emulate all of the devices in the system and focuses on simulating the CPU and the memory system. Much easier to configure since you do not instantiate all

FS: Full system emulates the entire hrdware system and runs n unmodified kernel. Full system mode is similar to running a virtual machine.

> see: http://learning.gem5.org/book/part1/simple_config.html

*Tips* 
- Use debug flags via: --debug-flags=*name of flag*
- you can create a debug flag in SConscript file. See https://www.gem5.org/documentation/learning_gem5/part2/debugging/ for more info on how to do this properly.
_______________________________________________________________________________________________________________________________________________
##### **Simulating a program**

See the Huawei wiki: https://wiki.huawei.com/domains/1390/wiki/9608/WIKI2021122401759

1. First build gem5
2. Compile your script you want to simulate in gem5
	a. gcc -o <binary> <yourscript>
	   file <binary>
	   Result should be something like this: ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked
3. For a syscall emulation config, you run the following code:
	./build/ARM/gem5.opt configs/example/se.py -c <armbinary>
_______________________________________________________________________________________________________________________________________________
> Files with **key typdefs**
> - types.hh: machInst

**Possibly useful functions/Typedefs/ to know:**

```
/**
 * Generate a 64-bit mask of 'nbits' 1s, right justified. If a number of bits
 * greater than 64 is given, it is truncated to 64.
 *
 * @param nbits The number of bits set in the mask.
 *
 * @ingroup api_bitfield
 */
constexpr uint64_t
mask(unsigned nbits)
{
    return (nbits >= 64) ? (uint64_t)-1LL : (1ULL << nbits) - 1;
}
```
```
/**
 * Extract the bitfield from position 'first' to 'last' (inclusive)
 * from 'val' and right justify it.  MSB is numbered 63, LSB is 0.
 *
 * @ingroup api_bitfield
 */
template <class T>
constexpr T
bits(T val, unsigned first, unsigned last)
{
    assert(first >= last);
    int nbits = first - last + 1;
    return (val >> last) & mask(nbits);
}
```
These functions return a value from 0....n such that they can be used as switch statements...


mask generates a 64 bit mask from nbits...

makeSP(rn) makes the register the stackpointer regsiter if == X31
```
template<class T>
    static inline T
    cSwap(T val, bool big)
    {
        if (big) {
            return letobe(val);
        } else {
            return val;
        }
    }
```
This function returns the a big/little endian swapping of a set of data.


isLLSC() - indicates whether the request is an Load-Link/Store conditional (via isSet() --> this just says if it is valid or not...)

Addr *[types.hh]* just defines the type of the address (uint64_t)

______________________________________________________________________________

### **Instruction decoding in Gem5**

>**Structure to look through in source files**:
> ARM ISA.isa > Decoder.isa > arm.isa > format/....isa  > insts/....isa

- Typically (especially in x86 and CISC architectures), each instruction is implemented as a combination of several smaller parts. The entire instruction is typically referred to as a macro-op, while the smaller parts are referred to as micro-ops.

- In .isa files this is where the instruction sets are expressed. 
- At the highest level, an ISA description file is divided into two parts: a declarations section and a decode section. 
	-->The decode section specifies the structure of the decoder and defines the specific instructions returned by the decoder. 
	-->The declarations section defines the global information (classes, instruction formats, templates, etc.) required to support the decoder.
- When the ISA description file is processed, each instruction definition does in fact invoke a function call to generate the appropriate C++ code for the decode file.
- Instruction formats defined by the ISA can be specified in two ways: it is given by :: (double semicolon)
	- a format block be used instead of the :: if there are sets of definitions within the same instruction format. 
	- they are functionally equivalent - see https://www.gem5.org/documentation/general_docs/architecture_support/isa_parser/
	- the format of the innermost block will be used. 
	- The instruction format determines the number and type of the arguments given to the instruction definition, and how they are processed to generate the corresponding output.
- Format blocks and decode blocks can be nested with each other arbitrarily. 
- default cases may be for unspecified cases to unknown/illegal instructions

- for ARM bitfield definitions are done in types.hh 
	- these definitions generates a C++ preprocessor macro that extracts the specified bitfield from the implicit variable machInst.

- the machinst to the decode function is also called machInst.

- templates are used to generate pieces of C++ code with specialized parameters....
- templates can reference other templates...
- output blocks allow the ISA description to include c++ thaat is copied to the output file
- let blocks provide for global Python code
- implementations of bitfields based on preprocessor macros and C++ template functions, so the size of the resulting value will depend on the context.

- finding how the .isa files get translated to the C++ objects, look at main.isa and ...(work up the tree...) --> look at definition of ISADesc

>**Adding an instruction** --> see the above link for how to specify the default.
>1. Look at the .isa file of the architecture you are working in --> go to decoder.isa 
>2. build an instruction format for the instruction you want in formats... 
	- basically a python function that takes the arguments supplied by an instruction definition (found in the decode block)
> 3. build the instruction in the insts/....isa section where c++ will be generated once built**
_______________________________________________________________________________________
###  *Solving any confusion with naming:*

> 64 + 32 bit
> Architecture: AArch64
> Specification: ARMv8-A
> Instruction sets: A64 + A32
> Suffixes: v8-A
> 
> 32 + 16 (Thumb) bit
> Architecture: AArch32
> Specification: ARMv8-R / ARMv7-A
> Instruction sets: A32 + T32
> Suffixes: -A32 / -R / v7-A
> Example: ARMv8-R, Cortex-A32[8]

#### Instructions in AArch64:

ldrb X0 "address" = when a sub-register-sized quantity of data is loaded, the rest of the register is zeroed (zero extended)

ldrsb X0/W0 "address" = Adding an S to the operation causes the value to be sign extended instead. How far the size
						extension goes depends on whether the target is a W or X register
						if W: the  only sign extend up the the end of the word
						if X: sign extend right till the end...


> 4 register modes:
>1. Base register
>2. Offset
>3. Pre indexing: Pre-indexed addressing is like offset addressing, except that the base pointer is updated as
	   a result of the instruction. In the preceding figure, X1 would have the value X1+12 after the
	   instruction has completed.
>4. Post indexing...

#### *Instructions to memory: WHERE TF DOES THIS HAPPEN...*

Things to find out:
Where assembly happens...
How the instruction is fetched from either the ITLB/Cache etc.. (from the ISA specic side)
    --> So the generated C++ files produce the following lines:

```c++
LDRW64ROHAN_IMM::LDRW64ROHAN_IMM(ExtMachInst machInst,
	RegIndex _dest, RegIndex _base, int64_t _imm) :
	ArmISA::MemoryImm64("ldrrohan", machInst, MemReadOp,
				(RegIndex)_dest, (RegIndex)_base, _imm)
{
	.... some instruction specific stuff...
}
```
####
```c++
The class LDRW64ROHAN_IMM inherits from Memory64Imm...
class LDRW64ROHAN_IMM : public ArmISA::MemoryImm64
This class inherts from Memory64 in insts/mem64.hh
```
The **decoder-ns.cc** file creates an instance of the instruction object and thus the constructor is called and fills in the pointers... ```(return new LDRW64ROHAN_IMM(machInst, rt, rnsp, imm12 << 2);) ```


- Instructions can be macroops or microops whereby if a macroop, the microops will be fetched separately...
- Microops get assigned in uops and this is fetched in fetchMicroOOP()
- not all isntrucitons have microops (for a reason I have to find out) and thus are fetched differently as a macroop.
- machinInst must carry through, and it does all the way up to a parent class called ArmStaticInst
- ArmStaticInst inherits from StaticInst 
- cannot seem to print _machInsts in MightBeMicro64 (why???)
- the printable of the instruction that is decoded in arm/decoder is the same as the machine instruciton...
- I believe then a staticInst becomes a dynamic instruction which adds some parameters on. 
- from the fetch.cc to the decoder.cc in cpu/, the instructions are not processed one by one but more placed in a queue then decoded one by one and then sent to the rename...

### Load/Store 

During a *LOAD/STORE*:
- See below link for good step by step of the process.
	https://www.gem5.org/documentation/general_docs/cpu_models/O3CPU
- *IMPORTANT*: some of the instructions may be reoordered and thus the execution of instructions may be out of ordered...
- Each instruction when called (look at generated C++ files from .isa files) calls dyn_Inst::initiateMemRead() and then is directed to LSQ::pushRequest...
- The generated C++ files from the .isa files specify the type of load and call the appropriate initiateAcc/CompleteAcc/Execute files
    -Lets dig deeper into initiateMemRead here...
	- Look at into the Rabbit hole part2 (clean up for final tips...)

- Important methods:
	- IEW::executeInsts()
	- LSQ_unit::read()  --> *request should constain the metadata of the load??(need to rabbit hole it again maybe)
	- LSQ_unit::store()
	- LSQ::pushRequest() --> the request (created in line817/820) is passed through to read() (line847) 
	- LSQ::SingleDataRequest::initiateTranslation
	- LSQ_unit::completeDataAccess is where tjhe accessing happens???

#### **Alias time**
[LSQ_UNIT.cc]
- in the function LSQ_unit::executeLoad() --> line602 load_fault = inst->initiateAcc() --> calls the instructions initiateAcc()
- in the function lsq_unit::read(), checks are performed to see if the load is aliased with any previous store (need to see how far back it goes in the store queue) [needtogooverthisagain]
	- {From gem5 website}
	- Either it can do a forward, and will schedule WritebackEvent for the next cycle.
	- If it is aliased but cannot forward, it calls InstructionQueue::rescheduleMemInst() and LSQReuqest::discard().
	- Otherwise, it send packets to the cache.
- in the function LSQ_unit::executeStore() --> line672 points to a potential load that might violate and the function returns a checkViolation...
	- this is different from the executeLoad() function because that function returns load_fault or checkViolations(if entered condition).
	- this function returns store_fault (if entered condition) or always returns checkViolations.
[InstructionQueue.cc]
- rescheduling instructions happen here: InstructionQueue::rescheduleMemInst
	- this calls the reschedule function in the [mem_dep_unit.cc]
[mem_dep_unit.cc]
- MemDepUnit::reschedule push backs the dynamic instruction to the instsToREplay dynamic pointer list...
- functions in the LSQ_unit::completestore & LSQ_UNIT::storePostSend

[iew.cc]
- IEW::executeInsts() [needtogooverfunctionagain]
	- line1216 LSQUNIT::executeLoad() called
		- violations are checked in return checkViolations --> line650
	- line1348 ldstQueue.violation... --> ldstQueue is an object of the LSQ class
		- [LSQ.cc]::violation checks if there are violations in any of the threads...
			--> somewhere thisthread[tido].violation() returns a bool to indicate whether there was a violation ....
			- [LSQ_unit.hh] bool violation() returns memDepViolator which is a DynamicinstrPtr
**SO it seems there is two violation checkers one as above and one as below ** 
	- line1340 (approx) checks if there is a memory order violation and causes a squash to happen --> squashDuetoMemOrder(violator, tid)
	- tells the instruction queue --> instQueue.violation(inst, violator) --> instQueue is an object of the InstructionQueue class
		- [InstructionQueue.cc]
			- ::violation --> memDepUnit.violation called.
				- [mem_dep_unit.cc]
					- ::violation -> depPred.violation called --> depPred is an object of the storeSet class
						- [store_set.cc]
							- ::violation --> checks if the load/stores are valid  
											--> NEED TO UNDERSTAND THE GROUP OF IF STATEMENTS... 
________________________________________________________________________________________________________________________________________________
## Changing the alias predictor + Personal Logs

**POSSIBLE MODIFICATIONS TO THE ALIAS UNIT**
*Turning:off:prediction*
[IEW::executeInsts] 
1) line1321 change the conditions that check for violation in [1: the compiler has issued instruction indicating the load is alias free | 2: the compiler has already seen aliased occurence and needs to just reschedule]
look at whether the LSU/alias predictor does address arithmetic to decide the reordering/aliasing .....
	- DOUBLE CHECK THIS IS THE RIGHT VIOLATION (not branch predictions..)
2) line1299
[LSQ_unit::executeLoad] *unsure* but maybe something heree....
	- ::checkViolations...
		- line538, ld_inst->hitExternalSnoop() --> need to figure this out..
			- but seems like you can add conditionals here to pass in the alias free case...
[LSQ_unit::executeStore]
	--::checkViolations...		
[Jump:to:rescheduling]
1)

The flow of instructions in the CPU pipeline:  fetch.cc --> decode.cc --> rename.cc

Flow of misspeculation instruction:

IEW::tick()->IEW::executeInsts()
  ->LSQUnit::executeLoad()
    ->StaticInst::initiateAcc()
		-> LSQUNit::read()
    ->LSQUnit::checkViolation()
  ->IEW::squashDueToMemOrder()
__________________________________________________________________________________________________________________________________________
**CASE (in all cases *insert* needs to be modified)**
1) Speculate load without store set (re-order etc..) and validate to see if not aliased...
	[Solution]: in the MemDepUnit::insert line 223 have logic to prevent a checkInst in the storeset... --> check with someone.. (do i choose not to insert in instqueue::insert line 594)
	- We still want to do the whole producing_stores check because we still may have instrcutions which are dependant and these need to be dealt with accordingly. 
	- This means that store set isnt used..
	- validation still happens with IEW::executeInsts line 1348
		--> line 1338 do I need to check InstQueue::violation --> memDepViolation and disable any thing w that in the store set... (dont need to go in because not inserted in storeset)
	- violation checks happen in LSQ_Unit::executeLoad line 649 
		- This violation check is linked to checkViolations because memDepViolator is assigned there.
		- *not sure how it checks in relation to a store though...*
		- are load violations not just as a result of aliasing???
2) Do not speculate load and do not reorder the buffer because they will alias.. DO NOT USE STORE SET
	[Solution]: insert a flag for the given instruction for isReadBarrier --> double check this with someone.../isNonSpeculative/strictly ordered flag... (what is the difference??)
			- This allows the new instruciton to be inserted by the instQueue.insertBarrier method.../isNonSpeculative insert....
			- Find where storeset is used to check if the instruciton is violated and then never allows speculation again...
			- before issueing load, need to check LSQ for a corresponding store to its address....
3) Speculate and do not validate (guarantee)
	[Solution]: in the MemDepUnit::insert line 223 have logic to prevent a checkInst in the storeset... --> check with someone..
			--> to prevent validation we do not even do checkViolations --> we can in LSQ_unit::checkViolations line525 indicate to skip the checking proceedure...:
			--> the places where the violation checks are:
				-->	in IEW::executeInsts line 1348
			 	--> in LSQ_Unit::executeLoad line 649
				--> indicate instruction using a flag boolean and then do not enter condition if the instruction is present...  
4) Normal load
	[Solution]



[Changes+tests]
[1] Changes made in:
		1) Mem_Dep_Unit.cc::insert line 226 --> added condition not to check in storeset in the case of HINT instruction
            		-> Could add logic to prevent checks of twether an instruction is a producing store... 
		2) Check still happens and nothing is changed here...
[2] Changes made in:
		1) Mem_Dep_Unit.cc::insert --> do not need to insert into the store set but we indicate a dependancy via 2 methods, 1) straight in put it in the else line 262 2) put the instruction as a producing_store...
		2) lsq unit line 1559
		3) 
[3] Changes made in:
		1) Mem_Dep_Unit.cc::insert line223 --> added condition to not check in storeset in the case of a not aliased instruction..
		<!-- 2) Mem_dep_unit.cc::insert line229 --> added conditional to not update insertedLoads in storeset... [didnotwork] -->
[willRemoveforJimMem] 2) LSQ-unit.cc::executeLoad() --> added condition to checkviolations line 671
		3) NEED TO CHANGE:
               <!-- -> LSQ-unit.cc::checkViolations --> line 527 to skip load instruction checking when a store has been executed??  -->
[4] --> no changes: 


SHOULD I BE CHANGING STORES AS WELL TO NOT INSERT IN THE LIST OF DEPENDANTS


SO BASICALLY I NEED THE FUCKING STORE_SSID.... of the DEPENDENT STORE BUT HOWWWWWWWW......
In normal cases I would get this when the store actually violates...

if this is a valid inst_SSID, then there is a dependency...

so a valid inst_SSID happens when the instruction is has either:
	if there is a valid load set but no store associated:
		the validSSIT[store_index]
	if there is a valid store set but no load associated:
		the validSSIT[load_index]
	the valid load/store set is set by validSSIT[load/store_index]

the correct inst_SSID is obtained from SSIT[index]

when a violation happens either if
	the load needs to add a store to its storeset:
		SSIT[store_index] = load_SSID;
	the store needs to add a load to its storeset:
		SSIT[load_index] = store_SSID;

an insertion of a store is when:
	storeSSID = SSIT[store PC index]
	LFST[storeSSID] = store_seqNum

so in the checkInst, we get the inst_SSID from the SSIT[index].
so if a previous violation happened, i.e a load violated due to being speculated ahead of a store, the store get's put into that load's store set...
the SSIT[index] = SSIT[load instruction index...] = store_SSID...
 
when the check happens the dependent instruction seqnum is contained in
LFST[load instruction] = store_SSID...

so our counter must have its indexes based off the 

----
**Several ideas:**

Changing the order of execution [didntwork]
Changing the order in the instQUEUE scheduleReadyInsts (not sure when an instruction becomes "ready")
yes so i can change the order of commiting but how does it prevent in the reoorder buffer... --> not sure how 

- do I change when the instruction is being committed in the commit.cc or in the commiting in the instruction queueu????
do I change in the LSQ_unit read() [i have tried that but rescheduleInstructions does not workk...aaaaaaa] 
----
distance of stores

hold store sequence number extra PC, 

insert store 

load is held back in the instructionqueue or LSU

only way is waking dependents and hold back the sets of stores depending on the seqeunce numbers....

pC + iteration hashing ...

______________________________________________________________________________________________________________________________________________
##### Talk with vass:

> still going to pay the price of silicon if still using storeset

> if you decide not to speculate and scan instruciton queue and wait till only stores with sequence number is smaller. Allow loads to only issue when all the stores are complete.
youngest oldest store...

> take a snapshot of the oldest bitvector, every time a store issues, it resets the snapshot,. 16 entry LSQ

> 4 stores: 0 1 2 3 
> Bunch of loads, location 15.
> For load in 15, need to speculate. Every store inside the LSQ has a flip flopsaying it is a store, bit vector. Every load takes a snapshot of the > entries in the instruction queue. 
> whenever a store issues it clears it bit 0 for each load snapshot...
> when bit vector is 0 then you are guaranteed to go ahead. 

tell inst queue wakes depenedents
______________________________________________________________________________________________________________________________________________
#### *Generating correct flags*
--> ldr64.isa --> changing the flavour and adding a flag accordingly...
_______________________________________________________________________________________________________________________________________________

### Debugging

> Side note: The way the DPRINTF debug flags are executed may indicate(but may not possibly) the flow of the program from the micro-architecture to the CPU being simulated:
> Here's the flow of where the instruction gets printed: 
> --> arm/decoder.cc::Decoder::process() --> arm/insts/staticInsts.hh::ARMStaticInst constructor --> arm/insts/mem64.hh::Memory64 constructor (higher up parent class in this file) --> arm/decoder.hh::Decoder::decode() [this probably gets called in the return of decoder::decode] --> cpu/fetch.cc receives the instruction....	

> an instruction is added in the aarch64.isa which seems to be a bit like a decoder... 
> the instruction is outputted in is in decoder.hh 
> can for now just leave this and understand later... --> you just need to find out where instructions in general are decoded and used...
> O3 CPU is ISA independent. The base O3 will not be handling your ISA specific instructions and so ignore this translation for now...
--> the instruction inherits one of the functions in the .cc files of the ISA
--> this should be derived from ArmStaticInst which is then derived from StaticInst

### How the instruction is received in O3 CPU??/Log

--> decoded I presume in the decoder (lol) (might need to look at the actual way it's getting decoded)
--> lets make a python config file to ouput the instructions that are used from a program... (i'll do for the fetch first...)
	--> dont be daft. Why make an config while when you have debug flags...
	--> line 1069 fetch.cc --> might wanna change to the debug flag of instruction decode to find the instruction that is being fetched...
	--> the fetch.cc sends to the decode line1086...
	--> decoder.cc sees the instruction and sends to rename line648/653
	--> rename sees the instruction and once renamed it is sent to IEW line571...

How the instruction is processed in the memory management unit...  
	--> I guess this will be looking at DTLB/Caches first and how the memory management unit connects to this (where is the bus system???)
	--> possible key files... o3/ iew.cc (instruction, execute, writeback) inst_queue.cc lsq_unit.cc mem_dep_unit.cc
	--> IEW::dispatch & IEW::dispatchInsts line inserts the renamed instructions into the IQ and LSQ line1012/1023 and other corresponding inserts...
	--> Inst_queue class line751 manages the ready instructions and schedules to an available FU(free unit)?????
	--> IEW::executeInsts  execute() is invoked and sent to commit... (inside tick() line1461)
	--> Line1213 IEW::execute() the load is added to the LDSTQ for execution.
	--> Line 602 LSQUNIT::executeLoad --> initiateAcc() is called for the specific instructions initiation...
	--> InitiateAcc() is somehow overridden??? Might need to ask how.... [more of a C++ thing I think...]
	--> IEW::writebackInsts  InstructionQueue::wakeDependents() is invoked... Dependant instructions are added to the ready list for scheduling... (inside tick() line1463)
	--> I think the writing to the destinatio register happens in completeAcc() in the generated C++ code. If you look at the line getMemLE, this returns the memory contents (I think)
	--> If there is NoFault, then cSwap() [see below for what Cswap is] for the *mem* happens and the value gets loaded into the XDest...
	--> Subsquent context things get updated (I think)
	--> Commit::tick() & then Commit::commitInsts(). Once the instruction reaches the head of ROB, it will be committed and released from ROB.
	--> other important files: rob, store_set, lsq_unit, lsq, mem_dep_unit


_______________________________________________________________________________________
### Running Aarch64 in gem5
Running a program file for 64bit ARM is not documented on the wiki, so need to cross compile as such: aarch64-linux-gnu-gcc (installing the right dependancies etc --> 
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu)
___________________________________________________________________________________________________________________________
### Some lil commands for me
copying the binaries from coremark pro... 
cp -R ~/coremark-pro/builds/linux64/gcc64/bin/ ~/gem5/coremarkpro
r50028931@7.191.137.87
transfering gem5 to server: scp -r ~/gem5 r50028931@7.191.137.87
from the docker: scp r50028931@7.191.137.87:run_testcases.sh .
ssh r50028931@7.191.137.87
bash
sudo -E docker exec -d c4523a9d69cc ./../home/rohan/run_benchmarks.sh
aarch64-linux-gnu-objdump hello -S > hello.s
./build/ARM/gem5.opt --debug-flags=IEWFetchDirectLsqviolation,IEWlsqviolation,LSQUNIThitexternalsnoopviolation,LSQUNITisnotLoadviolation,Exec configs/example/se.py --cpu-type=DerivO3CPU --caches -c ../testprogs/hello > output.log

 ./build/ARM/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches -c ../micro-verification_proc --options "2.12312 2.1212"

scons build/ARM/gem5.opt -j9

scp r50028931@7.191.137.87:~/patcher.py ~/patcher.py
scp user1@example.com:/path/to/file /path/to/store/file
scp run_benchmarks.sh  r50028931@10.206.153.30:~
the . copies to the current directory
yo!!!!!!!! okokokok 

docker exec -d c4523a9d69cc bash -c "cd /home/rohan && ./run_testcases.sh 2>&1 > testcase1.log"
in the docker 
kill -SIGINT [gem5processID]

run command in background 

LD_FLAGS="-fuse-ld=lld" /home/root/Programming/llvm-alias-pass/build/bin/clang verification.c get_indecies.c -lc -O0 -static -target aarch64-linux-gnu -fuse-ld=lld -o verification-base

command > ~/extraaliasing.log 2>&1 & 

Violator PC: \(0x4007c8=>0x4007cc\).\(0=>1\)\s\[sn:.*\], inst PC: \(0x4007c0=>0x4007c4\)

Violator PC: \(0x4007c8=>0x4007cc\).\(0=>1\)\s\[sn:.*\], inst PC: \(0x4007d0=>0x4007d4\)

_______________________________________________________________________________________________________________________________________________
## Project timeline

**Week 1**: 
- Getting set up, going through gem5 and understanding how things link. Reading documentation and doing some tutorials
  
**Week 2**:

- Adding an instruction, figuring how to do that. How this instruction links to the CPU abstraction, debugging, compilation in 64bits not 32...

**Week 3**:

- How the instruction interacts with the memory unit. Going through the alias predictor and figuring out how I can modify it with my instruction...
	- testing added instruction and changing binaries to swap current load instructions with my instruction

**Week 4**:

- building benchmark testing and clarifying changes Im making to the prediction in the CPU
	- building complete benchmark testing 
	- looking more at the memory unit and seeing how/where to changing the prediction unit

**Week 5**:

- Adding flags to all new instructions...
	- Verifying cases: 1 --> NOT ALIASED GUARANTEE 
	- meetings to verfying what we are doing...
	- baseline testing prepped
	- figuring out DEFINITELY ALIASED CASE
	- other cases seem to be more trivial and need to wait for proper pass.

**Week 6**

- baseline testing complete
- still figuring out DEFINITELY ALIASED CASE
- *holiday* - 3 days 

**Week 7**:

- finishing off DEFINITELY ALIASED CASE...

**Week 8**:

- fixing DEFINITELY ALIAS CASE

**Week 9**:

- constructing appriopriate microbenchmark tests

**Week 10**:

- testing on microbenchmark + tweaking it + retesting

**Week 11**:

- Holiday... + presentation delivery + continuation of tweaking gem5

**Week 12**:

- finishing off DEFINITELY ALIASED CASE...
- making data presentable
- writing documentation/cleaning git project...

_______________________________________________________________________________________
## Summary of changes

1) ```src/arch/arm/isa/formats/aarch64.isa```

	- added custom instructions for IMM, PRE, POST, REG load (32, 64 & FP64 & FP128) instructions 
	- names are as follows: 
		- [*name_of_load_inst*]**SBC** (SpeculateButCheck/Predict No Alias)
		- [*name_of_load_inst*]**DA** (DefinitelyAliased/Predict Alias)
		- 

2) ```src/arch/arm/isa/insts/ldr64.isa```
 
	- adding the *buildLoads function* for the custom load instructions
		- line 443 ish
	- created specific mneumonics which allow for flags to be associated to the given load...
		- e.g ```if "ldr_def_alias" then append instFlag("IsNotAlias")``` 
		- line 80 ish
		 
3) ```src/cpu/o3/dyn_inst.hh``` / ```src/cpu/static_inst.hh```

	- added flag handling for each custom instruction type. This is used by instructions in the CPU which generally take the object of DynInstPtr...
	- added flag for SpecbCheck, DefAlias, NotAlias (unused)
	- DynInst is based off staticInst. Theres some documentation on the gem5 website abou this...

4) ```src/cpu/o3/fetch.cc```
	- some debug flags added

5) ```src/cpu/o3/iew.cc``` (and the .hh too)
	- some debug flags added (IEWFetchDirectLsqviolation, IEWInstruction, IEWlsqviolation, Speculate, ViolationEvent, DefAlias )
	- custom statistics (notExactPhysicalAddrViolation,baseStoreSetViolationAddition, bypassStoreSetViolationAddition )
	- ```IEW::dispatchInsts()```

		- line 1112 ish 
			- added for *definitely aliased case* which gets the sequence number of the tail of the store queue.
			- this is passed to the instruction queue (detailed in instqueue.cc)  

	- ```IEW::executeInsts()```

		- line 1385 ish 
			- added stat for when Gem5 catches a violation when the addresses aren't actually equal.
		- line 1393 ish onward
		 	- stats to indicate CPU does not add to storeset when a violation happens with the custom isntructions... 
			- similar stat to indicate that we DO add to store set when violation happens in base case...
		- line 1411 
			- debug flag to count when violations happen....
	
6) ```src/cpu/o3/inst_queue.cc``` (and the .hh too)
	-  ```InstructionQueue::insert()```
		- line 596 ish
		- when an instruction is inserted into memDepUnit, we handle if there is an extra dependancy... 
		- debug flags here and there (DefAlias, DefNotAlias, Speculate)

7) ```src/cpu/o3/lsq_unit.cc``` (and the .hh too) 
	- ```LSQUnit::checkViolations```
		- just some debug flags ( LSQUNITisnotLoadviolation, LSQUnitInstruction, ViolationEvent,DefNotAlias, DefAlias )
		- here maybe is where you want to skip the check for a violation if it is a guarantee load...
	- added a function which checks the load queue when a new store enters in case that is aliased with a load it is actually going to alias with. Sometimes if the load enters the load queue yet the store it has aliased with is not in the store queue, there may be a dependancy issue...
		- ```::getDepStoreSeqNum(DynInstPtr load_inst)``` - left uncommented at the bottom (and in ```lsq.cc```) because it is unused anyway.
		- **DID NOT WORK**
		- this is because the *effAddr* of instructions aren't available at the time of dispatching... 
		- so checking for aliasing is not possible when you first insert instructions...
	-```LSQUnit::read```
		- added debug flag to find what AddrRangeCoverage is....
		- is usually NoAddrRangeCoverage i think...
	- ```src/cpu/o3/lsq_.cc``` (and the .hh)
		- you also have to delcare the functions you have in ```lsq_unit``` in ``lsq``` LSQ is higher than LSQ unit in the hierarchy (i think)...

8) ```src/cpu/o3/mem_dep_unit.cc``` (and the .hh too) 
	- ```MemDepUnit::insert(const DynInstPtr &inst)```
		- skip the checking of storeset when a custom instruction is being inserted into the MemDepUnit...
			- statistics added to show this (BypassStoreSetCheck, baseUsingStoreSetCheck)
		- debug flag in the event that there is no dependency (other debug flag names: DefNotAlias, DefAlias)
	-  ```MemDepUnit::wakeDependents(const DynInstPtr &inst)```
		- ***Counter additions...***
		- Increment/Decrement counter based on falsed dependency of a woken instruction...
		- false dependancy check relies on same concept as ```LSQUnit::checkViolations```
		- Several debugging stuff (flag name: Counter )

7) ```src/cpu/o3/store.cc``` (and the .hh too) 
	- ```StoreSet::clearStore(Addr store_PC)```
		- Counter stuff
		- Clear a specific store off the store-set
	- ```StoreSet::incr_clearCounter(Addr store_PC)```
		- increment counter for storePC
	- ```StoreSet::decr_clearCounter(Addr store_PC)```
		- decrement counter for storePC
		- if the counter is 0 remove it too.
	- ```StoreSet::violation(Addr store_PC, Addr load_PC)```
		- if a violation occurs (in all cases) we set the counter to a counter size of choosing (4 done for now...)

8) ```src/cpu/SConscript```
	- this is where all the debug flags are added...



Implemented instruction:
No fp32 ldp done

LDP PNA: 
	- I32 (given before)
	- i64 (in decodeSVEMem section)
	- FP64 (given before)
	- FP128 (in decodeSVEMem section)

----
LDP PA:
	- I32 (given before)
	- i64 (in decodeSVEMem section)
	- FP64 (given before)
	- FP128 (in decodeSVEMem section)

noalloc not done + ldpsw
---

LDR PNA:
REG/PRE/POST/IMM
	- (LDRB) 8 bit: 0x0f (normal is 0x01)
	- (LDRH) 16 bit 0x1f (normal is 0x09)
	- (LDRW) 32 bit:  0xe (normal is 0x11)
	- (LDRSW) 32 bit: 0x13 (normal is 0x12)
	- (LDRSFP) 32 bit: 0x17 (normal is 0x15)
	- (LDRX) 64 bit:  0x1b (normal is 0x19)
	- (LDRFP) 64 bit:  0x1e (normal is 0x1d)
	- (LDRFP) 128 bit: 0x16 (normal is 0x07)

LDUR PNA:
IMM
	- (LDURB) 8 bit: 0x0f (normal is 0x01)
	- (LDURH) 16 bit 0x1f (normal is 0x09)
	- (LDURW) 32 bit:  0xe (normal is 0x11)
	- (LDURSW) 32 bit: 0x13 (normal is 0x12)
	- (LDURSFP) 32 bit: 0x17 (normal is 0x15)
	- (LDURX) 64 bit:  0x1b (normal is 0x19)
	- (LDURFP) 64 bit:  0x1e (normal is 0x1d)
	- (LDURFP) 128 bit: 0x16 (normal is 0x07)

LDTR PNA:
    - (LDTRB) 8 bit: 0x0f (normal is 0x01)
	- (LDTRH) 16 bit 0x1f (normal is 0x09)
	- (LDTRW) 32 bit:  0xe (normal is 0x11)
	- (LDTRSW) 32 bit: 0x13 (normal is 0x12)
	- (LDTRX) 64 bit:  0x1b (normal is 0x19)
	- has more space for PAs here (since no FPs) (13 free spaces...)


-----------
PAs 
in the Sve section....

0 xx 0010 (31:25) 
To select there is bits 30:29

00: IMM
01: REG
10: PRE
11: POST

Within all of these the following types of loads are added in:

	- (LDRB) 8 bit: 0x00
	- (LDRH) 16 bit: 0x01
	- (LDRW) 32 bit:  0x02
	- (LDRSW) 32 bit: 0x03
	- (LDRSFP) 32 bit: 0x04
	- (LDRX) 64 bit:  0x05
	- (LDRFP) 64 bit:  0x06
	- (LDRFP) 128 bit: 0x07

--------------


fixed the macro instructions by added mnemonics to the micro constructors..
__________________________________________
IgnoreLUKE!


____________________
To add for each load..s.
---
Keeping same as other loads
LDRB - 0x0f (keep)
<!-- LDRSBX - 0x13 (not needed)
LDRSBW - 0x17 (not needed)
LDRBFP - 0x1f (not needed) -->

LDRSW (keep) 0x13
LDRSFP (keep) 0x17
one more space for something
LDRH (keep) 0x1f
--

---
New place:
but we have 3 bits (so 8PAs)



------
unimlpemented...
LDRSHX (not needed)
LDRSHW (not needed)
LDRHFP (not needed)



______
other loads to consider
LDTR (easy)

LDRAA/LDRAB (get rid off)

LDAPR (not needed)

LDAR (not needed to mark)

LDAXP (not needed to mark)

LDXR (not needed to mark)
____________________________
Key pages 176,
Memory instructions have the form (p548/11952)


31      28 (opc0) | 27 | 26 (opc1) | 25 | 24 23 (opc2) | 22 | 21  16 (opc3) | 15  12 | 11 10 (opc4) |  9 		 0 |

What is free:

opc0 = 0
opc1 = 1 
opc2 = x0


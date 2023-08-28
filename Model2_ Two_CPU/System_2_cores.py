# Libraries import
import m5
from m5.objects import *
from caches import *

#Declaration of the system
system = System()
# -----------------------------------------------#


#Clock definition
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()
# -----------------------------------------------#


#Creation of the memory in timing mode
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]
# -----------------------------------------------#


#Adding two CPU to the system
system.cpu = [ArmO3CPU(),ArmO3CPU()]
# -----------------------------------------------#


#Creation of the memory bus
system.membus = SystemXBar()
# -----------------------------------------------#


# Mandatory Interrupt Controller
system.cpu[0].createInterruptController()
system.cpu[1].createInterruptController()
# -----------------------------------------------#


#Link between cpu and memory bus
system.system_port = system.membus.cpu_side_ports
# -----------------------------------------------#


#Creation of the memory controller
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports
# -----------------------------------------------#


#Cache creation
print("      Creating caches for CPU's : ")
system.cpu[0].icache = L1ICache()   # Each CPU has an instruction 
system.cpu[0].dcache = L1DCache()   # and a data cache
system.cpu[1].icache = L1ICache()   
system.cpu[1].dcache = L1DCache()   
print("      Done ! ")
# -----------------------------------------------#


#Connection between caches and CPU's
print("      Connecting caches to CPU's : ")
system.cpu[0].icache.connectCPU(system.cpu[0])
system.cpu[0].dcache.connectCPU(system.cpu[0])

system.cpu[1].icache.connectCPU(system.cpu[1])
system.cpu[1].dcache.connectCPU(system.cpu[1])
print("      Done ! ")
# -----------------------------------------------#


#Memory bus between level 1 nd level 2
system.l2bus = L2XBar()
# -----------------------------------------------#

#Connection caches I & D 		
print("      Connecting L1 caches to L2 bus : ")
system.cpu[0].icache.connectBus(system.l2bus)
system.cpu[0].dcache.connectBus(system.l2bus)

system.cpu[1].icache.connectBus(system.l2bus)
system.cpu[1].dcache.connectBus(system.l2bus)
print("      Done ! ")
# -----------------------------------------------#


#Creation of l2 cache
system.l2cache = L2Cache()
system.l2cache.connectCPUSideBus(system.l2bus)
system.l2cache.connectMemSideBus(system.membus)
# -----------------------------------------------#


#Setting up the instructions
print("Setting up the instruction")
# Here indicate to your system which binary you execute for each CPU
# Indicate bellow the correct path to your binary !

binary0='.../code_victim_Prime_and_probe'
binary1='.../code_attacker_Prime_and_probe'

#binary0='.../code_attacker_Flush_and_Reload'
#binary1='.../code_attacker_Flush_and_Reload'

system.workload = SEWorkload.init_compatible(binary0)
# -----------------------------------------------#

#Indicate a different pid for each process
process0 = Process(pid=100)
process1 = Process(pid=101)
process0.cmd = [binary0]
process1.cmd = [binary1]
system.cpu[0].workload = [process0]
system.cpu[1].workload = [process1]
system.cpu[0].createThreads()
system.cpu[1].createThreads()
# -----------------------------------------------#


print("      Instruction setup done ! ")
print("")
print("      Instanciation for the system")
print("")
#Création d'une instanciation pour le système en créant l'objet root
root = Root(full_system = False, system = system)
m5.instantiate()
print("")
print("Beginning simulation!  ----------------------------")
print("")

#Simulation starting
exit_event = m5.simulate()

print("")
print("")
print('Exiting @ tick {} because {}'.format(m5.curTick(), exit_event.getCause()))
print("")

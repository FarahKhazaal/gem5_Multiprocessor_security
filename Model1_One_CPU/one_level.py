# Import des librairies m5
import m5
from m5.objects import *
#Import of the ache.py file
from caches import *



#Creation of options that can be retrieved from the terminal when typing --help.
import argparse

parser = argparse.ArgumentParser(description='A simple system with 2-level cache.')
parser.add_argument("binary", default="", nargs="?", type=str,
                    help="Path to the binary to execute.")
parser.add_argument("--l1i_size",
                    help=f"L1 instruction cache size. Default: 16kB.")
parser.add_argument("--l1d_size",
                    help="L1 data cache size. Default: Default: 64kB.")


options = parser.parse_args()




#Creation of the global system 
system = System()
#------------------------------------------------#

#Definition of the clock
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
#Type of voltage used
system.clk_domain.voltage_domain = VoltageDomain()


#------------------------------------------------#



#------------------------------------------------#
#              MEMORY CREATION                   #
#------------------------------------------------#

#Creation of the memory in timing mode
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]




#------------------------------------------------#

#This CPU model executes each instruction in a single clock cycle
#The cpu used is the ArmO3CPU()
system.cpu = ArmO3CPU()
#------------------------------------------------#



#Creation of the memory bus
system.membus = SystemXBar()

#------------------------------------------------#
#Creation of the interrupt Controller
system.cpu.createInterruptController()
# connexion of the ports

system.system_port = system.membus.cpu_side_ports

# #------------------------------------------------#

#Creation of the memory controller
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

#------------------------------------------------#



#------------------------------------------------#
#                CACHE CREATION                  #
#------------------------------------------------#

#Creation of the cache memory 
system.cpu.icache = L1ICache(options)
system.cpu.dcache = L1DCache(options)

#Connection of the caches to the CPU 
system.cpu.icache.connectCPU(system.cpu)
system.cpu.dcache.connectCPU(system.cpu)

#Connection of the caches to the memory 
system.cpu.icache.connectBus(system.membus)
system.cpu.dcache.connectBus(system.membus)


#------------------------------------------------#





#----------------------------------#
#      EXECUTION OF INSTRUCTIONS   #
#----------------------------------#
print("Setting up the instruction")





#Instructing gem5 about the location to find the command for execution.
binary_flushandreloadtest ='.../FlushAndReload_Basic_Attack'
binary_stolenkey ='.../RealSituation'


# for gem5 V21 and beyond
system.workload = SEWorkload.init_compatible(binary_stolenkey)
#system.workload = SEWorkload.init_compatible(options.binary_test_a3)


process = Process()

#precision of the type od workload
process.cmd = [binary_stolenkey]
system.cpu.workload = process
system.cpu.createThreads()

print("")
print("Instanciation for the system")
print("")


#Creating an instance for the system by initializing the root object
root = Root(full_system = False, system = system)
m5.instantiate()

print("Beginning simulation!")
#starting the simulation
exit_event = m5.simulate()

print('Exiting @ tick {} because {}'.format(m5.curTick(), exit_event.getCause()))



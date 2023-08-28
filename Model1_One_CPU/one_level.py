# Import des librairies m5
import m5
from m5.objects import *
#Import of the ache.py file
from caches import *



#creation des options qu'on peut recuperer a partir du terminal quand on tappe --help
import argparse

parser = argparse.ArgumentParser(description='A simple system with 2-level cache.')
parser.add_argument("binary", default="", nargs="?", type=str,
                    help="Path to the binary to execute.")
parser.add_argument("--l1i_size",
                    help=f"L1 instruction cache size. Default: 16kB.")
parser.add_argument("--l1d_size",
                    help="L1 data cache size. Default: Default: 64kB.")


options = parser.parse_args()




#Création de notre objet global
system = System()

#------------------------------------------------#

#Définition de la clock
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
#Type de voltage utilisé
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
#Création d'un controleur d'interruptions
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





#Indication à gem5 de l'endriot où trouver la commande à exécuter
binary = 'tests/test-progs/hello/bin/x86/linux/hello'
binary_test_a3='cpu_tests/benchmarks/bin/arm/PropreFichier/test_a_3'
binary_flushandreloadtest ='cpu_tests/benchmarks/bin/arm/PropreFichier/flushandreload'
binary_stolenkey ='cpu_tests/benchmarks/bin/arm/PropreFichier/stolenkey'


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



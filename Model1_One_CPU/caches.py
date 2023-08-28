#creation of cache memory

from m5.objects import Cache



#definition of the class cache
#setting some of the parameters of the BaseCache that do not have default values.
class L1Cache(Cache):
#Simple L1 Cache with default values

    assoc = 2
    tag_latency = 2
    data_latency = 2
    response_latency = 2
    mshrs = 4
    tgts_per_mshr = 20
    def __init__(self, options=None):
        super(L1Cache, self).__init__()
        pass
    def connectBus(self, bus):
       # Connect this cache to a memory-side bus
        self.mem_side = bus.cpu_side_ports
    def connectCPU(self, cpu):
        #Connect this cache's port to a CPU-side port This must be defined in a subclass
        raise NotImplementedError
    


#-------------------------------------------------------------------
class L1ICache(L1Cache):
    size = '8kB'
    def __init__(self, options=None):
    	super(L1ICache, self).__init__(options)
    	if not options or not options.l1i_size:
        	return
    	self.size = options.l1i_size

	#connect the port of the memory cache to the port of cpu
    def connectCPU(self, cpu):
        self.cpu_side = cpu.icache_port
        
    def connectMemSideBus(self, bus):
        self.mem_side = bus.cpu_side_ports

class L1DCache(L1Cache):
    size = '8kB'
    def __init__(self, options=None):
    	super(L1DCache, self).__init__(options)
    	if not options or not options.l1d_size:
        	return
    	self.size = options.l1d_size

    def connectCPU(self, cpu):
        self.cpu_side = cpu.dcache_port
        
    def connectMemSideBus(self, bus):
        self.mem_side = bus.cpu_side_ports


#-------------------------------------------------------------------------
#creaton one memory cache extented from the basecache to be common for the 2 previous memory cahe 
class L2Cache(Cache):
    size = '16kB'
    assoc = 8
    tag_latency = 20
    data_latency = 20
    response_latency = 20
    mshrs = 20
    tgts_per_mshr = 12
    def __init__(self, options=None):
    	super(L2Cache, self).__init__()
    	if not options or not options.l2_size:
        	return
    	self.size = options.l2_size

    def connectCPUSideBus(self, bus):
    	self.cpu_side = bus.mem_side_ports

    def connectMemSideBus(self, bus):
   	 self.mem_side = bus.cpu_side_ports
   	 
   	 
  # Definition of the L3 Cache class
class L3Cache(Cache):
    size = '1MB'  # Specify the size of the L3 cache
    assoc = 16
    tag_latency = 30
    data_latency = 30
    response_latency = 30
    mshrs = 32
    tgts_per_mshr = 24

    def __init__(self, options=None):
        super(L3Cache, self).__init__()
        if not options or not options.l3_size:
            return
        self.size = options.l3_size

    def connectCPUSideBus(self, bus):
        self.cpu_side = bus.mem_side_ports

    def connectMemSideBus(self, bus):
        self.mem_side = bus.cpu_side_ports





EXPERIMENT1 = """\
[Config ChannelPacketLossRateSimulation]
Simulation.numTransmitters = 1
Simulation.radius = {}
Simulation.TransmitterNodeModule[*].PacketGeneratorModule.iatDistribution=0.01
Simulation.ReceiverNodeModule.PacketSinkModule.filename="exp1_stat_dist_{}.log"
"""

EXPERIMENT2 = """\
[Config MACPacketLossRateSimulation]
Simulation.numTransmitters = {}
Simulation.radius = 20
Simulation.TransmitterNodeModule[*].PacketGeneratorModule.iatDistribution=0.02
Simulation.ReceiverNodeModule.PacketSinkModule.filename="exp_stat_nodenum_{}.log"
"""


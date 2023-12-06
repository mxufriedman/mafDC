# mafDC
Dynamic clamp interface for Wavemetrics Igor

These files include the source C code for generating a dynamic clamp interface within Wavemetrics Igor.  Dynamic clamp allows a neuroscience researcher to specify a conductance in a neuron, such as a channel or receptor, and the dynamic clamp interface mimics the current that would flow through that conductance.
The conductances supported include: leak (i.e. constant conductance), time-varying linear (suitable for mimicking AMPA/GABA receptors), time-varying voltage-dependent (suitable for mimicking NMDA receptors), and complex Markov process (suitable for mimicking voltage-gated ion channels).
The tool also implements additional utility features, such as a test mode to evaluate the speed and latency of the dynamic clamp.
See Yang et al (2015) for more information: https://pubmed.ncbi.nlm.nih.gov/25632075/
To simply use the dynamic clamp, add mafDC.XOP, mafDC Help.ihf, and mafDC helper.ipf to the appropriate Wavemetrics folders.
The other files (.c.r.rc.h) were used to generate the XOP in Visual C++.

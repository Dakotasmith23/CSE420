Dakota Smith
5162129
Average Speed up: 1.76

Printout from inxi -Fx
System:    Host: dakota-Surface-Pro-6 Kernel: 5.13.0-30-generic x86_64 bits: 64 compiler: N/A Desktop: Gnome 3.36.9 
           Distro: Ubuntu 20.04.3 LTS (Focal Fossa) 
Machine:   Type: Laptop System: Microsoft product: Surface Pro 6 v: 124000000000000000000000D:0B:13F:5C:10P:38S:01E:0 
           serial: <superuser/root required> 
           Mobo: Microsoft model: Surface Pro 6 serial: <superuser/root required> UEFI: Microsoft v: 235.3732.768 
           date: 04.20.2021 
Battery:   ID-1: BAT1 charge: 40.4 Wh condition: 41.7/45.0 Wh (93%) model: DYN M1009168 status: Charging 
CPU:       Topology: Quad Core model: Intel Core i5-8250U bits: 64 type: MT MCP arch: Kaby Lake rev: A L2 cache: 6144 KiB 
           flags: avx avx2 lm nx pae sse sse2 sse3 sse4_1 sse4_2 ssse3 vmx bogomips: 28800 
           Speed: 800 MHz min/max: 400/3400 MHz Core speeds (MHz): 1: 800 2: 1653 3: 800 4: 800 5: 800 6: 800 7: 800 8: 800 
Graphics:  Device-1: Intel UHD Graphics 620 vendor: Microsoft driver: i915 v: kernel bus ID: 00:02.0 
           Display: x11 server: X.Org 1.20.13 driver: i915 resolution: 2736x1824~60Hz 
           OpenGL: renderer: Mesa Intel UHD Graphics 620 (KBL GT2) v: 4.6 Mesa 21.0.3 direct render: Yes 
Audio:     Device-1: Intel Xeon E3-1200 v5/E3-1500 v5/6th Gen Core Processor Imaging Unit driver: ipu3-imgu bus ID: 00:05.0 
           Device-2: Intel driver: ipu3-cio2 bus ID: 00:14.3 
           Device-3: Intel Sunrise Point-LP HD Audio driver: snd_hda_intel v: kernel bus ID: 00:1f.3 
           Sound Server: ALSA v: k5.13.0-30-generic 
Network:   Device-1: Marvell 88W8897 [AVASTAR] 802.11ac Wireless driver: mwifiex_pcie v: 1.0 port: 3000 bus ID: 01:00.0 
           IF: wlp1s0 state: up mac: b8:31:b5:91:13:5b 
           Device-2: Marvell Bluetooth and Wireless LAN Composite Device type: USB driver: btusb bus ID: 1-5:3 
Drives:    Local Storage: total: 238.47 GiB used: 16.72 GiB (7.0%) 
           ID-1: /dev/nvme0n1 vendor: Samsung model: KUS030202M-B000 size: 238.47 GiB 
Partition: ID-1: / size: 22.79 GiB used: 16.69 GiB (73.2%) fs: ext4 dev: /dev/nvme0n1p5 
           ID-2: swap-1 size: 8.94 GiB used: 0 KiB (0.0%) fs: swap dev: /dev/nvme0n1p6 
Sensors:   System Temperatures: cpu: 33.5 C mobo: N/A 
           Fan Speeds (RPM): N/A 
Info:      Processes: 325 Uptime: 32m Memory: 7.68 GiB used: 2.92 GiB (38.0%) Init: systemd runlevel: 5 Compilers: gcc: 9.3.0 
           Shell: bash v: 5.0.17 inxi: 3.0.38 

I did not achieve the 2x speed up but there are several factors that could have played a role in that. One is that my sequential implementation could have been more efficient than it needed to be which would cause for a lower speed up. Another factor could be that my clock speed is relatively not as fast as some other processors. This would also mean that the sequential would take longer and therefor lower the speedup. For my machine and implementation it makes sense that I didn't achieve a 2x speed up given that my clock speed isn't as fast as it could be and also my sequential implemenation is likely more efficient.


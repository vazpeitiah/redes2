import telnetlib
import os
import time

TFTP_DIR = '/home/kubuntu/tftp'

HOST_R1 = "192.168.0.250" # ip addr router r1
HOST_R2 = "192.168.1.2" # ip addr router r2
IP_TFTP = "192.168.0.133" # ip tfpt server

password = 'pass123' # password telnet
password_enable = 'Password' # password for enable mode

''' 
    SEND AND RECIVE RUNNIG CONFIG FOR ROUTER R1

'''

tn_r1 = telnetlib.Telnet(HOST_R1) # connecto to r1

tn_r1.read_until(b"Password: ")
tn_r1.write(password.encode('ascii') + b"\r") # write password telnet

print("You are connected to {}".format(HOST_R1))

tn_r1.read_until(b"R1>")
tn_r1.write('enable'.encode('ascii') + b"\r") # write enable command

tn_r1.read_until(b"Password: ")
tn_r1.write(password_enable.encode('ascii') + b"\r") # write password enable

print('R1: "enable" mode activated')

tn_r1.read_until(b"R1#")
tn_r1.write('copy running-config tftp:'.encode('ascii') + b"\r") # send runnig config to tftp server in linux

tn_r1.read_until(b"Address or name of remote host []? ")
tn_r1.write(IP_TFTP.encode('ascii') + b"\r") # set server tftp address

tn_r1.read_until(b"Destination filename [r1-confg]? ")
tn_r1.write('backup'.encode('ascii') + b"\r") # set file name

time.sleep(6) # wait to send file

print('R1: runnig-config send to tftp server')


path = os.path.join(TFTP_DIR, 'backup')
file = open(path, 'r');

# Read operation
lines = []
for line in file.readlines():
    if 'R1' in line:
        lines.append(line.replace('R1', 'Remplazo1'))
    else:
        lines.append(line)
file.close()

# Write operation
new_file = open(path, 'w+')
new_file.writelines(lines)
new_file.close()

print('R1: hostname R1 is update to Remplazo1')

tn_r1.read_until(b"R1#")
tn_r1.write('copy tftp: running-config'.encode('ascii') + b"\r") # send runnig config to router in gns3

tn_r1.read_until(b"Address or name of remote host []? ")
tn_r1.write(IP_TFTP.encode('ascii') + b"\r") # set tftp server ip address

tn_r1.read_until(b"Source filename []? ")
tn_r1.write('backup'.encode('ascii') + b"\r") # set filename of backup

tn_r1.read_until(b"Destination filename [running-config]? ")
tn_r1.write(b"\r") # set return key to continue

print('R1: running-config modified send to router R1')

tn_r1.read_until(b"Remplazo1#")
tn_r1.write('write'.encode('ascii') + b"\r") # save config

print('R1: configuration is saved')

tn_r1.read_until(b"Remplazo1#")
tn_r1.close()
print('R1: telnet connection is close')

''' 
    SEND AND RECIVE RUNNIG CONFIG FOR ROUTER R2

'''

tn_r2 = telnetlib.Telnet(HOST_R2) # connecto to r1

tn_r2.read_until(b"Password: ")
tn_r2.write(password.encode('ascii') + b"\r") # write password telnet

print("You are connected to {}".format(HOST_R2))

tn_r2.read_until(b"R2>")
tn_r2.write('enable'.encode('ascii') + b"\r") # write enable command

tn_r2.read_until(b"Password: ")
tn_r2.write(password_enable.encode('ascii') + b"\r") # write password enable

print('R2: "enable" mode activated')

tn_r2.read_until(b"R2#")
tn_r2.write('copy running-config tftp:'.encode('ascii') + b"\r") # send runnig config to tftp server in linux

tn_r2.read_until(b"Address or name of remote host []? ")
tn_r2.write(IP_TFTP.encode('ascii') + b"\r") # set server tftp address

tn_r2.read_until(b"Destination filename [r2-confg]? ")
tn_r2.write('backup2'.encode('ascii') + b"\r") # set file name

time.sleep(6) # wait to send file

print('R2: runnig-config send to tftp server')


path = os.path.join(TFTP_DIR, 'backup2')
file = open(path, 'r');

# Read operation
lines = []
for line in file.readlines():
    if 'R2' in line:
        lines.append(line.replace('R2', 'Remplazo2'))
    else:
        lines.append(line)
file.close()

# Write operation
new_file = open(path, 'w+')
new_file.writelines(lines)
new_file.close()

print('R2: hostname R2 is update to Remplazo2')

tn_r2.read_until(b"R2#")
tn_r2.write('copy tftp: running-config'.encode('ascii') + b"\r") # send runnig config to router in gns3

tn_r2.read_until(b"Address or name of remote host []? ")
tn_r2.write(IP_TFTP.encode('ascii') + b"\r") # set tftp server ip address

tn_r2.read_until(b"Source filename []? ")
tn_r2.write('backup2'.encode('ascii') + b"\r") # set filename of backup

tn_r2.read_until(b"Destination filename [running-config]? ")
tn_r2.write(b"\r") # set return key to continue

print('R2: running-config modified send to router R2')

tn_r2.read_until(b"Remplazo2#")
tn_r2.write('write'.encode('ascii') + b"\r") # save config

print('R2: configuration is saved')

tn_r2.read_until(b"Remplazo2#")
tn_r2.close()
print('R2: telnet connection is close')
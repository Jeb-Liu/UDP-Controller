import socket
from threading import Thread
from time import sleep
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time

#変数
dispX = np.zeros(100)
dispY = np.zeros(100)
dispZ = np.zeros(100)
dispX2 = dispX
dispY2 = dispY
dispZ2 = dispZ

#fig初期化
fig, (ax1,ax2,ax3) = plt.subplots(3,1)
lineX, = ax1.plot(dispX)
lineY, = ax2.plot(dispY)
lineZ, = ax3.plot(dispZ)
for ax in [ax1, ax2, ax3]:
    ax.grid(True)
    ax.set_xlim(0, 100)
    ax.set_ylim(-180, 180)
    ax.set_xlabel("ms")
ax.set_ylim(0, 360)
fig.set_size_inches(8, 8)
ax1.set_ylabel("Roll")
ax1.set_ylabel("Pitch")
ax1.set_ylabel("Yaw")

#UDPによるデータを読み取る
def recv_data():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    addr = ("192.168.1.100",23333)
    s.bind(addr)
    recvdata, client_address = s.recvfrom(1024)
    recvdata = recvdata.decode('utf-8')
    data = recvdata.split(",")
    #print("[ %s ] from:[ %s ]\n"%(recvdata,client_address))
    #print(data)
    s.close()
    return data

def update(frame):
    global dispX,dispX2,lineX
    global dispY,dispY2,lineY
    global dispZ,dispZ2,lineZ
    #データ取得
    data = recv_data()
    #曲線をプロット  
    dispX[0:-1] = dispX2[1:]
    dispX[-1] = data[10]
    dispX2 = dispX
    dispY[0:-1] = dispY2[1:] 
    dispY[-1] = data[11]
    dispY2 = dispY
    dispZ[0:-1] = dispZ2[1:] 
    dispZ[-1] = data[12]
    dispZ2 = dispZ
    #図に書き込み
    lineX.set_ydata(dispX)
    lineY.set_ydata(dispY)
    lineZ.set_ydata(dispZ)
    #色
    plt.setp(lineX, 'color', 'r', 'linewidth', 2.0)
    plt.setp(lineY, 'color', 'g', 'linewidth', 2.0)
    plt.setp(lineZ, 'color', 'b', 'linewidth', 2.0)

    line = [lineX, lineY, lineZ]
    return line

# Main Fuction
ani = animation.FuncAnimation(fig, update, interval=1)
plt.show()

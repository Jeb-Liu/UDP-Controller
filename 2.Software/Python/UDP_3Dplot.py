import socket
from threading import Thread
from time import sleep
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import math as m

#変数定義
X = np.zeros(10) #経路の長さ
Y = np.zeros(10)
Z = np.zeros(10)
X2 = X
Y2 = Y
Z2 = Z


#fig初期化
fig = plt.figure()
ax1 = fig.add_subplot(projection='3d')
ax1.grid(True)
ax1.set_xlim(-1, 1)
ax1.set_ylim(-1, 1)
ax1.set_zlim(-1, 1)
ax1.set_xlabel("x")
ax1.set_ylabel("y")
ax1.set_zlabel("z")
fig.set_size_inches(8, 8)



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
    global X,X2,Y,Y2,Z,Z2,line
    #データ取得
    data = recv_data()

    #オイラー角 to 方向ベクトル
    roll  = float(data[10])/180*3.1415926
    pitch = float(data[11])/180*3.1415926
    yaw   = float(data[12])/180*3.1415926

    #曲線をプロット  
    X[0:-1] = X2[1:]
    X[-1] = m.sin(yaw)*m.cos(pitch)
    X2 = X

    Y[0:-1] = Y2[1:]
    Y[-1] = m.cos(yaw)*m.cos(pitch)
    Y2 = Y

    Z[0:-1] = Z2[1:]
    Z[-1] = m.sin(pitch)
    Z2 = Z

    #図に書き込み
    line, = ax1.plot(X, Y, Z)

    #色
    plt.setp(line, 'color', 'r', 'linewidth', 2.0)

    return line

# Main Fuction
ani = animation.FuncAnimation(fig, update, interval=0.1)
plt.show()
